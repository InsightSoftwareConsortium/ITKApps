/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    PolygonInteractionMode.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "PolygonInteractionMode.h"

#include "GlobalState.h"
#include "PolygonDrawing.h"
#include "UserInterfaceLogic.h"


PolygonInteractionMode
::PolygonInteractionMode(GenericSliceWindow *parent)
: GenericSliceWindow::EventHandler(parent)
{
  m_Drawing = new PolygonDrawing();
}

PolygonInteractionMode
::~PolygonInteractionMode()
{
  delete m_Drawing;
}

int
PolygonInteractionMode
::OnEitherEvent(const FLTKEvent &event, 
                const FLTKEvent &irisNotUsed(pressEvent))
{
  // We'll need these shorthands
  int id = m_Parent->m_Id;

#ifdef DRAWING_LOCK
  if (!m_GlobalState->GetDrawingLock(id)) break;
#endif /* DRAWING_LOCK */
  
  // Compute the dimension of a pixel on the screen
  Vector2f pixelSize = GetPixelSizeVector();

  // Map the event into slice coordinates 
  Vector2f xEvent = m_Parent->MapWindowToSlice(event.XSpace.extract(2));

  // Handle the event
  int rc = m_Drawing->Handle(event.Id,event.Button,
                             xEvent(0),xEvent(1),
                             pixelSize(0),pixelSize(1));

  if(rc) 
  {
    // Update the UI if the event was handled
    if (m_Drawing->GetState() == INACTIVE_STATE) {
      m_ParentUI->ActivatePaste(id, true);
      m_ParentUI->ActivateAccept(id, false);

  #ifdef DRAWING_LOCK
      m_GlobalState->ReleaseDrawingLock(id);
  #endif /* DRAWING_LOCK */

    } else {
      m_ParentUI->ActivatePaste(id, false);
    }
    if (m_Drawing->GetState() == EDITING_STATE)
      m_ParentUI->ActivateAccept(id, true);

    m_Parent->redraw();
  }
  
  return rc;
}

Vector2f 
PolygonInteractionMode
::GetPixelSizeVector()
{
  return 
    m_Parent->MapWindowToSlice(Vector2f(1.0f)) - 
    m_Parent->MapWindowToSlice(Vector2f(0.0f));
}


void
PolygonInteractionMode
::OnDraw()
{
  // Compute the dimension of a pixel on the screen
  Vector2f pixelSize = GetPixelSizeVector();

  // Call the poly's draw method
  m_Drawing->Draw(pixelSize(0),pixelSize(1));
}

