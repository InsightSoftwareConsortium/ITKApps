/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SegmentationImageIOWizardLogic.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SegmentationImageIOWizardLogic_h_
#define __SegmentationImageIOWizardLogic_h_

#include "IRISTypes.h"
#include "RestrictedImageIOWizardLogic.h"

/**
 * \class SegmentationImageIOWizardLogic
 * \brief A concrete instantiation of a wizard for loading and saving 
 * grey images
 */
class SegmentationImageIOWizardLogic : 
  public RestrictedImageIOWizardLogic<LabelType> {};

#endif

