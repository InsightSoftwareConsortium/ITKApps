/*
 * vtkFlRenderWindowInteractor - class to enable VTK to render to and interact
 * with a FLTK window.
 * 
 * Copyright (c) 2002 Charl P. Botha <cpbotha@ieee.org> http://cpbotha.net/
 * Based on original code and concept copyright (c) 2000,2001 David Pont
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 * 
 * Id
 */

/*  
 * You must not delete one of these classes.  Make use of the Delete()
 * method... this thing makes use of VTK reference counting.  Let me
 * repeat that: never "delete" an instance of this class, always use
 * ->Delete().
 */

#include "BinaryViewer.h"
// FLTK
#include <FL/x.H>
// vtk
#include <vtkImageViewer.h>

#include <vtkInteractorStyle.h>
#include <vtkVersion.h>
#include <vtkCommand.h>

//---------------------------------------------------------------------------
BinaryViewer::BinaryViewer() : 
  vtkFlImageViewer()
{
  // this is a subclass of Fl_Group, call end so children cant be added
  this->end();
}
//---------------------------------------------------------------------------
BinaryViewer::BinaryViewer( int x, int y, int w, int h, const char * ) : 
  vtkFlImageViewer(x,y,w,h)
{
}
//---------------------------------------------------------------------------
BinaryViewer::~BinaryViewer()
{
  // according to the fltk docs, destroying a widget does NOT remove it from
  // its parent, so we have to do that explicitly at destruction
  // (and remember, NEVER delete() an instance of this class)
  if (parent())
    ((Fl_Group*)parent())->remove(*(Fl_Gl_Window*)this);
}
//---------------------------------------------------------------------------
BinaryViewer * BinaryViewer::New()
{
  // we don't make use of the objectfactory, because we're not registered
  return new BinaryViewer;
}


//---------------------------------------------------------------------------
// main FLTK event handler
int BinaryViewer::handle( int event ) {
    switch( event ) 
    {
      case FL_FOCUS:

      case FL_UNFOCUS:

      case FL_KEYBOARD:   // keypress
      
      case FL_PUSH: 

      case FL_DRAG:

      case FL_MOVE:

      case FL_RELEASE:    // mouse up

    default:    // let the base class handle everything else 
      return Fl_Gl_Window::handle( event );
      
    } // switch(event)...
    
    return 1; // we handled the event if we didn't return earlier
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

static char const rcsid[] =
  "Id";

const char *BinaryViewer_rcsid(void)
{
    return rcsid;
}
