/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    Window3D.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "Window3D.h"
#include "IRISImageData.h"
#include "SNAPImageData.h"
#include "UserInterfaceLogic.h"
#include "GlobalState.h"
#include <iostream>
#include "IRISApplication.h"
#include "ImageRayIntersectionFinder.h"

#include <FL/glut.h>
                         
/** These classes are used internally for m_Ray intersection testing */
class LabelImageHitTester 
{
public:
  LabelImageHitTester(const IRISImageData *irisData = NULL)
  {
    m_IRISData = irisData;
  }

  int operator()(LabelType label) const
  {
    assert(m_IRISData);
    const ColorLabel &cl = m_IRISData->GetColorLabel(label);
    return cl.IsValid() && cl.IsVisible() ? 1 : 0;
  }

private:
  const IRISImageData *m_IRISData;
};

class SnakeImageHitTester 
{
public:
  int operator()(float levelSetValue) const
  {
    return levelSetValue <= 0 ? 1 : 0;
  }
};

/**
 * \class Trackball3DInteractionMode
 * \brief 3D interaction mode that takes care of 3D rotation and zoom
 *
 * \see Window3D
 */
class Trackball3DInteractionMode : public Window3D::EventHandler {
public:
  Trackball3DInteractionMode(Window3D *parent) : 
    Window3D::EventHandler(parent) {}

  int OnMousePress(const FLTKEvent &event);
  int OnMouseRelease(const FLTKEvent &event, const FLTKEvent &pressEvent);    
  int OnMouseDrag(const FLTKEvent &event, const FLTKEvent &pressEvent);
};

int 
Trackball3DInteractionMode
::OnMousePress(const FLTKEvent &event)
{
  int x = event.XCanvas[0];
  int y = event.XCanvas[1];

  switch (event.SoftButton)
    {
    case FL_LEFT_MOUSE:   m_Parent->OnRotateStartAction(x,y);break;
    case FL_MIDDLE_MOUSE: m_Parent->OnPanStartAction(x,y);break;
    case FL_RIGHT_MOUSE:  m_Parent->OnZoomStartAction(x,y);break;
    default: return 0;
    }

  return 1;
}

int 
Trackball3DInteractionMode
::OnMouseRelease(const FLTKEvent &irisNotUsed(event),
                 const FLTKEvent &irisNotUsed(startEvent))
{
  m_Parent->OnTrackballStopAction();
  return 1;
}

int 
Trackball3DInteractionMode
::OnMouseDrag(const FLTKEvent &event,
              const FLTKEvent &irisNotUsed(startEvent))
{
  m_Parent->OnTrackballDragAction(event.XCanvas[0],event.XCanvas[1]);
  return 1;
}

/**
 * \class Crosshair3DInteractionMode
 * \brief 3D interaction mode that takes care of 3D crosshair interaction
 *
 * \see Window3D
 */
class Crosshairs3DInteractionMode : public Window3D::EventHandler {
public:
  Crosshairs3DInteractionMode(Window3D *parent) : 
    Window3D::EventHandler(parent) {}

  int OnMousePress(const FLTKEvent &event);
};

int 
Crosshairs3DInteractionMode
::OnMousePress(const FLTKEvent &event)
{
  if(event.SoftButton == FL_LEFT_MOUSE)
    {
    m_Parent->OnCrosshairClickAction(event.XCanvas[0],event.XCanvas[1]);
    return 1;
    }
  else return 0;
}

/**
 * \class Scalpel3DInteractionMode
 * \brief 3D interaction mode that takes care of cutting 3D view in two
 *
 * \see Window3D
 */
class Scalpel3DInteractionMode : public Window3D::EventHandler {
public:
  Scalpel3DInteractionMode(Window3D *parent) : 
    Window3D::EventHandler(parent) 
  { 
    m_Started = false;
    m_Inside = false;
  }

  int OnMousePress(const FLTKEvent &event);
  int OnMouseMotion(const FLTKEvent &event);    
  int OnMouseEnter(const FLTKEvent &event);
  int OnMouseExit(const FLTKEvent &event);

  irisGetMacro(Started,bool);
  irisGetMacro(Inside,bool);
  irisGetMacro(StartPoint,Vector2i);
  irisGetMacro(EndPoint,Vector2i);

protected:
  bool m_Inside;
  bool m_Started;
  Vector2i m_StartPoint;
  Vector2i m_EndPoint;
};

int 
Scalpel3DInteractionMode
::OnMousePress(const FLTKEvent &event)
{
  if(!m_Started)
    {
    // Only for left button
    if(event.SoftButton != FL_LEFT_MOUSE) return 0;

    // Record the starting and ending points
    m_StartPoint = event.XCanvas;
    m_EndPoint = event.XCanvas;
    m_Started = true;
    m_Inside = true;
    }
  else
    {
    // This is the second click, after the user finished dragging
    m_EndPoint = event.XCanvas;
    m_Started = false;

    // If the user clicks another button, disengage, otherwise
    // draw the plane in the parent
    if(event.SoftButton == FL_LEFT_MOUSE)
      m_Parent->OnScalpelPointPairAction(
        m_StartPoint[0],m_StartPoint[1],m_EndPoint[0],m_EndPoint[1]);    
    }

  // Redraw the parent
  m_Parent->redraw();

  // Eat the event
  return 1;
}

int 
Scalpel3DInteractionMode::
OnMouseMotion(const FLTKEvent &event)
{
  // Only valid if drawing has started
  if(!m_Started) return 0;
  
  // Record the end point
  m_EndPoint = event.XCanvas;

  // Redraw the parent
  m_Parent->redraw();

  // Eat the event
  return 1;
}

