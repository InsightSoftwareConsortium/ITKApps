/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ThresholdSettings.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ThresholdSettings_h_
#define __ThresholdSettings_h_

#include "SNAPCommon.h"

// Forward references
class GreyImageWrapper;

/**
 * This class encapsulates the thresholding settings used by the program.
 * for In/Out snake initialization
 */
class ThresholdSettings
{
public:
    virtual ~ThresholdSettings() { /*To avoid compiler warning.*/ }
  irisGetMacro(LowerThreshold, int);
  irisSetMacro(LowerThreshold, int);

  irisGetMacro(UpperThreshold, int);
  irisSetMacro(UpperThreshold, int);

  irisGetMacro(Smoothness,float);
  irisSetMacro(Smoothness,float);

  irisIsMacro(LowerThresholdEnabled);
  irisSetMacro(LowerThresholdEnabled,bool);

  irisIsMacro(UpperThresholdEnabled);
  irisSetMacro(UpperThresholdEnabled,bool);

  /** Compare two sets of settings */
  bool operator == (const ThresholdSettings &other) const;

  /**
   * Create a default instance of the settings based on an image wrapper
   */
  static ThresholdSettings MakeDefaultSettings(GreyImageWrapper *wrapper);

  /** This will create a slightly less useful settings object with thresholds
   * at 40 and 100.  Before using them, make sure that the image is in range
   * of 40 and 100 */
  static ThresholdSettings MakeDefaultSettingsWithoutImage();

  // Constructor: creates dummy settings
  ThresholdSettings();

private:
  int m_LowerThreshold;
  int m_UpperThreshold;
  float m_Smoothness;
  
  bool m_UpperThresholdEnabled;
  bool m_LowerThresholdEnabled;
};

#endif // __ThresholdSettings_h_

