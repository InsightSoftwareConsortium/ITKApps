/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    GreyImageIOWizardLogic.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __GreyImageIOWizardLogic_h_
#define __GreyImageIOWizardLogic_h_

#include "IRISTypes.h"
#include "RestrictedImageIOWizardLogic.h"

/**
 * \class GreyImageIOWizardLogic
 * \brief A concrete instantiation of a wizard for loading and saving 
 * grey images
 */
class GreyImageIOWizardLogic : 
  public ImageIOWizardLogic<GreyType> {};

#endif