int 
Scalpel3DInteractionMode
::OnMouseEnter(const FLTKEvent &event)
{
  // Only valid if drawing has started
  if(!m_Started) return 0;
  
  // Record that we're inside
  m_Inside = true;
  
  // Record the end point
  m_EndPoint = event.XCanvas;

  // Redraw the parent
  m_Parent->redraw();

  // Eat the event
  return 1;
}

int 
Scalpel3DInteractionMode
::OnMouseExit(const FLTKEvent &irisNotUsed(event))
{
  // Only valid if drawing has started
  if(!m_Started) return 0;
  
  // Record that we're inside
  m_Inside = false;
  
  // Redraw the parent
  m_Parent->redraw();

  // Eat the event
  return 1;
}


/**
 * \class Spraypaint3DInteractionMode
 * \brief 3D interaction mode that takes care of spraying on top of the 3D view
 *
 * \see Window3D
 */
class Spraypaint3DInteractionMode : public Window3D::EventHandler {
public:
  Spraypaint3DInteractionMode(Window3D *parent) : 
    Window3D::EventHandler(parent) {}

  int OnMousePress(const FLTKEvent &event);
  int OnMouseDrag(const FLTKEvent &event, const FLTKEvent &pressEvent);
};


int 
Spraypaint3DInteractionMode
::OnMousePress(const FLTKEvent &event)
{
  if(event.SoftButton == FL_LEFT_MOUSE)
    {
    m_Parent->OnSpraypaintClickAction(event.XCanvas[0],event.XCanvas[1]);
    return 1;
    }
  else return 0;
}

int 
Spraypaint3DInteractionMode
::OnMouseDrag(const FLTKEvent &event,
              const FLTKEvent &irisNotUsed(startEvent))
{
  if(event.SoftButton == FL_LEFT_MOUSE)
    {
    m_Parent->OnSpraypaintClickAction(event.XCanvas[0],event.XCanvas[1]);
    return 1;
    }
  else return 0;
}


Window3D
::Window3D( int x, int y, int w, int h, const char *l )
: FLTKCanvas(x, y, w, h, l)
{
  // Make sure FLTK canvas does not flip the Y coordinate
  SetFlipYCoordinate(false);
  
  // Clear the flags
  m_NeedsInitialization = 1;
  m_CursorVisible = 0;
  m_Mode = WIN3D_NONE;
  m_Plane.valid = -1;

  // Reset the vectors to zero
  m_Spacing.fill(1.0);
  m_ImageSize.fill(0);
  m_Center.fill(0);
  m_DefaultHalf.fill(0);
  m_ViewHalf.fill(0);

  // Initialize the interaction modes
  m_CrosshairsMode = new Crosshairs3DInteractionMode(this); 
  m_TrackballMode = new Trackball3DInteractionMode(this);
  m_SpraypaintMode = new Spraypaint3DInteractionMode(this);
  m_ScalpelMode = new Scalpel3DInteractionMode(this);

  // Start with the trackball mode, which is prevailing
  PushInteractionMode(m_TrackballMode);
}


/** Enter the cross-hairs mode of operation */
void 
Window3D
::EnterCrosshairsMode()
{
  PopInteractionMode();
  PushInteractionMode(m_CrosshairsMode);
}

/** Enter the trackball mode of operation */
void 
Window3D
::EnterTrackballMode()
{
  PopInteractionMode();
  PushInteractionMode(m_TrackballMode);
}

/** Enter the scalpel mode of operation */
void 
Window3D
::EnterScalpelMode()
{
  PopInteractionMode();
  PushInteractionMode(m_ScalpelMode);
}

/** Enter the spraypaint mode of operation */
void 
Window3D
::EnterSpraypaintMode()
{
  PopInteractionMode();
  PushInteractionMode(m_SpraypaintMode);
}

void 
Window3D
::Register(int i, UserInterfaceLogic *ui)
{
  // Assign an ID
  m_Id = i; 

  // Copy parent pointers
  m_ParentUI = ui;
  m_Driver = m_ParentUI->GetDriver();
  m_GlobalState = m_Driver->GetGlobalState();    

  // Pass parent pointer to the mesh object
  this->m_Mesh.Initialize(m_Driver);
}

Window3D
::~Window3D()
{
  delete m_CrosshairsMode;
  delete m_TrackballMode;
  delete m_SpraypaintMode;
  delete m_ScalpelMode;
}


/* Initializes lightning and other basic GL-state for the window */
void 
Window3D
::Initialize()
{
  ResetView();

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable( GL_DEPTH_TEST );

  // Set up the materials
  GLfloat light0Pos[4] = { 0.0, 0.0, 1.0, 0.0};
  GLfloat matAmb[4] = { 0.01, 0.01, 0.01, 1.00};
  GLfloat matDiff[4] = { 0.65, 0.65, 0.65, 1.00};
  GLfloat matSpec[4] = { 0.30, 0.30, 0.30, 1.00};
  GLfloat matShine = 10.0;
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
  glEnable(GL_COLOR_MATERIAL);

  // Setup Lighting
  glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
  glEnable(GL_LIGHT0);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glEnable(GL_LIGHTING);
}

void 
Window3D
::ClearScreen()
{
  // Update the GL state
  make_current(); 

  // Hide the crosshairs.
  m_CursorVisible = 0;  

  // Hide the mesh.
  m_Mesh.Reset(); 
}

