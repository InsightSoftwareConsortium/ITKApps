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
 * See the .cxx for more notes.
 * 
 * Id
 */

#ifndef _SourceViewer_h
#define _SourceViewer_h

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include "vtkFlImageViewer.h"
#include "vtkBinaryVolume.h"
#include <vtkImageResample.h>
#include "vtkPatchedImageReader.h"
#include "vtkWSLookupTableManager.h"

class SegmentedViewer;
class BinaryViewer;

class SourceViewer : public vtkFlImageViewer {
 public:
   // Fl_Gl_Window overrides
   int  handle( int event );

 public:
   // constructors
   SourceViewer();
   // fltk required constructor
   SourceViewer( int x, int y, int w, int h, const char *l="");
   // vtk ::New()
   static SourceViewer* New();
   // destructor
   ~SourceViewer( void );

   void SetResampler(vtkImageResample*);
   void SetDataReader(vtkPatchedImageReader*);
   void SetWSManager(vtkWSLookupTableManager*);
   void SetLabeledViewer(SegmentedViewer*);
   void SetBinaryVolume(vtkBinaryVolume*);
   void SetBinaryViewer(BinaryViewer*);

   void PaintPixels(int x, int y);
   void UnpaintPixels(int x, int y);
   void SetPaintRadius(double r);

 private:
   vtkImageResample* resampler;
   vtkPatchedImageReader* dataReader;
   vtkWSLookupTableManager* WSmanager;
   SegmentedViewer* labeledViewer;
   vtkBinaryVolume* binaryVolume;
   BinaryViewer* binaryViewer;
  };

#endif
