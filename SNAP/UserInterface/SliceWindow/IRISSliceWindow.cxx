/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IRISSliceWindow.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "IRISSliceWindow.h"

#include "GlobalState.h"
#include "OpenGLSliceTexture.h"
#include "IRISApplication.h"
#include "IRISImageData.h"
#include "UserInterfaceLogic.h"
#include "CrosshairsInteractionMode.h"
#include "PolygonInteractionMode.h"
#include "RegionInteractionMode.h"
#include "ZoomPanInteractionMode.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>

#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"

using namespace itk;
using namespace std;

IRISSliceWindow
::IRISSliceWindow(int x, int y, int w, int h, const char *l) 
: GenericSliceWindow(x, y, w, h, l)
{
  // Initialize the interaction modes
  m_PolygonMode = new PolygonInteractionMode(this);
  m_RegionMode = new RegionInteractionMode(this);

  // Initialize polygon slice canvas to NULL
  m_PolygonSlice = NULL;
}

IRISSliceWindow
::~IRISSliceWindow()
{
  // Delete the interaction modes
  delete m_CrosshairsMode;
  delete m_ZoomPanMode;
}

void 
IRISSliceWindow
::Register(int id,UserInterfaceLogic *parentUI)
{
  // Call the parent's version of this method
  GenericSliceWindow::Register(id, parentUI);

  // Register the interaction modes
  m_PolygonMode->Register();
  m_RegionMode->Register();
}

void 
IRISSliceWindow
::InitializeSlice(IRISImageData *imageData)
{
  // Call the parent's version of this method
  GenericSliceWindow::InitializeSlice(imageData);

  // Initialize the polygon drawing canvas
  Size<2> imgSize;
  imgSize[0] = m_SliceSize(0);
  imgSize[1] = m_SliceSize(1);
  m_PolygonSlice = PolygonSliceType::New();
  m_PolygonSlice->SetRegions(imgSize);
  m_PolygonSlice->Allocate();
}

void
IRISSliceWindow
::DrawOverlays()
{
  // Call the parent's version of this method
  GenericSliceWindow::DrawOverlays();

  // Draw the polygon
  m_PolygonMode->OnDraw();

  // Draw the region of interest if selected
  if(GetTopInteractionMode() == m_RegionMode)
    m_RegionMode->OnDraw();
}

void 
IRISSliceWindow
::EnterPolygonMode()
{
  // Place the mode on the stack
  ClearInteractionStack();
  PushInteractionMode(m_PolygonMode);
}

void 
IRISSliceWindow
::EnterRegionMode()
{
  // This interaction mode works in conjuction with the crosshairs mode
  ClearInteractionStack();
  PushInteractionMode(m_CrosshairsMode);
  PushInteractionMode(m_RegionMode);
}

void 
IRISSliceWindow
::AcceptPolygon() 
{
  assert(m_IsRegistered && m_IsSliceInitialized);

  // We'll be working with this PolygonDrawing object
  PolygonDrawing *draw = m_PolygonMode->m_Drawing;

  // Make sure we are in editing mode
  if (draw->GetState() != EDITING_STATE) return;

#ifdef DRAWING_LOCK
  if (m_GlobalState->GetDrawingLock(id)) {
#endif /* DRAWING_LOCK */

    // VERY IMPORTANT - makes GL state current!
    make_current(); 

    // Have the polygon drawing object render the polygon slice
    draw->AcceptPolygon((unsigned char *) m_PolygonSlice->GetBufferPointer(),
                        m_SliceSize(0),m_SliceSize(1));

    // take polygon rendered by polygon_drawing and merge with 
    // segmentation slice; send changes to voxel data set
    LabelType drawing_color = m_GlobalState->GetDrawingColorLabel();
    LabelType overwrt_color = m_GlobalState->GetOverWriteColorLabel();
    CoverageModeType mode = m_GlobalState->GetCoverageMode();

    // Get the current slice from the segmentation image
    LabelImageWrapper *seg = 
        m_Driver->GetCurrentImageData()->GetSegmentation();
    LabelImageWrapper::SlicePointer slice = seg->GetSlice(m_Id);
    slice->Update();

    // Iterate over the slice
    typedef ImageRegionIteratorWithIndex<PolygonSliceType> PolygonIterator;
    PolygonIterator itPolygon(m_PolygonSlice,
                              m_PolygonSlice->GetLargestPossibleRegion());

    for (itPolygon.Begin(); !itPolygon.IsAtEnd(); ++itPolygon)
    {
      // Get the current polygon pixel      
      PolygonSliceType::PixelType pxPolygon = itPolygon.Get();
      
      // Check for non-zero alpha of the pixel
      if((pxPolygon[2] != 0) ^  m_GlobalState->GetPolygonInvert())
      {
        // Get the corresponding segmentation image pixel
        Index<2> idx = itPolygon.GetIndex();
        LabelType pxLabel = slice->GetPixel(idx);

        // Check if we should be overriding that pixel
        if (mode == PAINT_OVER_ALL ||
            (mode == PAINT_OVER_ONE && pxLabel == overwrt_color) ||
            (mode == PAINT_OVER_COLORS && pxLabel != 0))
        {
          // Get the index into the image that we'll be updating
          Vector3f idxImageFloat = MapSliceToImage(Vector2f(idx[0],idx[1]));

          // Convert to integer
          Vector3i idxImage = to_int(idxImageFloat);
          
          // Check for rounding errors!  We could be editing the wrong pixel
          assert(to_float(idxImage) == idxImageFloat);

          // Set the value of the pixel in segmentation image
          m_Driver->GetCurrentImageData()->SetSegmentationVoxel(
            idxImage, drawing_color);
        }
      }
    }

    m_ParentUI->ActivatePaste(m_Id, true);
    m_ParentUI->ActivateAccept(m_Id, false);
#ifdef DRAWING_LOCK
    m_GlobalState->ReleaseDrawingLock(m_Id);
  }
#endif /* DRAWING_LOCK */
}

void 
  IRISSliceWindow
  ::PastePolygon()
{
  assert(m_IsRegistered && m_IsSliceInitialized);

#ifdef DRAWING_LOCK
  if (m_GlobalState->GetDrawingLock(m_Id)) {
#endif /* DRAWING_LOCK */

    if (m_PolygonMode->m_Drawing->CachedPolygon()) {
      m_PolygonMode->m_Drawing->PastePolygon();
      m_ParentUI->ActivateAccept(m_Id, true);
      redraw();
    }

#ifdef DRAWING_LOCK
  } else {
    m_GlobalState->ReleaseDrawingLock(m_Id);
  }
#endif /* DRAWING_LOCK */
}