void 
Window3D
::ResetView()
{
  // Update the GL state
  make_current(); 

  // Reset the trackball
  m_Trackball.Reset();

  // Rotate a little bit, so we see the three axes
  m_Trackball.StartRot(12,10,20,20);
  m_Trackball.TrackRot(10,8,20,20);
  m_Trackball.StopRot();

  if (m_Driver->GetCurrentImageData()->IsGreyLoaded())
    {
    // data dimensions
    m_ImageSize = m_Driver->GetCurrentImageData()->GetVolumeExtents();  

    // voxel m_Spacing        
    m_Spacing = m_Driver->GetCurrentImageData()->GetVoxelScaleFactor(); 

    // Volume size
    m_VolumeSize[0] = m_Spacing[0] * m_ImageSize[0];
    m_VolumeSize[1] = m_Spacing[1] * m_ImageSize[1];
    m_VolumeSize[2] = m_Spacing[2] * m_ImageSize[2];
    } 
  else
    {
    m_ImageSize.fill(0);
    m_Spacing.fill(0.0);
    m_VolumeSize.fill(0.0);
    }

  unsigned int maxdim = 0;
  for (int i=0; i<3; i++)
    {
    unsigned int len = m_ImageSize[i];
    m_Center[i] = len / 2.0;
    if (maxdim < len) 
      maxdim = len;
    }
  m_DefaultHalf[X] = m_DefaultHalf[Y] = m_DefaultHalf[Z] = maxdim * 0.7 + 1.0;
  m_DefaultHalf[Z] *= 4.0;

  this->SetupProjection();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  m_CursorVisible = 1;  // Show the crosshairs.
  m_Plane.valid = -1; // Resets the Cut m_Plane
  
  m_Samples.clear();
}

void 
Window3D
::UpdateMesh()
{
  make_current();
  m_Mesh.GenerateMesh();
  redraw();
}

void 
Window3D
::Accept()
{
  // Get the current drawing color
  unsigned char colorid = m_GlobalState->GetDrawingColorLabel();  

  // Apply the spraypaint samples to the image
  for(SampleListIterator it=m_Samples.begin();it!=m_Samples.end();++it)
    {
    m_Driver->GetCurrentImageData()->SetSegmentationVoxel(
      to_unsigned_int(*it), colorid );
    }
  
  // Clear the list of samples
  m_Samples.clear();
  
  // If the plane is valid, apply it for relabeling
  if (1 == m_Plane.valid )
    {   
    m_Driver->GetCurrentImageData()->RelabelSegmentationWithCutPlane(
      m_Plane.vNormal, m_Plane.dIntercept, m_GlobalState);
    m_Plane.valid = -1;
    }
}

void
Window3D
::CheckErrors()
{
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR)
    {
    cerr << "GL-Error: " << (char *) gluErrorString(error) << endl;
    }
}

void 
Window3D
::draw()
{
  if ( !valid() || m_NeedsInitialization )
    {
    glViewport(0,0,w(),h());
    Initialize();
    m_NeedsInitialization = 0;
    }

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // Compute the center of rotation  
  Vector3ui crosshair = m_GlobalState->GetCrosshairsPosition();
  for (int i=0; i<3; i++) 
    m_CenterOfRotation[i] = m_Spacing[i] * crosshair[i];

  // Set up the model view matrix
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();  
  glLoadIdentity();

  // Update the screen geometry
  glTranslatef( m_Trackball.GetPanX(), m_Trackball.GetPanY(), 0.0 );
  glMultMatrixf( m_Trackball.GetRot() );
  glTranslatef( -m_CenterOfRotation[0], -m_CenterOfRotation[1], -m_CenterOfRotation[2] );

  // Scale by the voxel spacing
  glPushMatrix();
  glScalef( m_Spacing[X], m_Spacing[Y], m_Spacing[Z] );

  // Draw things in pixel coords
  DrawCrosshairs();
  DrawSamples();
  
  // Undo scaling by voxel spacing
  glPopMatrix();

  // Draw the cut plane
  DrawCutPlane();

  // The mesh is already in isotropic coords (needed for marching cubes)
  m_Mesh.Display();

  // Restore the matrix state
  glPopMatrix(); 

  // Draw any overlays there may be
  if(m_ScalpelMode->GetStarted() && m_ScalpelMode->GetInside())
    {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,w(),h(),0);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glPushAttrib(GL_DEPTH_BUFFER_BIT);
    glDepthFunc(GL_ALWAYS);

    // Get the points
    Vector2d x1 = to_double(m_ScalpelMode->GetStartPoint());
    Vector2d x2 = to_double(m_ScalpelMode->GetEndPoint());
    Vector2d d = x2-x1;

    // Draw a line between the two points
    glColor3d(1,1,1);
    glBegin(GL_LINES);
    glVertex2d(x1[0],x1[1]);    
    glVertex2d(x2[0],x2[1]);    
    glEnd();

    // If the points are far enough apart, draw the normal
    if(d.two_norm() > 10)
      {
      // Draw the normal midway through the line
      Vector2d n = Vector2d(-d[1],d[0]).normalize();
      Vector2d p1 = 0.5 * (x1 + x2);
      Vector2d p2 = p1 + 10.0 * n;
      
      glBegin(GL_LINES);
      glVertex2d(p1[0],p1[1]);    
      glVertex2d(p2[0],p2[1]);    
      glEnd();

      // Draw a colored triangle
      d.normalize();
      Vector2d u1 = p2 + 4.0 * d;
      Vector2d u2 = p2 - 4.0 * d;
      Vector2d u3 = p2 + 1.732 * 4.0 * n;

      glBegin(GL_TRIANGLES);
      glVertex2d(u1[0],u1[1]);    
      glVertex2d(u2[0],u2[1]);    
      glVertex2d(u3[0],u3[1]);    
      glEnd();
      
      }
    
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glPopAttrib();
  }

  glFlush();
  // CheckErrors();
}

