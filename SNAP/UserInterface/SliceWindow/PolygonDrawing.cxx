/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    PolygonDrawing.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "PolygonDrawing.h"

#include <GL/glu.h>
#include <iostream>
#include <stdlib.h>

using std::cerr;
using std::endl;

// glu Tess callbacks

#ifdef WIN32
typedef void (CALLBACK *TessCallback)();
#else
typedef void (*TessCallback)();
#endif

void 
#ifdef WIN32
CALLBACK 
#endif
BeginCallback(GLenum which)
{
  glBegin(which);
}

void 
#ifdef WIN32
CALLBACK 
#endif
EndCallback(void) 
{
  glEnd();
}

void 
#ifdef WIN32
CALLBACK
#endif
ErrorCallback(GLenum errorCode)
{
  const GLubyte *estring;

  estring = gluErrorString(errorCode);
  cerr << "Tesselation Error-Exiting: " << estring << endl;
  exit(-1);
}


void 
#ifdef WIN32
CALLBACK 
#endif
CombineCallback(GLdouble coords[3], GLdouble *vertex_data[4],  
                GLfloat weight[4], GLdouble **dataOut) 
{
  GLdouble *vertex;

  vertex = new GLdouble[3];
  vertex[0] = coords[0];
  vertex[1] = coords[1];
  vertex[2] = coords[2];
  *dataOut = vertex;
}

/**
 * PolygonDrawing()
 *
 * purpose: 
 * create initial vertex and m_Cache arrays, init GLUm_Tesselatorelator
 */
PolygonDrawing
::PolygonDrawing()
{
  m_Vertices = new PolygonVertex[8];
  m_Cache = new PolygonVertex[8];
  m_NumberOfUsedVertices = 0; 
  m_NumberOfAllocatedVertices = 8;
  m_CachedPolygon = 0;
  m_State = INACTIVE_STATE;
  m_SelectedVertices = 0;
  m_DraggingPickBox = 0;

  // create glu Tesselator for rendering polygons

  m_Tesselator = gluNewTess();
  gluTessCallback(m_Tesselator,(GLenum) GLU_TESS_VERTEX, (TessCallback) glVertex3dv);
  gluTessCallback(m_Tesselator,(GLenum) GLU_TESS_BEGIN, (TessCallback) BeginCallback);
  gluTessCallback(m_Tesselator,(GLenum) GLU_TESS_END, (TessCallback) EndCallback);
  gluTessCallback(m_Tesselator,(GLenum) GLU_TESS_ERROR, (TessCallback) ErrorCallback);     
  gluTessCallback(m_Tesselator,(GLenum) GLU_TESS_COMBINE, (TessCallback) CombineCallback);
  gluTessProperty(m_Tesselator,(GLenum) GLU_TESS_WINDING_RULE,GLU_TESS_WINDING_NONZERO);  
  gluTessNormal(m_Tesselator,0,0,1);
}

/**
 * ~PolygonDrawing()
 *
 * purpose: 
 * free arrays and GLUm_Tesselatorelator
 */
PolygonDrawing
::~PolygonDrawing()
{
  delete [] m_Vertices;

  delete [] m_Cache;
  gluDeleteTess(m_Tesselator);
}

/**
 * ComputeEditBox()
 *
 * purpose: 
 * compute the bounding box around selected vertices
 * 
 * post:
 * if m_Vertices are selected, sets m_SelectedVertices to 1, else 0
 */
void
PolygonDrawing
::ComputeEditBox() 
{
  // set m_SelectedVertices if a selected vertex is found; 
  // also init box extents with that vertex

  int i;

  m_SelectedVertices = 0;
  for (i = 0; i < m_NumberOfUsedVertices; i++) 
    {
    if (m_Vertices[i].selected) 
      {
      m_EditBox[0] = m_EditBox[1] = m_Vertices[i].x;
      m_EditBox[2] = m_EditBox[3] = m_Vertices[i].y;
      m_SelectedVertices = 1;
      break;
      }
    }

  if (!m_SelectedVertices) return;

  // grow box extents
  for (i = 0; i < m_NumberOfUsedVertices; i++) 
    {
    if (m_Vertices[i].selected) 
      {
      if (m_Vertices[i].x < m_EditBox[0]) m_EditBox[0] = m_Vertices[i].x;
      else if (m_Vertices[i].x > m_EditBox[1]) m_EditBox[1] = m_Vertices[i].x;

      if (m_Vertices[i].y < m_EditBox[2]) m_EditBox[2] = m_Vertices[i].y;
      else if (m_Vertices[i].y > m_EditBox[3]) m_EditBox[3] = m_Vertices[i].y;
      }
    }
}

