/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IRISApplication.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "IRISApplication.h"
#include "GlobalState.h"
#include "IRISImageData.h"
#include "IRISVectorTypesToITKConversion.h"
#include "SNAPImageData.h"
#include "UserInterfaceLogic.h"
#include "IntensityCurveVTK.h"

#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkPasteImageFilter.h"

using namespace itk;


IRISApplication
::IRISApplication() 
{
  // Construct new global state object
  m_GlobalState = new GlobalState;

  // Contruct the IRIS and SNAP data objects
  m_IRISImageData = new IRISImageData();
  m_SNAPImageData = NULL;

  // Set the current IRIS pointer
  m_CurrentImageData = m_IRISImageData;

  // Initalize the user interface
  m_UserInterface = new UserInterfaceLogic(this);

  // Construct a new intensity curve
  m_IntensityCurve = IntensityCurveVTK::New();
  m_IntensityCurve->Initialize(4);
}

IRISApplication
::~IRISApplication() 
{
  m_IntensityCurve = NULL;

  delete m_UserInterface;
  delete m_IRISImageData;
  delete m_SNAPImageData;
  delete m_GlobalState;
}

void 
IRISApplication
::InitializeSNAPImageData(const RegionType &roi) 
{
  assert(m_SNAPImageData == NULL);

  // Create the SNAP image data object
  m_SNAPImageData = new SNAPImageData();

  // Copy the contents
  m_IRISImageData->DeepCopyROI(roi,*m_SNAPImageData,m_GlobalState->GetDrawingColorLabel());

  // Assign the intensity mapping function to the Snap data
  m_SNAPImageData->GetGrey()->SetIntensityMapFunction(m_IntensityCurve);

  // Initialize the speed image of the SNAP image data
  m_SNAPImageData->InitializeSpeed();
}

void 
IRISApplication
::UpdateIRISWithSnapImageData()
{
  assert(m_SNAPImageData != NULL);

  // Get pointers to the source and destination images
  typedef LevelSetImageWrapper::ImageType SourceImageType;
  typedef LabelImageWrapper::ImageType TargetImageType;
  
  SourceImageType *source = m_SNAPImageData->GetSnake()->GetImage();
  TargetImageType *target = m_IRISImageData->GetSegmentation()->GetImage();

  // Construct are region of interest into which the result will be pasted
  SourceImageType::RegionType roi = m_GlobalState->GetSegmentationROI();

  // Create iterators for copying from one to the other
  typedef ImageRegionConstIterator<SourceImageType> SourceIteratorType;
  typedef ImageRegionIterator<TargetImageType> TargetIteratorType;
  SourceIteratorType itSource(source,source->GetLargestPossibleRegion());
  TargetIteratorType itTarget(target,roi);

  // We need this information for merging the images 
  LabelType clrDrawing = m_GlobalState->GetDrawingColorLabel();
  LabelType clrOverwrite = m_GlobalState->GetOverWriteColorLabel();
  CoverageModeType mode = m_GlobalState->GetCoverageMode();

  // Figure out which color draws and which color is clear
  unsigned int iClear = m_GlobalState->GetPolygonInvert() ? 1 : 0;

  // Construct a merge table that contains an output intensity for every 
  // possible combination of two input intensities (note that snap image only
  // has two possible intensities
  LabelType mergeTable[2][MAX_COLOR_LABELS];
  for(unsigned i=0;i<MAX_COLOR_LABELS;i++)
    {
    // Whe the SNAP image is clear, IRIS passes through to the output
    // except for the IRIS voxels of the drawing color, which get cleared out
    mergeTable[iClear][i] = (i!=clrDrawing) ? i : 0;

    // Assign the output intensity based on the current drawing mode
    if ((mode == PAINT_OVER_ALL ||
        (mode == PAINT_OVER_ONE && i == clrOverwrite) ||
        (mode == PAINT_OVER_COLORS &&
         m_IRISImageData->GetColorLabel(i).IsVisible())))
      {
      mergeTable[1-iClear][i] = clrDrawing;
      }
    }

  // Go through both iterators, copy the new over the old
  itSource.GoToBegin();
  itTarget.GoToBegin();
  while(!itSource.IsAtEnd())
    {
    // Get the two voxels
    LabelType &voxIRIS = itTarget.Value();    
    float voxSNAP = itSource.Value();

    // Check that we're ok (debug mode only)
    assert(!itTarget.IsAtEnd());

    // Perform the merge
    voxIRIS = mergeTable[voxSNAP <= 0 ? 1 : 0][voxIRIS];

    // Iterate
    ++itSource;
    ++itTarget;
    }

  // The target has been modified
  target->Modified();
}

void 
IRISApplication
::ReleaseSNAPImageData() 
{
  assert(m_SNAPImageData && m_CurrentImageData != m_SNAPImageData);

  delete m_SNAPImageData;
  m_SNAPImageData = NULL;
}

void 
IRISApplication
::SetCurrentImageDataToIRIS() 
{
  assert(m_IRISImageData);
  m_CurrentImageData = m_IRISImageData;
}

void IRISApplication
::SetCurrentImageDataToSNAP() 
{
  assert(m_SNAPImageData);
  m_CurrentImageData = m_SNAPImageData;
}