/**
 * handle(int event)
 *
 * purpose:
 * this handle() overrides the handle() of Fl_Gl_Window
 * keyboard input events ignored; mouse push/drag/release events processed
 *
 * pre: ??? does voxel data need to be loaded/3D window initialized???
 *
 * post:
 *
 *   results depend on m_GlobalState->GetToolbarMode():
 *   NAVIGATION_MODE:
 *   press starts, drag continues, release stops
 *    rotate        pan     zoom
 *      left button 3rd button  right button
 *   CROSSHAIRS_MODE:
 *   left button push repositions crosshairs
 *   PAINT3D_MODE:
 *   drag does the spray painting
 *     POLYGON_DRAWING_MODE:
 *   no action is done (all polygon drawing is done in Window2D)
 *   CUT_PLANES_MODE: 
 *    right now does nothing
 *  else no change in state
 *  if event is used, 1 is returned, else 0
 */
/*
int 
Window3D
::handle(int event)
{
  int button = Fl::event_button();
  // accomodate 2-button mice: Ctrl-right => middle
  if (Fl::event_state(FL_CTRL) && button == FL_RIGHT_MOUSE)
    button = FL_MIDDLE_MOUSE;

  switch ( m_GlobalState->GetToolbarMode() )
    {
    case NAVIGATION_MODE:
      switch ( event )
        {
        case FL_PUSH:  MousePressFunc(button); return 1;
        case FL_DRAG:  MouseMotionFunc(); return 1;
        case FL_RELEASE: MouseReleaseFunc(); return 1;
        }
      break;
    case CROSSHAIRS_MODE:
      switch ( event )
        {
        case FL_PUSH: MouseCrossPressFunc(button); return 1;
        }
      break;
    case PAINT3D_MODE:
      switch ( event )
        {
        case FL_PUSH:   MouseCutPressFunc(button); return 1;
        case FL_DRAG: MousePointPressFunc(button); return 1;
        }
      break;
    case ROI_MODE:
    case POLYGON_DRAWING_MODE:
      break;
    default:
      cerr << "Case not handled in  Window3D::handle(int)" << endl;
    }

  return 0;
}
*/
  
void 
Window3D
::OnRotateStartAction(int x, int y)
{
  if ( m_Mode != WIN3D_NONE ) return;
  m_Mode = WIN3D_ROTATE;
  m_Trackball.StartRot(x, y, w(), h());
}

void 
Window3D
::OnPanStartAction(int x, int y)
{
  if ( m_Mode != WIN3D_NONE ) return;
  m_Mode = WIN3D_PAN;
  m_Trackball.StartPan(x, y);
}

void 
Window3D
::OnZoomStartAction(int irisNotUsed(x), int y)
{
  if ( m_Mode != WIN3D_NONE ) return;
  m_Mode = WIN3D_ZOOM;
  m_Trackball.StartZoom(y);
}

void 
Window3D
::OnTrackballDragAction(int x, int y)
{
  switch (m_Mode)
    {
    case WIN3D_ROTATE: m_Trackball.TrackRot(x,y,w(),h()); break;
    case WIN3D_ZOOM:   
      m_Trackball.TrackZoom(y); 
      this->SetupProjection();
      break;
    case WIN3D_PAN:    
      m_Trackball.TrackPan(x,y,w(),h(),2*m_ViewHalf[X],2*m_ViewHalf[Y]);
      break;
    default: break;
    }

  redraw();
}

void 
Window3D
::OnTrackballStopAction()
{
  switch (m_Mode)
    {
    case WIN3D_ROTATE:  m_Trackball.StopRot(); break;
    case WIN3D_PAN:     m_Trackball.StopPan(); break;
    case WIN3D_ZOOM:    
      m_Trackball.StopZoom(); 
      this->SetupProjection();
      break;
    default: break;
    }
  m_Mode = WIN3D_NONE;
}

void 
Window3D
::OnCrosshairClickAction(int x,int y)
{
  // Make sure that there is a valid image
  if (!m_Driver->GetCurrentImageData()->IsGreyLoaded()) return;

  // Only respond to the left mouse button (why?)
  Vector3i hit;
  if (IntersectSegData(x, y, hit))
    {
    // TODO: Unify this!
    m_Driver->GetCurrentImageData()->SetCrosshairs(to_unsigned_int(hit));
    m_GlobalState->SetCrosshairsPosition(to_unsigned_int(hit));
    }

  m_ParentUI->OnCrosshairPositionUpdate();
  m_ParentUI->RedrawWindows();
}

void 
Window3D
::OnSpraypaintClickAction(int x,int y)
{
  // Make sure that there is a valid image
  if (!m_Driver->GetCurrentImageData()->IsGreyLoaded()) return;

  Vector3i hit;
  if (this->IntersectSegData(x, y, hit))
    {
    AddSample( hit );
    m_ParentUI->Activate3DAccept(true);
    redraw();
    }
}

void 
Window3D
::OnScalpelPointPairAction(int x1, int y1, int x2, int y2)
{
  // Requires a loaded image
  if (!m_Driver->GetCurrentImageData()->IsGreyLoaded()) return;

  // Pass in the first point
  // OnCutPlanePointRayAction(x1, y1, 1);
  // OnCutPlanePointRayAction(x2, y2, 2);

  if(ComputeCutPlane(x1,y1,x2,y2)) 
    {
    m_Plane.valid = 1;
    m_ParentUI->Activate3DAccept(true);
    }
        
  redraw();
}

/**
  Does translation and rotation from the m_Trackball (not anisotropic scaling).
  Make sure to glPopMatrix() after use!
  */
void 
Window3D
::SetupModelView()
{
}

void 
Window3D
::SetupProjection()
{
  make_current();

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  float zoom = m_Trackball.GetZoom();
  m_ViewHalf[X] = m_DefaultHalf[X]/zoom;
  m_ViewHalf[Y] = m_DefaultHalf[Y]/zoom;
  m_ViewHalf[Z] = m_DefaultHalf[Z]/zoom;
  glOrtho( -m_ViewHalf[X], m_ViewHalf[X], 
           -m_ViewHalf[Y], m_ViewHalf[Y], 
           -m_DefaultHalf[Z], m_DefaultHalf[Z]);
}

