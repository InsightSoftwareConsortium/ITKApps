/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    LevelSetMeshPipeline.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __LevelSetMeshPipeline_h_
#define __LevelSetMeshPipeline_h_

#include "SNAPCommon.h"
#include "itkSmartPointer.h"
#include "MeshOptions.h"

// Forward reference to itk classes
namespace itk {
  template <class TPixel,unsigned int VDimension> class Image;
  template <class TInputImage> class VTKImageExport;
}

// Forward references to vtk classes
class vtkImageImport;
class vtkContourFilter;
class vtkSmoothPolyDataFilter;
class vtkDecimatePro;
class vtkStripper;
class vtkPolyData;
class vtkPolyDataNormals;

/**
 * \class LevelSetMeshPipeline
 * \brief A pipeline used to compute a mesh of the zero level set in SNAP.
 *
 * This pipeline takes a floating point image computed by the level
 * set filter and uses a contour algorithm to get a triangular mesh
 */
class LevelSetMeshPipeline
{
public:  
  /** Input image type */
  typedef itk::Image<float,3> InputImageType;
  typedef itk::SmartPointer<InputImageType> InputImagePointer;

  /** Set the input segmentation image */
  void SetImage(InputImageType *input);

  /** Set the mesh options for this filter */
  void SetMeshOptions(const MeshOptions &options);

  /** Compute the mesh for the segmentation level set */
  void ComputeMesh(vtkPolyData *outData);
  
  
  /** Constructor, which builds the pipeline */
  LevelSetMeshPipeline();

  /** Deallocate the pipeline filters */
  ~LevelSetMeshPipeline();

private:
  // Type definitions for the various filters used by this object
  typedef InputImageType                             InternalImageType;
  typedef itk::SmartPointer<InternalImageType>       InternalImagePointer;

  typedef itk::VTKImageExport<InternalImageType>     VTKExportType;
  typedef itk::SmartPointer<VTKExportType>           VTKExportPointer;
  
  // Current set of mesh options
  MeshOptions                 m_MeshOptions;

  // The input image
  InputImagePointer           m_InputImage;

  // The VTK exporter for the data
  VTKExportPointer            m_VTKExporter;

  // The VTK importer for the data
  vtkImageImport *            m_VTKImporter;

  // The contour filter
  vtkContourFilter *          m_ContourFilter;

  // A filter that computes normals
  vtkPolyDataNormals *        m_NormalsFilter;
  
  // The triangle decimation driver
  vtkDecimatePro *            m_DecimateFilter;

  // The polygon smoothing filter
  vtkSmoothPolyDataFilter *   m_PolygonSmoothingFilter;

  // Triangle stripper
  vtkStripper *               m_StripperFilter;
};

#endif //__LevelSetMeshPipeline_h_
