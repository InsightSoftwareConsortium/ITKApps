/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    RestoreSettingsDialogBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __RestoreSettingsDialogBase_h_
#define __RestoreSettingsDialogBase_h_

class RestoreSettingsDialogBase {
public:
  virtual void OnRestoreSettingsAction() = 0;
  virtual void OnDoNotRestoreSettingsAction() = 0;
  virtual void OnCancelAction() = 0;
};

#endif // __RestoreSettingsDialogBase_h_
