/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkReadITKImageShowVTK.cxx
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
#include "itkCurvatureFlowImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRGBPixel.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "vtkImageData.h"

#include "vtkImageImport.h"
#include "vtkImageExport.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleImage.h" 

/**
 * This will be setup as a callback for a progress event on an ITK
 * filter.
 */
struct ProgressDisplay
{
  ProgressDisplay(itk::ProcessObject* process): m_Process(process) {}
  
  void Display()
    {
    float progress = m_Process->GetProgress()*100.0;
    std::cout << "Progress " << progress << " percent." << std::endl;
    }
  
  itk::ProcessObject::Pointer m_Process;
};


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
 *    vtkImageImport ==> vtkImageActor
 *
 * The resulting vtkImageActor is displayed in a vtkRenderWindow.
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
    std::cerr << "Usage: " << argv[0] << " inputImageFilename " << std::endl;
    return 1;
    }
  
  try
    {
    typedef itk::RGBPixel< unsigned char > PixelType;
    typedef itk::Image< PixelType, 2 > ImageType;
    
    typedef itk::ImageFileReader< ImageType > ReaderType;

    ReaderType::Pointer reader  = ReaderType::New();
    reader->SetFileName( argv[1] );
    reader->Update();

    typedef itk::VTKImageExport< ImageType > ExportFilterType;
    ExportFilterType::Pointer itkExporter = ExportFilterType::New();

    itkExporter->SetInput( reader->GetOutput() );

    // Create the vtkImageImport and connect it to the
    // itk::VTKImageExport instance.
    vtkImageImport* vtkImporter = vtkImageImport::New();  
    ConnectPipelines(itkExporter, vtkImporter);
    

    // Just for double checking export it from VTK back into ITK 
    // and save it into a file.
    typedef itk::VTKImageImport< ImageType > ImportFilterType;
    ImportFilterType::Pointer itkImporter = ImportFilterType::New();


    vtkImageExport* vtkExporter = vtkImageExport::New();  
    ConnectPipelines(vtkExporter, itkImporter);
    
    vtkExporter->SetInput( vtkImporter->GetOutput() );
    
    typedef itk::ImageFileWriter< ImageType > WriterType;
    WriterType::Pointer itkWriter = WriterType::New();
    itkWriter->SetInput( itkImporter->GetOutput() );
    
    if( argc > 2 )
      {
      const char * filename = argv[2];
      std::cout << "Writing file " << filename << std::endl;
      itkWriter->SetFileName( filename );
      itkWriter->Update();
      }

    //------------------------------------------------------------------------
    // VTK pipeline.
    //------------------------------------------------------------------------
    
    // Create a vtkImageActor to help render the image.  Connect it to
    // the vtkImporter instance.
    vtkImageActor* actor = vtkImageActor::New();
    actor->SetInput(vtkImporter->GetOutput());
    
    vtkInteractorStyleImage * interactorStyle = vtkInteractorStyleImage::New();

    // Create a renderer, render window, and render window interactor to
    // display the results.
    vtkRenderer* renderer = vtkRenderer::New();
    vtkRenderWindow* renWin = vtkRenderWindow::New();
    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
    
    renWin->SetSize(500, 500);
    renWin->AddRenderer(renderer);
    iren->SetRenderWindow(renWin);
    iren->SetInteractorStyle( interactorStyle );
    
    // Add the vtkImageActor to the renderer for display.
    renderer->AddActor(actor);
    renderer->SetBackground(0.4392, 0.5020, 0.5647);

    // Bring up the render window and begin interaction.
    renWin->Render();
    iren->Start();

    // Release all VTK components
    actor->Delete();
    interactorStyle->Delete(); 
    vtkImporter->Delete();
    vtkExporter->Delete();
    renWin->Delete();
    renderer->Delete();
    iren->Delete();

    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << "Exception catched !! " << e << std::endl;
    }


  
  return 0;
}
