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

#include "SourceViewer.h"
// FLTK
#include <FL/x.H>
// vtk
#include <vtkImageViewer.h>

#include <vtkInteractorStyle.h>
#include <vtkVersion.h>
#include <vtkCommand.h>

#include <BinaryViewer.h>
#include <SegmentedViewer.h>

//---------------------------------------------------------------------------
SourceViewer::SourceViewer() : 
vtkFlImageViewer()
{
  // this is a subclass of Fl_Group, call end so children cant be added
  this->end();
}
//---------------------------------------------------------------------------
SourceViewer::SourceViewer( int x, int y, int w, int h, const char *l ) : 
vtkFlImageViewer(x,y,w,h)
{
}
//---------------------------------------------------------------------------
SourceViewer::~SourceViewer()
{
  // according to the fltk docs, destroying a widget does NOT remove it from
  // its parent, so we have to do that explicitly at destruction
  // (and remember, NEVER delete() an instance of this class)
  if (parent())
    ((Fl_Group*)parent())->remove(*(Fl_Gl_Window*)this);
}
//---------------------------------------------------------------------------
SourceViewer * SourceViewer::New()
{
  // we don't make use of the objectfactory, because we're not registered
  return new SourceViewer;
}


//---------------------------------------------------------------------------
// main FLTK event handler
int SourceViewer::handle( int event ) {
  int x = Fl::event_x();
  int y = Fl::event_y();
  int slice;

  int button = Fl::event_button();

    switch( event ) 
    {
      case FL_PUSH: 
        if(button == 1) {
          PaintPixels(x, y);
        } 
        else if(button == 2) {
          if( (Fl::get_key(FL_Shift_L)) || (Fl::get_key(FL_Shift_R)) ) {
            labeledViewer->AppendRegion(x,y);
          }
          else {
            labeledViewer->SelectRegion(x,y);
          }
        }
       else if(button == 3) {
         UnpaintPixels(x, y);
       }
         break;
      case FL_DRAG:
        if(button == 1) {
          PaintPixels(x, y);
        } 
        else if(button == 3) {
          UnpaintPixels(x, y);
        }

      case FL_FOCUS:

      case FL_UNFOCUS:

      case FL_KEYBOARD:   // keypress
      
      case FL_MOVE:

      case FL_RELEASE:    // mouse up

      default:    // let the base class handle everything else 
       return Fl_Gl_Window::handle( event );

    } // switch(event)...

    return 1; // we handled the event if we didn't return earlier
}
//---------------------------------------------------------------------------
void SourceViewer::SetResampler(vtkImageResample* r) {
  resampler = r;
}
//---------------------------------------------------------------------------
void SourceViewer::SetDataReader(vtkPatchedImageReader* r) {
  dataReader = r;
}
//---------------------------------------------------------------------------
void SourceViewer::SetWSManager(vtkWSLookupTableManager* m) {
  WSmanager = m;
}
//---------------------------------------------------------------------------
void SourceViewer::SetLabeledViewer(SegmentedViewer* v) {
  labeledViewer = v;
}
//---------------------------------------------------------------------------
void SourceViewer::SetBinaryVolume(vtkBinaryVolume* v) {
  binaryVolume = v;
}
//---------------------------------------------------------------------------
void SourceViewer::SetBinaryViewer(BinaryViewer* v) {
  binaryViewer = v;
}
//---------------------------------------------------------------------------
void SourceViewer::PaintPixels(int x, int y) {
  float magX = resampler->GetAxisMagnificationFactor(0);
  float magY = resampler->GetAxisMagnificationFactor(1);

  vtkImageData* input = this->GetInput();

  int z = this->GetZSlice();

  // y is flipped upside down
  int* size = this->GetSize();
  int height = size[1];
  y = height-y;

  // make sure point is in the whole extento f the image
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
    x = x/magX;
  }

  if( magY != 0 ) {
    y = y/magY;
  }

  binaryVolume->SetWithRadius(x, y, z);
  binaryVolume->Modified();

  int paintRadiusValue = binaryVolume->GetPaintRadius();

  binaryVolume->SetUpdateExtent(x-paintRadiusValue, x+paintRadiusValue, y-paintRadiusValue, y+paintRadiusValue, z, z);

  binaryViewer->Render();
  this->Render();

  binaryVolume->SetUpdateExtentToWholeExtent();
}
//---------------------------------------------------------------------------
void SourceViewer::UnpaintPixels(int x, int y) {
  float magX = resampler->GetAxisMagnificationFactor(0);
  float magY = resampler->GetAxisMagnificationFactor(1);

  vtkImageData* input = this->GetInput();

  int z = this->GetZSlice();
  
  // y is flipped upside down
  int* size = this->GetSize();
  int height = size[1];
  y = height-y;

  // make sure point is in the whole extento f the image
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
    x = x/magX;
  }

  if( magY != 0 ) {
    y = y/magY;
  }

  int paintRadiusValue = binaryVolume->GetPaintRadius();
  binaryVolume->UnsetWithRadius(x, y, z);
  binaryVolume->Modified();

  binaryVolume->SetUpdateExtent(x-paintRadiusValue, x+paintRadiusValue, y-paintRadiusValue, y+paintRadiusValue, z, z);

  binaryViewer->Render();
  this->Render();

  binaryVolume->SetUpdateExtentToWholeExtent();

}

//---------------------------------------------------------------------------
void SourceViewer::SetPaintRadius(double r) {
  binaryVolume->SetPaintRadius((int)r);
}
//---------------------------------------------------------------------------

static char const rcsid[] =
  "Id";

const char *SourceViewer_rcsid(void)
{
    return rcsid;
}
