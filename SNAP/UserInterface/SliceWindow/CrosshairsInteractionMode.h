/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    CrosshairsInteractionMode.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __CrosshairsInteractionMode_h_
#define __CrosshairsInteractionMode_h_

#include "GenericSliceWindow.h"
#include "GlobalState.h"

/**
 * \class CrosshairsInteractionMode
 * \brief UI interaction mode that takes care of crosshair positioning.
 *
 * \see GenericSliceWindow
 */
class CrosshairsInteractionMode : public GenericSliceWindow::EventHandler {
public:
  CrosshairsInteractionMode(GenericSliceWindow *parent);

  int OnMousePress(const FLTKEvent &event)
  {
    UpdateCrosshairs(event);
    m_Parent->m_GlobalState->SetUpdateSliceFlag(0);
    return 1;
  }

  int OnMouseRelease(const FLTKEvent &event, const FLTKEvent &pressEvent)
  {
    UpdateCrosshairs(event);
    return 1;
  }

  int OnMouseDrag(const FLTKEvent &event, const FLTKEvent &pressEvent)
  {
    UpdateCrosshairs(event);
    m_Parent->m_GlobalState->SetUpdateSliceFlag(1);
    return 1;
  }

  void OnDraw();

private:
  void UpdateCrosshairs(const FLTKEvent &event);
};

#endif // __CrosshairsInteractionMode_h_
