/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    VTKMeshPipeline.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "VTKMeshPipeline.h"

using namespace std;
using namespace itk;

VTKMeshPipeline
::VTKMeshPipeline()
{
  // Initialize all the filters involved in the transaction, but do not
  // pipe the inputs and outputs between these filters. The piping is quite
  // complicated and depends on the set of options that the user wishes to 
  // apply
  
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

  // Initialize the Gaussian filter
  m_VTKGaussianFilter = vtkImageGaussianSmooth::New();
  m_VTKGaussianFilter->ReleaseDataFlagOn();
  
  // Create and configure a filter for polygon smoothing
  m_PolygonSmoothingFilter = vtkSmoothPolyDataFilter::New();
  m_PolygonSmoothingFilter->ReleaseDataFlagOn();

  // Create and configure a filter for triangle strip generation
  m_StripperFilter = vtkStripper::New();
  m_StripperFilter->ReleaseDataFlagOn();

#ifdef USE_VTK_PATENTED

  // Create and configure the marching cubes filter
  m_MarchingCubesFilter = vtkImageMarchingCubes::New();
  m_MarchingCubesFilter->ReleaseDataFlagOn();
  m_MarchingCubesFilter->ComputeScalarsOff();
  m_MarchingCubesFilter->ComputeGradientsOff();
  m_MarchingCubesFilter->SetNumberOfContours(1);
  m_MarchingCubesFilter->SetValue(0,0.0f);

  // Create and configure a filter for triangle decimation
  m_DecimateFilter = vtkDecimate::New();
  m_DecimateFilter->ReleaseDataFlagOn();  

#else // USE_VTK_PATENTED

  // Create and configure the contour filter
  m_ContourFilter = vtkContourFilter::New();
  m_ContourFilter->ReleaseDataFlagOn();
  m_ContourFilter->ComputeNormalsOff();
  m_ContourFilter->ComputeScalarsOff();
  m_ContourFilter->ComputeGradientsOff();
  m_ContourFilter->UseScalarTreeOn();
  m_ContourFilter->SetNumberOfContours(1);
  m_ContourFilter->SetValue(0,0.0f);

  // Create and configure the normal computer
  m_NormalsFilter = vtkPolyDataNormals::New();
  m_NormalsFilter->SplittingOff();
  m_NormalsFilter->ConsistencyOff();
  m_NormalsFilter->ReleaseDataFlagOn();

  // Create and configure a filter for triangle decimation
  m_DecimateProFilter = vtkDecimatePro::New();
  m_DecimateProFilter->ReleaseDataFlagOn();

#endif // USE_VTK_PATENTED  
  
}

VTKMeshPipeline
::~VTKMeshPipeline()
{
  // Destroy the filters
  m_VTKImporter->Delete();
  m_VTKGaussianFilter->Delete();
  m_PolygonSmoothingFilter->Delete();
  m_StripperFilter->Delete();

#ifdef USE_VTK_PATENTED
  m_MarchingCubesFilter->Delete();
  m_DecimateFilter->Delete();
#else  
  m_ContourFilter->Delete();
  m_NormalsFilter->Delete();
  m_DecimateProFilter->Delete();
#endif
}

