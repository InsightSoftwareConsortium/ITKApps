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
    { 1, 0, 1, 1, 0, 1, 1 },    // Crosshairs
    { 1, 0, 0, 0, 1, 1, 1 },    // Markers
    { 1, 1, 1, 1, 0, 0, 1 },    // ROI
    { 1, 0, 0, 0, 0, 0, 0 },    // Slice Background
    { 1, 0, 0, 0, 0, 0, 0 },    // 3D Background
    { 1, 1, 1, 1, 0, 1, 1 },    // Zoom thumbnail
    { 1, 0, 1, 1, 0, 1, 1 },    // 3D Crosshairs
    { 1, 0, 1, 1, 0, 1, 1 },    // Thumbnail Crosshairs
    };

SNAPAppearanceSettings::Element 
SNAPAppearanceSettings
::m_DefaultElementSettings[SNAPAppearanceSettings::ELEMENT_COUNT];

void 
SNAPAppearanceSettings
::InitializeDefaultSettings()
{
  // Crosshairs
  Element &elt = m_DefaultElementSettings[CROSSHAIRS];
  elt.NormalColor = Vector3d(0.3, 0.3, 1.0);
  elt.ActiveColor = Vector3d(0.0, 0.0, 0.0);
  elt.LineThickness = 1.0;
  elt.DashSpacing = 1.0;
  elt.FontSize = 0.0;
  elt.Visible = true;
  elt.AlphaBlending = false;

  // Markers
  elt = m_DefaultElementSettings[MARKERS];
  elt.NormalColor = Vector3d(1.0, 0.75, 0.0);
  elt.ActiveColor = Vector3d(0.0, 0.0, 0.0);
  elt.LineThickness = 0.0;
  elt.DashSpacing = 0.0;
  elt.FontSize = 16.0;
  elt.Visible = true;
  elt.AlphaBlending = false;

  // ROI
  elt = m_DefaultElementSettings[ROI_BOX];
  elt.NormalColor = Vector3d(1.0, 0.0, 0.2);
  elt.ActiveColor = Vector3d(1.0, 1.0, 0.2);
  elt.LineThickness = 1.0;
  elt.DashSpacing = 3.0;
  elt.FontSize = 0.0;
  elt.Visible = true;
  elt.AlphaBlending = false;

  // Slice background
  elt = m_DefaultElementSettings[BACKGROUND_3D];
  elt.NormalColor = Vector3d(0.0, 0.0, 0.0);
  elt.ActiveColor = Vector3d(0.0, 0.0, 0.0);
  elt.LineThickness = 0.0;
  elt.DashSpacing = 0.0;
  elt.FontSize = 0.0;
  elt.Visible = true;
  elt.AlphaBlending = false;

  // 3D Window background
  elt = m_DefaultElementSettings[BACKGROUND_3D];
  elt.NormalColor = Vector3d(0.0, 0.0, 0.0);
  elt.ActiveColor = Vector3d(0.0, 0.0, 0.0);
  elt.LineThickness = 0.0;
  elt.DashSpacing = 0.0;
  elt.FontSize = 0.0;
  elt.Visible = true;
  elt.AlphaBlending = false;

  // Zoom thumbail
  elt = m_DefaultElementSettings[ZOOM_THUMBNAIL];
  elt.NormalColor = Vector3d(1.0, 1.0, 0.0);
  elt.ActiveColor = Vector3d(1.0, 1.0, 1.0);
  elt.LineThickness = 1.0;
  elt.DashSpacing = 0.0;
  elt.FontSize = 0.0;
  elt.Visible = true;
  elt.AlphaBlending = false;

  // 3D crosshairs
  elt = m_DefaultElementSettings[CROSSHAIRS_3D];
  elt.NormalColor = Vector3d(0.3, 0.3, 1.0);
  elt.ActiveColor = Vector3d(0.0, 0.0, 0.0);
  elt.LineThickness = 1.0;
  elt.DashSpacing = 1.0;
  elt.FontSize = 0.0;
  elt.Visible = true;
  elt.AlphaBlending = true;

  // Thumbnail crosshairs
  elt = m_DefaultElementSettings[CROSSHAIRS_THUMB];
  elt.NormalColor = Vector3d(0.3, 0.3, 1.0);
  elt.ActiveColor = Vector3d(0.0, 0.0, 0.0);
  elt.LineThickness = 1.0;
  elt.DashSpacing = 1.0;
  elt.FontSize = 0.0;
  elt.Visible = true;
  elt.AlphaBlending = false;
}

const char *
SNAPAppearanceSettings
::m_ElementNames[SNAPAppearanceSettings::ELEMENT_COUNT] = 
  { "CROSSHAIRS", "MARKERS", "ROI_BOX", "BACKGROUND_2D", "BACKGROUND_3D", 
    "ZOOM_THUMBNAIL", "CROSSHAIRS_3D", "CROSSHAIRS_THUMB" };

SNAPAppearanceSettings
::SNAPAppearanceSettings()
{
  // Initialize the default settings
  InitializeDefaultSettings();

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
    elt.AlphaBlending = f["AlphaBlending"][def.AlphaBlending];
    elt.Visible = f["Visible"][def.Visible];
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
    f["AlphaBlending"] << elt.AlphaBlending;
    f["Visible"] << elt.Visible;
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
    if( elt.AlphaBlending )
      {
      glEnable(GL_BLEND);
      glEnable(GL_LINE_SMOOTH);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
    glLineWidth( elt.LineThickness );
    }
  if(applyStipple && elt.DashSpacing != 0)
    {
    // Set the line thickness and stipple
    glEnable(GL_LINE_STIPPLE);
    glLineStipple( elt.DashSpacing, 0x9999 ); // 0011 0011 0011 0011  // 1001 1001 1001 1001
    }
}


