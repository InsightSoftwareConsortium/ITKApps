/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    EdgePreprocessingSettings.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __EdgePreprocessingSettings_h_
#define __EdgePreprocessingSettings_h_

#include "SNAPCommon.h"

// Forward references
class GreyImageWrapper;

/**
 * This class encapsulates the thresholding settings used by the program.
 * for In/Out snake initialization
 */
class EdgePreprocessingSettings
{
public:
  irisGetMacro(GaussianBlurScale,float);
  irisSetMacro(GaussianBlurScale,float);

  irisGetMacro(RemappingSteepness,float);
  irisSetMacro(RemappingSteepness,float);

  irisGetMacro(RemappingExponent,float);
  irisSetMacro(RemappingExponent,float);
  
  /** Compare two sets of settings */
  bool operator == (const EdgePreprocessingSettings &other) const;

  /**
   * Create a default instance of the settings based on an image wrapper
   */
  static EdgePreprocessingSettings MakeDefaultSettings();

  // Constructor
  EdgePreprocessingSettings();
  // Destructor
  virtual ~EdgePreprocessingSettings();

private:
  float m_GaussianBlurScale;
  float m_RemappingSteepness;
  float m_RemappingExponent;
};

#endif // __EdgePreprocessingSettings_h_

