/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ResizeRegionDialogLogic.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ResizeRegionDialogLogic_h_
#define __ResizeRegionDialogLogic_h_

#include "ResizeRegionDialog.h"
#include "SNAPSegmentationROISettings.h"

class ResizeRegionDialogLogic : public ResizeRegionDialog {
public:
  // A list of scaling choices
  static const int NumberOfScaleChoices;
  static const int ScaleChoices[10][2];

  // Resampling method
  enum ResamplingMethod {
    NearestNeighbor, Linear
  };
  
  // Get the new spacing
  double GetSpacing(unsigned int dim) {
    return m_InSize[dim]->value();
  }

  // Get the selected resampling method
  ResamplingMethod GetResamplingMethod() {
    return (ResamplingMethod)(NearestNeighbor + m_InInterpolation->value());
  }

  // Callback functions
  void MakeWindow();
  bool DisplayDialog(const double *voxelSpacing,
                     SNAPSegmentationROISettings &targetROI);
  void OnVoxelSizeChange();
  void OnVoxelScaleChange();
  void OnOkAction();
  void OnCancelAction();

private:
  bool m_Accept;
};

#endif // __ResizeRegionDialogLogic_h_
