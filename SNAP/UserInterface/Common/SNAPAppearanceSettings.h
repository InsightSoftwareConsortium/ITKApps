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
  /**
   * A structure that describes the appearance of a screen element 
   */
  struct Element 
    {
    Vector3d NormalColor, ActiveColor;
    double LineThickness, DashSpacing;
    int FontSize;
    };

  /** An enumeration of available screen elements */
  enum UIElements 
    { 
    CROSSHAIRS = 0, MARKERS, ROI_BOX, BACKGROUND_3D, BACKGROUND_2D, 
    ZOOM_THUMBNAIL, ELEMENT_COUNT
    };  

  /** An enumeration of the fields that an element may possess */
  enum UIElementFeatures
    {
    NORMAL_COLOR = 0, ACTIVE_COLOR, LINE_THICKNESS, DASH_SPACING, 
    FONT_SIZE, FEATURE_COUNT
    };

  SNAPAppearanceSettings();

  void LoadFromRegistry(Registry &registry);
  void SaveToRegistry(Registry &registry);

  // Access a user interface element
  Element &GetUIElement(unsigned int iElement)
    { return m_Elements[iElement]; }

  // Set a user interface element
  void SetUIElement(unsigned int iElement, const Element &value)
    { m_Elements[iElement] = value; }

  // Check whether the feature is applicable to an element
  static bool IsFeatureApplicable(unsigned int iElement, unsigned int iFeature)
    { return m_Applicable[iElement][iFeature] != 0; }

  // Apply the GL settings associated with an appearance element
  static void ApplyUIElementLineSettings(const Element &elt, 
    bool applyThickness = true, bool applyStipple = true);
  
  irisGetMacro(FlagDisplayZoomThumbnail, bool); 
  irisSetMacro(FlagDisplayZoomThumbnail, bool);

  irisGetMacro(FlagLinkedZoomByDefault, bool); 
  irisSetMacro(FlagLinkedZoomByDefault, bool);

  irisGetMacro(ZoomThumbnailSizeInPercent, double); 
  irisSetMacro(ZoomThumbnailSizeInPercent, double);

  irisGetMacro(ZoomThumbnailMaximumSize, int); 
  irisSetMacro(ZoomThumbnailMaximumSize, int);

private:
  // Global settings
  bool m_FlagDisplayZoomThumbnail;
  bool m_FlagLinkedZoomByDefault;
  double m_ZoomThumbnailSizeInPercent;
  int m_ZoomThumbnailMaximumSize;

  /** An array of user interface elements */
  Element m_Elements[ELEMENT_COUNT];
    
  /** A list of flags that indicate for each element, whether each feature is 
   * applicable or not */
  static const int m_Applicable[ELEMENT_COUNT][FEATURE_COUNT];

  /** The set of default values for each element */
  static const Element m_DefaultElementSettings[ELEMENT_COUNT];

  /** Text constants for the elements */
  static const char *m_ElementNames[ELEMENT_COUNT];


};


#endif // __SNAPAppearanceSettings_h_
