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

#include "SegmentedViewer.h"
// FLTK
#include <FL/x.H>
// vtk
#include <vtkImageViewer.h>

#include <vtkInteractorStyle.h>
#include <vtkVersion.h>
#include <vtkCommand.h>

//---------------------------------------------------------------------------
SegmentedViewer::SegmentedViewer() : 
vtkFlImageViewer()
{
  // this is a subclass of Fl_Group, call end so children cant be added
  this->end();
}
//---------------------------------------------------------------------------
SegmentedViewer::SegmentedViewer( int x, int y, int w, int h, const char *) : 
vtkFlImageViewer(x,y,w,h)
{
}
//---------------------------------------------------------------------------
SegmentedViewer::~SegmentedViewer()
{
  // according to the fltk docs, destroying a widget does NOT remove it from
  // its parent, so we have to do that explicitly at destruction
  // (and remember, NEVER delete() an instance of this class)
  if (parent())
    ((Fl_Group*)parent())->remove(*(Fl_Gl_Window*)this);
}
//---------------------------------------------------------------------------
SegmentedViewer * SegmentedViewer::New()
{
  // we don't make use of the objectfactory, because we're not registered
  return new SegmentedViewer;
}


//---------------------------------------------------------------------------
// main FLTK event handler
int SegmentedViewer::handle( int event ) {
  int x = Fl::event_x();
  int y = Fl::event_y();

  int button = Fl::event_button();

    switch( event ) 
    {
      case FL_PUSH: 
        if( (Fl::get_key(FL_Shift_L)) || (Fl::get_key(FL_Shift_R)) ) {
          if(button == 1) {
            AppendRegion(x,y);
          }
        }
        else {
          if(button == 1) {
            SelectRegion(x,y);
          }
        }
        break;
      case FL_FOCUS:
        break;
      case FL_UNFOCUS:
        break;
      case FL_DRAG:
        break;
      case FL_MOVE:
        break;
      case FL_RELEASE:    // mouse up
        break;
      case FL_KEYUP:   // keypress
        break;
      case FL_KEYBOARD:
        break;
      case FL_NO_EVENT:
        break;
      default:    // let the base class handle everything else 
        return Fl_Gl_Window::handle( event );

    } // switch(event)...

    return 1; // we handled the event if we didn't return earlier
}
//---------------------------------------------------------------------------
void SegmentedViewer::SetResampler(vtkImageResample* r) {
  resampler = r;
}
//--------------------------------------------------------------------------
void SegmentedViewer::SetDataReader(vtkPatchedImageReader* r) {
  dataReader = r;
}
//--------------------------------------------------------------------------
void SegmentedViewer::SetWSManager(vtkWSLookupTableManager* m) {
  WSmanager = m;
}
//--------------------------------------------------------------------------
void SegmentedViewer::SetSourceViewer(SourceViewer* v) {
  sourceViewer = v;
}
//---------------------------------------------------------------------------
void SegmentedViewer::SelectRegion(int x, int y) {
  
  float magX = resampler->GetAxisMagnificationFactor(0);
  float magY = resampler->GetAxisMagnificationFactor(1);
  
  int z = this->GetZSlice();

  vtkImageData* input = this->GetInput();

  // y is flipped upside down
  int* size = this->GetSize();
  int height = size[1];
  y = height-y;

  // make sure point is in the whole extent of the image

  int* extent = input->GetWholeExtent();
  int xMin = extent[0];
  int xMax = extent[1];
  int yMin = extent[2];
  int yMax = extent[3];
  int zMin = extent[4];
  int zMax = extent[5];

  if( (x < xMin) || (x > xMax) || (y < yMin) || (y > yMax) || (z < zMin) || (z > zMax)) {
    return;
  }

  if( magX != 0 ) {
    x = (int)(x/magX);
  }

  if( magY != 0 ) {
    y = (int)(y/magY);
  }

  WSmanager->CompileEquivalenciesFor( x, y, z, dataReader->GetOutput() );
  WSmanager->ClearHighlightedValuesToSameColor();
  WSmanager->HighlightComputedEquivalencyList();

  
  this->Render();

}

//---------------------------------------------------------------------------
void SegmentedViewer::AppendRegion(int x, int y) {
  
  float magX = resampler->GetAxisMagnificationFactor(0);
  float magY = resampler->GetAxisMagnificationFactor(1);
  
  int z = this->GetZSlice();

  vtkImageData* input = this->GetInput();

  // y is flipped upside down
  int* size = this->GetSize();
  int height = size[1];
  y = height-y;

  // make sure point is in the whole extent of the image

  int* extent = input->GetWholeExtent();
  int xMin = extent[0];
  int xMax = extent[1];
  int yMin = extent[2];
  int yMax = extent[3];
  int zMin = extent[4];
  int zMax = extent[5];

  if( (x < xMin) || (x > xMax) || (y < yMin) || (y > yMax) || (z < zMin) || (z > zMax)) {
    return;
  }

  if( magX != 0 ) {
    x =(int)(x/magX);
  }

  if( magY != 0 ) {
    y = (int)(y/magY);
  }

  WSmanager->AppendEquivalenciesFor( x, y, z, dataReader->GetOutput() );
  WSmanager->ClearHighlightedValuesToSameColor();
  WSmanager->HighlightComputedEquivalencyList();

  
  this->Render();
}
//---------------------------------------------------------------------------
static char const rcsid[] =
  "Id";

const char *SegmentedViewer_rcsid(void)
{
    return rcsid;
}
