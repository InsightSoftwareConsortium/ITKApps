/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    CrosshairsInteractionMode.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "CrosshairsInteractionMode.h"
#include "IRISApplication.h"
#include "IRISImageData.h"
#include "UserInterfaceLogic.h"

CrosshairsInteractionMode
::CrosshairsInteractionMode(GenericSliceWindow *parent) 
: GenericSliceWindow::EventHandler(parent)
{

}

void
CrosshairsInteractionMode
::UpdateCrosshairs(const FLTKEvent &event)
{
  // Compute the position in slice coordinates
  Vector2f uvClick = m_Parent->MapWindowToSlice(event.XSpace.extract(2));
  
  // Compute the new cross-hairs position in image space
  Vector3f xCross = m_Parent->MapSliceToImage(uvClick);

  // Round the cross-hairs position down to integer
  Vector3i xCrossInteger = to_int(xCross);
  
  // Make sure that the cross-hairs position is within bounds by clamping
  // it to image dimensions
  Vector3i xSize = m_Driver->GetCurrentImageData()->GetVolumeExtents();
  Vector3i xCrossClamped = xCrossInteger.clamp(Vector3i(0),xSize - Vector3i(1));

  // Update the crosshairs position in the current image data
  m_Parent->m_ImageData->SetCrosshairs(xCrossClamped);
  
  // Update the crosshairs position in the global state
  m_GlobalState->SetCrosshairsPosition(xCrossClamped);

  // Cause a repaint
  m_ParentUI->ResetScrollbars();
  m_ParentUI->RedrawWindows();
}

void 
CrosshairsInteractionMode::
OnDraw()
{
  // Get the current cursor position
  Vector3i xCursorInteger = m_GlobalState->GetCrosshairsPosition();

  // Shift the cursor position by by 0.5 in order to have it appear
  // between voxels
  Vector3f xCursor = to_float(xCursorInteger) + Vector3f(0.5f);
  
  // Get the cursor position on the slice
  Vector2f uvCursor = m_Parent->MapImageToSlice(xCursor);

  // Upper and lober bounds to which the crosshairs are drawn
  Vector2i lower(0);
  Vector2i upper = m_Parent->m_SliceSize.extract(2);

  // Set line properties
  glPushAttrib(GL_LINE_BIT);  
  glLineWidth(1);  
  glEnable(GL_LINE_STIPPLE);
  
  glColor3f(0,0,1);
  glLineStipple(2,0xAAAA);
  glBegin(GL_LINES);
  
  // Paint the cross-hairs
  glVertex2f(lower(0), uvCursor(1));
  glVertex2f(upper(0), uvCursor(1));
  glVertex2f(uvCursor(0), lower(1));
  glVertex2f(uvCursor(0), upper(1));
  
  glEnd();
  glPopAttrib();
}

