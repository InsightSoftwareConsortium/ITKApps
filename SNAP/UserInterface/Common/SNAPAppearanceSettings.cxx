/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPAppearanceSettings.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "SNAPAppearanceSettings.h"
#include "Registry.h"

SNAPAppearanceSettings
::SNAPAppearanceSettings()
{
  m_FlagLinkedZoomByDefault = false;
  m_ZoomThumbnailMaximumSize = 160;
  m_ZoomThumbnailSizeInPercent = 30.0;
  m_FlagDisplayZoomThumbnail = true;
}

void
SNAPAppearanceSettings
::LoadFromRegistry(Registry &r)
{
  m_FlagDisplayZoomThumbnail =
    r["FlagDisplayZoomThumbnail"][m_FlagDisplayZoomThumbnail];

  m_FlagLinkedZoomByDefault = 
    r["FlagLinkedZoomByDefault"][m_FlagLinkedZoomByDefault];

  m_ZoomThumbnailSizeInPercent = 
    r["ZoomThumbnailSizeInPercent"][m_ZoomThumbnailSizeInPercent];

  m_ZoomThumbnailMaximumSize = 
    r["ZoomThumbnailMaximumSize"][m_ZoomThumbnailMaximumSize];
}

void
SNAPAppearanceSettings
::SaveToRegistry(Registry &r)
{
  r["FlagDisplayZoomThumbnail"] << m_FlagDisplayZoomThumbnail;
  r["FlagLinkedZoomByDefault"] << m_FlagLinkedZoomByDefault;
  r["ZoomThumbnailSizeInPercent"] << m_ZoomThumbnailSizeInPercent;
  r["ZoomThumbnailMaximumSize"] << m_ZoomThumbnailMaximumSize;
}


