/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPRegistryIO.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "SNAPRegistryIO.h"

/** Read snake parameters from a registry */
SnakeParameters 
SNAPRegistryIO
::ReadSnakeParameters(
  Registry &registry, const SnakeParameters &defaultSet)
{
  SnakeParameters out;
  
  out.SetAutomaticTimeStep(
    registry["AutomaticTimeStep"][defaultSet.GetAutomaticTimeStep()]);

  out.SetTimeStep(
    registry["TimeStep"][defaultSet.GetTimeStep()]);

  out.SetGround(
    registry["Ground"][defaultSet.GetGround()]);

  out.SetClamp(
    registry["Clamp"][defaultSet.GetClamp()]);

  out.SetSnakeType((SnakeParameters::SnakeType) 
    registry["SnakeType"][defaultSet.GetSnakeType()]);

  out.SetPropagationWeight(
    registry["PropagationWeight"][defaultSet.GetPropagationWeight()]);
  
  out.SetPropagationSpeedExponent(
    registry["PropagationSpeedExponent"][defaultSet.GetPropagationSpeedExponent()]);

  out.SetCurvatureWeight(
    registry["CurvatureWeight"][defaultSet.GetCurvatureWeight()]);

  out.SetCurvatureSpeedExponent(
    registry["CurvatureSpeedExponent"][defaultSet.GetCurvatureSpeedExponent()]);

  out.SetLaplacianWeight(
    registry["LaplacianWeight"][defaultSet.GetLaplacianWeight()]);

  out.SetLaplacianSpeedExponent(
    registry["LaplacianSpeedExponent"][defaultSet.GetLaplacianSpeedExponent()]);

  out.SetAdvectionWeight(
    registry["AdvectionWeight"][defaultSet.GetAdvectionWeight()]);

  out.SetAdvectionSpeedExponent(
    registry["AdvectionSpeedExponent"][defaultSet.GetAdvectionSpeedExponent()]);

  return out;
}

/** Write snake parameters to a registry */
void 
SNAPRegistryIO
::WriteSnakeParameters(const SnakeParameters &in,Registry &registry)
{
  registry["AutomaticTimeStep"] << in.GetAutomaticTimeStep();
  registry["TimeStep"] << in.GetTimeStep();
  registry["Ground"] << in.GetGround();
  registry["Clamp"] << in.GetClamp();
  registry["SnakeType"] << in.GetSnakeType();
  registry["PropagationWeight"] << in.GetPropagationWeight();
  registry["PropagationSpeedExponent"][in.GetPropagationSpeedExponent()];
  registry["CurvatureWeight"] << in.GetCurvatureWeight();
  registry["CurvatureSpeedExponent"] << in.GetCurvatureSpeedExponent();
  registry["LaplacianWeight"] << in.GetLaplacianWeight();
  registry["LaplacianSpeedExponent"] << in.GetLaplacianSpeedExponent();
  registry["AdvectionWeight"] << in.GetAdvectionWeight();
  registry["AdvectionSpeedExponent"] << in.GetAdvectionSpeedExponent();
}

/** Read mesh options from a registry */
MeshOptions 
SNAPRegistryIO
::ReadMeshOptions(
  Registry &registry, const MeshOptions &defaultSet)
{
  MeshOptions out;
  
  out.SetUseGaussianSmoothing(
    registry["UseGaussianSmoothing"][defaultSet.GetUseGaussianSmoothing()]);
    
  out.SetUseDecimation(
    registry["UseDecimation"][defaultSet.GetUseDecimation()]);

  out.SetUseMeshSmoothing(
    registry["UseMeshSmoothing"][defaultSet.GetUseMeshSmoothing()]);

  out.SetGaussianStandardDeviation(
    registry["GaussianStandardDeviation"][defaultSet.GetGaussianStandardDeviation()]);

  out.SetGaussianError(
    registry["GaussianError"][defaultSet.GetGaussianError()]);

  out.SetDecimateTargetReduction(
    registry["DecimateTargetReduction"][defaultSet.GetDecimateTargetReduction()]);

  out.SetDecimateInitialError(
    registry["DecimateInitialError"][defaultSet.GetDecimateInitialError()]);

  out.SetDecimateAspectRatio(
    registry["DecimateAspectRatio"][defaultSet.GetDecimateAspectRatio()]);

  out.SetDecimateFeatureAngle(
    registry["DecimateFeatureAngle"][defaultSet.GetDecimateFeatureAngle()]);

  out.SetDecimateErrorIncrement(
    registry["DecimateErrorIncrement"][defaultSet.GetDecimateErrorIncrement()]);

  out.SetDecimateMaximumIterations(
    registry["DecimateMaximumIterations"][defaultSet.GetDecimateMaximumIterations()]);

  out.SetDecimatePreserveTopology(
    registry["DecimatePreserveTopology"][defaultSet.GetDecimatePreserveTopology()]);

  out.SetMeshSmoothingRelaxationFactor(
    registry["MeshSmoothingRelaxationFactor"][defaultSet.GetMeshSmoothingRelaxationFactor()]);

  out.SetMeshSmoothingIterations(
    registry["MeshSmoothingIterations"][defaultSet.GetMeshSmoothingIterations()]);

  out.SetMeshSmoothingConvergence(
    registry["MeshSmoothingConvergence"][defaultSet.GetMeshSmoothingConvergence()]);

  out.SetMeshSmoothingFeatureAngle(
    registry["MeshSmoothingFeatureAngle"][defaultSet.GetMeshSmoothingFeatureAngle()]);

  out.SetMeshSmoothingFeatureEdgeSmoothing(
    registry["MeshSmoothingFeatureEdgeSmoothing"][defaultSet.GetMeshSmoothingFeatureEdgeSmoothing()]);

  out.SetMeshSmoothingBoundarySmoothing(
    registry["MeshSmoothingBoundarySmoothing"][defaultSet.GetMeshSmoothingBoundarySmoothing()]);

  return out;
}

