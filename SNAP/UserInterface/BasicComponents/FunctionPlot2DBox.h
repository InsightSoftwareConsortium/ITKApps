/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    FunctionPlot2DBox.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __FunctionPlot2DBox_h_
#define __FunctionPlot2DBox_h_

#include "FunctionPlot2D.h"
#include "FLTKCanvas.h"

/**
 * \class FunctionPlot2DBox
 * \brief An FLTK Box used for drawing a plot using the above object
 */
class FunctionPlot2DBox : public FLTKCanvas
{
public:
  /** The standard FLTK window constructor */
  FunctionPlot2DBox(int x,int y,int w,int h,const char *label=NULL);
  
  /** Access the plotting object associated with this window */
  FunctionPlot2D &GetPlotter()
  {
    return m_Plotter;
  }

  /** The draw method */
  void draw();

private:
  FunctionPlot2D m_Plotter;
};

#endif
