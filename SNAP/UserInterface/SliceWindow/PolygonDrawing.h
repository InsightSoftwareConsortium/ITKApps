/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    PolygonDrawing.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __PolygonDrawing_h_
#define __PolygonDrawing_h_

#include <FL/Fl.H>
#include <FL/gl.h>
#include <GL/glu.h>

#include <iostream>

enum PolygonState
{
  INACTIVE_STATE,
  DRAWING_STATE,
  EDITING_STATE
};

struct PolygonVertex 
{
  float x,y;
  int selected;
};

/**
 * \class PolygonDrawing
 * \brief Code for drawing and editing polygons
 */
class PolygonDrawing
{

public:
  PolygonDrawing();
  ~PolygonDrawing();
  PolygonState GetState();
  int  CachedPolygon(void);
  void AcceptPolygon(unsigned char *buffer, int width, int height);
  void PastePolygon(void);
  void Draw(float pixel_x, float pixel_y);
  int  Handle(int event, int button, float x, float y, float pixel_x, float pixel_y);

private:
  PolygonState m_State;
  PolygonVertex *m_Vertices;
  PolygonVertex *m_Cache;
  
  int m_CachedPolygon;
  int m_NumberOfAllocatedVertices;
  int m_NumberOfUsedVertices;
  int m_NumberOfCachedVertices;
  int m_SelectedVertices;
  int m_DraggingPickBox;

  // contains selected points
  float m_EditBox[4];         

  // box the user drags to select new points
  float m_SelectionBox[4];         
  
  float m_StartX, m_StartY;


  GLUtesselatorObj *m_Tesselator;

  void ComputeEditBox();
  void Add(float x, float y, int selected);

  void Delete();

  void Insert();
};

#endif // __PolygonDrawing_h_


/*Log: PolygonDrawing.h
/*Revision 1.4  2003/07/12 01:34:18  pauly
/*More final changes before ITK checkin
/*
/*Revision 1.3  2003/07/11 23:28:10  pauly
/**** empty log message ***
/*
/*Revision 1.2  2003/06/08 23:27:56  pauly
/*Changed variable names using combination of ctags, egrep, and perl.
/*
/*Revision 1.1  2003/03/07 19:29:47  pauly
/*Initial checkin
/*
/*Revision 1.2  2002/12/16 16:40:19  pauly
/**** empty log message ***
/*
/*Revision 1.1.1.1  2002/12/10 01:35:36  pauly
/*Started the project repository
/*
/*
/*Revision 1.2  2002/03/08 14:06:30  moon
/*Added Header and Log tags to all files
/**/
