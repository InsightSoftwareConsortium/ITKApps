/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IRISMeshPipeline.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "IRISMeshPipeline.h"

// SNAP includes
#include "ColorLabel.h"
#include "GlobalState.h"
#include "ImageWrapper.h"
#include "IRISApplication.h"
#include "IRISVectorTypesToITKConversion.h"
#include "IRISImageData.h"
#include "SNAPImageData.h"

// ITK includes
#include "itkRegionOfInterestImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkVTKImageExport.h"
#include "itkImageRegionConstIteratorWithIndex.h"

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
#include <cstdlib>
#include <FL/gl.h>

using namespace std;
using namespace itk;

IRISMeshPipeline
::IRISMeshPipeline()
{
  // Initialize the region of interest filter
  m_ROIFilter = ROIFilter::New();
  m_ROIFilter->ReleaseDataFlagOn();

  // Define the binary thresholding filter that will map the image onto the 
  // range 0 to 255
  m_ThrehsoldFilter = ThresholdFilter::New();
  m_ThrehsoldFilter->SetInput(m_ROIFilter->GetOutput());
  m_ThrehsoldFilter->ReleaseDataFlagOn();
  m_ThrehsoldFilter->SetInsideValue(255);
  m_ThrehsoldFilter->SetOutsideValue(0);

  // Define and initialize the Gaussian blur filter
  m_GaussianFilter = GaussianFilter::New();
  m_GaussianFilter->SetInput(m_ThrehsoldFilter->GetOutput());
  m_GaussianFilter->ReleaseDataFlagOn();
  m_GaussianFilter->SetUseImageSpacing(true);

  // Initialize the VTK Exporter
  m_VTKExporter = VTKExportType::New();
  m_VTKExporter->SetInput(m_ThrehsoldFilter->GetOutput());
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

  m_VTKGaussianFilter = vtkImageGaussianSmooth::New();
  m_VTKGaussianFilter->SetInput(m_VTKImporter->GetOutput());
  
  // Create and configure the contour filter
  m_ContourFilter = vtkContourFilter::New();
  m_ContourFilter->SetInput(m_VTKGaussianFilter->GetOutput());
  m_ContourFilter->ReleaseDataFlagOn();
  m_ContourFilter->ComputeNormalsOff();
  m_ContourFilter->ComputeScalarsOff();
  m_ContourFilter->ComputeGradientsOff();
  m_ContourFilter->UseScalarTreeOn();
  m_ContourFilter->SetNumberOfContours(1);
  m_ContourFilter->SetValue(0, 128.0);

  // Create and configure a filter for triangle decimation
  m_DecimateFilter = vtkDecimatePro::New();
  m_DecimateFilter->SetInput(m_ContourFilter->GetOutput());
  m_DecimateFilter->ReleaseDataFlagOn();

  // Create and configure the normal computer
  m_NormalsFilter = vtkPolyDataNormals::New();
  m_NormalsFilter->SetInput(m_DecimateFilter->GetOutput());
  m_NormalsFilter->SplittingOff();
  m_NormalsFilter->ConsistencyOff();
  m_NormalsFilter->ReleaseDataFlagOn();

  // Create and configure a filter for polygon smoothing
  m_PolygonSmoothingFilter = vtkSmoothPolyDataFilter::New();
  m_PolygonSmoothingFilter->SetInput(m_NormalsFilter->GetOutput());
  m_PolygonSmoothingFilter->ReleaseDataFlagOn();

  // Create and configure a filter for triangle strip generation
  m_StripperFilter = vtkStripper::New();
  m_StripperFilter->SetInput( m_NormalsFilter->GetOutput() );
  m_StripperFilter->ReleaseDataFlagOn();
}

IRISMeshPipeline
::~IRISMeshPipeline()
{
  // Destroy the filters
  m_StripperFilter->Delete();
  m_PolygonSmoothingFilter->Delete();
  m_DecimateFilter->Delete();
  m_NormalsFilter->Delete();
  m_ContourFilter->Delete();
  m_VTKGaussianFilter->Delete();
  m_VTKImporter->Delete();
}

