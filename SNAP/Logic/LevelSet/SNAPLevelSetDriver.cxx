/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPLevelSetDriver.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "SNAPLevelSetDriver.h"
#include "IRISVectorTypesToITKConversion.h"

#include "itkRescaleIntensityImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"

using namespace itk;

// Create an inverting functor
class InvertFunctor {
public:
  unsigned char operator()(unsigned char input) { 
    return input == 0 ? 1 : 0; 
  }  
};

void
SNAPLevelSetDriver
::Initialize(BubbleImageType *init, FloatImageType *speed,
             const SnakeParameters *sparms)
{
  // Create the inverse image
  typedef UnaryFunctorImageFilter<BubbleImageType,BubbleImageType,
    InvertFunctor> InvertFilterType;
  InvertFilterType::Pointer fltInvert = InvertFilterType::New();
  fltInvert->SetInput(init);
  fltInvert->ReleaseDataFlagOn();

  // Compute the signed distance function from the bubble image
  typedef DanielssonDistanceMapImageFilter
    <BubbleImageType,FloatImageType> DistanceFilterType;
  DistanceFilterType::Pointer fltDistanceOutside = DistanceFilterType::New();
  fltDistanceOutside->SetInput(init);
  fltDistanceOutside->SetInputIsBinary(true);
  fltDistanceOutside->ReleaseDataFlagOn();

  // Compute the second distance function
  DistanceFilterType::Pointer fltDistanceInside = DistanceFilterType::New();
  fltDistanceInside->SetInput(fltInvert->GetOutput());
  fltDistanceInside->SetInputIsBinary(true);
  fltDistanceInside->ReleaseDataFlagOn();

  // Subtract the inside from the outside, forming a signed distance map
  typedef SubtractImageFilter<FloatImageType,
    FloatImageType,FloatImageType> SubtractFilterType;
  SubtractFilterType::Pointer fltSubtract = SubtractFilterType::New();
  fltSubtract->SetInput1(fltDistanceOutside->GetDistanceMap());
  fltSubtract->SetInput2(fltDistanceInside->GetDistanceMap());

  // Update this filter.  Now we have a distance transform image
  fltSubtract->Update();
  FloatImageType::Pointer imgDistance = fltSubtract->GetOutput();

  // Create the level set function
  m_Phi = LevelSetFunctionType::New();

  // Pass the speed image to the function
  m_Phi->SetSpeedImage(speed);  
  
  // Pass the parameters to the level set function
  AssignParametersToPhi(sparms);

  // Create the 'fake' filter used for snake stepping
  m_LevelSetFilter = LevelSetFilterType::New();
  
  // Configure the level set filter
  m_LevelSetFilter->SetInput(imgDistance);
  m_LevelSetFilter->SetNumberOfLayers(3);
  m_LevelSetFilter->SetIsoSurfaceValue(0.0f);
  m_LevelSetFilter->SetDifferenceFunction(m_Phi);
  m_LevelSetFilter->Start();
}

void 
SNAPLevelSetDriver
::AssignParametersToPhi(const SnakeParameters *sparms)
{
  // Set up the level set function
  m_Phi->SetAdvectionWeight(-sparms->GetAdvectionWeight());
  m_Phi->SetAdvectionSpeedExponent(sparms->GetAdvectionSpeedExponent());
  m_Phi->SetCurvatureWeight(sparms->GetCurvatureWeight());
  m_Phi->SetCurvatureSpeedExponent(sparms->GetCurvatureSpeedExponent()+1);  
  m_Phi->SetPropagationWeight(-sparms->GetPropagationWeight());
  m_Phi->SetPropagationSpeedExponent(sparms->GetPropagationSpeedExponent());  
  m_Phi->SetLaplacianSmoothingWeight(sparms->GetLaplacianWeight());
  m_Phi->SetLaplacianSmoothingSpeedExponent(sparms->GetLaplacianSpeedExponent());  
  m_Phi->SetTimeStep(sparms->GetTimeStep());
  m_Phi->CalculateInternalImages();
  m_Phi->Initialize(to_itkSize(Vector3i(1)));
}

void 
SNAPLevelSetDriver
::Run(int nIterations)
{
  m_LevelSetFilter->Run(nIterations);
}

SNAPLevelSetDriver::FloatImageType * 
SNAPLevelSetDriver
::GetCurrentState()
{
  // This should do nothing at all, just make the output not dirty
  m_LevelSetFilter->Update();

  // Return the filter's output
  return m_LevelSetFilter->GetOutput();
}

void 
SNAPLevelSetDriver
::CleanUp()
{
  // Clear the level set and the function - free memory
  m_LevelSetFilter = NULL;
  m_Phi = NULL;
}

void
SNAPLevelSetDriver
::SetSnakeParameters(const SnakeParameters *sparms)
{
  // Pass the parameters to the phi function
  AssignParametersToPhi(sparms);

  // We need to reinitialize the filter (perhaps more is needed?)
  m_LevelSetFilter->Start();
}
