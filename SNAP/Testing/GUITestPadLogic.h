/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    GUITestPadLogic.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __GUITestPadLogic_h_
#define __GUITestPadLogic_h_

#include <iostream>
#include "GUITestPad.h"

class GreyImageWrapper;

class GUITestPadLogic : public GUITestPad {
public:
  void OnTestInputWizard();
  void OnTestIntensityCurves();
  void OnTestGaussianPerformance();
  
  void OnSingleImageBrowseAction();
  void OnSingleImageTestAction();
protected:
  bool LoadShortImage(GreyImageWrapper *wrapper);
};





#endif
