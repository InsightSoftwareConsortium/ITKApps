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
#include "SnakeParametersUILogic.h"
#include "SNAPOpenGL.h"

extern void fl_alert(const char *, ...);

using namespace itk;                              
                              
SnakeParametersPreviewBox
::SnakeParametersPreviewBox(int lx, int ly, int lw, int lh, const char *llabel)
: FLTKCanvas(lx,ly,lw,lh,llabel), m_Interactor(this)
{
  // Initialize the texture object
  m_Texture = new TextureType();
  m_Texture->SetGlComponents(4);
  m_Texture->SetGlFormat(GL_RGBA);

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
    // glScaled(4,4,1);
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
  m_Texture->SetImage(m_Pipeline->GetDisplayImage());
  m_Texture->Draw(Vector3d(1.0));

  // Set up the line drawing mode
  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glLineWidth(2.0);
  glColor3d(1.0, 0.0, 0.0);

  // Draw the evolving contour if it's available
  if(m_Pipeline->IsDemoLoopRunning())
    {
    std::vector<Vector2d> &points = m_Pipeline->GetDemoLoopContour();
    glColor3d(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    std::vector<Vector2d>::iterator it = points.begin();
    for(; it != points.end(); ++it)
      glVertex(*it);
    glEnd();

    glLineWidth(1.0);
    glColor4d(1.0, 0.0, 0.0, 0.5);
    }


  // Draw the vectors
  //glLineWidth(1.0);

  // No more image scaling
  glPopMatrix();
  glPushMatrix();

  // Get the point collection
  const SnakeParametersPreviewPipeline::SampledPointList 
    &list = m_Pipeline->GetSampledPoints();
    
  // Draw the spline
  glBegin(GL_LINE_LOOP);
  for(unsigned int j=0;j<list.size();j++)
    {
    glVertex2d(list[j].x[0],list[j].x[1]);
    }
  glEnd();

  // Draw the forces on the spline
  // Draw the vectors from the curve
  glColor3d(1.0,0.0,0.0);
  glBegin(GL_LINES);
  for(unsigned int ii=0;ii<list.size();ii+=4)
    {
    // A reference so we can access the point in shorthand
    const SnakeParametersPreviewPipeline::SampledPoint &p = list[ii];
    
    // Decide which force to draw, depending on the current state
    double force = 0;
    switch(m_ForceToDisplay) 
      {
      case PROPAGATION_FORCE : 
        force = p.PropagationForce; 
        break;
      case CURVATURE_FORCE : 
        force = p.CurvatureForce; 
        break;
      case ADVECTION_FORCE : 
        force = p.AdvectionForce; 
        break;
      case TOTAL_FORCE : 
        force = p.PropagationForce + p.CurvatureForce + p.AdvectionForce;
        break;
      }

    // Scale the force for effect
    force *= 10;

    // Draw the forces
    glVertex2d(p.x[0],p.x[1]);
    glVertex2d(p.x[0] + force * p.n[0] / w(),p.x[1] + force * p.n[1] / w());     
  }

  glEnd();  

  // Draw the interactor
  m_Interactor.OnDraw();
  
  // Pop the matrix
  glPopMatrix();
  
  // Restore the attribute state
  glPopAttrib();
}

SnakeParametersPreviewBox::Interactor
::Interactor(SnakeParametersPreviewBox *owner)
: InteractionMode(owner)
{
  m_Owner = owner;
  m_ControlPicked = m_ControlsVisible = false;
}

int
SnakeParametersPreviewBox::Interactor
::OnMousePress(const FLTKEvent &event)
{
  // Get the point of the event
  Vector2d xClick(
    event.XCanvas[0] * 1.0 / m_Owner->w(), 
    event.XCanvas[1] * 1.0 / m_Owner->h());

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
    if(minDistance < 6.0 / m_Owner->w())
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
::OnMouseRelease(const FLTKEvent &event, const FLTKEvent &irisNotUsed(pressEvent))
{
  Vector2d xClick(
    event.XCanvas[0] * 1.0 / m_Owner->w(), 
    event.XCanvas[1] * 1.0 / m_Owner->h());

  if(m_ControlPicked)
    {
    // Update the control point
    m_Owner->m_Pipeline->ChangeControlPoint(
      m_ActiveControl, xClick, false);

    // Redraw the parent
    m_Owner->GetParentUI()->RedrawAllBoxes();
    }

  return true;
}

int
SnakeParametersPreviewBox::Interactor
::OnMouseDrag(const FLTKEvent &event, const FLTKEvent &irisNotUsed(pressEvent))
{
  Vector2d xClick(
    event.XCanvas[0] * 1.0 / m_Owner->w(), 
    event.XCanvas[1] * 1.0 / m_Owner->h());

  if(m_ControlPicked)
    {
    // Update the control point
    m_Owner->m_Pipeline->ChangeControlPoint(
      m_ActiveControl, xClick, true);

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
      gluDisk(obj,0, 4.0 / m_Owner->w(),8,3);
      gluDeleteQuadric(obj);
      glPopMatrix();
      }
    }
}

