/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SnakeParametersPreviewBox.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#include "SnakeParametersPreviewBox.h"
#include "SnakeParametersPreviewPipeline.h"
#include "OpenGLSliceTexture.h"
#include "GL/glu.h"


using namespace itk;                              
                              
SnakeParametersPreviewBox
::SnakeParametersPreviewBox(int x, int y, int w, int h, const char *label)
: FLTKCanvas(x,y,w,h,label), m_Interactor(this)
{
  // Initialize the texture object
  m_Texture = new TextureType();
  m_Texture->SetGlType(GL_FLOAT);

  // Set up the interactor
  PushInteractionMode(&m_Interactor);
}

SnakeParametersPreviewBox
::~SnakeParametersPreviewBox()
{
  delete m_Texture;
}

void
SnakeParametersPreviewBox
::draw()
{
  // Set up the projection if necessary
  if(!valid()) 
  {
    // The window will have coordinates (0,0) to (1,1)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,1.0,0.0,1.0);
    glViewport(0,0,w(),h());

    // Establish the model view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  // Update everything
  m_Pipeline->Update(this);

  // Clear the display
  glClearColor(0.0,0.0,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

  // Set up the line drawing attributes
  glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT | GL_COLOR_BUFFER_BIT);

  // Set up the model matrix
  glPushMatrix();
  glScaled(1.0 / m_Pipeline->GetSpeedImage()->GetBufferedRegion().GetSize(0),
           1.0 / m_Pipeline->GetSpeedImage()->GetBufferedRegion().GetSize(1),
           1.0);
  
  // Draw the speed image
  m_Texture->SetImage(m_Pipeline->GetSpeedImage());
  m_Texture->Draw();


  // Set up the line drawing mode
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glLineWidth(2.0);

  // Set the line color
  glColor3d(1.0,0.0,0.0);

  // Get the point collection
  const SnakeParametersPreviewPipeline::SampledPointList 
    &list = m_Pipeline->GetSampledPoints();
    
  glBegin(GL_LINE_LOOP);
  for(unsigned int j=0;j<list.size();j++)
    {
    glVertex2d(list[j][0],list[j][1]);
    }
  glEnd();

  // Draw the vectors
  glLineWidth(1.0);

  const SnakeParametersPreviewPipeline::ImagePointList
    &plist = m_Pipeline->GetImagePoints();
  
  glBegin(GL_LINES);
  for(unsigned int j=0;j<plist.size();j++)
    {
    const SnakeParametersPreviewPipeline::PointInfo &point = plist[j];
    
    float length = 10.0f;
    switch(m_ForceToDisplay)
      {
      case CURVATURE_FORCE   : length *= point.CurvatureForce; break;
      case ADVECTION_FORCE   : length *= point.AdvectionForce; break;
      case PROPAGATION_FORCE : length *= point.PropagationForce; break;
      case TOTAL_FORCE       : 
        length *= (point.CurvatureForce + point.AdvectionForce + 
                   point.PropagationForce); break;
      }
    
    glVertex2d(point.index[0],point.index[1]);
    glVertex2d(point.index[0] + length * point.normal[0],
               point.index[1] + length * point.normal[1]);
    }
  glEnd();


  // Draw the interactor
  m_Interactor.OnDraw();
  
  // Restore the attribute state
  glPopAttrib();

  // Pop the matrix
  glPopMatrix();
}

SnakeParametersPreviewBox::Interactor
::Interactor(SnakeParametersPreviewBox *owner)
{
  m_Owner = owner;
  m_ControlPicked = m_ControlsVisible = false;
}

int
SnakeParametersPreviewBox::Interactor
::OnMousePress(const FLTKEvent &event)
{
  // Get the point of the event
  Vector2d xClick = to_double(event.XCanvas);

  if(m_ControlsVisible) 
    {
    // The closest point index
    m_ActiveControl = 0;
    double minDistance;

    // Get a hold of the control points
    const SnakeParametersPreviewPipeline::ControlPointList 
      &cp = m_Owner->m_Pipeline->GetControlPoints();    

    // Find the closest control point
    for(unsigned int i=0;i<cp.size();i++)
      {
      double distance = (cp[i] - xClick).two_norm();
      if(i == 0 || minDistance > distance) 
        {
        minDistance = distance;
        m_ActiveControl = i;
        }
      }

    // Make sure the distance is large enough
    if(minDistance < 3.0)
      {
      m_ControlPicked = true;
      }
    else
      {
      m_ControlsVisible = m_ControlPicked = false;
      m_Owner->redraw();
      }
    }
  else 
    {
    m_ControlsVisible = true;
    m_Owner->redraw();
    }

  return true;
}

int
SnakeParametersPreviewBox::Interactor
::OnMouseRelease(const FLTKEvent &event, const FLTKEvent &pressEvent)
{
  if(m_ControlPicked)
    {
    // Update the control point
    m_Owner->m_Pipeline->ChangeControlPoint(
      m_ActiveControl,to_double(event.XCanvas),false);

    // Redraw the parent
    m_Owner->redraw();
    }

  return true;
}

int
SnakeParametersPreviewBox::Interactor
::OnMouseDrag(const FLTKEvent &event, const FLTKEvent &pressEvent)
{
  if(m_ControlPicked)
    {
    // Update the control point
    m_Owner->m_Pipeline->ChangeControlPoint(
      m_ActiveControl,to_double(event.XCanvas),true);

    // Redraw the parent
    m_Owner->redraw();
    }

  return true;
}

void
SnakeParametersPreviewBox::Interactor
::OnDraw()
{
  if(m_ControlsVisible)
    {
    glColor3f(1,0.67,0.33);

    // Get a hold of the control points
    const SnakeParametersPreviewPipeline::ControlPointList 
      &cp = m_Owner->m_Pipeline->GetControlPoints();    
    
    for(unsigned int i=0;i<cp.size();i++)
      {
      glPushMatrix();
      glTranslated(cp[i][0],cp[i][1],0);
      
      GLUquadric *obj = gluNewQuadric();
      gluDisk(obj,0,3.0,8,3);
      gluDeleteQuadric(obj);
      glPopMatrix();
      }
    }
}

