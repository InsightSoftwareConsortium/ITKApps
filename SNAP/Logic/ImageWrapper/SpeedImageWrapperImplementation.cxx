/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SpeedImageWrapperImplementation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "SpeedImageWrapperImplementation.h"

using namespace itk;

float
SpeedImageWrapperImplementation::MappingFunctor
::operator()(float in)
{
  return 0.5f * in + 0.5f;
}

SpeedImageWrapperImplementation
::SpeedImageWrapperImplementation()
: ImageWrapperImplementation<float> ()
{
  // Intialize display filters
  for(unsigned int i=0;i<3;i++) 
    {
    // Create the intensity mapping filter
    m_DisplayFilter[i] = IntensityFilterType::New();

    // Set the corresponding slice as the input image
    m_DisplayFilter[i]->SetInput(GetSlice(i));

    // Create the overlay mapping filter
    m_OverlayFilter[i] = OverlayFilterType::New();

    // Set the corresponding slice as the input image
    m_OverlayFilter[i]->SetInput(GetSlice(i));
    }

  // Initialize to Edge mode
  m_IsModeInsideOutside = false;
}

SpeedImageWrapperImplementation
::~SpeedImageWrapperImplementation()
{
}

SpeedImageWrapperImplementation::DisplaySlicePointer 
SpeedImageWrapperImplementation
::GetDisplaySlice(unsigned int iSlice)
{
  // Depending on the current mode, return the display slice or the 
  // original slice from the parent
  return m_IsModeInsideOutside ? 
    DisplaySlicePointer(m_DisplayFilter[iSlice]->GetOutput()) : 
    GetSlice(iSlice);
}


SpeedImageWrapperImplementation::OverlaySlicePointer 
SpeedImageWrapperImplementation
::GetOverlaySlice(unsigned int dim)
{
  return m_OverlayFilter[dim]->GetOutput();
}

void
SpeedImageWrapperImplementation
::SetOverlayCutoff(float cutoff)
{
  if(cutoff != m_OverlayFunctor.m_Cutoff) 
    {
    // Update the variable
    m_OverlayFunctor.m_Cutoff = cutoff;

    // Update the filters
    for(unsigned int i=0;i<3;i++)
      m_OverlayFilter[i]->SetFunctor(m_OverlayFunctor);
    }
}

float
SpeedImageWrapperImplementation
::GetOverlayCutoff() const
{
  return m_OverlayFunctor.m_Cutoff;
}

void
SpeedImageWrapperImplementation
::SetOverlayColor(const OverlayPixelType &color)
{
  if(color != m_OverlayFunctor.m_Color) 
    {
    // Update the variable
    m_OverlayFunctor.m_Color = color;

    // Update the filters
    for(unsigned int i=0;i<3;i++)
      m_OverlayFilter[i]->SetFunctor(m_OverlayFunctor);
    }
}

SpeedImageWrapperImplementation::OverlayPixelType 
SpeedImageWrapperImplementation
::GetOverlayColor() const
{
  return m_OverlayFunctor.m_Color;
}
  
SpeedImageWrapperImplementation::OverlayPixelType 
SpeedImageWrapperImplementation::OverlayFunctor
::operator()(float in)
{
  // Initialize with a clear pixel
  static unsigned char clear[] = {0,0,0,0};
  SpeedImageWrapperImplementation::OverlayPixelType rtn(clear);
  
  // Check the threshold and return appropriate value
  return in < m_Cutoff ? clear : m_Color;
}