void
IRISMeshPipeline
::SetMeshOptions(const MeshOptions &options)
{
  // Store the options
  m_MeshOptions = options;

  // Route the pipeline according to the settings
  if(options.GetUseGaussianSmoothing()) 
    {
    // Pipe smoothed output into the pipeline
    // m_VTKExporter->SetInput(m_GaussianFilter->GetOutput());
    m_ContourFilter->SetInput(m_VTKGaussianFilter->GetOutput());

    // Apply parameters to the Gaussian filter
    float sigma = options.GetGaussianStandardDeviation();
    Vector3f variance(sigma * sigma);
    m_GaussianFilter->SetVariance(variance.data_block());

    // Sigma is in millimeters
    const InputImageType::SpacingType &spacing = m_InputImage->GetSpacing();
    m_VTKGaussianFilter->SetStandardDeviation(
      sigma / spacing[0], sigma / spacing[1], sigma / spacing[2]);
    m_VTKGaussianFilter->SetRadiusFactors(
      3 * sigma / spacing[0], 3 * sigma / spacing[1], 3 * sigma / spacing[2]);

    // What would be a suitable setting?  I suppose we don't really care
    // how close the the Gaussian this filter is, we just want some smoothing
    // to happen
    m_GaussianFilter->SetMaximumError(options.GetGaussianError());           
    }
  else
    {
    // Bypass the gaussian
    // m_VTKExporter->SetInput(m_ThrehsoldFilter->GetOutput());
    m_ContourFilter->SetInput(m_VTKImporter->GetOutput());
    }

  // Include/exclude decimation filter
  if(options.GetUseDecimation())
    {
    // The decimate filter is the new pipe end
    m_NormalsFilter->SetInput(m_DecimateFilter->GetOutput());

    // Apply parameters to the decimation filter
    m_DecimateFilter->SetTargetReduction(
      options.GetDecimateTargetReduction());

    m_DecimateFilter->SetPreserveTopology(
      options.GetDecimatePreserveTopology());
    }
  else
    {
    m_NormalsFilter->SetInput(m_ContourFilter->GetOutput());
    }
  
  // Include/exclude mesh smoothing filter
  if(options.GetUseMeshSmoothing())
    {
    // Pipe smoothed output into the pipeline
    m_StripperFilter->SetInput(m_PolygonSmoothingFilter->GetOutput());

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
  else 
    {
    // Pipe previous output into the pipeline
    m_StripperFilter->SetInput(m_NormalsFilter->GetOutput());
    }
}

void
IRISMeshPipeline
::ComputeBoundingBoxes()
{
  unsigned int i;

  // Get the dimensions of the image
  InputImageType::SizeType size = 
    m_InputImage->GetLargestPossibleRegion().GetSize();

  // These vectors represent the extents of the image.  
  Vector3l extLower(0l);
  Vector3l extUpper = 
    Vector3l(reinterpret_cast<const long *>(size.GetSize())) - Vector3l(1l);

  // For each intensity present in the image, we will compute a bounding
  // box.  Intialize a list of bounding boxes with 'inverted' boxes
  Vector3l bbMin[MAX_COLOR_LABELS];
  Vector3l bbMax[MAX_COLOR_LABELS];

  // Initialize the histogram and bounding boxes
  for(i=0;i<MAX_COLOR_LABELS;i++)
    {
    m_Histogram[i] = 0l;
    bbMin[i] = extUpper;
    bbMax[i] = extLower;
    }

  // Create an iterator for parsing the image
  typedef ImageRegionConstIteratorWithIndex<InputImageType> InputIterator;
  InputIterator it(m_InputImage,m_InputImage->GetLargestPossibleRegion());

  // Parse through the image using an iterator and compute the bounding boxes
  for(it.GoToBegin();!it.IsAtEnd();++it)
    {
    // Get the intensity at current pixel
    LabelType label = it.Value();

    // For non-zero labels, compute the bounding box
    if(label != 0)
      {
      // Increment the histogram
      m_Histogram[label]++;

      // Get the current image index
      Vector3l point(it.GetIndex().GetIndex());

      // Update the bounding box extents
      bbMin[label] = vector_min(bbMin[label],point);
      bbMax[label] = vector_max(bbMax[label],point);        
      }
    }

  // Convert the bounding box to a region
  for(i=0;i<MAX_COLOR_LABELS;i++)
    {
    Vector3l bbSize = Vector3l(1l) + bbMax[i] - bbMin[i];
    m_BoundingBox[i].SetSize(to_itkSize(bbSize));
    m_BoundingBox[i].SetIndex(to_itkIndex(bbMin[i]));
    }  
}

#include <ctime>

bool
IRISMeshPipeline
::ComputeMesh(LabelType label, vtkPolyData *outMesh)
{
  // The label must be present in the image
  if(m_Histogram[label] == 0)
    return false;

  // TODO: make this more elegant
  clock_t now,start = clock();

  InputImageType::RegionType bbWiderRegion = m_BoundingBox[label];
  bbWiderRegion.PadByRadius(5);
  bbWiderRegion.Crop(m_InputImage->GetLargestPossibleRegion()); 
  
  // Pass the region to the ROI filter and propagate the filter
  m_ROIFilter->SetInput(m_InputImage);
  m_ROIFilter->SetRegionOfInterest(bbWiderRegion);
  m_ROIFilter->Update();
  
  cout << endl << "Timing: " << (int) label << endl;
  now = clock(); cout << "ROI " << now - start << endl; start = now;

  // Set the parameters for the thresholding filter
  m_ThrehsoldFilter->SetLowerThreshold(label);
  m_ThrehsoldFilter->SetUpperThreshold(label);
  m_ThrehsoldFilter->UpdateLargestPossibleRegion();

  now = clock(); cout << "THR " << now - start << endl; start = now;

  // Graft the polydata to the last filter in the pipeline
  m_StripperFilter->SetOutput(outMesh);
  
  // Connect importer and exporter
  m_VTKImporter->SetCallbackUserData(
    m_VTKExporter->GetCallbackUserData());

  // All these update methods are here for timing, otherwise they can
  // be collapsed to m_StripperFilter->UpdateWholeExtent

  // Update the ITK portion of the pipeline
  m_VTKExporter->UpdateLargestPossibleRegion();
  
  // This does the image processing steps
  m_VTKImporter->UpdateWholeExtent();
  
  now = clock(); cout << "IMP " << now - start << endl; start = now;
  
  // Update the Gaussian filter
  if(m_MeshOptions.GetUseGaussianSmoothing())
    m_VTKGaussianFilter->UpdateWholeExtent();

  now = clock(); cout << "GAU " << now - start << endl; start = now;
  
  // Now the subsequent filters
  m_ContourFilter->UpdateWholeExtent();

  now = clock(); cout << "CNT " << now - start << endl; start = now;

  // Now the subsequent filters
  if(m_MeshOptions.GetUseDecimation())
    m_DecimateFilter->UpdateWholeExtent();

  now = clock(); cout << "DEC " << now - start << endl; start = now;

  // The normal filter
  m_NormalsFilter->UpdateWholeExtent();

  now = clock(); cout << "NRM " << now - start << endl; start = now;

  // Now the subsequent filters
  if(m_MeshOptions.GetUseMeshSmoothing())
    m_PolygonSmoothingFilter->UpdateWholeExtent();

  now = clock(); cout << "SMO " << now - start << endl; start = now;  

  // Now the subsequent filters
  m_StripperFilter->UpdateWholeExtent();

  now = clock(); cout << "STR " << now - start << endl; start = now;

  // Success
  return true;
}

void 
IRISMeshPipeline
::SetImage(IRISMeshPipeline::InputImageType *image)
{
  m_InputImage = image;
}

