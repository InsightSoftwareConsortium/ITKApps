/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkReadITKImage3DSegmentShowVTK.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkCommand.h"
#include "itkImage.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "itkConfidenceConnectedImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "vtkImageImport.h"
#include "vtkImageExport.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkContourFilter.h"
#include "vtkImageData.h"
#include "vtkDataSet.h"
#include "vtkProperty.h"
#include "vtkImagePlaneWidget.h"
#include "vtkCellPicker.h"
#include "vtkPolyDataWriter.h"


/**
 * This function will connect the given itk::VTKImageExport filter to
 * the given vtkImageImport filter.
 */
template <typename ITK_Exporter, typename VTK_Importer>
void ConnectPipelines(ITK_Exporter exporter, VTK_Importer* importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

/**
 * This function will connect the given vtkImageExport filter to
 * the given itk::VTKImageImport filter.
 */
template <typename VTK_Exporter, typename ITK_Importer>
void ConnectPipelines(VTK_Exporter* exporter, ITK_Importer importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}


/**
 * This program implements an example connection between ITK and VTK
 * pipelines.  The combined pipeline flows as follows:
 *
 * itkImageFileReader ==> itkVTKImageExport ==>
 *    vtkImageImport ==> vtkImagePlaneWidget
 *
 * The resulting vtkImagePlaneWidget is displayed in a vtkRenderWindow.
 * Whenever the VTK pipeline executes, information is propagated
 * through the ITK pipeline.  If the ITK pipeline is out of date, it
 * will re-execute and cause the VTK pipeline to update properly as
 * well.
 */
int main(int argc, char * argv [] )
{  

  // Load a color image using ITK and display it with VTK

  if( argc < 2 )
    {
    std::cerr << "Missing parameters" << std::endl;
    std::cerr << "Usage: " << argv[0] << " inputImageFilename [seedX seedY seedZ] [output.vtk]" << std::endl;
    return 1;
    }
  
  try
    {
    typedef unsigned char PixelType;
    const unsigned int Dimension = 3;
    typedef itk::Image< PixelType, Dimension > ImageType;
    
    typedef itk::ImageFileReader< ImageType > ReaderType;

    ReaderType::Pointer reader  = ReaderType::New();
    reader->SetFileName( argv[1] );
    reader->Update();

    typedef itk::ConfidenceConnectedImageFilter<ImageType,ImageType> SegmentationFilterType;

    SegmentationFilterType::Pointer filter = SegmentationFilterType::New();

    filter->SetInput( reader->GetOutput() );

    filter->SetNumberOfIterations(2);
    filter->SetReplaceValue(255);
    filter->SetMultiplier(2.5);


    // Obtain center index of the image
    // 
    ImageType::Pointer inputImage = reader->GetOutput();
    ImageType::SizeType  size  = inputImage->GetBufferedRegion().GetSize();
    ImageType::IndexType start = inputImage->GetBufferedRegion().GetIndex();

    // set a seed by default in the center of the image.
    ImageType::IndexType seed;
    seed[0] = start[0] + size[0] / 2;
    seed[1] = start[1] + size[1] / 2;
    seed[2] = start[2] + size[2] / 2;

    if( argc >= 4 )
      {
      seed[0] = atoi( argv[2] );
      seed[1] = atoi( argv[3] );
      seed[2] = atoi( argv[4] );
      }

    filter->SetSeed( seed );
      
    
    typedef itk::VTKImageExport< ImageType > ExportFilterType;
    ExportFilterType::Pointer itkExporter1 = ExportFilterType::New();
    ExportFilterType::Pointer itkExporter2 = ExportFilterType::New();

    itkExporter1->SetInput( reader->GetOutput() );
    itkExporter2->SetInput( filter->GetOutput() );

    // Create the vtkImageImport and connect it to the
    // itk::VTKImageExport instance.
    vtkImageImport* vtkImporter1 = vtkImageImport::New();  
    ConnectPipelines(itkExporter1, vtkImporter1);
    
    vtkImageImport* vtkImporter2 = vtkImageImport::New();  
    ConnectPipelines(itkExporter2, vtkImporter2);
    

     
    //------------------------------------------------------------------------
    // VTK pipeline.
    //------------------------------------------------------------------------

    // Create a renderer, render window, and render window interactor to
    // display the results.
    vtkRenderer* renderer = vtkRenderer::New();
    vtkRenderWindow* renWin = vtkRenderWindow::New();
    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();

    renWin->SetSize(500, 500);
    renWin->AddRenderer(renderer);
    iren->SetRenderWindow(renWin);
    

    // use cell picker for interacting with the image orthogonal views.
    //
    vtkCellPicker * picker = vtkCellPicker::New();
    picker->SetTolerance(0.005);


    //assign default props to the ipw's texture plane actor
    vtkProperty * ipwProp = vtkProperty::New();
     

    // Create 3 orthogonal view using the ImagePlaneWidget
    //
    vtkImagePlaneWidget * xImagePlaneWidget =  vtkImagePlaneWidget::New();
    vtkImagePlaneWidget * yImagePlaneWidget =  vtkImagePlaneWidget::New();
    vtkImagePlaneWidget * zImagePlaneWidget =  vtkImagePlaneWidget::New();

    // The 3 image plane widgets are used to probe the dataset.
    //
    xImagePlaneWidget->DisplayTextOn();
    xImagePlaneWidget->SetInput(vtkImporter1->GetOutput());
    xImagePlaneWidget->SetPlaneOrientationToXAxes();
    xImagePlaneWidget->SetSliceIndex(size[0]/2);
    xImagePlaneWidget->SetPicker(picker);
    xImagePlaneWidget->RestrictPlaneToVolumeOn();
    xImagePlaneWidget->SetKeyPressActivationValue('x');
    xImagePlaneWidget->GetPlaneProperty()->SetColor(1, 0, 0);
    xImagePlaneWidget->SetTexturePlaneProperty(ipwProp);
    xImagePlaneWidget->SetResliceInterpolateToNearestNeighbour();

    yImagePlaneWidget->DisplayTextOn();
    yImagePlaneWidget->SetInput(vtkImporter1->GetOutput());
    yImagePlaneWidget->SetPlaneOrientationToYAxes();
    yImagePlaneWidget->SetSliceIndex(size[1]/2);
    yImagePlaneWidget->SetPicker(picker);
    yImagePlaneWidget->RestrictPlaneToVolumeOn();
    yImagePlaneWidget->SetKeyPressActivationValue('y');
    yImagePlaneWidget->GetPlaneProperty()->SetColor(1, 1, 0);
    yImagePlaneWidget->SetTexturePlaneProperty(ipwProp);
    yImagePlaneWidget->SetLookupTable(xImagePlaneWidget->GetLookupTable());

    zImagePlaneWidget->DisplayTextOn();
    zImagePlaneWidget->SetInput(vtkImporter1->GetOutput());
    zImagePlaneWidget->SetPlaneOrientationToZAxes();
    zImagePlaneWidget->SetSliceIndex(size[2]/2);
    zImagePlaneWidget->SetPicker(picker);
    zImagePlaneWidget->SetKeyPressActivationValue('z');
    zImagePlaneWidget->GetPlaneProperty()->SetColor(0, 0, 1);
    zImagePlaneWidget->SetTexturePlaneProperty(ipwProp);
    zImagePlaneWidget->SetLookupTable(xImagePlaneWidget->GetLookupTable());

    xImagePlaneWidget->SetInteractor( iren );
    xImagePlaneWidget->On();
     
    yImagePlaneWidget->SetInteractor( iren );
    yImagePlaneWidget->On();
     
    zImagePlaneWidget->SetInteractor( iren );
    zImagePlaneWidget->On();


    // Set the background to something grayish
    renderer->SetBackground(0.4392, 0.5020, 0.5647);


    // Draw contours around the segmented regions
    vtkContourFilter * contour = vtkContourFilter::New();
    contour->SetInput( vtkImporter2->GetOutput() );
    contour->SetValue(0, 128); // edges of a binary image with values 0,255


    vtkPolyDataMapper * polyMapper = vtkPolyDataMapper::New();
    vtkActor          * polyActor  = vtkActor::New();

    polyActor->SetMapper( polyMapper );
    polyMapper->SetInput( contour->GetOutput() );

    vtkProperty * property = vtkProperty::New();
    property->SetAmbient(0.1);
    property->SetDiffuse(0.1);
    property->SetSpecular(0.5);
    property->SetColor(1.0,0.0,0.0);
    property->SetLineWidth(2.0);
    property->SetRepresentationToWireframe();

    polyActor->SetProperty( property );
  
    renderer->AddActor( polyActor );
    
    if( argc >=5 )
      {
      vtkPolyDataWriter * writer = vtkPolyDataWriter::New();
      writer->SetFileName(argv[5]);
      writer->SetInput( contour->GetOutput() );
      writer->Write();
      }
 
    // Bring up the render window and begin interaction.
    renWin->Render();
    iren->Start();

    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << "Exception catched !! " << e << std::endl;
    }


  
  return 0;
}
