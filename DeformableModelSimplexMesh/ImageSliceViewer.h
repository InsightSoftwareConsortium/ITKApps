
#ifndef __ImageSliceViewer_h__
#define __ImageSliceViewer_h__

#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkImageActor.h"
#include "vtkRenderWindow.h"
#include "vtkCommand.h"
#include "vtkInteractorStyleImage.h"
#include "vtkSphereSource.h" 
#include "vtkPolyDataMapper.h" 
#include "vtkPolyData.h" 

#include "itkEventObject.h"
#include "itkCommand.h"

#ifndef vtkFloatingPointType
# define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

  /** 
  \class ImageSlicerViewer
  \brief This class visualizes slices of a loaded volume. The slice orientation
  can be set as AXIAL, CORONAL or SAGGITAL. 
  This class also supports recording a mouse click at any arbitrary point in the 
  loaded volume. This point may be used as a seed point in the segmentation process.
    \warning
  \sa 
  */

class ImageSliceViewer
{
public:

  /** 
  Orientation Type: Publically declared orientation types supported for slice viewing.
  */
  typedef enum
    { 
    Sagittal, 
    Coronal, 
    Axial 
    } 
    OrientationType;

  ImageSliceViewer( void );

  virtual ~ImageSliceViewer( void );

  /**
  The SetInput method provides pointer to the image data to be viewed. 
  */
  void SetInput( vtkImageData * image );

  /**
  The SelectSlice method provides slice number of the image data to be viewed
  in the set orientation type. 
  */
  void SelectSlice( int slice );

  /**
  The SetInteractor method provides an Interactor for the viewing window.
  */
  virtual void SetInteractor( vtkRenderWindowInteractor * interactor );

  /**
  The Render method updates the view window.
  */
  void Render( void );

  /**
  The SetOrientation method sets the orientation type of slices to be viewed.
  */
  void SetOrientation( OrientationType orientation );

  /**
  The SelectPoint method inputs a select point in the viewing volume. Usually
  this method is used for synchronizing the selected point with other ImageSliceView
  windows.
  */
  virtual void SelectPoint( double x, double y, double z );

  /**
  The GetSelectPoint method provides co-ordinates of the selected point 
  in the volume.
  */

  virtual void  GetSelectPoint(double data[3]); 
  
  /**
  The SelectPoint method with two parameters is used for transforming 
  points from screen co-ordinate space to the image co-ordinate space.
  The screen co-ordinates are usually obtained through mouse click over
  the image slice window.
  */
  void SelectPoint( int x, int y);

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

  OrientationType     m_Orientation;

  int                 m_SliceNum;

  vtkCommand        * m_InteractorObserver;

  itk::Object::Pointer   m_Notifier;

  double              m_NearPlane;

  double              m_FarPlane;

  double              m_ZoomFactor;

  double               m_SelectPoint[3];

  vtkActor          * m_SurfaceActor;
  vtkActor          * m_SphereActor;

  vtkSphereSource   * m_Sphere;
  vtkPolyDataMapper * m_SurfaceMapper;
  vtkPolyDataMapper * m_SphereMapper;

  
};



#endif



