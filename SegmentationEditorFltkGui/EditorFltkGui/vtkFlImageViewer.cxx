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

#include "vtkFlImageViewer.h"
// FLTK
#include <FL/x.H>
// vtk
#include <vtkImageViewer.h>

#include <vtkInteractorStyle.h>
#include <vtkVersion.h>
#include <vtkCommand.h>

//---------------------------------------------------------------------------
vtkFlImageViewer::vtkFlImageViewer() : 
Fl_Gl_Window( 0, 0, 300, 300, "" ), vtkImageViewer()
{
  // this is a subclass of Fl_Group, call end so children cant be added
  this->end();
}
//---------------------------------------------------------------------------
vtkFlImageViewer::vtkFlImageViewer( int x, int y, int w, int h, const char *l ) : 
  Fl_Gl_Window( x, y, w, h, l ), vtkImageViewer()
{
  int size[2];
  size[0] = 256;
  size[1] = 256;
  this->SetSize(size);
  // this is a subclass of Fl_Group, call end so children cant be added
  this->end();
}
//---------------------------------------------------------------------------
vtkFlImageViewer::~vtkFlImageViewer()
{
  // according to the fltk docs, destroying a widget does NOT remove it from
  // its parent, so we have to do that explicitly at destruction
  // (and remember, NEVER delete() an instance of this class)
  if (parent())
    ((Fl_Group*)parent())->remove(*(Fl_Gl_Window*)this);
}
//---------------------------------------------------------------------------
vtkFlImageViewer * vtkFlImageViewer::New()
{
  // we don't make use of the objectfactory, because we're not registered
  return new vtkFlImageViewer;
}


//---------------------------------------------------------------------------
void vtkFlImageViewer::flush(void)
{
  // err, we don't want to do any fansy pansy Fl_Gl_Window stuff, so we
  // bypass all of it (else we'll get our front and back buffers in all
  // kinds of tangles, and need extra glXSwapBuffers() calls and all that)
  draw();
}
//---------------------------------------------------------------------------
void vtkFlImageViewer::draw(void){
  //UpdateSize( this->w(), this->h() );
        SetWindowId( (void*)fl_xid( this ) );
#if !defined(WIN32)
        SetDisplayId( fl_display );
#endif
  Render();
}
//---------------------------------------------------------------------------
void vtkFlImageViewer::resize( int x, int y, int w, int h ) {
  //UpdateSize( w, h );

  // resize the FLTK window by calling ancestor method
  Fl_Gl_Window::resize( x, y, w, h ); 
}
//---------------------------------------------------------------------------
// main FLTK event handler
int vtkFlImageViewer::handle( int event ) {
  int x = Fl::event_x();
  int y = Fl::event_y();
  int slice;

  std::cerr << "vtkFlImageViewer HANDLE\n";

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

const char *vtkFlImageViewer_rcsid(void)
{
    return rcsid;
}
