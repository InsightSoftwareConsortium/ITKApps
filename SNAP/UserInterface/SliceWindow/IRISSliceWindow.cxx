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

#include <assert.h>
#include <stdio.h>
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

  // Get a pointer to the drawing object
  m_PolygonDrawing = m_PolygonMode->m_Drawing;

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

  // Reset the polygon drawing interface
  m_PolygonDrawing->Reset();

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
  if(!m_ThumbnailIsDrawing)
    m_PolygonMode->OnDraw();

  // Draw the region of interest if selected
  if(IsInteractionModeAdded(m_RegionMode))
    m_RegionMode->OnDraw();
}

void 
IRISSliceWindow
::EnterPolygonMode()
{
  EnterInteractionMode(m_PolygonMode);
}

void 
IRISSliceWindow
::EnterRegionMode()
{
  EnterInteractionMode(m_RegionMode);
}

bool 
IRISSliceWindow
::AcceptPolygon() 
{
  assert(m_IsRegistered && m_IsSliceInitialized);

  // Make sure we are in editing mode
  if (m_PolygonDrawing->GetState() != PolygonDrawing::EDITING_STATE) 
    return false;

#ifdef DRAWING_LOCK
  if (m_GlobalState->GetDrawingLock(id)) {
#endif /* DRAWING_LOCK */

    // VERY IMPORTANT - makes GL state current!
    make_current(); 

    // Have the polygon drawing object render the polygon slice
    m_PolygonDrawing->AcceptPolygon((unsigned char *) m_PolygonSlice->GetBufferPointer(),
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

    // Create an iterator to iterate over the slice
    typedef ImageRegionIteratorWithIndex<PolygonSliceType> PolygonIterator;
    PolygonIterator itPolygon(m_PolygonSlice,
                              m_PolygonSlice->GetLargestPossibleRegion());

    // Keep track of the number of pixels changed
    unsigned int nUpdates = 0;

    // Iterate
    for (itPolygon.Begin(); !itPolygon.IsAtEnd(); ++itPolygon)
    {
      // Get the current polygon pixel      
      PolygonSliceType::PixelType pxPolygon = itPolygon.Get();
      
      // Check for non-zero alpha of the pixel
      if((pxPolygon != 0) ^  m_GlobalState->GetPolygonInvert())
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
          Vector3f idxImageFloat = 
            MapSliceToImage(Vector3f(idx[0]+0.5f,idx[1]+0.5f,m_DisplayAxisPosition));

          // Convert to integer
          Vector3ui idxImage = to_unsigned_int(idxImageFloat);
          
          // Set the value of the pixel in segmentation image
          m_Driver->GetCurrentImageData()->SetSegmentationVoxel(
            idxImage, drawing_color);

          // Increment the counter
          nUpdates++;
          }
        }
      }

#ifdef DRAWING_LOCK
    m_GlobalState->ReleaseDrawingLock(m_Id);
  }
#endif /* DRAWING_LOCK */

  return (nUpdates > 0);
}

void 
IRISSliceWindow
::PastePolygon()
{
  assert(m_IsRegistered && m_IsSliceInitialized);

#ifdef DRAWING_LOCK
  if (m_GlobalState->GetDrawingLock(m_Id)) 
    {
#endif /* DRAWING_LOCK */

    if (m_PolygonDrawing->GetCachedPolygon()) 
      {
      m_PolygonDrawing->PastePolygon();
      redraw();
      }

#ifdef DRAWING_LOCK
    } 
  else 
    {
    m_GlobalState->ReleaseDrawingLock(m_Id);
    }
#endif /* DRAWING_LOCK */
}

void 
IRISSliceWindow
::ClearPolygon()
{
  m_PolygonDrawing->Delete();
  redraw();
}

void 
IRISSliceWindow
::DeleteSelectedPolygonPoints()
{
  m_PolygonDrawing->Delete();
  redraw();
}

void 
IRISSliceWindow
::InsertPolygonPoints()
{
  m_PolygonDrawing->Insert();
  redraw();
}