/**
 * GetState()
 *
 * purpose:
 * return the m_State of polygon_drawing, either INACTIVE_STATE, DRAWING_STATE,
 * or EDITING_STATE
 */
PolygonState
PolygonDrawing
::GetState()
{
  return m_State;
}

/**
 * Add()
 *
 * purpose:
 * to add a vertex to the existing contour
 * 
 * pre: 
 * m_NumberOfAllocatedVertices > 0
 */
void
PolygonDrawing
::Add(float x, float y, int selected)
{
  // make sure there's room

  if (m_NumberOfUsedVertices == m_NumberOfAllocatedVertices) 
  {
    PolygonVertex *temp;
    temp = new PolygonVertex[m_NumberOfAllocatedVertices * 2];
    m_NumberOfAllocatedVertices *= 2;
        
    for (int i = 0; i < m_NumberOfUsedVertices; i++) temp[i] = m_Vertices[i];
        
    delete [] m_Vertices;
    m_Vertices = temp;
  }

    
  // add a new vertex

  m_Vertices[m_NumberOfUsedVertices].x = x;
  m_Vertices[m_NumberOfUsedVertices].y = y;
  m_Vertices[m_NumberOfUsedVertices].selected = selected;
    
  m_NumberOfUsedVertices++;
}

/**
 * Delete()
 *
 * purpose: 
 * delete all vertices that are selected
 * 
 * post: 
 * if all m_Vertices removed, m_State becomes INACTIVE_STATE
 * length of m_Vertices array does not decrease
 */
void
PolygonDrawing
::Delete() 
{
  int kept = 0;
  for (int i=0; i<m_NumberOfUsedVertices; i++)
    if (!m_Vertices[i].selected) m_Vertices[kept++] = m_Vertices[i];

  m_SelectedVertices = 0;
  m_NumberOfUsedVertices = kept;
  if (m_NumberOfUsedVertices == 0) m_State = INACTIVE_STATE;
}

/**
 * Insert()
 *
 * purpose:
 * insert vertices between adjacent selected vertices
 * 
 * post: 
 * length of m_Vertices array does not decrease
 */
void
PolygonDrawing
::Insert() 
{
  // don't ever shrink m_Vertices array, or pasting might crash

  PolygonVertex *temp = new PolygonVertex[m_NumberOfAllocatedVertices];
  int i;
  for (i = 0; i < m_NumberOfUsedVertices; i++) temp[i] = m_Vertices[i];

  int num_m_Vertices = m_NumberOfUsedVertices;
  m_NumberOfUsedVertices = 0;

  for (i = 0; i < num_m_Vertices; i++) 
    {
    int n = (i + 1)%num_m_Vertices;
    Add(temp[i].x,temp[i].y,temp[i].selected);

    if (temp[i].selected && temp[n].selected)
      {
      Add((float)((temp[i].x + temp[n].x)/2.0),(float)((temp[i].y + temp[n].y)/2.0),1);
      }
    }
}

/**
 * CachedPolygon()
 *
 * purpose:
 * return whether polygon has m_Cached polygon
 */
int
PolygonDrawing
::CachedPolygon(void)
{
  return m_CachedPolygon;
}

/**
 * AcceptPolygon()
 *
 * purpose:
 * to rasterize the current polygon into a buffer & copy the edited polygon
 * into the polygon m_Cache
 *
 * parameters:
 * buffer - an array of unsigned chars interpreted as an RGBA buffer
 * width  - the width of the buffer
 * height - the height of the buffer 
 *
 * pre: 
 * buffer array has size width*height*4
 * m_State == EDITING_STATE
 *
 * post: 
 * m_State == INACTIVE_STATE
 */
