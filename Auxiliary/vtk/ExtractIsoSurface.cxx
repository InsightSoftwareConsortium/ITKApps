/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ExtractIsoSurface.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "vtkKWImage.h"
#include "vtkKWImageIO.h"

#include "vtkSmartPointer.h"
#include "vtkContourFilter.h"
#include "vtkImageData.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkSTLWriter.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"

#include <vtksys/SystemTools.hxx>

int main( int argc, char * argv[] )
{
  if( argc < 3 ) 
    { 
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImageFile  outputPolydata.[vtp,stl]  [isovalue]" << std::endl;
    return EXIT_FAILURE;
    }

  //
  // Note that if you use the vtkMetaImageReader here, then you will not have
  // access to the image orientation, and the resulting extracted surface will
  // not be aligned with the actual image data.
  //
  vtkKWImageIO * reader = vtkKWImageIO::New();

  reader->SetFileName( argv[1] );
  reader->ReadImage();

  vtkKWImage * image = reader->HarvestReadImage();

  vtkSmartPointer< vtkContourFilter > contourFilter = vtkSmartPointer< vtkContourFilter >::New();
  contourFilter->SetInput( image->GetVTKImage() );
  contourFilter->ComputeNormalsOff();
  contourFilter->Update();

  float isovalue = 128.0;

  if( argc > 3 )
    {
    isovalue = atof( argv[3] );
    }

  contourFilter->SetValue( 0, isovalue );

  vtkSmartPointer< vtkPolyData > polyData = contourFilter->GetOutput();

  vtkPoints * pointArray = polyData->GetPoints();

  unsigned int numberOfPoints = static_cast< unsigned int>( pointArray->GetNumberOfPoints() );

  typedef itk::Point< double, 3 > PointType;
  typedef itk::Vector< double, 3 > VectorType;

  vtkFloatingPointType vtkpoint[3];
  PointType itkpoint;
  PointType transformedPoint;

  VectorType relativePosition;
  VectorType rotatedRelativePosition;

  typedef itk::ImageBase<3>  BaseImageType;

  const BaseImageType * imageBase = image->GetITKImageBase(); 

  typedef BaseImageType::DirectionType DirectionType;

  DirectionType directionMatrix = imageBase->GetDirection();
  PointType imageOrigin = imageBase->GetOrigin();

  std::cout << "Direction = " << directionMatrix << std::endl;

  //
  // This for-loop body probably should be replaced with a vtkTransformFilter
  //
  for( unsigned int pointId = 0; pointId < numberOfPoints; pointId++ )
    {
    pointArray->GetPoint( pointId, vtkpoint );
  
    for( unsigned int i=0; i<3; i++)
      {
      itkpoint[i] = vtkpoint[i];
      }

    relativePosition = itkpoint - imageOrigin;

    rotatedRelativePosition = directionMatrix * relativePosition;

    transformedPoint = imageOrigin + rotatedRelativePosition;

    for( unsigned int j=0; j<3; j++)
      {
      vtkpoint[j] = transformedPoint[j];
      }

    pointArray->SetPoint( pointId, vtkpoint );
    }

  std::string filenameExtension = 
    vtksys::SystemTools::GetFilenameLastExtension( argv[2] );

  if( filenameExtension == ".vtp" )
    {
    vtkSmartPointer< vtkXMLPolyDataWriter > contourWriter = vtkSmartPointer< vtkXMLPolyDataWriter >::New();
    contourWriter->SetInput( contourFilter->GetOutput() );
    contourWriter->SetFileName( argv[2] );
    contourWriter->Update();
    }

  if( filenameExtension == ".stl" )
    {
    vtkSmartPointer< vtkSTLWriter > contourWriter = vtkSmartPointer< vtkSTLWriter >::New();
    contourWriter->SetInput( contourFilter->GetOutput() );
    contourWriter->SetFileName( argv[2] );
    contourWriter->Update();
    }

  reader->Delete();
  image->Delete();

  return EXIT_SUCCESS;
}
