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

//#ifdef WIN32
//#  include <glut.h>
//#  include <GL/glu.h>
//#else
//#  include <FL/glut.h>
//#endif

#include <FL/glut.h>

                
static Vector3d ray;
static Vector3d pt;
   
/** These classes are used internally for ray intersection testing */

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




Window3D
::Window3D( int x, int y, int w, int h, const char *l )
: Fl_Gl_Window(x, y, w, h, l)
{
  m_NeedsInitialization = 1;
  m_CursorVisible = 0;
  m_Mode = WIN3D_NONE;

  m_Plane.valid = -1;

  for (int i=0; i<3; i++)
    {
    m_ImageSize[i] = 0;
    m_Center[i] = m_DefaultHalf[i] = m_ViewHalf[i] = 0;
    m_Spacing[i] = 1.0;
    }
  m_NumberOfUsedSamples = 0;;
  m_NumberOfAllocatedSamples = 64;
  m_Samples = new Vector3i[m_NumberOfAllocatedSamples];
  if ( m_Samples == NULL )
    {
    cerr << "Memory Allocation failure in:" << endl;
    cerr << "   Window3D::Window3D(int,int,int,int, const char *)" << endl;
    cerr << "Exiting." << endl;
    exit( -1 );
    }
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
  if ( m_Samples != NULL ) delete [] m_Samples;
}


/*
   ===============================================
   Public Member Functions
   ===============================================
   */

/*
 Initializes lightning and other basic GL-state for the window
 */
void 
Window3D
::Init()
{
  ResetView();

  GLfloat light0Pos[4] = { 0.0, 0.0, 1.0, 0.0};
  GLfloat matAmb[4] = { 0.01, 0.01, 0.01, 1.00};
  GLfloat matDiff[4] = { 0.65, 0.65, 0.65, 1.00};
  GLfloat matSpec[4] = { 0.30, 0.30, 0.30, 1.00};
  GLfloat matShine = 10.0;

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable( GL_DEPTH_TEST );

  //    glEnable( GL_NORMALIZE );


  /*
   ** Setup Lighting
   */
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiff);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShine);

  glEnable(GL_COLOR_MATERIAL);

  glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
  glEnable(GL_LIGHT0);

  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glEnable(GL_LIGHTING);
}

void 
Window3D
::ClearScreen()
{
  make_current(); // update GL state
  m_CursorVisible = 0;  // Hide the crosshairs.
  m_Mesh.Reset(); // Hide the mesh.
}

void 
Window3D
::ResetView()
{
  // verbose << "Window 3D Reset View" << endl;

  make_current(); // update GL state

  m_Trackball.Reset();

  if (m_Driver->GetCurrentImageData()->IsGreyLoaded())
    {
    // data dimensions
    m_ImageSize = m_Driver->GetCurrentImageData()->GetVolumeExtents();  

    // voxel m_Spacing        
    m_Spacing = m_Driver->GetCurrentImageData()->GetVoxelScaleFactor(); 
    } 
  else
    {
    m_ImageSize.fill(0);
    m_Spacing.fill(0.0);
    }

  int maxdim = 0;
  for (int i=0; i<3; i++)
    {
    m_Center[i]       = m_ImageSize[i] / 2.0;
    if (maxdim < m_ImageSize[i]) maxdim = m_ImageSize[i];
    }
  m_DefaultHalf[X] = m_DefaultHalf[Y] = m_DefaultHalf[Z] = maxdim * 0.7 + 1.0;
  m_DefaultHalf[Z] *= 4.0;

  this->SetupProjection();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  m_CursorVisible = 1;  // Show the crosshairs.
  m_Plane.valid = -1; // Resets the Cut m_Plane
  m_NumberOfUsedSamples = 0;  // Resets the spray paint
}

void 
Window3D
::UpdateMesh()
{
  // verbose << "Window 3D Update m_Mesh" << endl;

  make_current(); // update GL state
  m_Mesh.GenerateMesh();
  redraw();
}