// Get a copy of the viewport/m_Modelview/projection matrices OpenGL uses
void Window3D::ComputeMatricies( int *vport, double *mview, double *proj )
{
  // Compute the center of rotation  
  Vector3ui crosshair = m_GlobalState->GetCrosshairsPosition();
  for (int i=0; i<3; i++) 
    m_CenterOfRotation[i] = m_Spacing[i] * crosshair[i];

  // Set up the model view matrix
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();  
  glLoadIdentity();

  // Update the screen geometry
  glTranslatef( m_Trackball.GetPanX(), m_Trackball.GetPanY(), 0.0 );
  glMultMatrixf( m_Trackball.GetRot() );
  glTranslatef( -m_CenterOfRotation[0], -m_CenterOfRotation[1], -m_CenterOfRotation[2] );
  glScalef( m_Spacing[X], m_Spacing[Y], m_Spacing[Z] );

  glGetIntegerv( GL_VIEWPORT, vport );
  glGetDoublev( GL_MODELVIEW_MATRIX, mview );
  glGetDoublev( GL_PROJECTION_MATRIX, proj );

  glPopMatrix();
}

void Window3D::ComputeRay( int x, int y, double *mvmatrix, double *projmatrix,
                           int *viewport, Vector3d &v, Vector3d &r )
{
  int val;
  val = gluUnProject( (GLdouble) x, (GLdouble) y, 0.0,
                      mvmatrix, projmatrix, viewport,
                      &(v[0]), &(v[1]), &(v[2]) );
  if ( val == GL_FALSE ) cerr << "gluUnProject #1 FAILED!!!!!" << endl;
  
  val = gluUnProject( (GLdouble) x, (GLdouble) y, 1.0,
                      mvmatrix, projmatrix, viewport,
                      &(r[0]), &(r[1]), &(r[2]) );
  if ( val == GL_FALSE ) cerr << "gluUnProject #2 FAILED!!!!!" << endl;

  r[0] = r[0] - v[0];
  r[1] = r[1] - v[1];
  r[2] = r[2] - v[2];
}

bool 
Window3D
::ComputeCutPlane(int wx1, int wy1, int wx2, int wy2)
{
  // Open GL matrices
  double mvmatrix[16];
  double projmatrix[16];
  int viewport[4];
  Vector3d x1,x2,p1,p2;

  // Compute the GL matrices
  ComputeMatricies( viewport, mvmatrix, projmatrix );

  // Flip the y coordinate
  wy1 = viewport[3] - wy1 - 1;
  wy2 = viewport[3] - wy2 - 1;

  // Compute the normal to the viewplane  
  gluUnProject(0,0,0,mvmatrix,projmatrix,viewport,
               p1.data_block(),p1.data_block()+1,p1.data_block()+2);
  gluUnProject(0,0,1,mvmatrix,projmatrix,viewport,
               p2.data_block(),p2.data_block()+1,p2.data_block()+2);
  
  // W is a vector pointing into the screen
  Vector3d w = p2 - p1;

  // Compute the vector connecting the two points currently lying on the
  // view plane
  gluUnProject(wx1,wy1,0,mvmatrix,projmatrix,viewport,
               x1.data_block(),x1.data_block()+1,x1.data_block()+2);
  gluUnProject(wx2,wy2,0,mvmatrix,projmatrix,viewport,
               x2.data_block(),x2.data_block()+1,x2.data_block()+2);

  // Now we have two orthogonal vectors laying on the cut plane.  All we have
  // to do is take the cross product
  Vector3d delta = x2-x1;
  Vector3d n = - cross_3d((vnl_vector<double>) delta,
                            (vnl_vector<double>) w);

  // Compute the length of the normal and exit if it's zero
  double l = n.two_norm();
  if(l == 0.0) return false;
  
  // Compute the distance to the origin
  m_Plane.vNormal = n.normalize();
  m_Plane.dIntercept = dot_product(x1,m_Plane.vNormal);

  // Now, this is not enough, because we want to be able to draw the plane
  // in space.  In order to do this we need four corners of a square on the
  // plane.

  // Compute the points on the plane in world space
  Vector3d x1World(x1[0] * m_Spacing[0],x1[1] * m_Spacing[1],x1[2] * m_Spacing[2]);
  Vector3d x2World(x2[0] * m_Spacing[0],x2[1] * m_Spacing[1],x2[2] * m_Spacing[2]);
  Vector3d p1World(p1[0] * m_Spacing[0],p1[1] * m_Spacing[1],p1[2] * m_Spacing[2]);
  Vector3d p2World(p2[0] * m_Spacing[0],p2[1] * m_Spacing[1],p2[2] * m_Spacing[2]);
  
  // Compute the normal in world coordinates
  Vector3d nWorld = - cross_3d((vnl_vector<double>) (x2World - x1World),
                               (vnl_vector<double>) (p2World - p1World));
  m_Plane.vNormalWorld = nWorld.normalize();

  // Compute the intercept in world coordinates  
  double interceptWorld = dot_product(x1World,m_Plane.vNormalWorld);

  // Compute the center of the volume
  Vector3d xVol = to_double(m_VolumeSize) * 0.5;
  
  // Use that to compute the center of the square
  double edgeLength = (xVol[0] > xVol[1]) ? xVol[0] : xVol[1];
  edgeLength = (edgeLength > xVol[2]) ? edgeLength : xVol[2];

  // Now compute the center point of the square   
  Vector3d xCenter = 
    xVol - m_Plane.vNormalWorld *
     (dot_product(xVol,m_Plane.vNormalWorld) - interceptWorld);
  
  // Compute the 'up' vector and the 'in' vector
  Vector3d vUp = (x2World - x1World).normalize();
  Vector3d vIn = (p2World - p1World).normalize();

  // Compute the corners
  m_Plane.xDisplayCorner[0] = xCenter + edgeLength * (vUp + vIn);
  m_Plane.xDisplayCorner[1] = xCenter + edgeLength * (vUp - vIn);
  m_Plane.xDisplayCorner[2] = xCenter + edgeLength * (- vUp - vIn);
  m_Plane.xDisplayCorner[3] = xCenter + edgeLength * (- vUp + vIn);

  return true;
}



