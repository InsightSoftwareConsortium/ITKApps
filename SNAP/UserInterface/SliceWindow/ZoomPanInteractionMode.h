/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ZoomPanInteractionMode.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ZoomPanInteractionMode_h_
#define __ZoomPanInteractionMode_h_

#include "GenericSliceWindow.h"

/**
 * \class ZoomPanInteractionMode
 * \brief UI interaction mode that takes care of zooming and panning.
 *
 * \see GenericSliceWindow
 */
class ZoomPanInteractionMode : public GenericSliceWindow::EventHandler {
public:
  ZoomPanInteractionMode(GenericSliceWindow *parent);

  int OnMousePress(const FLTKEvent &event);
  int OnMouseRelease(const FLTKEvent &event, const FLTKEvent &pressEvent);    
  int OnMouseDrag(const FLTKEvent &event, const FLTKEvent &pressEvent);
  
  // void OnDraw();
protected:
  /** The starting point for panning */
  Vector2f m_StartViewPosition;

  /** The starting zoom level */
  float m_StartViewZoom;
  
  /** The upper and lower limits on the zoom level */
  float m_ViewZoomMax,m_ViewZoomMin;
};

#endif // __ZoomPanInteractionMode_h_