void 
Window3D
::Accept()
{
  unsigned char colorid;
  colorid = m_GlobalState->GetDrawingColorLabel();

  for ( int i = 0; i < m_NumberOfUsedSamples; i++ )
    m_Driver->GetCurrentImageData()->SetSegmentationVoxel( m_Samples[i], colorid );
  m_NumberOfUsedSamples = 0;

  if (1 == m_Plane.valid )
    {    // relabel
    m_Driver->GetCurrentImageData()->RelabelSegmentationWithCutPlane(m_Plane.coords, m_Plane.distZero, colorid);
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
    Init();
    m_NeedsInitialization = 0;
    }

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  this->SetupModelView();

  //SetupModelView calls glPushMatrix
  glPushMatrix();
  glScalef( m_Spacing[X], m_Spacing[Y], m_Spacing[Z] );
  DrawCrosshairs();
  DrawSamples();
  DrawCutPlane();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  // The mesh is already in isotropic coords (needed for marching cubes)
  m_Mesh.Display();

  glMatrixMode(GL_MODELVIEW);//Nathan Moon
  glPopMatrix();  // from SetupModelView()

  glFlush();
  CheckErrors();
  //  cerr << "Window3D::draw finished!" << endl;
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


void 
Window3D
::MousePressFunc(int button)
{
  // Allow only one action at a time.
  if ( m_Mode != WIN3D_NONE ) return;

  switch (button)
    {
    case FL_LEFT_MOUSE:
      m_Mode = WIN3D_ROTATE;
      m_Trackball.StartRot( Fl::event_x(), Fl::event_y(), w(), h() );
      break;
    case FL_MIDDLE_MOUSE:
      m_Mode = WIN3D_PAN;
      m_Trackball.StartPan( Fl::event_x(), Fl::event_y() );
      break;
    case FL_RIGHT_MOUSE:
      m_Mode = WIN3D_ZOOM;
      m_Trackball.StartZoom( Fl::event_y() );
      break;
    default: break;
    }
}

void 
Window3D
::MouseReleaseFunc()
{
  switch (m_Mode)
    {
    case WIN3D_ROTATE:
      m_Trackball.StopRot(); break;
    case WIN3D_PAN:
      m_Trackball.StopPan(); break;
    case WIN3D_ZOOM:
      m_Trackball.StopZoom(); break;
    default: break;
    }
  m_Mode = WIN3D_NONE;
}

void 
Window3D
::MouseMotionFunc()
{
  switch (m_Mode)
    {
    case WIN3D_ROTATE:
      m_Trackball.TrackRot( Fl::event_x(), Fl::event_y(), w(), h() );
      redraw();
      break;
    case WIN3D_ZOOM:
      m_Trackball.TrackZoom( Fl::event_y() );
      this->SetupProjection();
      redraw();
      break;
    case WIN3D_PAN:
      m_Trackball.TrackPan( Fl::event_x(), Fl::event_y(), 
                            w(), h(), 2*m_ViewHalf[X], 2*m_ViewHalf[Y] );
      redraw();
      break;
    default: break;
    }
}

void 
Window3D
::MouseCrossPressFunc(int button)
{
  Vector3i hit;

  if (!m_Driver->GetCurrentImageData()->IsGreyLoaded())
    return;

  switch (button)
    {
    case FL_LEFT_MOUSE:

      if (this->IntersectSegData(Fl::event_x(), Fl::event_y(), hit))
        {

        // TODO: Unify this!
        m_Driver->GetCurrentImageData()->SetCrosshairs(hit);
        m_GlobalState->SetCrosshairsPosition( hit );
        }

      //Addition: Nathan Moon
      if (m_GlobalState->GetSnakeActive())
        {
        m_ParentUI->ResetSNAPScrollbars();
        m_ParentUI->RedrawWindows();
        } else
        {
        m_ParentUI->ResetScrollbars();
        m_ParentUI->RedrawWindows();
        }   
      break;
    default: break;
    }
}

/**
 MousePointPressFunc() 
 when dragging the left button, the function gets the coordinates and
 adds it to the spray painted points (m_Samples).
 Vector3i hit is in image coordinates

 post: if max num of m_Samples was not exceeded, then the added spray
 painted point was added and then drawn on the screen.
 */
void 
Window3D
::MousePointPressFunc(int button)
{
  Vector3i hit;

  if (!m_Driver->GetCurrentImageData()->IsGreyLoaded())
    return;

  switch (button)
    {
    case FL_LEFT_MOUSE:
      if (this->IntersectSegData(Fl::event_x(), Fl::event_y(), hit))
        {
        AddSample( hit );
        m_ParentUI->Activate3DAccept(true);
        redraw();
        }
      break;
    default: break;
    }
}

void 
Window3D
::MouseCutPressFunc(int button)
{
  if (!m_Driver->GetCurrentImageData()->IsGreyLoaded())
    return;

  switch (button)
    {
    case FL_LEFT_MOUSE:
      cout << "Left click, drag to start spray" << endl;
      break;
    case FL_RIGHT_MOUSE:
      //    cout << "Handling rt click in cut planes m_Mode" << endl;
      if (-1==m_Plane.valid)
        {
        cout << "saving 1st pt ";   // in middle of defining plane
        OnCutPlanePointRayAction(Fl::event_x(), Fl::event_y(), 1);
        redraw();
        m_Plane.valid = 0;
        break;
        }     // 0 means is in middle of def cut plane
      else if (0==m_Plane.valid)
        {
        cout << "saving 2nd pt, calculating plane" << endl;
        OnCutPlanePointRayAction(Fl::event_x(), Fl::event_y(), 2);

        m_Plane.valid = 1;
        ComputePlane();
        redraw();
        m_ParentUI->Activate3DAccept(true);
        break;
        } else
        {  // plane.valid is 1, plane is already defined
        cout << "m_Plane already defined - Press Accept or Reset View" << endl;
        }
      break;

    default:
      cout << "Cut m_Planes, but not lt/rt click" << endl;
      break;
    }
}

/**
  Does translation and rotation from the m_Trackball (not anisotropic scaling).
  Make sure to glPopMatrix() after use!
  */
void 
Window3D
::SetupModelView()
{
  // Set m_Center of rotation
  Vector3i crosshair = m_GlobalState->GetCrosshairsPosition();
  for (int i=0; i<3; i++) m_CenterOfRotation[i] = crosshair[i];

  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();

  glTranslatef( m_Trackball.GetPanX(), m_Trackball.GetPanY(), 0.0 );
  glMultMatrixf( m_Trackball.GetRot() );
  glTranslatef( -m_CenterOfRotation[0], -m_CenterOfRotation[1], -m_CenterOfRotation[2] );
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
  this->SetupModelView();
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



//------------------------------------------------------------------------
// IntersectSegData(int mouse_x, int mouse_y, Vector3i *hit)
// computes a ray going straight back from the current viewpoint
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
  ComputeRay( x, y, mvmatrix, projmatrix, viewport, pt, ray );

  // The result 
  int result = 0;
  
  // Depending on the situation, we may intersect with a snake image or a
  // segmentation image
  // TODO: Need both conditions?
  if(m_GlobalState->GetSnakeActive() && 
     m_Driver->GetSNAPImageData()->IsSnakeInitialized())
    {
    typedef ImageRayIntersectionFinder<
      float,SnakeImageHitTester> RayCasterType;

    RayCasterType caster;

    result = 
      caster.FindIntersection(
        m_Driver->GetSNAPImageData()->GetLevelSetImage(),
        pt,ray,hit);
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
        pt,ray,hit);
    }
  
  // ray now has the proj ray, pt is the pt in image space
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
    // ray[i] and pt[] have been set
    } else if ( 2==i )
    {
    ComputeRay( x, y, mvmatrix, projmatrix, viewport, m_Plane.cutPt2,
                m_Plane.cutRay2 );
    cout << "Second pt: X = " << Fl::event_x() << " Y = " << Fl::event_y() << " in image space is ("
    << m_Plane.cutPt2[0] << ", " << m_Plane.cutPt2[1] << ", " << m_Plane.cutPt2[2] << ")" << endl;
    cout << "Ray 2: x = " << m_Plane.cutRay2[0] << " y = " << m_Plane.cutRay2[1] << " z = "
    << m_Plane.cutRay2[2] << endl;
    } else
    {
    cerr << "Not able to set Cut m_Plane point: " << i << endl; 
    }
}