void
VTKMeshPipeline
::SetMeshOptions(const MeshOptions &options)
{
  // Store the options
  m_MeshOptions = options;

  // Define the current pipeline end-point
  vtkImageData *pipeImageTail = m_VTKImporter->GetOutput();
  vtkPolyData *pipePolyTail = NULL;

  // Route the pipeline according to the settings
  // 1. Check if Gaussian smoothing will be used

  if(options.GetUseGaussianSmoothing()) 
    {    
    // The Gaussian filter is enabled
    m_VTKGaussianFilter->SetInput(pipeImageTail);
    pipeImageTail = m_VTKGaussianFilter->GetOutput();

    // Apply parameters to the Gaussian filter
    float sigma = options.GetGaussianStandardDeviation();

    // Sigma is in millimeters
    const double *spacing = m_InputImage->GetSpacing().GetDataPointer();
    m_VTKGaussianFilter->SetStandardDeviation(
      sigma / spacing[0], sigma / spacing[1], sigma / spacing[2]);
    m_VTKGaussianFilter->SetRadiusFactors(
      3 * sigma / spacing[0], 3 * sigma / spacing[1], 3 * sigma / spacing[2]);
    }

  // 2. Set input to the appropriate contour filter

#ifdef USE_VTK_PATENTED
  
  // Marching cubes gets the tail
  m_MarchingCubesFilter->SetInput(pipeImageTail);
  pipePolyTail = m_MarchingCubesFilter->GetOutput();

#else // USE_VTK_PATENTED

  // Contour filter gets the tail
  m_ContourFilter->SetInput(pipeImageTail);
  pipePolyTail = m_ContourFilter->GetOutput();

#endif // USE_VTK_PATENTED  

  // 3. Check if decimation is required
  if(options.GetUseDecimation())
    {

#ifdef USE_VTK_PATENTED

    // Decimate filter gets the pipe tail
    m_DecimateFilter->SetInput(pipePolyTail);
    pipePolyTail = m_DecimateFilter->GetOutput();

    // Apply parameters to the decimation filter
    m_DecimateFilter->SetTargetReduction(
      options.GetDecimateTargetReduction());

    m_DecimateFilter->SetAspectRatio(
      options.GetDecimateAspectRatio());

    m_DecimateFilter->SetInitialError(
      options.GetDecimateInitialError());

    m_DecimateFilter->SetErrorIncrement(
      options.GetDecimateErrorIncrement());

    m_DecimateFilter->SetMaximumIterations(
      options.GetDecimateMaximumIterations());

    m_DecimateFilter->SetInitialFeatureAngle(
      options.GetDecimateFeatureAngle());

    m_DecimateFilter->SetPreserveTopology(
      options.GetDecimatePreserveTopology());

#else // USE_VTK_PATENTED    
    
    // Decimate Pro filter gets the pipe tail
    m_DecimateProFilter->SetInput(pipePolyTail);
    pipePolyTail = m_DecimateProFilter->GetOutput();

    // Apply parameters to the decimation filter
    m_DecimateProFilter->SetTargetReduction(
      options.GetDecimateTargetReduction());

    m_DecimateProFilter->SetPreserveTopology(
      options.GetDecimatePreserveTopology());

#endif // USE_VTK_PATENTED  
    
    } // If decimate enabled

  // 4. Compute the normals (non-patented only)

#ifndef USE_VTK_PATENTED  

  m_NormalsFilter->SetInput(pipePolyTail);
  pipePolyTail = m_NormalsFilter->GetOutput();

#endif // USE_VTK_PATENTED

  // 5. Include/exclude mesh smoothing filter
  if(options.GetUseMeshSmoothing())
    {
    // Pipe smoothed output into the pipeline
    m_PolygonSmoothingFilter->SetInput(pipePolyTail);
    pipePolyTail = m_PolygonSmoothingFilter->GetOutput();

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

  // 6. Pipe in the final output into the stripper
  m_StripperFilter->SetInput(pipePolyTail);
}

#include <ctime>

void
VTKMeshPipeline
::ComputeMesh(vtkPolyData *outMesh)
{
  // TODO: make this more elegant
  clock_t now,start = clock();

  // Graft the polydata to the last filter in the pipeline
  m_StripperFilter->SetOutput(outMesh);
  
  // This is a work-around. We set the origin to zero, because somehow it gets
  // messed up in the pipeline. We then add it later on
  double xZeroOrigin[] = {0.0, 0.0, 0.0};
  ImageType::PointType xRealOrigin = m_InputImage->GetOrigin();
  m_InputImage->SetOrigin(ImageType::PointType(xZeroOrigin));
  
  // Connect importer and exporter
  m_VTKImporter->SetCallbackUserData(
    m_VTKExporter->GetCallbackUserData());

  // All these update methods are here for timing, otherwise they can
  // be collapsed to m_StripperFilter->UpdateWholeExtent

  // Update the ITK portion of the pipeline
  m_VTKExporter->SetInput(m_InputImage);
  m_VTKExporter->UpdateLargestPossibleRegion();
  
  // This does the image processing steps
  m_VTKImporter->UpdateWholeExtent();
  
  now = clock(); cout << "IMP " << now - start << endl; start = now;
  
  // Update the Gaussian filter
  if(m_MeshOptions.GetUseGaussianSmoothing())
    m_VTKGaussianFilter->UpdateWholeExtent();

  now = clock(); cout << "GAU " << now - start << endl; start = now;
  
  // The marching cubes filter
#ifdef USE_VTK_PATENTED
  m_MarchingCubesFilter->UpdateWholeExtent();
  now = clock(); cout << "MAR " << now - start << endl; start = now;
#else
  m_ContourFilter->UpdateWholeExtent();
  now = clock(); cout << "CNT " << now - start << endl; start = now;
#endif

  // Now the decimation filters
  if(m_MeshOptions.GetUseDecimation())
    {
#ifdef USE_VTK_PATENTED
    m_DecimateFilter->UpdateWholeExtent();
#else
    m_DecimateProFilter->UpdateWholeExtent();
#endif
    }
  now = clock(); cout << "DEC " << now - start << endl; start = now;

  // The normal filter
#ifndef USE_VTK_PATENTED
  m_NormalsFilter->UpdateWholeExtent();
  now = clock(); cout << "NRM " << now - start << endl; start = now;
#endif
  
  // Now the subsequent filters
  if(m_MeshOptions.GetUseMeshSmoothing())
    m_PolygonSmoothingFilter->UpdateWholeExtent();
  now = clock(); cout << "SMO " << now - start << endl; start = now;  

  // Now the subsequent filters
  m_StripperFilter->UpdateWholeExtent();
  now = clock(); cout << "STR " << now - start << endl; start = now;
  
  // Shift all the points in the output mesh
  for(vtkIdType i = 0; i < outMesh->GetNumberOfPoints(); i++)
    {
    vtkFloatingPointType *x = outMesh->GetPoint(i);
    x[0] += xRealOrigin[0];
    x[1] += xRealOrigin[1];
    x[2] += xRealOrigin[2];
    outMesh->GetPoints()->SetPoint(i, x);
    }
  
  // Restore the image origin
  m_InputImage->SetOrigin(xRealOrigin);  
}

void
VTKMeshPipeline
::SetImage(ImageType *image)
{
  m_InputImage = image;
}

