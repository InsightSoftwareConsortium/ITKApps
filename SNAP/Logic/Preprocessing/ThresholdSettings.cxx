/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ThresholdSettings.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "ThresholdSettings.h"
#include "GreyImageWrapper.h"

bool 
ThresholdSettings
::operator == (const ThresholdSettings &other) const
{
  return memcmp(this,&other,sizeof(ThresholdSettings)) == 0;
}

ThresholdSettings
ThresholdSettings
::MakeDefaultSettings(GreyImageWrapper *wrapper)
{
  // Use the min and the max of the wrapper
  int iMin = wrapper->GetImageMin();
  int iMax = wrapper->GetImageMax();
  
  ThresholdSettings settings;
  settings.m_LowerThresholdEnabled = true;
  settings.m_UpperThresholdEnabled = true;  
  settings.m_LowerThreshold = iMin + (iMax-iMin) / 3;
  settings.m_UpperThreshold = iMin + 2 * (iMax-iMin) / 3;
  
  settings.m_Smoothness = (iMax-iMin) / 6;
  // settings.m_Smoothness = 0.5;
  settings.m_Smoothness = 3;

  return settings;
}

ThresholdSettings
ThresholdSettings
::MakeDefaultSettingsWithoutImage()
{
  ThresholdSettings settings;
  settings.m_LowerThresholdEnabled = true;
  settings.m_UpperThresholdEnabled = true;  
  settings.m_LowerThreshold = 40;
  settings.m_UpperThreshold = 80; 
  settings.m_Smoothness = 3;

  return settings;
}

ThresholdSettings
::ThresholdSettings()
{
  m_LowerThresholdEnabled = true;
  m_UpperThresholdEnabled = true;  
  m_LowerThreshold = 0;
  m_UpperThreshold = 0; 
  m_Smoothness = 1;
}
