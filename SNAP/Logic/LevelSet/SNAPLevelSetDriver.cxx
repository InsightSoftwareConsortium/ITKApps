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
::Initialize(FloatImageType *init, FloatImageType *speed,
             const SnakeParameters *sparms)
{
  // Create the level set function
  m_Phi = LevelSetFunctionType::New();

  // Pass the speed image to the function
  m_Phi->SetSpeedImage(speed);  
  
  // Pass the parameters to the level set function
  AssignParametersToPhi(sparms);

  // Create the 'fake' filter used for snake stepping
  this->Restart(init);
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
::Restart(FloatImageType *init)
{
  // Create the 'fake' filter used for snake stepping
  m_LevelSetFilter = LevelSetFilterType::New();
  
  // Configure the level set filter
  m_LevelSetFilter->SetInput(init);
  m_LevelSetFilter->SetNumberOfLayers(3);
  m_LevelSetFilter->SetIsoSurfaceValue(0.0f);
  m_LevelSetFilter->SetDifferenceFunction(m_Phi);
  m_LevelSetFilter->Start();
}

void 
SNAPLevelSetDriver
::Run(int nIterations)
{
  m_LevelSetFilter->GetOutput()->SetRequestedRegionToLargestPossibleRegion();
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

  // TODO: Figure out if this overhead is necessary.  
  // Restart(m_LevelSetFilter->GetOutput());
}
