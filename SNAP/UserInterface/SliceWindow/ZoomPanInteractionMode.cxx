/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ZoomPanInteractionMode.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "ZoomPanInteractionMode.h"

#include <cmath>
#include <iostream>

ZoomPanInteractionMode
::ZoomPanInteractionMode(GenericSliceWindow *parent)
: GenericSliceWindow::EventHandler(parent)
{

}

int
ZoomPanInteractionMode
::OnMousePress(const FLTKEvent &irisNotUsed(event))
{
  // Record the current zoom and view position
  m_StartViewZoom = m_Parent->m_ViewZoom;
  m_StartViewPosition = m_Parent->m_ViewPosition;

  // Compute the limits for the view zoom
  m_ViewZoomMax = 0.25 * m_Parent->w() / m_Parent->m_SliceScale(0);
  m_ViewZoomMin = 
    32.0f / (m_Parent->m_SliceSize(0) * m_Parent->m_SliceScale(0));
  
  // Done
  return 1;
}

int 
ZoomPanInteractionMode
::OnMouseDrag(const FLTKEvent &event,const FLTKEvent &dragEvent)
{
  if(dragEvent.Button == FL_LEFT_MOUSE)
  {
    // Compute the start and end point in slice coordinates
    Vector2f xStart = m_Parent->MapWindowToSlice(dragEvent.XSpace.extract(2));
    Vector2f xEnd = m_Parent->MapWindowToSlice(event.XSpace.extract(2));
    
    // Under the left button, the tool changes the view_pos by the
    // distance traversed
    m_Parent->m_ViewPosition = m_StartViewPosition - (xEnd - xStart);
  }
  else 
  {
    // Under the right button, the tool causes us to zoom based on the vertical
    // motion
    float zoom = 
      m_StartViewZoom * pow(1.02,event.XSpace(1) - dragEvent.XSpace(1));

    // We are not allowed to zoom past certain limits
    m_Parent->m_ViewZoom = 
      zoom < m_ViewZoomMin ? m_ViewZoomMin :
      zoom > m_ViewZoomMax ? m_ViewZoomMax : zoom;
  }

  // Redraw the screen
  m_Parent->redraw();

  // Nothing to do here.
  return 1;
}

int 
ZoomPanInteractionMode
::OnMouseRelease(const FLTKEvent &irisNotUsed(event),
                 const FLTKEvent &irisNotUsed(dragEvent))
{
  // Nothing to do here!
  return 1;
}

