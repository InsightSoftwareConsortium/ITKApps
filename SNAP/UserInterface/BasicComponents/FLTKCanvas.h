/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    FLTKCanvas.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __FLTKCanvas_h_
#define __FLTKCanvas_h_

#include <FL/Fl_Gl_Window.H>
#include "FLTKEvent.h"
#include "InteractionMode.h"

#include <list>

/**
 * \class FLTKCanvas
 * \brief An extension of Fl_Gl_Window with advanced interaction handling.
 * 
 * This is an extension of the Fl_Gl_Window from FLTK that allows
 * the concept of interaction modes to be used.  InteractionModes 
 * are event handlers that are associated with different ways of user
 * interaction with the window.  This is used with toolbars, where different
 * toolbar buttons change the behavior of the interaction.  
 * 
 * Multiple interaction modes can be used simultaneously in a stack.  The top mode
 * in the stack is the first to receive events, and the event is propagated through
 * the stack until it has been handled properly.
 */
class FLTKCanvas : public Fl_Gl_Window 
{
public:
  /**
   * Constructor sets up some basics, sets interaction mode stack to be empty
   */
  FLTKCanvas(int x, int y, int w, int h, const char *label);
  virtual ~FLTKCanvas() {}

  /**
   * Push an interaction mode onto the stack of modes.  Mode becomes first to 
   * receive events.  The events that it does not receive are passed on to the
   * next mode on the stack.
   */
  void PushInteractionMode(InteractionMode *mode);

  /**
   * Pop the last interaction mode off the stack
   */
  InteractionMode *PopInteractionMode();

  /**
   * Get the top interaction mode on the stack
   */
  InteractionMode *GetTopInteractionMode();

  /**
   * Remove all interaction modes
   */
  void ClearInteractionStack();

  /**
   * Get the number of interaction modes on the stack
   */
  unsigned int GetInteractionModeCount();

  /**
   * Handle events
   */
  virtual int handle(int eventID);

  /** Are we dragging ? */
  irisIsMacro(Dragging);

protected:
  /**
   * This method should be called to draw the interactors (call their OnDraw methods 
   * in a sequence from bottom to top)
   */
  void FireInteractionDrawEvent();

private:
  // The stack of interaction modes
  std::list<InteractionMode *> m_Interactors;

  // The event at the start of a drag operation (if there is one going on)
  FLTKEvent m_DragStartEvent;

  // Are we dragging or not
  bool m_Dragging;

};

#endif // __FLTKCanvas_h_

