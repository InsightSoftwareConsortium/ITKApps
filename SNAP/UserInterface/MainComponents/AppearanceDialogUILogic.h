/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    AppearanceDialogUILogic.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __AppearanceDialogUILogic_h_
#define __AppearanceDialogUILogic_h_

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "AppearanceDialogUI.h"

class UserInterfaceLogic;
class SNAPAppearanceSettings;
class GlobalState;

class AppearanceDialogUILogic : public AppearanceDialogUI
{
public:
  AppearanceDialogUILogic();
  virtual ~AppearanceDialogUILogic();

  void Register( UserInterfaceLogic *parent );

  /* Show the dialog */
  void ShowDialog();
  
  // Close callbacks
  void OnCloseAction() ;

  // General slice options
  void OnSliceDisplayApplyAction();
  void OnSliceDisplayResetAction();

  // View arrangement callbacks
  void OnScreenLayoutApplyAction();
  void OnScreenLayoutResetAction();
  void OnSliceAnatomyOptionsChange(unsigned int order) ;

  // 3D Rendering callbacks
  void On3DRenderingApplyAction();
  void On3DRenderingResetAction();

  // Element appearance callbacks
  void OnUIElementUpdate() ;
  void OnUIElementSelection(int value) ;
  void OnElementAppearanceResetAllAction();
  void OnElementAppearanceResetAction();
  void OnElementAppearanceApplyAction();

private:
  // Referece to the parent
  UserInterfaceLogic *m_Parent;

  // Pointer to the appearance settings, from parent.
  SNAPAppearanceSettings *m_Appearance;

  // Default appearance settings
  SNAPAppearanceSettings *m_DefaultAppearance;

  // global state pointer
  GlobalState *m_GlobalState;

  // Fill some global options
  void FillSliceLayoutOptions();
  void FillRenderingOptions();
  void FillAppearanceSettings();

  // Apply the global options
  void ApplyRenderingOptions();
  void ApplySliceLayoutOptions();
  void ApplyAppearanceSettings();

  // A mapping from SNAP appearance elements to menu indices
  const static int m_MapMenuToElementIndex[];
};

#endif
