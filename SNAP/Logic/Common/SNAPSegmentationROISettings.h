/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPSegmentationROISettings.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SNAPSegmentationROISettings_h_
#define __SNAPSegmentationROISettings_h_

// #include "itkImageRegion.h"
#include "SNAPCommon.h"
#include "itkImageRegion.h"

/** 
 * Settings describing the region of interest selected for the SnAP 
 * segmentation and the resampling associated with it */
class SNAPSegmentationROISettings
{
public:
  // List of available interpolation methods
  enum InterpolationMethod {
    NEAREST_NEIGHBOR, TRILINEAR, TRICUBIC
  };

  SNAPSegmentationROISettings() {}
  virtual ~SNAPSegmentationROISettings() {}

  // Get the region of interest, in the main IRIS image
  irisSetMacro(ROI,itk::ImageRegion<3>);
  
  // Set the region of interest, in the main IRIS image
  irisGetMacro(ROI,itk::ImageRegion<3>);

  // Get whether or not resampling is desired for the region
  irisSetMacro(ResampleFlag,bool);
  
  // Set whether or not resampling is desired for the region
  irisGetMacro(ResampleFlag,bool);

  // Get the scaling factor for each dimension
  irisSetMacro(VoxelScale,Vector3d);
  
  // Set the scaling factor for each dimension
  irisGetMacro(VoxelScale,Vector3d);

  // Get the interpolation method used
  irisSetMacro(InterpolationMethod,InterpolationMethod);
  
  // Set the interpolation method used
  irisGetMacro(InterpolationMethod,InterpolationMethod);

private:
  // The region of interest, in the main IRIS image
  itk::ImageRegion<3> m_ROI;
  
  // Whether or not resampling is desired for the region
  bool m_ResampleFlag;
  
  // The scaling factor for each dimension
  Vector3d m_VoxelScale;
  
  // The interpolation method used
  InterpolationMethod m_InterpolationMethod;
};


#endif // __SNAPSegmentationROISettings_h_
