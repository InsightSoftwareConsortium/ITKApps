/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPSegmentationROISettings.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "SNAPSegmentationROISettings.h"

bool 
SNAPSegmentationROISettings
::TransformImageVoxelToROIVoxel(const Vector3ui &vImage, Vector3ui &vROI)
{
  itk::Index<3> idx = to_itkIndex(vImage);
  if(m_ROI.IsInside(idx)) 
  {
    for(unsigned int i = 0; i < 3; i++)
    {
      unsigned int p = vImage[i] - m_ROI.GetIndex()[i];
      vROI[i] = (unsigned int) (p / m_VoxelScale[i]);
    }
    return true;
  }
  else return false;
}

void
SNAPSegmentationROISettings
::TransformROIVoxelToImageVoxel(const Vector3ui &vROI, Vector3ui &vImage)
{
  for(unsigned int i = 0; i < 3; i++)
  {
    unsigned int p = (unsigned int) (vROI[i] * m_VoxelScale[i]);
    vImage[i] = p + m_ROI.GetIndex()[i];
  }
}
