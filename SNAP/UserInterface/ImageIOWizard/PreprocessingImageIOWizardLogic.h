/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    PreprocessingImageIOWizardLogic.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __PreprocessingImageIOWizardLogic_h_
#define __PreprocessingImageIOWizardLogic_h_

#include "IRISTypes.h"
#include "RestrictedImageIOWizardLogic.h"

/**
 * \class PreprocessingImageIOWizardLogic
 * \brief A concrete instantiation of a wizard for loading and saving 
 * grey images
 */
class PreprocessingImageIOWizardLogic : 
  public RestrictedImageIOWizardLogic<float> 
{
public:
  typedef ImageIOWizardLogic<float>::FileFormat FileFormat;

  /** Not all file types support saving and loading floating 
   * point images. This method returns the allowed types */
  virtual bool CanLoadFileFormat(FileFormat type) const
  {
    return (type != FORMAT_GIPL);
  }

  /** Not all file types support saving and loading floating 
   * point images. This method returns the allowed types */
  virtual bool CanSaveFileFormat(FileFormat type) const
  {
    return CanLoadFileFormat(type);
  }

};

#endif

