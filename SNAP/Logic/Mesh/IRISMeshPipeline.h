/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IRISMeshPipeline.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __IRISMeshPipeline_h_
#define __IRISMeshPipeline_h_

#include "IRISTypes.h"
#include "itkImageRegion.h"
#include "itkSmartPointer.h"
#include "MeshOptions.h"

#include <vector>

// Forward reference to itk classes
namespace itk {
  template <class TPixel,unsigned int VDimension> class Image;
  template <class TInputImage, class TOutputImage> class RegionOfInterestImageFilter;
  template <class TInputImage, class TOutputImage> class BinaryThresholdImageFilter;
  template <class TInputImage, class TOutputImage> class DiscreteGaussianImageFilter;
  template <class TInputImage> class VTKImageExport;
}

// Forward references to vtk classes
class vtkImageImport;
class vtkContourFilter;
class vtkSmoothPolyDataFilter;
class vtkDecimatePro;
class vtkStripper;
class vtkPolyData;

/**
 * \class IRISMeshPipeline
 * \brief A small pipeline used to convert a segmentation image to a mesh in IRIS.
 *
 * This pipeline preprocesses each label in the segmentation image by blurring it.
 */
class IRISMeshPipeline 
{
public:
  /** Input image type */
  typedef itk::Image<LabelType,3> InputImageType;
  typedef itk::SmartPointer<InputImageType> InputImagePointer;
  
  /** Set the input segmentation image */
  void SetImage(InputImageType *input);

  /** Compute the bounding boxes for different regions.  Prerequisite for 
   * calling ComputeMesh() */
  void ComputeBoundingBoxes();

  /** Set the mesh options for this filter */
  void SetMeshOptions(const MeshOptions &options);

  /** Can we compute a mesh for this label? */
  bool CanComputeMesh(LabelType label)
  {
    return m_Histogram[label] > 0l;
  }

  /** Compute a mesh for a particular color label.  Returns true if 
   * the color label is not present in the image */
  bool ComputeMesh(LabelType label, vtkPolyData *outData);
  
  /** Constructor, which builds the pipeline */
  IRISMeshPipeline();

  /** Deallocate the pipeline filters */
  ~IRISMeshPipeline();

private:
  // Type definitions for the various filters used by this object
  typedef itk::Image<float,3>                        InternalImageType;
  typedef itk::SmartPointer<InternalImageType>       InternalImagePointer;
  
  typedef itk::RegionOfInterestImageFilter<
    InputImageType,InputImageType>                   ROIFilter;
  typedef itk::SmartPointer<ROIFilter>               ROIFilterPointer;
  
  typedef itk::BinaryThresholdImageFilter<
    InputImageType,InternalImageType>                ThresholdFilter;
  typedef itk::SmartPointer<ThresholdFilter>         ThresholdFilterPointer;
  
  typedef itk::DiscreteGaussianImageFilter<
    InternalImageType,InternalImageType>             GaussianFilter;
  typedef itk::SmartPointer<GaussianFilter>          GaussianFilterPointer;
  
  typedef itk::VTKImageExport<InternalImageType>     VTKExportType;
  typedef itk::SmartPointer<VTKExportType>           VTKExportPointer;
  
  // Current set of mesh options
  MeshOptions                 m_MeshOptions;

  // The input image
  InputImagePointer           m_InputImage;

  // The ROI extraction filter used for constructing a bounding box
  ROIFilterPointer            m_ROIFilter;

  // The Gaussian smoothing filter
  GaussianFilterPointer       m_GaussianFilter;

  // The thresholding filter used to map intensity in the bounding box to
  // standardized range
  ThresholdFilterPointer      m_ThrehsoldFilter;

  // The VTK exporter for the data
  VTKExportPointer            m_VTKExporter;

  // The VTK importer for the data
  vtkImageImport *            m_VTKImporter;

  // The contour filter
  vtkContourFilter *          m_ContourFilter;
  
  // The triangle decimation driver
  vtkDecimatePro *            m_DecimateFilter;

  // The polygon smoothing filter
  vtkSmoothPolyDataFilter *   m_PolygonSmoothingFilter;

  // Triangle stripper
  vtkStripper *               m_StripperFilter;

  // Set of bounding boxes
  itk::ImageRegion<3>         m_BoundingBox[MAX_COLOR_LABELS];

  // Histogram of the image
  long                        m_Histogram[MAX_COLOR_LABELS];
};

#endif
