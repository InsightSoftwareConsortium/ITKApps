/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    RestrictedImageIOWizardLogic.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#include "RestrictedImageIOWizardLogic.h"

template<class TPixel>
bool
RestrictedImageIOWizardLogic<TPixel>
::DisplayInputWizard(const char *file)
{
  // Make sure there is a grey image as a reference
  assert(m_GreyImage);

  // Get the size and spacing of the grey image
  SizeType requiredSize = m_GreyImage->GetBufferedRegion().GetSize();

  const double *requiredSpacing = m_GreyImage->GetSpacing().GetDataPointer();

  // Prepare the header page of the wizard UI
  m_InHeaderPageDimX->value(requiredSize[0]);
  m_InHeaderPageDimY->value(requiredSize[1]);
  m_InHeaderPageDimZ->value(requiredSize[2]);
  m_InHeaderPageSpacingX->value(requiredSpacing[0]);
  m_InHeaderPageSpacingY->value(requiredSpacing[1]);
  m_InHeaderPageSpacingZ->value(requiredSpacing[2]);

  // Disable the orientation page
  m_PageOrientation->deactivate();

  // Call the parent's method
  return Superclass::DisplayInputWizard(file);    
}

template<class TPixel>
bool
RestrictedImageIOWizardLogic<TPixel>
::CheckImageValidity()
{
  SizeType requiredSize = m_GreyImage->GetBufferedRegion().GetSize();
  SizeType loadedSize = m_Image->GetBufferedRegion().GetSize();

  // Check whether or not the image size matches the 'forced' image size
  if(!(requiredSize == loadedSize))
    {
    // Bark at the user
    fl_alert(
      "The size of the image you are attempting to load does not match "
      "the size of the 'grey' image already loaded.");

    return false;
    }
  else
    return true;
}