void 
PolygonDrawing
::AcceptPolygon(unsigned char *buffer, int width, int height) 
{
  // Push the GL attributes
  glPushAttrib(GL_COLOR_BUFFER_BIT | GL_VIEWPORT_BIT);

  // Draw polygon into back buffer - back buffer should get redrawn
  // anyway before it gets swapped to the screen.
  glDrawBuffer(GL_BACK);
  glReadBuffer(GL_BACK);

  glClearColor(0,0,0,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Create a new projection matrix.  Why do this?  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0,(float)width,0,(float)height);

  // New model matrix
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // Set up the viewport
  glViewport(0,0,width,height);

  // Paint in white
  glColor3d(1,1,1);

  // GLU m_Tesselatorelator draws the poly
  double (*v)[3];
  int i;
  v = new double [m_NumberOfUsedVertices][3];
  for (i = 0; i < m_NumberOfUsedVertices; i++) 
    {
    v[i][0] = (double)m_Vertices[i].x;
    v[i][1] = (double)m_Vertices[i].y;
    v[i][2] = 0.0;
    }

  gluTessBeginPolygon(m_Tesselator,NULL);
  gluTessBeginContour(m_Tesselator);
  for (i = 0; i < m_NumberOfUsedVertices; i++) 
    gluTessVertex(m_Tesselator,v[i],v[i]);
  gluTessEndContour(m_Tesselator);
  gluTessEndPolygon(m_Tesselator);

  delete [] v;

  glPopMatrix();

  glMatrixMode(GL_PROJECTION);

  glPopMatrix();


  // fetch it back into buffer

  glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,buffer);

  // copy polygon into polygon m_Cache

  glPopAttrib();

  delete [] m_Cache;
  m_Cache = new PolygonVertex[m_NumberOfUsedVertices];
  for (i = 0; i < m_NumberOfUsedVertices; i++) m_Cache[i] = m_Vertices[i];
  m_NumberOfCachedVertices = m_NumberOfUsedVertices;
  m_CachedPolygon = 1;

  // reset the vertex array for next time

  m_NumberOfUsedVertices = 0;
  m_State = INACTIVE_STATE;
  m_SelectedVertices = 0;
}

/**
 * PastePolygon()
 *
 * purpose:
 * copy the m_Cached polygon to the edited polygon
 * 
 * pre: 
 * m_CachedPolygon == 1
 * m_State == INACTIVE_STATE
 * 
 * post: 
 * m_State == EDITING_STATE
 */
void 
PolygonDrawing
::PastePolygon(void)
{
  // the number of m_Vertices alloced never decreases, so the m_Cache will 
  // always fit in the current vertex list

  for (int i = 0; i < m_NumberOfCachedVertices; i++) 
    {    
    m_Vertices[i].x = m_Cache[i].x;
    m_Vertices[i].y = m_Cache[i].y;
    m_Vertices[i].selected = 1;
    }
  m_NumberOfUsedVertices = m_NumberOfCachedVertices;
  m_State = EDITING_STATE;
  ComputeEditBox();
  m_SelectedVertices = 1;
}

/**
 * Draw()
 *
 * purpose: 
 * draw the polyline being drawn or the polygon being edited
 *
 * parameters:
 * pixel_x - this is a width in the polygon's space that is a single 
 *           pixel width on screen
 * pixel_y - this is a height in the polygon's space that is a single 
 *           pixel height on screen
 *  
 * pre: none - expected to exit if m_State is INACTIVE_STATE
 */
