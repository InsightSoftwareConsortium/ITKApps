/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPAppearanceSettings.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SNAPAppearanceSettings_h_
#define __SNAPAppearanceSettings_h_

// Include the common items from the logic part of SNAP
#include "SNAPCommonUI.h"

class Registry;

/**
 * \class SNAPAppearanceSettings
 * \brief User interface settings that the user can configure
 */
class SNAPAppearanceSettings 
{
public:
  SNAPAppearanceSettings();

  void LoadFromRegistry(Registry &registry);
  void SaveToRegistry(Registry &registry);
  
  irisGetMacro(FlagDisplayZoomThumbnail, bool); 
  irisSetMacro(FlagDisplayZoomThumbnail, bool);

  irisGetMacro(FlagLinkedZoomByDefault, bool); 
  irisSetMacro(FlagLinkedZoomByDefault, bool);

  irisGetMacro(ZoomThumbnailSizeInPercent, double); 
  irisSetMacro(ZoomThumbnailSizeInPercent, double);

  irisGetMacro(ZoomThumbnailMaximumSize, int); 
  irisSetMacro(ZoomThumbnailMaximumSize, int);

private:
  bool m_FlagDisplayZoomThumbnail;
  bool m_FlagLinkedZoomByDefault;
  double m_ZoomThumbnailSizeInPercent;
  int m_ZoomThumbnailMaximumSize;
};


#endif // __SNAPAppearanceSettings_h_
