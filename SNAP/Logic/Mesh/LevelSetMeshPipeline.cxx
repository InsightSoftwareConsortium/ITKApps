/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    LevelSetMeshPipeline.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "LevelSetMeshPipeline.h"
#include "VTKMeshPipeline.h"

LevelSetMeshPipeline
::LevelSetMeshPipeline()
{
  // Initialize the VTK Exporter
  m_VTKPipeline = new VTKMeshPipeline();

  // Initialize the options
  SetMeshOptions(m_MeshOptions);
}

LevelSetMeshPipeline
::~LevelSetMeshPipeline()
{
  delete m_VTKPipeline;
}

void
LevelSetMeshPipeline
::SetMeshOptions(const MeshOptions &options)
{
  // Store the options
  m_MeshOptions = options;

  // Turn of the Gaussian smoothing
  m_MeshOptions.SetUseGaussianSmoothing(false);

  // Apply the options to the internal pipeline
  m_VTKPipeline->SetMeshOptions(m_MeshOptions);
}

void
LevelSetMeshPipeline
::ComputeMesh(vtkPolyData *outMesh)
{
  // Run the pipeline
  m_VTKPipeline->ComputeMesh(outMesh);
}

void 
LevelSetMeshPipeline
::SetImage(InputImageType *image)
{
  // Hook the input into the pipeline
  m_VTKPipeline->SetImage(image);
}

