
#include <iostream>
#include "ImageSliceViewer.h"

#include "vtkImageData.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"

#include "InteractorObserver.h"

#include "itkPoint.h"

#include "ClickedPointEvent.h"



ImageSliceViewer
::ImageSliceViewer()
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

  this->SetOrientation( Axial );

  InteractorObserver * observer = InteractorObserver::New();
  observer->SetImageSliceViewer( this );
  m_InteractorObserver = observer;

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

//  m_SurfaceMapper->Delete();
//  m_SphereMapper->Delete();
}



ImageSliceViewer
::~ImageSliceViewer()
{
  if( m_RenderWindow )
    {
    m_RenderWindow->Delete();
    }   

  if( m_InteractorObserver )
    {
    m_InteractorObserver->Delete();
    }
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
ImageSliceViewer
::Render()
{
  m_Camera->SetClippingRange( 0.1, 100000 );
  m_RenderWindow->Render();
}


void
ImageSliceViewer
::SetInput( vtkImageData * image )
{
  m_Actor->SetInput( image );
  m_Renderer->AddActor( m_SphereActor );
  this->SetupCamera();
}



void
ImageSliceViewer
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
  switch( m_Orientation )
    {
  case Sagittal:
      {
      idx = 0;
      m_Camera->SetViewUp (     0,  0,  -1 );
      break;
      }
  case Coronal:
      {
      idx = 1;
      m_Camera->SetViewUp (     0,  0,  -1 );
      break;
      }
  case Axial:
      {
      idx = 2;
      m_Camera->SetViewUp (     0,  -1,  0 );
      break;
      }
    }

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
  ImageSliceViewer
::SetZoomFactor( double factor )
{
  m_ZoomFactor = factor;
}



void
  ImageSliceViewer
::SetInteractor( vtkRenderWindowInteractor * interactor )
{
  m_RenderWindow->SetInteractor( interactor );

  vtkInteractorStyleImage * interactorStyle = vtkInteractorStyleImage::New();
  interactor->SetInteractorStyle( interactorStyle );
  interactorStyle->Delete();
  interactor->AddObserver( ::vtkCommand::LeftButtonPressEvent, m_InteractorObserver );
  interactor->AddObserver( ::vtkCommand::LeftButtonReleaseEvent, m_InteractorObserver );
  interactor->AddObserver( ::vtkCommand::MouseMoveEvent, m_InteractorObserver );
}





void
  ImageSliceViewer
::SetOrientation( OrientationType orientation )
{
  m_Orientation = orientation;
  this->SetupCamera();
}




void
  ImageSliceViewer
::SelectSlice( int slice )
{
  if (!m_Actor->GetInput()) 
    {
    return;     // return, if no image is loaded yet.
    }

  int ext[6];
  m_Actor->GetInput()->GetExtent( ext );

  switch( m_Orientation )
    {
  case Sagittal:
      if ((slice>=ext[0]) && (slice<=ext[1]))
      {
        ext[0] = slice;
        ext[1] = slice;
        m_SliceNum = slice;
      }
      break;
  case Coronal:
      if ((slice>=ext[2]) && (slice<=ext[3]))
      {
        ext[2] = slice;
        ext[3] = slice;
        m_SliceNum = slice;
      }
      break;
  case Axial:
      if ((slice>=ext[4]) && (slice<=ext[5]))
      {
        ext[4] = slice;
        ext[5] = slice;
        m_SliceNum = slice;
      }
      break;
    }

  m_Actor->SetDisplayExtent( ext );
}


void
ImageSliceViewer
::SelectPoint( int x, int y )
{
  if (!m_Actor->GetInput()) 
    {
    return;     // return, if no image is loaded yet.
    }

  // Invert the y coordinate (vtk uses opposite y as FLTK)
  int* winsize = m_RenderWindow->GetSize();
  y = winsize[1] - y;

  // Convert display point to world point
  double wpoint[4];

  m_Renderer->SetDisplayPoint( x, y, 0 );
  m_Renderer->DisplayToWorld();
  m_Renderer->GetWorldPoint( wpoint );

  // Fix camera Z coorinate to match the current slice
  vtkFloatingPointType spacing[3]={1,1,1};
  vtkFloatingPointType origin[3] ={0,0,0};

  int dimensions[3] = { 100, 100, 100 };
  if ( m_Actor->GetInput() )
    {
    m_Actor->GetInput()->GetSpacing(spacing);
    m_Actor->GetInput()->GetOrigin(origin);
    m_Actor->GetInput()->GetDimensions(dimensions);
    }

  int idx = 0;
  switch( m_Orientation )
    {
  case Sagittal:
      {
      idx = 0;
      break;
      }
  case Coronal:
      {
      idx = 1;
      break;
      }
  case Axial:
      {
      idx = 2;
      break;
      }
    }
  double realz = m_SliceNum * spacing[idx] + origin[idx];
  wpoint[idx] = realz;

  // At this point we have 3D position in the variable wpoint
  this->SelectPoint(wpoint[0], wpoint[1], wpoint[2]);
  
 
  m_Notifier->InvokeEvent( ClickedPointEvent() );
}

void  
ImageSliceViewer::SelectPoint( double x, double y, double z )
{
  
  if (!m_Actor->GetInput()) 
    {
    return;     // return, if no image is loaded yet.
    }
  m_SelectPoint[0] = x;
  m_SelectPoint[1] = y;
  m_SelectPoint[2] = z;
  
  m_SphereActor->SetPosition( x, y, z );

  int dimensions[3] = { 100, 100, 100 };

  if ( m_Actor->GetInput() )
    {
    m_Actor->GetInput()->GetDimensions(dimensions);
    }
}

void 
ImageSliceViewer::GetSelectPoint(double data[3])
{
  for(int i=0; i<3; i++)
    {
    data[i] = m_SelectPoint[i];
    }
}


unsigned long 
ImageSliceViewer::AddObserver( const itk::EventObject & event, itk::Command * command)
{
  return m_Notifier->AddObserver( event, command );
}



void 
ImageSliceViewer
::SetSimplexMesh(vtkPolyData * mesh)
{
  m_SurfaceMapper->SetInput( mesh );
  m_Renderer->AddActor( m_SurfaceActor );
}

vtkPolyData*
ImageSliceViewer
::GetSimplexMesh()
{
  return m_SurfaceMapper->GetInput();
}
