#ifndef _TransferFunctionEditor_h_     // prevent multiple includes
#define _TransferFunctionEditor_h_

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include "DeformableRegistration3DTimeSeriesBase.h"
#include <FL/Fl_Color_Chooser.H>
struct coord
{
  int x;       //the x coordinate;
  int y;       //the y coordinate; 
  double r;
  double g;
  double b;//maybe I should use union

};

class TransferFunctionEditor : public Fl_Gl_Window {
   public:
      TransferFunctionEditor(int x,int y,int w,int h,const char *l=0);
      void draw();
      int handle(int eventType);
      void SetHistogramValues(int bin,int value);
      void SetHistogramSize(int size);
      void SetActiveTransferFunction(int number);
      std::list<coord> & TransferFunctionEditor::GetOpacityTransferFunction() ;
      void SetImageSliceViewer(fltk::DeformableRegistration3DTimeSeriesBase* viewer);
      void SetColorChooser(Fl_Color_Chooser* colorchooser);
      void SetPointcolor(double r,double g,double b);
   private:
      int histogram[4095]; 
      std::list<coord>  transFuncPoints[2];
      std::list<coord>::iterator currentSelection;
      coord CurrentPoint;
      int maxTransFuncPoints;
      int activeTransFunc;  
      double max,min;//The interval between which the current point can be dragged
      fltk::DeformableRegistration3DTimeSeriesBase * m_SliceViewer;
      Fl_Color_Chooser * m_colorchooser;
};

#endif


