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
  m_NeedToRepaintControls = false;
}

int
CrosshairsInteractionMode
::OnMousePress(const FLTKEvent &event)
{
  UpdateCrosshairs(event);
  m_Parent->m_GlobalState->SetUpdateSliceFlag(0);
  return 1;
}

int
CrosshairsInteractionMode
::OnMouseRelease(const FLTKEvent &event, 
                   const FLTKEvent &irisNotUsed(pressEvent))
{
  UpdateCrosshairs(event);
  return 1;
}

int
CrosshairsInteractionMode
::OnMouseDrag(const FLTKEvent &event, 
                const FLTKEvent &irisNotUsed(pressEvent))
{
  UpdateCrosshairs(event);
  m_Parent->m_GlobalState->SetUpdateSliceFlag(1);
  return 1;
}

int
CrosshairsInteractionMode
::OnMouseWheel(const FLTKEvent &event)
{
  // Get the amount of the scroll
  float scroll = (float) Fl::event_dy();
  
  // Get the cross-hairs position in image space
  Vector3ui xCrossImage = m_GlobalState->GetCrosshairsPosition();

  // Map it into slice space
  Vector3f xCrossSlice = 
    m_Parent->MapImageToSlice(to_float(xCrossImage) + Vector3f(0.5f));

  // Advance by the scroll amount
  xCrossSlice[2] += scroll;

  // Map back into display space
  xCrossImage = to_unsigned_int(m_Parent->MapSliceToImage(xCrossSlice));

  // Clamp by the display size
  Vector3ui xSize = m_Driver->GetCurrentImageData()->GetVolumeExtents();
  Vector3ui xCrossClamped = xCrossImage.clamp(
    Vector3ui(0,0,0),xSize - Vector3ui(1,1,1));
  
  // Update the crosshairs position in the current image data
  m_Parent->m_ImageData->SetCrosshairs(xCrossClamped);
  
  // Set the crosshair
  m_GlobalState->SetCrosshairsPosition(xCrossClamped);

  // Cause a repaint
  m_NeedToRepaintControls = true;
  m_ParentUI->RedrawWindows();    

  return 1;
}


void
CrosshairsInteractionMode
::UpdateCrosshairs(const FLTKEvent &event)
{
  // Compute the position in slice coordinates
  Vector3f xClick = m_Parent->MapWindowToSlice(event.XSpace.extract(2));
  
  // Compute the new cross-hairs position in image space
  Vector3f xCross = m_Parent->MapSliceToImage(xClick);

  // Round the cross-hairs position down to integer
  Vector3i xCrossInteger = to_int(xCross);
  
  // Make sure that the cross-hairs position is within bounds by clamping
  // it to image dimensions
  Vector3i xSize = to_int(m_Driver->GetCurrentImageData()->GetVolumeExtents());
  Vector3ui xCrossClamped = to_unsigned_int(
    xCrossInteger.clamp(Vector3i(0),xSize - Vector3i(1)));

  // Update the crosshairs position in the current image data
  m_Parent->m_ImageData->SetCrosshairs(xCrossClamped);
  
  // Update the crosshairs position in the global state
  m_GlobalState->SetCrosshairsPosition(xCrossClamped);

  // Cause a repaint
  m_NeedToRepaintControls = true;
  m_ParentUI->RedrawWindows();
}

void 
CrosshairsInteractionMode::
OnDraw()
{
  // Do the painting only if necessary
  if(m_NeedToRepaintControls)
    {
    // Update the image probe and scrollbar controls
    m_ParentUI->OnCrosshairPositionUpdate();
    
    // No need to call this until another update
    m_NeedToRepaintControls = false;
    }

  // Get the current cursor position
  Vector3ui xCursorInteger = m_GlobalState->GetCrosshairsPosition();

  // Shift the cursor position by by 0.5 in order to have it appear
  // between voxels
  Vector3f xCursorImage = to_float(xCursorInteger) + Vector3f(0.5f);
  
  // Get the cursor position on the slice
  Vector3f xCursorSlice = m_Parent->MapImageToSlice(xCursorImage);

  // Upper and lober bounds to which the crosshairs are drawn
  Vector2i lower(0);
  Vector2i upper = m_Parent->m_SliceSize.extract(2);

  // Set line properties
  glPushAttrib(GL_LINE_BIT);  
  glLineWidth(1.0);  
  glEnable(GL_LINE_STIPPLE);
  
  glColor3f(0.5,0.5,1);
  glLineStipple(2,0xAAAA);
  glBegin(GL_LINES);
  
  // Paint the cross-hairs
  glVertex2f(lower(0), xCursorSlice(1));
  glVertex2f(upper(0), xCursorSlice(1));
  glVertex2f(xCursorSlice(0), lower(1));
  glVertex2f(xCursorSlice(0), upper(1));
  
  glEnd();
  glPopAttrib();
}

