/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SegmentationImageIOWizardLogic.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
// Borland compiler is very lazy so we need to instantiate the template
//  by hand 
#if defined(__BORLANDC__)
#include "SNAPBorlandDummyTypes.h"
#endif

#include "SegmentationImageIOWizardLogic.h"
#include "ImageIOWizardLogic.txx"
#include "RestrictedImageIOWizardLogic.txx"

template class ImageIOWizardLogic<LabelType>;
template class RestrictedImageIOWizardLogic<LabelType>;