//-----------------------------------------------------------------------
// Computem_Plane
//   returns 0 if plane invalid, 1 if plane sucessfully computed
//
// parameters:
//   pt1    1st user-defined pt
//   pt2    2nd user-defined pt
//   ray    ray projected back from pt2 through image from viewpt
//   ABC    output m_Plane coordinates Ax+By+Cz+1=0 if DisZero
//          Ax+By+Cz=0 if !DixZero 
//   DisZero    true if plane goes through origin, false otherwise
// pre:
//   if they pt1==pt2 return 0, plane cannot be calculated
// post
//   pt1 != pt2 return 1
//  ABC returns coordinates of plane equation, and DisZero set
//-----------------------------------------------------------------------

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

  Vector3d ray3;    // create new ray between the 2 pts to help calc plane
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

void Window3D::AddSample( Vector3i s )
{
  // Check to make sure there's room for another sample.
  if ( m_NumberOfUsedSamples == m_NumberOfAllocatedSamples )
    {
    Vector3i *temp;
    temp = new Vector3i[m_NumberOfAllocatedSamples * 2];
    m_NumberOfAllocatedSamples *= 2;
    for ( int i = 0; i < m_NumberOfUsedSamples; i++ )
      temp[i] = m_Samples[i];
    delete [] m_Samples;
    m_Samples = temp;
    }

  // Add another sample.
  m_Samples[m_NumberOfUsedSamples] = s;
  m_NumberOfUsedSamples++;
}