//------------------------------------------------------------------------
// IntersectSegData(int mouse_x, int mouse_y, Vector3i *hit)
// computes a m_Ray going straight back from the current viewpoint
// from the mouse click position on screen.
// The output Vector3i hit is in image coords.
//
//------------------------------------------------------------------------
int Window3D::IntersectSegData(int mouse_x, int mouse_y, Vector3i &hit)
{
  double mvmatrix[16];
  double projmatrix[16];
  int viewport[4];

  make_current(); // update GL state
  ComputeMatricies( viewport, mvmatrix, projmatrix );
  int x = mouse_x;
  int y = viewport[3] - mouse_y - 1;
  ComputeRay( x, y, mvmatrix, projmatrix, viewport, m_Point, m_Ray );

  // The result 
  int result = 0;
  
  // Depending on the situation, we may intersect with a snake image or a
  // segmentation image
  // TODO: Need both conditions?
  if(m_GlobalState->GetSnakeActive() && 
     m_Driver->GetSNAPImageData()->IsSnakeLoaded())
    {
    typedef ImageRayIntersectionFinder<
      float,SnakeImageHitTester> RayCasterType;

    RayCasterType caster;

    result = 
      caster.FindIntersection(
        m_Driver->GetSNAPImageData()->GetLevelSetImage(),
        m_Point,m_Ray,hit);
    }
  else
    {
    typedef ImageRayIntersectionFinder<
      LabelType,LabelImageHitTester> RayCasterType;

    RayCasterType caster;
    caster.SetHitTester(LabelImageHitTester(m_Driver->GetCurrentImageData()));
    result = 
      caster.FindIntersection(
        m_Driver->GetCurrentImageData()->GetSegmentation()->GetImage(),
        m_Point,m_Ray,hit);
    }
  
  // m_Ray now has the proj m_Ray, m_Point is the m_Point in image space
  switch (result)
    {
    case 1: return 1;
    case -1: /*cerr << "RAY WAS INVALID!" << endl;*/ break;
    /* default: cerr << "No hit found" << endl;*/
    }
  return 0;
}

//-----------------------------------------------------------------------
// ComputePointRay
// 
//-----------------------------------------------------------------------
/*
void Window3D::OnCutPlanePointRayAction(int mouse_x, int mouse_y, int i)
{
  double mvmatrix[16];
  double projmatrix[16];
  int viewport[4];

  make_current();     // update GL state
  ComputeMatricies( viewport, mvmatrix, projmatrix );
  int x = mouse_x;
  int y = viewport[3] - mouse_y - 1;
  if ( 1==i )
    {
    ComputeRay( x, y, mvmatrix, projmatrix, viewport, m_Plane.cutPt1,
                m_Plane.cutRay1 );
    cout << "1st Point: X = " << Fl::event_x() << " Y = " << Fl::event_y() << " in image space is ("
    << m_Plane.cutPt1[0] << ", " << m_Plane.cutPt1[1] << ", " << m_Plane.cutPt1[2] << ")" << endl;
    cout << "Ray 1: x = " << m_Plane.cutRay1[0] << " y = " << m_Plane.cutRay1[1] << " z = "
    << m_Plane.cutRay1[2] << endl;
    // m_Ray[i] and m_Point[] have been set
    } 
  else if ( 2==i )
    {
    ComputeRay( x, y, mvmatrix, projmatrix, viewport, m_Plane.cutPt2,
                m_Plane.cutRay2 );
    cout << "Second m_Point: X = " << Fl::event_x() << " Y = " << Fl::event_y() << " in image space is ("
    << m_Plane.cutPt2[0] << ", " << m_Plane.cutPt2[1] << ", " << m_Plane.cutPt2[2] << ")" << endl;
    cout << "Ray 2: x = " << m_Plane.cutRay2[0] << " y = " << m_Plane.cutRay2[1] << " z = "
    << m_Plane.cutRay2[2] << endl;
    } 
  else
    {
    cerr << "Not able to set Cut m_Plane point: " << i << endl; 
    }
}
*/

