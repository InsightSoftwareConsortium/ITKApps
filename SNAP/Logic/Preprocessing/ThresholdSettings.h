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

#include "IRISTypes.h"

// Forward references
class GreyImageWrapper;

/**
 * This class encapsulates the thresholding settings used by the program.
 * for In/Out snake initialization
 */
class ThresholdSettings
{
public:
  irisGetMacro(LowerThreshold,unsigned int);
  irisSetMacro(LowerThreshold,unsigned int);

  irisGetMacro(UpperThreshold,unsigned int);
  irisSetMacro(UpperThreshold,unsigned int);

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

private:
  unsigned int m_LowerThreshold;
  unsigned int m_UpperThreshold;
  float m_Smoothness;
  
  bool m_UpperThresholdEnabled;
  bool m_LowerThresholdEnabled;
};

#endif // __ThresholdSettings_h_

