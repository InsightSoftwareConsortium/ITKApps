/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SpeedColorMap.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SpeedColorMap_h_
#define __SpeedColorMap_h_

#include "itkRGBAPixel.h"
#include "GlobalState.h"

/**
 * \class SpeedColorMap
 * \brief A very simple functor used to map intensities from 
 * the range (-1,1) to RGB color space.
 */
class SpeedColorMap 
{
public:
  typedef itk::RGBAPixel<unsigned char> OutputType;

  /** Constructor, sets default color map */
  SpeedColorMap();

  /** Mapping operator, maps range [-1, 1] into colors */
  OutputType operator()(float in);
  
  /** Set the color map by specifying three points */
  void SetColorMap( OutputType inMinus, OutputType inZero, OutputType inPlus); 

  /** Generate a color map for one of the presets */
  static SpeedColorMap GetPresetColorMap(ColorMapPreset xPreset);

private:
  // The colors at the extremes and middle of the color map
  OutputType m_Plus, m_Minus, m_Zero;
};  

#endif
