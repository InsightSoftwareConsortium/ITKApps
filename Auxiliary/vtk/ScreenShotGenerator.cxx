/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ScreenShotGenerator.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkJPEGWriter.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"
#include "vtkSmartPointer.h"
#include "vtkWindowToImageFilter.h"
#include "vtkLightKit.h"

#include <vtksys/SystemTools.hxx>

int main(int argc, char * argv [] ) 
{

  if( argc < 5 )
    {
    std::cerr << "Error: missing arguments" << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0];
    std::cerr << " inputSurface outputScreenshotname.jpg textAnnotation interactive" << std::endl;
    return EXIT_FAILURE;
    }

  std::string inputSurfaceFileName = argv[1];

  if( ! vtksys::SystemTools::FileExists( inputSurfaceFileName.c_str() ) )
    {
    std::cerr << "Error " << std::endl;
    std::cerr << "The following file doesn't exist " << std::endl;
    std::cerr << inputSurfaceFileName << std::endl;
    return EXIT_FAILURE;
    }


  vtkSmartPointer< vtkActor > actor = vtkSmartPointer<vtkActor>::New();

  vtkSmartPointer<vtkProperty> property  = vtkSmartPointer<vtkProperty>::New();

  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();

  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();

  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
     vtkSmartPointer<vtkRenderWindowInteractor>::New();

  vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();

  std::string baseAnnotationText = argv[3];

  vtkTextProperty * textProperty = textActor->GetTextProperty();

  textProperty->SetFontSize(24);
  textProperty->SetFontFamilyToArial();
  textProperty->SetJustificationToLeft();
  textProperty->BoldOff();
  textProperty->ItalicOff();
  textProperty->ShadowOff();
  textProperty->SetColor(0,0,0);

  textActor->SetDisplayPosition(5,5);

  textActor->SetInput( baseAnnotationText.c_str() );

  vtkSmartPointer< vtkWindowToImageFilter > windowToImageFilter =
    vtkSmartPointer< vtkWindowToImageFilter >::New();

  vtkSmartPointer< vtkJPEGWriter > writer = vtkSmartPointer< vtkJPEGWriter >::New();

  vtkSmartPointer< vtkXMLPolyDataReader > reader = vtkSmartPointer< vtkXMLPolyDataReader >::New();

  reader->SetFileName( argv[1] );

  windowToImageFilter->SetInput( renderWindow );

  writer->SetInput( windowToImageFilter->GetOutput() );
  

  renderWindow->SetSize( 1024, 1024);

  renderer->SetViewport(0, 0, 1.0, 1.0);

  renderWindow->AddRenderer( renderer );

  renderWindowInteractor->SetRenderWindow( renderWindow );

  renderer->SetBackground(0.8, 0.9, 0.9);

  actor->SetMapper( mapper );
  mapper->ScalarVisibilityOff();

  mapper->SetInput( reader->GetOutput() );

  property->SetAmbient(0.0);
  property->SetDiffuse(1.0);
  property->SetSpecular(0.1);
  property->SetSpecularPower(100);
  property->SetColor(1,1,1);
  property->SetRepresentationToSurface();

  actor->SetProperty( property );

  renderer->AddActor( actor );

  renderer->AddViewProp( textActor );

  renderer->ResetCamera();

  vtkCamera * camera = renderer->GetActiveCamera();

  const double zoomFactor = 1.2;

  camera->Zoom( zoomFactor );
  camera->Elevation( 90.0 );

  vtkSmartPointer< vtkLightKit > lightKit = vtkSmartPointer< vtkLightKit >::New();

  lightKit->AddLightsToRenderer( renderer );

  renderWindow->Render();
  renderWindowInteractor->Initialize();

  const unsigned int interactive = atoi( argv[4] );
  if( interactive == 1 )
    {
    renderWindowInteractor->Start();
    }

  writer->SetFileName( argv[2] );
  
  windowToImageFilter->Update();
  renderWindow->Render();
  
  writer->Write();

  return EXIT_SUCCESS;
}

