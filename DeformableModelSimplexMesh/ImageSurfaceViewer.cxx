
#include <iostream>
#include "ImageSurfaceViewer.h"

#include "vtkImageData.h"
#include "vtkInteractorStyleImage.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkInteractorStyle.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkCellPicker.h"
#include "vtkImagePlaneWidget.h"
#include "vtkProperty.h"

//#include "InteractorObserver.h"

#include "ClickedPointEvent.h"



ImageSurfaceViewer
::ImageSurfaceViewer()
{
  m_Actor         = vtkImageActor::New();
  m_Renderer      = vtkRenderer::New();
  m_RenderWindow  = vtkRenderWindow::New();

  m_Renderer->AddActor( m_Actor );

  m_Actor->Delete();

  m_Camera = m_Renderer->GetActiveCamera();
  m_Camera->ParallelProjectionOn();

  m_RenderWindow->AddRenderer( m_Renderer );

  m_Renderer->Delete();
  /*
  InteractorObserver * observer = InteractorObserver::New();
  observer->SetImageSliceViewer( this );
  m_InteractorObserver = observer;*/

  m_Notifier      = itk::Object::New();

  m_NearPlane = 0.1;
  m_FarPlane  = 1000.0;

  m_ZoomFactor = 1.0;

  m_Renderer->SetBackground( 0, 0, 0 );

  m_Sphere       = vtkSphereSource::New();
  m_SphereActor  = vtkActor::New();  
  m_SphereMapper = vtkPolyDataMapper::New();

  m_SphereMapper->SetInput( m_Sphere->GetOutput() );  
  m_SphereActor->SetMapper( m_SphereMapper );
  m_SphereActor->GetProperty()->SetColor(0,1,0); // set to red


  m_SurfaceActor  = vtkActor::New();  
  m_SurfaceMapper = vtkPolyDataMapper::New();

  m_SurfaceActor->SetMapper( m_SurfaceMapper );
  m_SurfaceActor->GetProperty()->SetColor(1,0,0); // set to red

  m_PlaneWidgetX =  vtkImagePlaneWidget::New();
  m_PlaneWidgetY =  vtkImagePlaneWidget::New();
  m_PlaneWidgetZ =  vtkImagePlaneWidget::New();

//  m_SurfaceMapper->Delete();
//  m_SphereMapper->Delete();
}



ImageSurfaceViewer
::~ImageSurfaceViewer()
{
  if( m_RenderWindow )
    {
    m_RenderWindow->Delete();
    }   
  /*
  if( m_InteractorObserver )
    {
    m_InteractorObserver->Delete();
    }*/
  if( m_Sphere )
    {
    m_Sphere->Delete();
    }   

  if( m_SurfaceActor )
    {
    m_SurfaceActor->Delete();
    }    
}



void
ImageSurfaceViewer
::Render()
{
  // m_Camera->SetClippingRange( 0.1, 100000 );
  
  m_RenderWindow->Render();
}


