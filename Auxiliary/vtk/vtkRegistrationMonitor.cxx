/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    vtkRegistrationMonitor.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkRegistrationMonitor.h"

#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkMatrix4x4.h"
#include "vtkPNGWriter.h"
#include "vtkWindowToImageFilter.h"

#define DeleteIfNotNullMacro(x) \
  if( this->x ) this->x->Delete();

/** Constructor */
vtkRegistrationMonitor::vtkRegistrationMonitor()
{
  this->FixedSurface  = NULL; // connected externally
  this->MovingSurface = NULL; // connected externally

  // Variable for the conversion of the ITK transform into VTK Matrix
  this->Matrix     = vtkMatrix4x4::New();

  // Own member objects
  this->FixedActor     = vtkActor::New();
  this->FixedProperty  = vtkProperty::New();
  this->FixedMapper    = vtkPolyDataMapper::New();

  this->MovingActor    = vtkActor::New();
  this->MovingProperty = vtkProperty::New();
  this->MovingMapper   = vtkPolyDataMapper::New();

  this->Renderer                 = vtkRenderer::New();
  this->RenderWindow             = vtkRenderWindow::New();
  this->RenderWindowInteractor   = vtkRenderWindowInteractor::New();

//  this->WindowToImageFilter      = vtkWindowToImageFilter::New();
//  this->ScreenShotWriter         = vtkPNGWriter::New();

  // ITK Objects, does not require to call Delete()
  this->StartObserver       = ObserverType::New();
  this->IterationObserver   = ObserverType::New();

  this->StartObserver->SetCallbackFunction( this, & Self::StartVisualization );

  this->IterationObserver->SetCallbackFunction( this, & Self::Update );

  this->CurrentIterationNumber = 0;
  this->NumberOfIterationPerUpdate = 1;
}

/** Destructor */
vtkRegistrationMonitor::~vtkRegistrationMonitor()
{
  DeleteIfNotNullMacro( Matrix );

  DeleteIfNotNullMacro( Renderer );
  DeleteIfNotNullMacro( RenderWindow );
  DeleteIfNotNullMacro( RenderWindowInteractor );

  DeleteIfNotNullMacro( FixedActor );
  DeleteIfNotNullMacro( FixedProperty );
  DeleteIfNotNullMacro( FixedMapper );

  DeleteIfNotNullMacro( MovingActor );
  DeleteIfNotNullMacro( MovingProperty );
  DeleteIfNotNullMacro( MovingMapper );

//  DeleteIfNotNullMacro( WindowToImageFilter );
//  DeleteIfNotNullMacro( ScreenShotWriter );
}

/** Set the Fixed Surface */
void vtkRegistrationMonitor::SetFixedSurface(vtkPolyData* surface)
{
  this->FixedSurface = surface;
}

/** Set the Moving Surface */
void vtkRegistrationMonitor::SetMovingSurface(vtkPolyData* surface)
{
  this->MovingSurface = surface;
}

/** Set directory for saving screenshots */
void vtkRegistrationMonitor::SetScreenshotOutputDirectory( const char * directory )
{
  this->ScreenshotOutputDirectory = directory;
}

/** Set base of the screenshots filename */
void vtkRegistrationMonitor::SetScreenshotBaseName( const char * filenamebase )
{
  this->ScreenshotBaseName = filenamebase;
}

/** Set transform */
void vtkRegistrationMonitor
::Observe( OptimizerType * optimizer, TransformType * transform )
{
  this->ObservedOptimizer = optimizer;
  this->ObservedTransform = transform;

  this->ObservedOptimizer->AddObserver( 
    itk::StartEvent(), this->StartObserver     );

  this->ObservedOptimizer->AddObserver( 
    itk::IterationEvent(), this->IterationObserver );
}

