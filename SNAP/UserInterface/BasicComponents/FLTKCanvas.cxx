/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    FLTKCanvas.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include <cstdlib>
#include <ctime>

#include "FLTKCanvas.h"

#include <FL/Fl.H>
#include "SNAPOpenGL.h"

using namespace std;

FLTKCanvas
::FLTKCanvas(int x, int y, int w, int h, const char *label)
: Fl_Gl_Window(x,y,w,h,label)
{
  m_Dragging = false;
  m_FlipYCoordinate = true;
}

void                            
FLTKCanvas
::PushInteractionMode(InteractionMode *mode)
{
  m_Interactors.push_front(mode);
}

InteractionMode *
  FLTKCanvas
  ::PopInteractionMode() {
  InteractionMode *lastMode = m_Interactors.front();
  m_Interactors.pop_front();
  return lastMode;
}

void 
  FLTKCanvas
  ::ClearInteractionStack() 
{
  m_Interactors.clear();
} 

unsigned int 
FLTKCanvas
::GetInteractionModeCount()
{
  return m_Interactors.size();
}

InteractionMode *
FLTKCanvas
::GetTopInteractionMode()
{
  return m_Interactors.front();
}

int 
FLTKCanvas
::handle(int eventID) 
{
  // Create an event object
  FLTKEvent event;
  event.Id = eventID;
  
  // Note that the y coordinate is optionally reflected
  event.XCanvas[0] = Fl::event_x();
  event.XCanvas[1] = m_FlipYCoordinate ? h()-1-Fl::event_y() : Fl::event_y();
  
  event.TimeStamp = clock();
  event.Source = this;  
  event.Button = Fl::event_button();
  event.State = Fl::event_state();

  // Construct the software button
  if(event.Button == FL_LEFT_MOUSE && (event.State & FL_ALT))
    event.SoftButton = FL_RIGHT_MOUSE;
  else if(event.Button == FL_LEFT_MOUSE && (event.State & FL_CTRL))
    event.SoftButton = FL_MIDDLE_MOUSE;
  else if(event.Button == FL_RIGHT_MOUSE && (event.State & FL_CTRL))
    event.SoftButton = FL_MIDDLE_MOUSE;
  else
    event.SoftButton = event.Button;

  // If the window has not been displayed, we can't compute the 
  // object space coordinates
  if (shown() && visible())
    {

    // Make this window the current context
    make_current();

    // Convert the event coordinates into the model view coordinates
    double modelMatrix[16], projMatrix[16];
    int viewport[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    glGetIntegerv(GL_VIEWPORT,viewport);

    // Projection works with doubles, event is a float
    Vector3d xProjection;
    gluUnProject(event.XCanvas[0],event.XCanvas[1],0,
                 modelMatrix,projMatrix,viewport,
                 &xProjection[0],&xProjection[1],&xProjection[2]);
    event.XSpace = to_float(xProjection);
    }

  // Record the event as the drag-start if the mouse was pressed
  if (eventID == FL_PUSH)
    {
    // Debug positions
    m_DragStartEvent = event;
    m_Dragging = true;
    }
  else if(eventID == FL_RELEASE)
    {
    m_Dragging = false;
    }

  // Propagate the event through the stack
  for (list<InteractionMode *>::iterator it = m_Interactors.begin();
      it != m_Interactors.end();it++)
    {
    InteractionMode *mode = *it;
    int result;

    // Delegate the event base on the ID
    switch (eventID)
      {
      case FL_PUSH : 
        result = mode->OnMousePress(event);
        break;
      case FL_DRAG : 
        result = mode->OnMouseDrag(event,m_DragStartEvent);
        break;
      case FL_RELEASE : 
        result = mode->OnMouseRelease(event,m_DragStartEvent);
        break;
      case FL_ENTER : 
        result = mode->OnMouseEnter(event);
        break;
      case FL_LEAVE : 
        result = mode->OnMouseLeave(event);
        break;
      case FL_MOVE : 
        result = mode->OnMouseMotion(event);
        break;
      case FL_MOUSEWHEEL :
        result = mode->OnMouseWheel(event);
        break;
      case FL_KEYDOWN :
        result = mode->OnKeyDown(event);
        break;
      case FL_KEYUP :
        result = mode->OnKeyUp(event);
        break;
      case FL_SHORTCUT :
        result = mode->OnShortcut(event);
      default:
        result = mode->OnOtherEvent(event);
      };

    // Break out if the event was taken care of
    if (result)
      return 1;
    }

  // The event was not handled
  return 1;
}

void 
FLTKCanvas
::FireInteractionDrawEvent()
{
  // Propagate the drawing event through the stack
  for (list<InteractionMode *>::reverse_iterator it = m_Interactors.rend();
      it!=m_Interactors.rbegin();it--)
    {
    InteractionMode *mode = *it;
    mode->OnDraw();
    }
}


