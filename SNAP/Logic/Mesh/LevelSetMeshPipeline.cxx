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

// SNAP includes
#include "ColorLabel.h"
#include "GlobalState.h"
#include "ImageWrapper.h"
#include "IRISApplication.h"
#include "IRISVectorTypesToITKConversion.h"
#include "IRISImageData.h"
#include "SNAPImageData.h"

// ITK includes
#include "itkImage.h"
#include "itkVTKImageExport.h"

// VTK includes
#include <vtkCellArray.h>
#include <vtkDecimatePro.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkContourFilter.h>
#include <vtkImageThreshold.h>
#include <vtkImageToStructuredPoints.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkStripper.h>

// System includes
#include <stdlib.h>
#include <FL/gl.h>

using namespace std;
using namespace itk;

LevelSetMeshPipeline
::LevelSetMeshPipeline()
{
  // Initialize the VTK Exporter
  m_VTKExporter = VTKExportType::New();
  m_VTKExporter->ReleaseDataFlagOn();
  
  // Initialize the VTK Importer
  m_VTKImporter = vtkImageImport::New();
  m_VTKImporter->ReleaseDataFlagOn();

  // Pipe the importer into the exporter (that's a lot of code)
  m_VTKImporter->SetUpdateInformationCallback(
    m_VTKExporter->GetUpdateInformationCallback());
  m_VTKImporter->SetPipelineModifiedCallback(
    m_VTKExporter->GetPipelineModifiedCallback());
  m_VTKImporter->SetWholeExtentCallback(
    m_VTKExporter->GetWholeExtentCallback());
  m_VTKImporter->SetSpacingCallback(
    m_VTKExporter->GetSpacingCallback());
  m_VTKImporter->SetOriginCallback(
    m_VTKExporter->GetOriginCallback());
  m_VTKImporter->SetScalarTypeCallback(
    m_VTKExporter->GetScalarTypeCallback());
  m_VTKImporter->SetNumberOfComponentsCallback(
    m_VTKExporter->GetNumberOfComponentsCallback());
  m_VTKImporter->SetPropagateUpdateExtentCallback(
    m_VTKExporter->GetPropagateUpdateExtentCallback());
  m_VTKImporter->SetUpdateDataCallback(
    m_VTKExporter->GetUpdateDataCallback());
  m_VTKImporter->SetDataExtentCallback(
    m_VTKExporter->GetDataExtentCallback());
  m_VTKImporter->SetBufferPointerCallback(
    m_VTKExporter->GetBufferPointerCallback());  
  m_VTKImporter->SetCallbackUserData(
    m_VTKExporter->GetCallbackUserData());

  // Create and configure the contour filter
  m_ContourFilter = vtkContourFilter::New();
  m_ContourFilter->SetInput(m_VTKImporter->GetOutput());    
  m_ContourFilter->ReleaseDataFlagOn();
  m_ContourFilter->ComputeScalarsOff();
  m_ContourFilter->ComputeGradientsOff();
  m_ContourFilter->UseScalarTreeOn();
  m_ContourFilter->SetNumberOfContours(1);
  m_ContourFilter->SetValue(0, 0.0);

  // Create and configure the normal computer
  m_NormalsFilter = vtkPolyDataNormals::New();
  m_NormalsFilter->SetInput(m_ContourFilter->GetOutput());
  m_NormalsFilter->SetFeatureAngle(60.0);

  // Create and configure a filter for triangle decimation
  m_DecimateFilter = vtkDecimatePro::New();
  m_DecimateFilter->SetInput(m_NormalsFilter->GetOutput());
  m_DecimateFilter->ReleaseDataFlagOn();

  // Create and configure a filter for polygon smoothing
  m_PolygonSmoothingFilter = vtkSmoothPolyDataFilter::New();
  m_PolygonSmoothingFilter->SetInput(m_NormalsFilter->GetOutput());
  m_PolygonSmoothingFilter->ReleaseDataFlagOn();

  // Create and configure a filter for triangle strip generation
  m_StripperFilter = vtkStripper::New();
  // m_StripperFilter->SetInput( m_PolygonSmoothingFilter->GetOutput() );
  m_StripperFilter->SetInput( m_PolygonSmoothingFilter->GetOutput() );
  m_StripperFilter->ReleaseDataFlagOn();
}

LevelSetMeshPipeline
::~LevelSetMeshPipeline()
{
  // Destroy the filters
  m_StripperFilter->Delete();
  m_PolygonSmoothingFilter->Delete();
  m_DecimateFilter->Delete();
  m_ContourFilter->Delete();
  m_VTKImporter->Delete();
}

void
LevelSetMeshPipeline
::SetMeshOptions(const MeshOptions &options)
{
  // Store the options
  m_MeshOptions = options;
  
  // Apply parameters to the decimation filter
  m_DecimateFilter->SetTargetReduction(
    options.GetDecimateTargetReduction());
  
  m_DecimateFilter->SetPreserveTopology(
    options.GetDecimatePreserveTopology());
  
  // Apply parameters to the mesh smoothing filter
  m_PolygonSmoothingFilter->SetNumberOfIterations(
    options.GetMeshSmoothingIterations());

  m_PolygonSmoothingFilter->SetRelaxationFactor(
    options.GetMeshSmoothingRelaxationFactor()); 

  m_PolygonSmoothingFilter->SetFeatureAngle(
    options.GetMeshSmoothingFeatureAngle());

  m_PolygonSmoothingFilter->SetFeatureEdgeSmoothing(
    options.GetMeshSmoothingFeatureEdgeSmoothing());

  m_PolygonSmoothingFilter->SetBoundarySmoothing(
    options.GetMeshSmoothingBoundarySmoothing());

  m_PolygonSmoothingFilter->SetConvergence(
    options.GetMeshSmoothingConvergence());
}

void
LevelSetMeshPipeline
::ComputeMesh(vtkPolyData *outMesh)
{
  // Hook the input into the pipeline
  m_VTKExporter->SetInput(m_InputImage);
  
  // This does the image processing steps
  m_VTKImporter->UpdateWholeExtent();
  
  // Graft the polydata to the last filter in the pipeline
  m_StripperFilter->SetOutput(outMesh);

  // Now the subsequent filters
  m_StripperFilter->UpdateWholeExtent();
}

void 
LevelSetMeshPipeline
::SetImage(LevelSetMeshPipeline::InputImageType *image)
{
  m_InputImage = image;
}

