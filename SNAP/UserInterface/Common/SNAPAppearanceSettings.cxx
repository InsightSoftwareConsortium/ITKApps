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
#include "FL/gl.h"

const int 
SNAPAppearanceSettings
::m_Applicable[SNAPAppearanceSettings::ELEMENT_COUNT][SNAPAppearanceSettings::FEATURE_COUNT] = {
    { 1, 0, 1, 1, 0 },
    { 1, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 0 },
    { 1, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 0 },
    { 1, 1, 1, 1, 0 }};  

const 
SNAPAppearanceSettings::Element
SNAPAppearanceSettings
::m_DefaultElementSettings[SNAPAppearanceSettings::ELEMENT_COUNT] = 
{
    { Vector3d(0.5, 0.5, 1.0), Vector3d(0.0, 0.0, 0.0), 1.0, 3.0, 0.0 },
    { Vector3d(1.0, 1.0, 0.2), Vector3d(0.0, 0.0, 0.0), 0.0, 0.0, 12.0 },
    { Vector3d(1.0, 0.0, 0.2), Vector3d(1.0, 1.0, 0.2), 1.0, 3.0, 0.0 },
    { Vector3d(0.0, 0.0, 0.0), Vector3d(0.0, 0.0, 0.0), 0.0, 0.0, 0.0 },
    { Vector3d(0.0, 0.0, 0.0), Vector3d(0.0, 0.0, 0.0), 0.0, 0.0, 0.0 },
    { Vector3d(1.0, 1.0, 0.0), Vector3d(0.0, 0.0, 0.0), 0.0, 0.0, 0.0 },
};  

const char *
SNAPAppearanceSettings
::m_ElementNames[SNAPAppearanceSettings::ELEMENT_COUNT] = 
  { "CROSSHAIRS", "MARKERS", "ROI_BOX", "BACKGROUND_3D", "BACKGROUND_2D", 
    "ZOOM_THUMBNAIL" };

SNAPAppearanceSettings
::SNAPAppearanceSettings()
{
  // Set the common flags
  m_FlagLinkedZoomByDefault = false;
  m_ZoomThumbnailMaximumSize = 160;
  m_ZoomThumbnailSizeInPercent = 30.0;
  m_FlagDisplayZoomThumbnail = true;

  // Set the UI elements to their default values
  for(unsigned int iElement = 0; iElement < ELEMENT_COUNT; iElement++)
    m_Elements[iElement] = m_DefaultElementSettings[iElement];
}

void
SNAPAppearanceSettings
::LoadFromRegistry(Registry &r)
{
  // Load the flags and settings
  m_FlagDisplayZoomThumbnail =
    r["FlagDisplayZoomThumbnail"][m_FlagDisplayZoomThumbnail];

  m_FlagLinkedZoomByDefault = 
    r["FlagLinkedZoomByDefault"][m_FlagLinkedZoomByDefault];

  m_ZoomThumbnailSizeInPercent = 
    r["ZoomThumbnailSizeInPercent"][m_ZoomThumbnailSizeInPercent];

  m_ZoomThumbnailMaximumSize = 
    r["ZoomThumbnailMaximumSize"][m_ZoomThumbnailMaximumSize];

  // Load the user interface elements
  for(unsigned int iElement = 0; iElement < ELEMENT_COUNT; iElement++)
    {
    // Create a folder to hold the element
    Registry& f = r.Folder( 
      r.Key("UserInterfaceElement[%s]", m_ElementNames[iElement]) );

    // Get the default element
    const Element &def = m_DefaultElementSettings[iElement];
    Element &elt = m_Elements[iElement];
    
    // Store the element in the folder
    elt.NormalColor = f["NormalColor"][def.NormalColor];
    elt.ActiveColor = f["ActiveColor"][def.ActiveColor];
    elt.LineThickness = f["LineThickness"][def.LineThickness];
    elt.DashSpacing = f["DashSpacing"][def.DashSpacing];
    elt.FontSize = f["FontSize"][def.FontSize];
    }
}

void
SNAPAppearanceSettings
::SaveToRegistry(Registry &r)
{
  // Save the flags and settings
  r["FlagDisplayZoomThumbnail"] << m_FlagDisplayZoomThumbnail;
  r["FlagLinkedZoomByDefault"] << m_FlagLinkedZoomByDefault;
  r["ZoomThumbnailSizeInPercent"] << m_ZoomThumbnailSizeInPercent;
  r["ZoomThumbnailMaximumSize"] << m_ZoomThumbnailMaximumSize;

  // Save each of the screen elements
  for(unsigned int iElement = 0; iElement < ELEMENT_COUNT; iElement++)
    {
    // Create a folder to hold the element
    Registry& f = r.Folder( 
      r.Key("UserInterfaceElement[%s]", m_ElementNames[iElement]) );

    // Get the default element
    Element &elt = m_Elements[iElement];
    
    // Store the element in the folder
    f["NormalColor"] << elt.NormalColor;
    f["ActiveColor"] << elt.ActiveColor;
    f["LineThickness"] << elt.LineThickness;
    f["DashSpacing"] << elt.DashSpacing;
    f["FontSize"] << elt.FontSize;
    }
}

void 
SNAPAppearanceSettings
::ApplyUIElementLineSettings(const Element &elt, bool applyThickness, bool applyStipple)
{
  // Apply the thickness properties
  if(applyThickness)
    {
    // Choose whether to use blending or not
    if( elt.LineThickness != floor(elt.LineThickness) )
      {
      glEnable(GL_BLEND);
      glEnable(GL_LINE_SMOOTH);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
    glLineWidth( elt.LineThickness );
    }
  if(applyStipple)
    {
    // Set the line thickness and stipple
    glEnable(GL_LINE_STIPPLE);
    glLineStipple( elt.DashSpacing, 0xaaaa );
    }
}


