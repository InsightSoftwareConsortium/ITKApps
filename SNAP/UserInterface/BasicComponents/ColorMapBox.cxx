/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ColorMapBox.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "ColorMapBox.h"
#include "SNAPOpenGL.h"

ColorMapBox
::ColorMapBox(int x,int y,int w,int h,const char *label)
  : FLTKCanvas(x, y, w, h, label)
{
}

void
ColorMapBox
::draw()
{
  // The standard 'valid' business
  if(!valid())
    {
    // Set up the basic projection with a small margin
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-0.005,1.005,-0.05,1.05);
    glViewport(0,0,w(),h());

    // Establish the model view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();    
    }
    
  // Clear the viewport
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

  // Push the related attributes
  glPushAttrib(GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);

  // Disable lighting
  glDisable(GL_LIGHTING);

  // Get the colors
  SpeedColorMap::OutputType xMinus = m_ColorMap(-1.0);
  SpeedColorMap::OutputType xPlus = m_ColorMap(1.0);
  SpeedColorMap::OutputType xZero = m_ColorMap(0.0);

  // Draw the color map
  glBegin(GL_QUADS);

  glColor3d(xMinus[0], xMinus[1], xMinus[2]);
  glVertex2d(0.0, 0.0); glVertex2d(0.0, 1.0);

  glColor3d(xZero[0], xZero[1], xZero[2]);
  glVertex2d(0.5, 1.0); glVertex2d(0.5, 0.0);
  glVertex2d(0.5, 0.0); glVertex2d(0.5, 1.0);
  
  glColor3d(xPlus[0], xPlus[1], xPlus[2]);
  glVertex2d(1.0, 1.0); glVertex2d(1.0, 0.0);
  
  glEnd();

  // Pop the attributes
  glPopAttrib();

  // Done
  glFlush();
}