//-----------------------------------------------------------------------
// Computem_Plane
//   returns 0 if plane invalid, 1 if plane sucessfully computed
//
// parameters:
//   pt1    1st user-defined m_Point
//   pt2    2nd user-defined m_Point
//   m_Ray    m_Ray projected back from pt2 through image from viewpt
//   ABC    output m_Plane coordinates Ax+By+Cz+1=0 if DisZero
//          Ax+By+Cz=0 if !DixZero 
//   DisZero    true if plane goes through origin, false otherwise
// pre:
//   if they pt1==pt2 return 0, plane cannot be calculated
// post
//   pt1 != pt2 return 1
//  ABC returns coordinates of plane equation, and DisZero set
//-----------------------------------------------------------------------
/*
void Window3D::ComputePlane() {
  if (m_Plane.valid != 1)
    {
    cerr << "m_Plane not properly initiated, cannot compute plane" << endl;
    return;
    }

  if (m_Plane.cutRay2[X]==0 && m_Plane.cutRay2[Y]==0 && m_Plane.cutRay2[Z]==0)
    {
    cerr << "oops, can't have cutRay2 be (0,0,0)" << endl;
    return;
    }

  Vector3d ray3;    // create new m_Ray between the 2 pts to help calc plane
  for (int i=0; i<3; i++) ray3[i] = m_Plane.cutPt2[i] - m_Plane.cutPt1[i];

  if (ray3[X] == 0 && ray3[Y] == 0 && ray3[Z] == 0)
    {
    cerr << "badness, points are the same - don't double click"  << endl;
    return;
    }

  Vector3d normal;  // Cross product of cutRay2 with ray3
  normal[X] =   m_Plane.cutRay2[Y]*ray3[Z] - m_Plane.cutRay2[Z]*ray3[Y];
  normal[Y] = -(m_Plane.cutRay2[X]*ray3[Z] - m_Plane.cutRay2[Z]*ray3[X]);
  normal[Z] =   m_Plane.cutRay2[X]*ray3[Y] - m_Plane.cutRay2[Y]*ray3[X];

  double D = normal[X]*m_Plane.cutPt2[X] 
             + normal[Y]*m_Plane.cutPt2[Y] 
             + normal[Z]*m_Plane.cutPt2[Z] ;
  m_Plane.distZero = (D == 0);

  if (D != 0)     // Normalize the plane
    for (int i=0; i<3; i++) normal[i] /= D;

  m_Plane.coords = normal;

#ifdef DEBUG
  cout << "Window3D::Computem_Plane() plane equation is: " << endl << "("
  << m_Plane.coords[0] << ")*x + ("
  << m_Plane.coords[1] << ")*y + ("
  << m_Plane.coords[2] << ")*z "
  << (m_Plane.distZero ? "- 1 " : "") << "== 0" << endl;
#endif
}
*/
void Window3D::AddSample( Vector3i s )
{
  // Add another sample.
  m_Samples.push_back(s);
}

void Window3D::DrawCrosshairs()
{
  if ( !m_CursorVisible ) return;

  Vector3ui crosshair = m_GlobalState->GetCrosshairsPosition();

  glDisable(GL_LIGHTING);

  glColor3f( 0.0, 0.0, 1.0 );
  glLineWidth( 2 );
  glEnable( GL_LINE_STIPPLE );
  glLineStipple( 2, 0xAAAA );
  glBegin( GL_LINES );

  for (int i=0; i<3; i++)
    {
    float end1[3], end2[3];
    for (int j=0; j<3; j++)
      end1[j] = end2[j] = crosshair[j];
    end1[i] = m_Center[i] - m_ImageSize[i]*0.7+1;
    end2[i] = m_Center[i] + m_ImageSize[i]*0.7+1;

    glVertex3fv(end1);
    glVertex3fv(end2);
    }

  glEnd();
  glDisable( GL_LINE_STIPPLE );

#if DEBUGGING
  glColor3f( 1.0, 1.0, 0.0 );
  glBegin( GL_LINES );
  glVertex3d( m_Point[0],        m_Point[1],        m_Point[2] );
  glVertex3d( m_Point[0]+m_Ray[0], m_Point[1]+m_Ray[1], m_Point[2]+m_Ray[2] );
  glEnd();

  cerr << "A = ( " << m_Point[0] << ", " << m_Point[1] << ", " << m_Point[2] << " )" << endl;
  cerr << "B = ( " << m_Point[0]+m_Ray[0]
  << ", " << m_Point[1]+m_Ray[1]
  << ", " << m_Point[2]+m_Ray[2] << " )" << endl;
#endif

  glEnable(GL_LIGHTING);
}


void Window3D::DrawSamples()
{
  unsigned char index = m_GlobalState->GetDrawingColorLabel();
  unsigned char rgb[3];
  m_Driver->GetCurrentImageData()->GetColorLabel(index).GetRGBVector(rgb);

  glColor3ubv(rgb);
  for (SampleListIterator it=m_Samples.begin();it!=m_Samples.end();it++)
    {
    glPushMatrix(); 
    glTranslatef( (*it)[X], (*it)[Y], (*it)[Z] );

    GLUquadric *quad = gluNewQuadric();
    gluSphere(quad,1.0,4,4);
    gluDeleteQuadric(quad);
    // glutSolidSphere( 1.0, 4, 4 );

    glPopMatrix();
    }
}

//-----------------------------------------------------------------------
// DrawCutPlane
//   Provides User Feedback Information when in 3D Window Mode
//   when a Cutplane is defined.
// Input: the 'plane' member
// Output: GLplane representing the Cutplane that is defined by the user
//-----------------------------------------------------------------------
void Window3D
::DrawCutPlane() 
{
  if (m_Plane.valid != 1) return;

  Vector3d corner[4];  
  corner[0] = m_Plane.xDisplayCorner[0];
  corner[1] = m_Plane.xDisplayCorner[1];
  corner[2] = m_Plane.xDisplayCorner[2];
  corner[3] = m_Plane.xDisplayCorner[3];

  // Save the settings
  glPushAttrib(GL_LINE_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT);

  // Draw the plane using lines
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_LINE_STIPPLE);
  glEnable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glLineWidth(2.0);
  glLineStipple(2,0xAAAA);

  // Use the current label color
  unsigned char rgb[3];
  m_Driver->GetCurrentImageData()->GetColorLabel(
    m_GlobalState->GetDrawingColorLabel()).GetRGBVector(rgb);
  
  // Start with a white color
  glColor3d(1,1,1);
  Vector3d planeUnitNormal = Vector3d(m_Plane.vNormalWorld).normalize();
  for(unsigned int i=0;i<4;i++)
    {
    glPushMatrix();

    // Create a parallel plane to create a moving effect
    Vector3d vOffset = planeUnitNormal * (1.0 * i);
    glTranslated(vOffset[0],vOffset[1],vOffset[2]);

    // Draw a line loop
    glBegin(GL_LINE_LOOP);
    glVertex3dv(corner[0].data_block());
    glVertex3dv(corner[1].data_block());
    glVertex3dv(corner[2].data_block());
    glVertex3dv(corner[3].data_block());
    glEnd();

    // Switch to new color
    glColor3ubv(rgb);

    glPopMatrix();
    }

  glPopAttrib();
};

