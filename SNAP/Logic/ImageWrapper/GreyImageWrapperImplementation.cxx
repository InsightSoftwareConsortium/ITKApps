/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    GreyImageWrapperImplementation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "GreyImageWrapperImplementation.h"

GreyImageWrapperImplementation
::GreyImageWrapperImplementation()
: ImageWrapperImplementation<GreyType> ()
{
  // Instantiate the cache
  m_IntensityMapCache = CacheType::New();

  // Set the target of the cache
  m_IntensityMapCache->SetInputFunctor(&m_IntensityFunctor);

  // Instantiate the filters
  for(unsigned int i=0;i<3;i++) 
  {
    m_IntensityFilter[i] = IntensityFilterType::New();
    m_IntensityFilter[i]->SetFunctor(m_IntensityMapCache->GetCachingFunctor());
    m_IntensityFilter[i]->SetInput(m_Slicer[i]->GetOutput());
  }
}

GreyImageWrapperImplementation
::~GreyImageWrapperImplementation()
{
}

void GreyImageWrapperImplementation
::SetIntensityMapFunction(IntensityMapType *curve) 
{
  // Store the curve pointer in the functor
  m_IntensityFunctor.m_IntensityMap = curve;

  // Get the range of the image
  GreyType iMin = GetImageMin();
  GreyType iMax = GetImageMax();

  // Set the input range of the functor
  m_IntensityFunctor.SetInputRange(iMin,iMax);
    
  // Set the active range of the cache
  m_IntensityMapCache->SetEvaluationRange(iMin,1+iMax-iMin);

  // Compute the cache
  m_IntensityMapCache->ComputeCache();

  // Dirty the intensity filters
  for(unsigned int i=0;i<3;i++)
    m_IntensityFilter[i]->Modified();  
}

GreyImageWrapperImplementation::DisplaySlicePointer
GreyImageWrapperImplementation
::GetDisplaySlice(unsigned int dim)
{
  return m_IntensityFilter[dim]->GetOutput();
}


void 
GreyImageWrapperImplementation::IntensityFunctor
::SetInputRange(GreyType intensityMin, GreyType intensityMax) 
{
  m_IntensityMin = intensityMin;
  m_IntensityFactor = 1.0f / (intensityMax-intensityMin);
}

unsigned char
GreyImageWrapperImplementation::IntensityFunctor
::operator()(const GreyType &in) const 
{
  // Map the input value to range of 0 to 1
  float inZeroOne = (in - m_IntensityMin) * m_IntensityFactor;

  // Input must be in range
  assert(inZeroOne >= 0.0f && inZeroOne <= 1.0f);

  // Compute the mapping
  float outZeroOne = m_IntensityMap->Evaluate(inZeroOne);

  // Output must be in range
  assert(0.0f <= outZeroOne && 1.0f >= outZeroOne);

  // Map the output to a byte
  return (unsigned char)(255.0f * outZeroOne);
}