/** Write mesh options to a registry */
void 
SNAPRegistryIO
::WriteMeshOptions(const MeshOptions &in,Registry &registry)
{
  registry["UseGaussianSmoothing"] << in.GetUseGaussianSmoothing();
  registry["UseDecimation"] << in.GetUseDecimation();
  registry["UseMeshSmoothing"] << in.GetUseMeshSmoothing();
  registry["GaussianStandardDeviation"] << in.GetGaussianStandardDeviation();
  registry["GaussianError"] << in.GetGaussianError();
  registry["DecimateTargetReduction"] << in.GetDecimateTargetReduction();
  registry["DecimateInitialError"] << in.GetDecimateInitialError();
  registry["DecimateAspectRatio"] << in.GetDecimateAspectRatio();
  registry["DecimateFeatureAngle"] << in.GetDecimateFeatureAngle();
  registry["DecimateErrorIncrement"] << in.GetDecimateErrorIncrement();
  registry["DecimateMaximumIterations"] << in.GetDecimateMaximumIterations();
  registry["DecimatePreserveTopology"] << in.GetDecimatePreserveTopology();
  registry["MeshSmoothingRelaxationFactor"] << in.GetMeshSmoothingRelaxationFactor();
  registry["MeshSmoothingIterations"] << in.GetMeshSmoothingIterations();
  registry["MeshSmoothingConvergence"] << in.GetMeshSmoothingConvergence();
  registry["MeshSmoothingFeatureAngle"] << in.GetMeshSmoothingFeatureAngle();
  registry["MeshSmoothingFeatureEdgeSmoothing"] << in.GetMeshSmoothingFeatureEdgeSmoothing();
  registry["MeshSmoothingBoundarySmoothing"] << in.GetMeshSmoothingBoundarySmoothing();
}

/** Read edge preprocessing settings from a registry */
EdgePreprocessingSettings 
SNAPRegistryIO
::ReadEdgePreprocessingSettings(
  Registry &registry, const EdgePreprocessingSettings &defaultSet)
{
  EdgePreprocessingSettings out;

  out.SetGaussianBlurScale(
    registry["GaussianBlurScale"][defaultSet.GetGaussianBlurScale()]);

  out.SetRemappingSteepness(
    registry["RemappingSteepness"][defaultSet.GetRemappingSteepness()]);

  out.SetRemappingExponent(
    registry["RemappingExponent"][defaultSet.GetRemappingExponent()]);
  
  return out;  
}

/** Write edge preprocessing settings to a registry */
void 
SNAPRegistryIO
::WriteEdgePreprocessingSettings(const EdgePreprocessingSettings &in,
                                 Registry &registry)
{
  registry["GaussianBlurScale"] << in.GetGaussianBlurScale();
  registry["RemappingSteepness"] << in.GetRemappingSteepness();
  registry["RemappingExponent"] << in.GetRemappingExponent();
}

/** Read threshold settings from a registry */
ThresholdSettings 
SNAPRegistryIO
::ReadThresholdSettings(
  Registry &registry, const ThresholdSettings &defaultSet)
{
  ThresholdSettings out;
  
  out.SetLowerThreshold(
    registry["LowerThreshold"][defaultSet.GetLowerThreshold()]);

  out.SetUpperThreshold(
    registry["UpperThreshold"][defaultSet.GetUpperThreshold()]);

  out.SetSmoothness(
    registry["Smoothness"][defaultSet.GetSmoothness()]);

  out.SetLowerThresholdEnabled(
    registry["LowerThresholdEnabled"][defaultSet.IsLowerThresholdEnabled()]);

  out.SetUpperThresholdEnabled(
    registry["UpperThresholdEnabled"][defaultSet.IsUpperThresholdEnabled()]);

  return out;
}

/** Write threshold settings to a registry */
void 
SNAPRegistryIO
::WriteThresholdSettings(const ThresholdSettings &in,Registry &registry)
{
  registry["LowerThreshold"] << in.GetLowerThreshold();
  registry["UpperThreshold"] << in.GetUpperThreshold();
  registry["Smoothness"] << in.GetSmoothness();
  registry["LowerThresholdEnabled"] << in.IsLowerThresholdEnabled();
  registry["UpperThresholdEnabled"] << in.IsUpperThresholdEnabled();
}