/*
 *Log: Window3D.cxx
 *Revision 1.12  2003/11/25 23:32:48  pauly
 *FIX: Snake evolution did not work in multiprocessor mode
 *
 *Revision 1.11  2003/11/10 00:24:50  pauly
 **** empty log message ***
 *
 *Revision 1.10  2003/10/14 13:44:27  pauly
 *FIX: Fixed warnings on gcc-3.3
 *
 *Revision 1.9  2003/10/10 15:04:21  pauly
 *ENH: Cut plane improvements (paint-over-visible and paint-over-one)
 *
 *Revision 1.8  2003/10/10 14:25:55  pauly
 *FIX: Ensured that code compiles on gcc 3-3
 *
 *Revision 1.7  2003/10/09 22:45:15  pauly
 *EMH: Improvements in 3D functionality and snake parameter preview
 *
 *Revision 1.6  2003/10/02 20:57:46  pauly
 *FIX: Made sure that the previous check-in compiles on Linux
 *
 *Revision 1.5  2003/10/02 14:55:53  pauly
 *ENH: Development during the September code freeze
 *
 *Revision 1.2  2003/09/11 19:23:29  pauly
 *FIX: Code compiles and runs on UNIX platform
 *
 *Revision 1.1  2003/09/11 13:51:15  pauly
 *FIX: Enabled loading of images with different orientations
 *ENH: Implemented image save and load operations
 *
 *Revision 1.4  2003/08/28 22:58:30  pauly
 *FIX: Erratic scrollbar behavior
 *
 *Revision 1.3  2003/08/27 14:03:24  pauly
 *FIX: Made sure that -Wall option in gcc generates 0 warnings.
 *FIX: Removed 'comment within comment' problem in the cvs log.
 *
 *Revision 1.2  2003/08/27 04:57:47  pauly
 *FIX: A large number of bugs has been fixed for 1.4 release
 *
 *Revision 1.1  2003/07/12 04:46:51  pauly
 *Initial checkin of the SNAP application into the InsightApplications tree.
 *
 *Revision 1.2  2003/07/12 01:34:18  pauly
 *More final changes before ITK checkin
 *
 *Revision 1.1  2003/07/11 23:25:33  pauly
 **** empty log message ***
 *
 *Revision 1.9  2003/06/08 23:27:56  pauly
 *Changed variable names using combination of ctags, egrep, and perl.
 *
 *Revision 1.8  2003/06/04 04:52:17  pauly
 *More UI fixes for the demo
 *
 *Revision 1.7  2003/05/08 21:59:05  pauly
 *SNAP is almost working
 *
 *Revision 1.6  2003/05/07 19:14:46  pauly
 *More progress on getting old segmentation working in the new SNAP.  Almost there, region of interest and bubbles are working.
 *
 *Revision 1.5  2003/04/29 14:01:42  pauly
 *Charlotte Trip
 *
 *Revision 1.4  2003/04/23 06:05:18  pauly
 **** empty log message ***
 *
 *Revision 1.3  2003/04/18 17:32:18  pauly
 **** empty log message ***
 *
 *Revision 1.2  2003/04/16 05:04:17  pauly
 *Incorporated intensity modification into the snap pipeline
 *New IRISApplication
 *Random goodies
 *
 *Revision 1.1  2003/03/07 19:29:48  pauly
 *Initial checkin
 *
 *Revision 1.2  2002/12/16 16:40:19  pauly
 **** empty log message ***
 *
 *Revision 1.1.1.1  2002/12/10 01:35:36  pauly
 *Started the project repository
 *
 *
 *Revision 1.10  2002/06/04 20:26:23  seanho
 *new rotation code from iris
 *
 *Revision 1.9  2002/04/27 18:31:05  moon
 *Finished commenting
 *
 *Revision 1.8  2002/04/24 17:15:13  bobkov
 *made no changes
 *
 *Revision 1.7  2002/04/23 22:00:39  moon
 *Just put in a couple glMatrixMode(GL_MODELVIEW) in a few places I thought it should
 *be just to be cautious.  I don't think it changed anything.
 *
 *Revision 1.6  2002/04/22 21:56:21  moon
 *Put in code to get crosshairs m_Mode working in 3D window.  Just checks global
 *flag in the mouseclick method to see if we're in snake m_Mode, so that the
 *right windows get redrawn.
 *
 *Revision 1.5  2002/04/13 16:22:40  moon
 *Fixed the problem with the 3D window drawing in black.
 *The draw method needed to call Init() in the if (!valid()) block.  It had two
 *checks, for !valid, and m_NeedsInitialization.  They just needed to be combined so that
 *lighting, etc. was set up either way.  The bug seems to be fixed.
 *
 *Revision 1.4  2002/04/10 21:22:12  moon
 *added some make_current calls to some methods, which seems to help the window to
 *update better, but the color/lighting problem is still there.
 *
 *Revision 1.3  2002/04/10 20:26:24  moon
 *just put in some debug statements.  Trying to debug the 3dwindow not drawing right.
 *
 *Revision 1.2  2002/03/08 14:06:32  moon
 *Added Header and Log tags to all files
 **/
