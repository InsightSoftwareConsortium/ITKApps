/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    GUITestPadBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __GUITestPadBase_h_
#define __GUITestPadBase_h_

class GUITestPadBase {
public:
  virtual void OnTestInputWizard() = 0;
  virtual void OnTestIntensityCurves() = 0;
  virtual void OnTestGaussianPerformance() = 0;

  virtual void OnSingleImageBrowseAction() = 0;
  virtual void OnSingleImageTestAction() = 0;
};





#endif
