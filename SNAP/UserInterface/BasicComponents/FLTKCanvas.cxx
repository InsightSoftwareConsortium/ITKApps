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
#include "GLToPNG.h"


FLTKCanvas
::FLTKCanvas(int lx, int ly, int lw, int lh, const char *llabel)
: Fl_Gl_Window(lx,ly,lw,lh,llabel)
{
  m_Dragging = false;
  m_FlipYCoordinate = true;
  m_GrabFocusOnEntry = false;
  m_Focus = false;
  m_DumpPNG = NULL;
}

void
FLTKCanvas
::SaveAsPNG(const char *filename)
{
  m_DumpPNG = filename;
  this->redraw();
  Fl::flush();
  m_DumpPNG = NULL;
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
  event.Key = Fl::event_key();

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
    GLint viewport[4];
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

  // Take care of focus grabbing on entry
  if(eventID == FL_ENTER && m_GrabFocusOnEntry)
    {
    this->take_focus();
    m_Focus = true;
    redraw();
    return 1;
    }
  else if(eventID == FL_LEAVE && m_GrabFocusOnEntry)
    {
    m_Focus = false;
    redraw();
    return 1;
    }
  else if(eventID == FL_FOCUS && m_GrabFocusOnEntry)
    {
    return 1;
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
  for (std::list<InteractionMode *>::iterator it = m_Interactors.begin();
      it != m_Interactors.end();it++)
    {
    InteractionMode *lmode = *it;
    int result;

    // Delegate the event base on the ID
    switch (eventID)
      {
      case FL_PUSH : 
        result = lmode->OnMousePress(event);
        break;
      case FL_DRAG : 
        result = lmode->OnMouseDrag(event,m_DragStartEvent);
        break;
      case FL_RELEASE : 
        result = lmode->OnMouseRelease(event,m_DragStartEvent);
        break;
      case FL_ENTER : 
        result = lmode->OnMouseEnter(event);
        break;
      case FL_LEAVE : 
        result = lmode->OnMouseLeave(event);
        break;
      case FL_MOVE : 
        result = lmode->OnMouseMotion(event);
        break;
      case FL_MOUSEWHEEL :
        result = lmode->OnMouseWheel(event);
        break;
      case FL_KEYDOWN :
        result = lmode->OnKeyDown(event);
        break;
      case FL_KEYUP :
        result = lmode->OnKeyUp(event);
        break;
      case FL_SHORTCUT :
        result = lmode->OnShortcut(event);
      default:
        result = lmode->OnOtherEvent(event);
      };

    // Break out if the event was taken care of
    if (result)
      return 1;
    }

  // The event was not handled
  return 0;
}

void
FLTKCanvas
::draw()
{
  // Let the children do the drawing
  FireInteractionDrawEvent();

  // dump png if requested
  if (m_DumpPNG != NULL)
  {
    vtkImageData* img = 
      GLToVTKImageData((unsigned int) GL_RGBA, 0, 0, w(), h());
    VTKImageDataToPNG(img, m_DumpPNG);
  }
}

