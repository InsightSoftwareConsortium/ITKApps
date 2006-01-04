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

Vector3ui 
SNAPSegmentationROISettings
::UpdateCursorPosition(const Vector3ui &cursor)
{
  itk::Index<3> idx = to_itkIndex(cursor);
  if(m_ROI.IsInside(idx)) 
  {
    Vector3ui pos1, pos2;
    for(unsigned int i = 0; i < 3; i++)
    {
      pos1[i] = cursor[i] - m_ROI.GetIndex()[i];
      pos2[i] = (unsigned int) (pos1[i] / m_VoxelScale[i]);
    }
    return pos2;
  }
  else
  {
    Vector3ui pos1, pos2;
    for(unsigned int i = 0; i < 3; i++)
    {
      pos1[i] = m_ROI.GetSize()[i] / 2;
      pos2[i] = (unsigned int) (pos1[i] / m_VoxelScale[i]);
    }
    return pos2;
  }
}