void
PolygonDrawing
::Draw(float pixel_x, float pixel_y)
{


  if (m_State == INACTIVE_STATE) return;
    
  int i;

  // set line and point drawing parameters

  glPointSize(4);
  glLineWidth(2);
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // draw the line segments

  if (m_State == EDITING_STATE)
  {
    glBegin(GL_LINES);
      
    for (i = 0; i < m_NumberOfUsedVertices; i++) 
    {
      int n = (i + 1)%m_NumberOfUsedVertices;
      if (m_Vertices[i].selected && m_Vertices[n].selected) glColor3f(0,1,0);
      else glColor3f(1,0,0);

      glVertex3f(m_Vertices[i].x,m_Vertices[i].y,0);
      glVertex3f(m_Vertices[n].x,m_Vertices[n].y,0);
    }
    glEnd();
  }
  else 
  {
    glBegin(GL_LINE_STRIP);
    glColor3f(1,0,0);
    for (i = 0; i < m_NumberOfUsedVertices; i++) glVertex3f(m_Vertices[i].x,m_Vertices[i].y,0);
    glEnd();
  }
    
  // draw the vertices

  glBegin(GL_POINTS);
  for (i = 0; i < m_NumberOfUsedVertices; i++) 
  {
    if (m_Vertices[i].selected) glColor3f(0, 1, 0); 
    else glColor3f(1, 0, 0);
    glVertex3f(m_Vertices[i].x,m_Vertices[i].y,0.0);

  }
  glEnd();

  // draw edit or pick box

  if (m_DraggingPickBox) 
  {
    glLineWidth(1);
    glColor3f(0, 1, 0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(m_SelectionBox[0],m_SelectionBox[2],0.0);
    glVertex3f(m_SelectionBox[1],m_SelectionBox[2],0.0);
    glVertex3f(m_SelectionBox[1],m_SelectionBox[3],0.0);
    glVertex3f(m_SelectionBox[0],m_SelectionBox[3],0.0);
    glEnd();    
  }
  else if (m_SelectedVertices) 
  {
    float border_x = (float)4.0 * pixel_x;
    float border_y = (float)4.0 * pixel_y;
    glLineWidth(1);
    glColor3f(0, 1, 0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(m_EditBox[0] - border_x,m_EditBox[2] - border_y,0.0);
    glVertex3f(m_EditBox[1] + border_x,m_EditBox[2] - border_y,0.0);
    glVertex3f(m_EditBox[1] + border_x,m_EditBox[3] + border_y,0.0);
    glVertex3f(m_EditBox[0] - border_x,m_EditBox[3] + border_y,0.0);
    glEnd();
  }

  glDisable(GL_BLEND);
  glDisable(GL_LINE_SMOOTH);
}

/**
 * Handle()
 *
 * purpose:
 * handle events from the window that contains polygon drawing object:
 * if internal m_State is DRAWING_STATE
 *   left-click creates subsequent vertices of the polygon, 
 *   right-click closes polygon and puts polygon in EDITING_STATE
 * if internal m_State is EDITING_STATE
 *   shift-left-click on vertex adds vertex to selection
 *   shift-left-click off vertex begins dragging a rubber-band box
 *
 *   shift-right-click performs same actions but de-selects vertices
 *
 *   left-click in selection box begins dragging of selected vertices
 *   left-click outside selection box cancels selection, begins a 
 *   dragging of a rubber-band box
 *   
 *   left-release after dragging adds vertices inside rubber-band box
 *   to selection
 * 
 *   pressing the 'insert' key calls Insert()
 *   pressing the 'delete' key calls Delete()
 * 
 * parameters:
 * event   - an Fl event number
 * x       - the x of the point clicked in the space of the polygon
 * y       - the y of the point clicked in the space of the polygon
 * pixel_x - see Draw()
 * pixel_y - see Draw()
 *
 * pre: 
 * window that calls this has the drawing lock
 * 
 * post:
 * if event is used, 1 is returned, else 0
 */
int
PolygonDrawing
::Handle(int event, int button, float x, float y, 
         float pixel_x, float pixel_y)
{
  int i;

  switch (m_State) {
  case INACTIVE_STATE:
    if ((event == FL_PUSH) && (button == FL_LEFT_MOUSE)) 
      {
      m_State = DRAWING_STATE;
      Add(x,y,0);
      return 1;
      }
    break;

  case DRAWING_STATE:
    if (event == FL_PUSH) 
      {
      if (button == FL_LEFT_MOUSE)
        {
        Add(x,y,0);
        return 1;
        } 
      else if (button == FL_RIGHT_MOUSE) 
        {
        m_State = EDITING_STATE;
        for (i = 0; i < m_NumberOfUsedVertices; i++) 
          m_Vertices[i].selected = 1;
        m_SelectedVertices = 1;
        ComputeEditBox();
        return 1;
        }
      }
    break;

  case EDITING_STATE:
    switch (event) {
    case FL_PUSH:
      m_StartX = x;
      m_StartY = y;

      if ((button == FL_LEFT_MOUSE) || (button == FL_RIGHT_MOUSE)) 
        {

        // if user is pressing shift key, add/toggle m_Vertices, or drag pick box
        if (Fl::event_state(FL_SHIFT)) 
          {
          // check if vertex clicked
          for (i = 0; i < m_NumberOfUsedVertices; i++) 
            {
            if ((x >= m_Vertices[i].x - 2.0*pixel_x) &&
              (x <= m_Vertices[i].x + 2.0*pixel_x) &&
              (y >= m_Vertices[i].y - 2.0*pixel_y) &&
              (y <= m_Vertices[i].y + 2.0*pixel_y)) 
              {
              m_Vertices[i].selected = (button == 1);
              ComputeEditBox();
              return 1;
              }
            }

          // otherwise start dragging pick box
          m_DraggingPickBox = 1;
          m_SelectionBox[0] = m_SelectionBox[1] = x;
          m_SelectionBox[2] = m_SelectionBox[3] = y;
          return 1;
          }

        // user not holding shift key; if user clicked inside edit box, 
        // edit box will be moved in drag event
        if (m_SelectedVertices &&
          (x >= (m_EditBox[0] - 4.0*pixel_x)) && 
          (x <= (m_EditBox[1] + 4.0*pixel_x)) && 
          (y >= (m_EditBox[2] - 4.0*pixel_y)) && 
          (y <= (m_EditBox[3] + 4.0*pixel_y))) return 1;

        // clicked outside of edit box & shift not held, this means the 
        // current selection will be cleared
        for (i = 0; i < m_NumberOfUsedVertices; i++) m_Vertices[i].selected = 0;
        m_SelectedVertices = 0;

        // check if point clicked
        for (i = 0; i < m_NumberOfUsedVertices; i++) 
          {
          if ((x >= m_Vertices[i].x - 2.0*pixel_x) &&
            (x <= m_Vertices[i].x + 2.0*pixel_x) &&
            (y >= m_Vertices[i].y - 2.0*pixel_y) &&
            (y <= m_Vertices[i].y + 2.0*pixel_y)) 
            {
            m_Vertices[i].selected = 1;
            ComputeEditBox();
            return 1;
            }
          }

        // didn't click a point - start dragging pick box
        m_DraggingPickBox = 1;
        m_SelectionBox[0] = m_SelectionBox[1] = x;
        m_SelectionBox[2] = m_SelectionBox[3] = y;
        return 1;
        }
      break;

    case FL_DRAG:
      if ((button == FL_LEFT_MOUSE) || (button == FL_RIGHT_MOUSE)) 
        {
        if (m_DraggingPickBox) 
          {
          m_SelectionBox[1] = x;
          m_SelectionBox[3] = y;
          } 
        else 
          {
          if (button == FL_LEFT_MOUSE) 
            {
            m_EditBox[0] += (x - m_StartX);
            m_EditBox[1] += (x - m_StartX);
            m_EditBox[2] += (y - m_StartY);
            m_EditBox[3] += (y - m_StartY);

            for (i = 0; i < m_NumberOfUsedVertices; i++) 
              {
              if (m_Vertices[i].selected) 
                {
                m_Vertices[i].x += (x - m_StartX);
                m_Vertices[i].y += (y - m_StartY);
                }
              }
            m_StartX = x;
            m_StartY = y;
            }
          }
        return 1;
        }
      break;

    case FL_RELEASE:
      if ((button == FL_LEFT_MOUSE) || (button == FL_RIGHT_MOUSE)) 
        {
        if (m_DraggingPickBox) 
          {
          m_DraggingPickBox = 0;

          float temp;
          if (m_SelectionBox[0] > m_SelectionBox[1]) 
            {
            temp = m_SelectionBox[0];
            m_SelectionBox[0] = m_SelectionBox[1];
            m_SelectionBox[1] = temp;
            }
          if (m_SelectionBox[2] > m_SelectionBox[3]) 
            {
            temp = m_SelectionBox[2];
            m_SelectionBox[2] = m_SelectionBox[3];
            m_SelectionBox[3] = temp;
            }

          for (i = 0; i < m_NumberOfUsedVertices; i++) 
            {
            if ((m_Vertices[i].x >= m_SelectionBox[0]) && 
              (m_Vertices[i].x <= m_SelectionBox[1]) &&
              (m_Vertices[i].y >= m_SelectionBox[2]) && 
              (m_Vertices[i].y <= m_SelectionBox[3]))
              m_Vertices[i].selected = (button == 1);
            }
          ComputeEditBox();
          }
        return 1;
        }
      break;

    case FL_SHORTCUT:
      if (Fl::event_key(FL_Delete) || Fl::event_key(FL_BackSpace) ||
        (Fl::event_state(FL_CTRL) && Fl::event_key('d')) ||
        (Fl::event_state(FL_CTRL) && Fl::event_key('D')) ) 
        { 
        Delete(); 
        return 1;
        } 
      else if (Fl::event_key(FL_Insert) ||
        (Fl::event_state(FL_CTRL) && Fl::event_key('i')) ||
        (Fl::event_state(FL_CTRL) && Fl::event_key('I')) ) 
        {
        Insert(); 
        return 1;
        }
      break;

    default: break;
    }
    break;

  default: cerr << "PolygonDrawing::Handle(): unknown m_State " << m_State << endl;
  }

  return 0;
}





/*Log: PolygonDrawing.cxx
/*Revision 1.1  2003/07/11 23:28:10  pauly
/**** empty log message ***
/*
/*Revision 1.3  2003/06/08 23:27:56  pauly
/*Changed variable names using combination of ctags, egrep, and perl.
/*
/*Revision 1.2  2003/04/29 14:01:42  pauly
/*Charlotte Trip
/*
/*Revision 1.1  2003/03/07 19:29:47  pauly
/*Initial checkin
/*
/*Revision 1.2  2002/12/16 16:40:19  pauly
/**** empty log message ***
/*
/*Revision 1.1.1.1  2002/12/10 01:35:36  pauly
/*Started the project repository
/*
/*
/*Revision 1.2  2002/03/08 14:06:30  moon
/*Added Header and Log tags to all files
/**/
