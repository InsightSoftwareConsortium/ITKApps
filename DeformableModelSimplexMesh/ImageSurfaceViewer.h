
#ifndef __ImageSurfaceViewer_h__
#define __ImageSurfaceViewer_h__

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkRenderWindow.h"
#include "vtkCommand.h"
#include "vtkInteractorStyleImage.h"
#include "vtkSphereSource.h" 
#include "vtkPolyDataMapper.h" 
#include "vtkPolyData.h" 
#include "vtkImagePlaneWidget.h"

#include "itkEventObject.h"
#include "itkCommand.h"

#ifndef vtkFloatingPointType
# define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

  /** 
  \class ImageSurfacerViewer
  \brief 
    \warning
  \sa 
  */

class ImageSurfaceViewer
{
public:


  ImageSurfaceViewer( void );

  virtual ~ImageSurfaceViewer( void );

  /**
  The SetInput method provides pointer to the image data to be viewed. 
  */
  void SetInput( vtkImageData * image );

 
  /**
  The SetInteractor method provides an Interactor for the viewing window.
  */
  virtual void SetInteractor( vtkRenderWindowInteractor * interactor );

  /**
  The Render method updates the view window.
  */
  void Render( void );

  /**
  The AddObserver method attaches a "itk command" to be performed as a result
  of occurrence of an "itk event".
  */
  unsigned long AddObserver( const itk::EventObject & event, itk::Command *);

  virtual void SetZoomFactor( double factor );

  void SetSimplexMesh(vtkPolyData * mesh);

  vtkPolyData* GetSimplexMesh();
  

protected:

 /**
 The SetUpCamera method sets up the camera parameters according to the
 image dimensions and the orientation type chosen for slice viewing.
 */
  void SetupCamera( void );

public:

  vtkImageActor     * m_Actor;

  vtkRenderer       * m_Renderer;

  vtkCamera         * m_Camera;

  vtkRenderWindow   * m_RenderWindow;

  vtkCommand        * m_InteractorObserver;

  itk::Object::Pointer   m_Notifier;

  double              m_NearPlane;

  double              m_FarPlane;

  double              m_ZoomFactor;

  vtkActor          * m_SurfaceActor;
  vtkActor          * m_SphereActor;

  vtkSphereSource   * m_Sphere;
  vtkPolyDataMapper * m_SurfaceMapper;
  vtkPolyDataMapper * m_SphereMapper;

  vtkImagePlaneWidget  * m_PlaneWidgetX;
  vtkImagePlaneWidget  * m_PlaneWidgetY;
  vtkImagePlaneWidget  * m_PlaneWidgetZ;

  
};



#endif