void Window3D::DrawCrosshairs()
{
  if ( !m_CursorVisible ) return;

  Vector3i crosshair = m_GlobalState->GetCrosshairsPosition();

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
  glVertex3d( pt[0],        pt[1],        pt[2] );
  glVertex3d( pt[0]+ray[0], pt[1]+ray[1], pt[2]+ray[2] );
  glEnd();

  cerr << "A = ( " << pt[0] << ", " << pt[1] << ", " << pt[2] << " )" << endl;
  cerr << "B = ( " << pt[0]+ray[0]
  << ", " << pt[1]+ray[1]
  << ", " << pt[2]+ray[2] << " )" << endl;
#endif

  glEnable(GL_LIGHTING);
}


void Window3D::DrawSamples()
{
  unsigned char index = m_GlobalState->GetDrawingColorLabel();
  unsigned char rgb[3];
  m_Driver->GetCurrentImageData()->GetColorLabel(index).GetRGBVector(rgb);

  glColor3ubv(rgb);
  glMatrixMode( GL_MODELVIEW );
  for ( int i = 0; i < m_NumberOfUsedSamples; i++ )
    {
    glPushMatrix(); 
    glTranslatef( m_Samples[i][X], m_Samples[i][Y], m_Samples[i][Z] );

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
void Window3D::DrawCutPlane() {
  if (m_Plane.valid != 1) return;

  GLdouble clipbottom[4]={0.0,1.0,0.0,0.0};
  GLdouble cliptop[4]={0.0,-1/m_ImageSize[1],0.0,1};
  GLdouble clipright[4]={1/m_ImageSize[0],0.0,0.0,1.0};
  GLdouble clipleft[4]={1,0,0,0};
  //glClipPlane(GL_CLIP_PLANE0,clipbottom);
  //glClipPlane(GL_CLIP_PLANE1,cliptop);
  //glClipPlane(GL_CLIP_PLANE2,clipright);
  //glClipPlane(GL_CLIP_PLANE3,clipleft);
  //glEnable(GL_CLIP_PLANE0);
  //glEnable(GL_CLIP_PLANE1);
  //glEnable(GL_CLIP_PLANE2);
  //glEnable(GL_CLIP_PLANE3);

  // Perhaps draw a simple rectangle and do the rest with glScale,
  // glMultMatrix, glTranslate, etc.?
  //glRectf(0.0, 0.0, 1.0, 1.0);

  glBegin(GL_QUADS);
  glColor3f(1.0,1.0,0.0);

  // Spit out a vertex with given x, y coords (assumes m_Plane.coords[Z] != 0)
#define Z_VERTEX(x,y) glVertex3d( (x), (y), \
        ( 1.0 - (m_Plane.coords[X]*(x)) - (m_Plane.coords[Y]*(y)) ) / m_Plane.coords[Z] )

  // Spit out a vertex with given x, z coords (assumes m_Plane.coords[Y] != 0)
#define Y_VERTEX(x,z) glVertex3d( (x), \
        ( 1.0 - (m_Plane.coords[X]*(x)) - (m_Plane.coords[Z]*(z)) ) / m_Plane.coords[Y],\
        (z) )

  if (m_Plane.coords[Z]==0)
    {   // plane is parallel to Z axis
    if (m_Plane.coords[Y]==0)
      {   // plane is parallel to Y axis (also)
      glVertex3d(1.0/m_Plane.coords[X],        0.0,        0.0);
      glVertex3d(1.0/m_Plane.coords[X],        0.0, m_ImageSize[Z]);
      glVertex3d(1.0/m_Plane.coords[X], m_ImageSize[Y], m_ImageSize[Z]);
      glVertex3d(1.0/m_Plane.coords[X], m_ImageSize[Y],        0.0);
      } else
      {
      Y_VERTEX(       0.0,        0.0);
      Y_VERTEX(       0.0, m_ImageSize[Z]);
      Y_VERTEX(m_ImageSize[X], m_ImageSize[Z]);
      Y_VERTEX(m_ImageSize[X],        0.0);
      }
    } else
    {        // Standard case, generic plane
    Z_VERTEX(       0.0,        0.0);
    Z_VERTEX(       0.0, m_ImageSize[Y]);
    Z_VERTEX(m_ImageSize[X], m_ImageSize[Y]);
    Z_VERTEX(m_ImageSize[X],        0.0);
    }

  glEnd();
};

/*Log: Window3D.cxx
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
