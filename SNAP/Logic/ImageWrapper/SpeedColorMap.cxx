/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SpeedColorMap.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "SpeedColorMap.h"

SpeedColorMap
::SpeedColorMap()
{
  m_Plus.Set(255,255,255,255); 
  m_Minus.Set(0,0,255,255); 
  m_Zero.Set(0,0,0,255);
}

SpeedColorMap::OutputType
SpeedColorMap
::operator()(float t)
{
  // Initialize with a clear pixel
  const unsigned char clear[] = {0,0,0,255};
  OutputType P(clear);

  // The red component is used when speed is positive
  if(t > 0)
    {
    float u = 1.0f - t;
    P[0] = (unsigned char)(t * m_Plus[0] + u * m_Zero[0]);
    P[1] = (unsigned char)(t * m_Plus[1] + u * m_Zero[1]);
    P[2] = (unsigned char)(t * m_Plus[2] + u * m_Zero[2]);
    }
  else
    {
    float u = 1.0f + t;
    P[0] = (unsigned char)(-t * m_Minus[0] + u * m_Zero[0]);
    P[1] = (unsigned char)(-t * m_Minus[1] + u * m_Zero[1]);
    P[2] = (unsigned char)(-t * m_Minus[2] + u * m_Zero[2]);
    }

  // Return
  return P;
}

void
SpeedColorMap
::SetColorMap(OutputType inMinus, OutputType inZero, OutputType inPlus)
{
  m_Plus = inPlus;
  m_Minus = inMinus;
  m_Zero = inZero;
}

SpeedColorMap
SpeedColorMap
::GetPresetColorMap(ColorMapPreset xPreset)
{
  unsigned char blue[]   = {0   , 0   , 255 , 255 };
  unsigned char red[]    = {255 , 0   , 0   , 255 };
  unsigned char white[]  = {255 , 255 , 255 , 255 };
  unsigned char gray[]   = {128 , 128 , 128 , 255 };
  unsigned char black[]  = {0   , 0   , 0   , 255 };
  
  SpeedColorMap xMap;
  switch(xPreset) 
    {
    case COLORMAP_BLUE_BLACK_WHITE : 
      xMap.SetColorMap(
        OutputType(blue), OutputType(black), OutputType(white));
      break;
      
    case COLORMAP_BLACK_GRAY_WHITE : 
      xMap.SetColorMap(
        OutputType(black), OutputType(gray), OutputType(white));
      break;
      
    case COLORMAP_BLUE_WHITE_RED : 
      xMap.SetColorMap(
        OutputType(blue), OutputType(white), OutputType(red));
      break;

    case COLORMAP_BLACK_BLACK_WHITE : 
      xMap.SetColorMap(
        OutputType(black), OutputType(black), OutputType(white));
      break;
    }

  return xMap;
}

