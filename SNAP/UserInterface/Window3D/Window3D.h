/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    Window3D.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __Window3D_h_
#define __Window3D_h_

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include "IRISTypes.h"
#include "Trackball.h"
#include "MeshObject.h"

// Forward references to parent classes
class UserInterfaceLogic;
class IRISApplication;
class GlobalState;

//--------------------------------------------------------------
// plane struct
//-------------------------------------------------------------
typedef struct
  {
  Vector3d cutPt1, cutPt2, cutRay1, cutRay2;
  Vector3d coords;
  bool distZero;
  int valid; 
  } CutPlaneStruct;


/**
 * \class Window3D
 * \brief Window used to display the 3D segmentation
 */
class Window3D : public Fl_Gl_Window
{
public:

  enum {X,Y,Z};

  Window3D( int x, int y, int w, int h, const char *l=0 );
  ~Window3D();
  Window3D& operator= ( const Window3D& W ) {};

  /**
   * Register the parent classes with this application
   */
  void Register(int i, UserInterfaceLogic *driver);


  void Init();
  void ClearScreen();
  void ResetView();
  void draw();
  int  handle( int event );

  void UpdateMesh();
  void Accept();

private:

  // Pointer to the application driver for this UI object
  IRISApplication *m_Driver;

  // Pointer to the global state object (shorthand)
  GlobalState *m_GlobalState;

  // Pointer to GUI that contains this Window3D object
  UserInterfaceLogic *m_ParentUI;   

  // Cut planes 3d mode added by Robin
  enum Win3DMode
    {
    WIN3D_NONE, WIN3D_PAN, WIN3D_ZOOM, WIN3D_ROTATE, WIN3D_CUT
    };


  Win3DMode m_Mode;
  Trackball  m_Trackball;
  MeshObject m_Mesh;
  CutPlaneStruct m_Plane;

  // id of window
  int m_Id;
  int m_NeedsInitialization;
  int m_CursorVisible;

  // Coordinate systems: image coords have unit distance equal to one voxel,
  // origin is at the origin of the image (left bottom back).
  // World coords have same origin, but distance is in mm.
  // The mesh object is in world coords, but the crosshairs position and the
  // rays to fire into the seg data are in image coords.

  // Extent of the image, in image coords
  Vector3i m_ImageSize;   

  // Dimensions of a voxel, in mm
  Vector3f m_Spacing;   

  // Center of image, in world coords
  Vector3f m_Center;    

  // Center of rotation, in world coords
  Vector3f m_CenterOfRotation;    

  // width of view, in world coords
  Vector3f m_DefaultHalf, m_ViewHalf; 

  Vector3i *m_Samples;
  int   m_NumberOfUsedSamples;
  int   m_NumberOfAllocatedSamples;

  void MousePressFunc(int button);
  void MouseReleaseFunc();
  void MouseMotionFunc();
  void MouseCrossPressFunc(int button);
  void MousePointPressFunc(int button);
  void MouseCutPressFunc(int button);
  void SetupModelView();
  void SetupProjection();
  void ComputeMatricies( int *vport, double *mview, double *proj );
  void ComputeRay( int x, int y, double *mvmatrix, double *projmat,
                   int *viewport, Vector3d &v, Vector3d &r );
  int IntersectSegData(int mouse_x, int mouse_y, Vector3i &hit);

  void OnCutPlanePointRayAction(int mouse_x, int mouse_y, int i);
  void ComputePlane();
  void DrawCutPlane(); // Added by Robin & Ming

  void AddSample( Vector3i s );
  void DrawCrosshairs();
  void DrawSamples();

  void CheckErrors();

};

#endif // __Window3D_h_

/*Log: Window3D.h
 *Revision 1.1  2003/07/12 04:46:51  pauly
 *Initial checkin of the SNAP application into the InsightApplications tree.
 *
 *Revision 1.6  2003/07/12 01:34:18  pauly
 *More final changes before ITK checkin
 *
 *Revision 1.5  2003/07/11 23:25:33  pauly
 **** empty log message ***
 *
 *Revision 1.4  2003/06/08 23:27:56  pauly
 *Changed variable names using combination of ctags, egrep, and perl.
 *
 *Revision 1.3  2003/04/23 06:05:18  pauly
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
 *Revision 1.1.1.1  2002/12/10 01:35:36  pauly
 *Started the project repository
 *
 *
 *Revision 1.2  2002/03/08 14:06:32  moon
 *Added Header and Log tags to all files
 **/
