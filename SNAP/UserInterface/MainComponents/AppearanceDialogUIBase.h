/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    AppearanceDialogUIBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __AppearanceDialogUIBase_h_
#define __AppearanceDialogUIBase_h_

class AppearanceDialogUIBase
{
public:
    virtual ~AppearanceDialogUIBase() {}
  virtual void OnUIElementUpdate() = 0;
  virtual void OnUIElementSelection(int value) = 0;
  virtual void OnSliceAnatomyOptionsChange(unsigned int order) = 0;
  virtual void OnCloseAction() = 0;

  virtual void OnSliceDisplayApplyAction() = 0;
  virtual void OnSliceDisplayResetAction() = 0;
  virtual void OnScreenLayoutApplyAction() = 0;
  virtual void OnScreenLayoutResetAction() = 0;
  virtual void On3DRenderingApplyAction() = 0;
  virtual void On3DRenderingResetAction() = 0;
  virtual void OnElementAppearanceResetAllAction() = 0;
  virtual void OnElementAppearanceResetAction() = 0;
  virtual void OnElementAppearanceApplyAction() = 0;
};

#endif