void
ImageSurfaceViewer
::SetInput( vtkImageData * image )
{
  // m_Actor->SetInput( image );

  vtkFloatingPointType spacing[3];
  vtkFloatingPointType origin[3];
  int   dimensions[3];

  image->GetSpacing(spacing);
  image->GetOrigin(origin);
  image->GetDimensions(dimensions);

  // use cell picker
  
  vtkCellPicker * picker = vtkCellPicker::New();
  picker->SetTolerance(0.005);

  //assign default props to the ipw's texture plane actor
  vtkProperty * ipwProp = vtkProperty::New();
   
  // The 3 image plane widgets are used to probe the dataset.
  m_PlaneWidgetX->DisplayTextOn();
  m_PlaneWidgetX->SetInput(image);
  m_PlaneWidgetX->SetPlaneOrientationToXAxes();
  m_PlaneWidgetX->SetSliceIndex(dimensions[0]/2);
  m_PlaneWidgetX->SetPicker(picker);
  m_PlaneWidgetX->RestrictPlaneToVolumeOn();
  m_PlaneWidgetX->SetKeyPressActivationValue('x');
  m_PlaneWidgetX->GetPlaneProperty()->SetColor(1, 0, 0);
  m_PlaneWidgetX->SetTexturePlaneProperty(ipwProp);
  m_PlaneWidgetX->SetResliceInterpolateToNearestNeighbour();

  m_PlaneWidgetY->DisplayTextOn();
  m_PlaneWidgetY->SetInput(image);
  m_PlaneWidgetY->SetPlaneOrientationToYAxes();
  m_PlaneWidgetY->SetSliceIndex(dimensions[1]/2);
  m_PlaneWidgetY->SetPicker(picker);
  m_PlaneWidgetY->RestrictPlaneToVolumeOn();
  m_PlaneWidgetY->SetKeyPressActivationValue('y');
  m_PlaneWidgetY->GetPlaneProperty()->SetColor(1, 1, 0);
  m_PlaneWidgetY->SetTexturePlaneProperty(ipwProp);
  m_PlaneWidgetY->SetLookupTable(m_PlaneWidgetX->GetLookupTable());

  m_PlaneWidgetZ->DisplayTextOn();
  m_PlaneWidgetZ->SetInput(image);
  m_PlaneWidgetZ->SetPlaneOrientationToZAxes();
  m_PlaneWidgetZ->SetSliceIndex(dimensions[2]/2);
  m_PlaneWidgetZ->SetPicker(picker);
  m_PlaneWidgetZ->SetKeyPressActivationValue('z');
  m_PlaneWidgetZ->GetPlaneProperty()->SetColor(0, 0, 1);
  m_PlaneWidgetZ->SetTexturePlaneProperty(ipwProp);
  m_PlaneWidgetZ->SetLookupTable(m_PlaneWidgetX->GetLookupTable());

  m_PlaneWidgetX->SetInteractor( m_RenderWindow->GetInteractor());
  m_PlaneWidgetX->On();
   
  m_PlaneWidgetY->SetInteractor( m_RenderWindow->GetInteractor());
  m_PlaneWidgetY->On();
   
  m_PlaneWidgetZ->SetInteractor(m_RenderWindow->GetInteractor());
  m_PlaneWidgetZ->On();


  // m_Renderer->AddActor( m_SphereActor );
  //
  this->SetupCamera();
  m_Renderer->ResetCamera();
  this->Render();
}



void
ImageSurfaceViewer
::SetupCamera()
{

  vtkImageData * image = m_Actor->GetInput();

  if ( !image )
    {
    return;
    }

  vtkFloatingPointType spacing[3];
  vtkFloatingPointType origin[3];
  int   dimensions[3];
  
  
  image->GetSpacing(spacing);
  image->GetOrigin(origin);
  image->GetDimensions(dimensions);
  
  double focalPoint[3];
  double position[3];
  for ( unsigned int cc = 0; cc < 3; cc++)
    {
    focalPoint[cc] = origin[cc] + ( spacing[cc] * dimensions[cc] ) / 2.0;
    position[cc]   = focalPoint[cc];
    }

  int idx = 0;
  m_Camera->SetViewUp (     0,  0,  -1 );

  const double distanceToFocalPoint = 1000;
  position[idx] += distanceToFocalPoint;

  m_Camera->SetPosition (   position );
  m_Camera->SetFocalPoint ( focalPoint );

#define myMAX(x,y) (((x)>(y))?(x):(y))  

   int d1 = (idx + 1) % 3;
   int d2 = (idx + 2) % 3;
 
  double max = myMAX( 
    spacing[d1] * dimensions[d1],
    spacing[d2] * dimensions[d2]);


  m_Camera->SetParallelScale( max / 2  * m_ZoomFactor ); 

  
}


void
  ImageSurfaceViewer
::SetZoomFactor( double factor )
{
  m_ZoomFactor = factor;
}



void
  ImageSurfaceViewer
::SetInteractor( vtkRenderWindowInteractor * interactor )
{
  
  m_RenderWindow->SetInteractor( interactor);
  
 
  vtkInteractorStyleTrackballCamera * interactorStyle = vtkInteractorStyleTrackballCamera::New();
  interactor->SetInteractorStyle( interactorStyle );
  interactorStyle->Delete();

   
  //interactor->AddObserver( ::vtkCommand::LeftButtonPressEvent, m_InteractorObserver );
  //interactor->AddObserver( ::vtkCommand::LeftButtonReleaseEvent, m_InteractorObserver );
  //interactor->AddObserver( ::vtkCommand::MouseMoveEvent, m_InteractorObserver );
}

unsigned long 
ImageSurfaceViewer::AddObserver( const itk::EventObject & event, itk::Command * command)
{
  return m_Notifier->AddObserver( event, command );
}



void 
ImageSurfaceViewer
::SetSimplexMesh(vtkPolyData * mesh)
{
  m_SurfaceMapper->SetInput( mesh );
  m_SurfaceActor->SetMapper( m_SurfaceMapper );
  m_Renderer->AddActor( m_SurfaceActor );
  m_Renderer->RemoveActor(m_SphereActor);
  this->Render();
}

vtkPolyData*
ImageSurfaceViewer
::GetSimplexMesh()
{
  return m_SurfaceMapper->GetInput();
}