/** Callback for the StartEvent */
void vtkRegistrationMonitor::StartVisualization()
{
  // flag for preventing the pipeline from being created
  // multiple times:
  static bool visualizationPipelineInitialized = false;

  this->CurrentIterationNumber = 0;
  this->CurrentScreenshotNumber = 0;

  if( visualizationPipelineInitialized )
    {
    return;
    }
  
  // Setup the RenderWindow and its basic infrastructure
  this->RenderWindow->SetSize( 600, 600 );
  this->RenderWindow->AddRenderer( this->Renderer );
  this->RenderWindowInteractor->SetRenderWindow( this->RenderWindow );

  // Set the background to something grayish
  this->Renderer->SetBackground(0.4392, 0.5020, 0.5647);

  vtkCamera * camera = this->Renderer->GetActiveCamera();

  camera->SetPosition( 200, 200, 150 );
  camera->SetViewUp( 0, 0, 1 );
  camera->SetClippingRange( 0.1, 10000.0 );
  camera->SetFocalPoint( 90.0, 108.0, 90.0 );

  // Setup the Fixed Surface infrastructure
  this->FixedActor->SetMapper( this->FixedMapper );
  this->FixedMapper->SetInput( this->FixedSurface );
  this->FixedMapper->ScalarVisibilityOff();

  this->FixedProperty->SetAmbient(0.1);
  this->FixedProperty->SetDiffuse(0.1);
  this->FixedProperty->SetSpecular(0.5);
  this->FixedProperty->SetColor(0.785,0.0,0.0);
  this->FixedProperty->SetLineWidth(2.0);
  this->FixedProperty->SetRepresentationToSurface();

  this->FixedActor->SetProperty( this->FixedProperty );


  // Setup the Moving Surface infrastructure
  this->MovingActor->SetMapper( this->MovingMapper );
  this->MovingMapper->SetInput( this->MovingSurface );
  this->MovingMapper->ScalarVisibilityOff();

  this->MovingProperty->SetAmbient(0.1);
  this->MovingProperty->SetDiffuse(0.1);
  this->MovingProperty->SetSpecular(0.5);
  this->MovingProperty->SetColor(0.0,0.0,0.785);
  this->MovingProperty->SetLineWidth(2.0);
  this->MovingProperty->SetRepresentationToSurface();

  this->MovingActor->SetProperty( this->MovingProperty );


  // Connecting the Fixed and Moving surfaces to the 
  // visualization pipeline
  this->Renderer->AddActor( this->FixedActor );
  this->Renderer->AddActor( this->MovingActor );

  // Connect the filters for generating screenshots 
//  this->WindowToImageFilter->SetInput( this->RenderWindow );
//  this->ScreenShotWriter->SetInput( this->WindowToImageFilter->GetOutput() );

  // Bring up the render window and begin interaction.
  this->Renderer->ResetCamera();
  this->RenderWindow->Render();
  this->RenderWindowInteractor->Initialize();

  visualizationPipelineInitialized = true;
}

/** Update the Visualization */
void vtkRegistrationMonitor
::SetNumberOfIterationPerUpdate( unsigned int number )
{
  this->NumberOfIterationPerUpdate = number;
}

/** Update the Visualization */
void vtkRegistrationMonitor::Update()
{

  if( this->CurrentIterationNumber % 
      this->NumberOfIterationPerUpdate )
    {
    this->CurrentIterationNumber++;
    return;
    }
  else
    {
    this->CurrentIterationNumber++;
    }

  TransformType::MatrixType matrix = 
    this->ObservedTransform->GetMatrix();

  TransformType::OffsetType offset = 
    this->ObservedTransform->GetOffset();

  for(unsigned int i=0; i<3; i++ )
    {
    for(unsigned int j=0; j<3; j++ )
      {
      this->Matrix->SetElement(i,j,
        matrix.GetVnlMatrix().get(i,j));   
      }

    this->Matrix->SetElement( i, 3, offset[i]);
    }

  //
  // Due to the direction of the transform in the ITK
  // registration framework, this transform must be 
  // applied to the FixedActor:
  //
  this->FixedActor->SetUserMatrix( this->Matrix );
 
  this->Renderer->ResetCamera();

  //
  //  Generate the screenshot
  //
  std::ostringstream screenshotFileName;
  screenshotFileName << this->ScreenshotOutputDirectory << "/";
  screenshotFileName << this->ScreenshotBaseName;
  screenshotFileName << setw(3) << setfill('0');
  screenshotFileName << this->CurrentScreenshotNumber;
  screenshotFileName << ".png";
 
  std::cout << "screenshotFileName " << screenshotFileName.str() << std::endl;
  
//  this->WindowToImageFilter->Update();

  this->RenderWindowInteractor->Render();

//  this->ScreenShotWriter->SetFileName( screenshotFileName.str().c_str() );
//  this->ScreenShotWriter->Write();


  //
  // Section copied from IGSTK : igstkView class.
  // Trying to aviod the bug of the first image being saved at every iteration.
  //
  vtkWindowToImageFilter * windowToImageFilter = vtkWindowToImageFilter::New();

  vtkPNGWriter * writer = vtkPNGWriter::New();

  windowToImageFilter->SetInput( this->RenderWindow );

  windowToImageFilter->Update();

  writer->SetInput( windowToImageFilter->GetOutput() );
  
  writer->SetFileName( screenshotFileName.str().c_str() );
  
  this->RenderWindow->Render();
  
  writer->Write();

  writer->SetInput( NULL );
  windowToImageFilter->SetInput( NULL );

  windowToImageFilter->Delete();
  writer->Delete();

  this->CurrentScreenshotNumber++;
}
