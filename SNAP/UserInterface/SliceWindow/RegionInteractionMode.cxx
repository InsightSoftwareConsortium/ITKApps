/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    RegionInteractionMode.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "RegionInteractionMode.h"

#include "IRISApplication.h"
#include "IRISImageData.h"
#include "IRISSliceWindow.h"
#include "UserInterfaceLogic.h"

#include <cassert>
#include <cmath>

// The click detection radius (delta)
const unsigned int RegionInteractionMode::m_PixelDelta = 4;

RegionInteractionMode
::RegionInteractionMode(GenericSliceWindow *parent)
:GenericSliceWindow::EventHandler(parent)
{
  // Initialize the edges
  for(unsigned int dir=0;dir<2;dir++) 
  {
    for(unsigned int i=0;i<2;i++) 
    {
      m_EdgeHighlighted[dir][i] = false;
    }
  }
}

void 
RegionInteractionMode
::GetEdgeVertices(unsigned int direction,unsigned int index,
                  Vector2f &x0,Vector2f &x1, 
                  const Vector2f corner[2])
{
  x0(direction) = corner[0](direction);
  x1(direction) = corner[1](direction);
  x0(1-direction) = x1(1-direction) = corner[index](1-direction);
}

float
RegionInteractionMode
::GetEdgeDistance(unsigned int direction,
                  unsigned int index,
                  const Vector2f &x,
                  const Vector2f corner[2])
{
  // Compute the vertices of the edge
  Vector2f x0,x1;
  GetEdgeVertices(direction,index,x0,x1,corner);
  
  // Compute the squared distance between the vertices
  float l2 = (x1-x0).squared_magnitude();
  float l = sqrt(l2);
  
  // Compute the projection of x onto x1-x0
  float p = dot_product(x-x0,x1-x0) / sqrt(l2);
  float p2 = p*p;
  
  // Compute the squared distance to the line of the edge
  float q2 = (x-x0).squared_magnitude() - p2;

  // Compute the total distance
  float d = sqrt(q2 + (p < 0 ? p2 : 0) + (p > l ? (p-l)*(p-l) : 0));

  // Return this distance
  return d;
}

int RegionInteractionMode
::OnMousePress(const FLTKEvent &event)
{
  // Flag indicating whether we respond to this event or not
  m_IsAnyEdgeHighlighted = false;
  
  // Convert the event location into slice u,v coordinates
  Vector2f uvSlice = m_Parent->MapWindowToSlice(event.XSpace.extract(2));    
  
  // Record the system's corners at the time of drag start
  GetSystemROICorners(m_CornerDragStart);
    
  // Repeat for vertical and horizontal edges
  for(unsigned int dir=0;dir<2;dir++) 
  {
    // Variables used to find the closest edge that's within delta
    int iClosest = -1;
    float dToClosest = m_PixelDelta;

    // Search for the closest edge
    for(unsigned int i=0;i<2;i++) 
    {
      float d = GetEdgeDistance(dir,i,uvSlice,m_CornerDragStart);
      if(d < dToClosest) 
      {
        dToClosest = d;
        iClosest = i;
      }
    }

    // Highlight the selected edge
    if(iClosest >= 0) 
      {
      m_EdgeHighlighted[dir][iClosest] = true;
      m_IsAnyEdgeHighlighted = true;
      }      
  }

  // If nothing was highlighted, then return and let the next handler process
  // the event
  if(!m_IsAnyEdgeHighlighted)
    return 0;

  // Event has been handled
  return 1;
}

void RegionInteractionMode
::GetSystemROICorners(Vector2f corner[2])
{
  // Get the region of interest in image coorinates
  Vector3i ul = m_GlobalState->GetROICorner(0);
  Vector3i lr = m_GlobalState->GetROICorner(1);

  // Remap to slice coordinates
  corner[0] = m_Parent->MapImageToSlice(to_float(ul));
  corner[1] = m_Parent->MapImageToSlice(to_float(lr));
}

void RegionInteractionMode
::UpdateCorners(const FLTKEvent &event, const FLTKEvent &pressEvent)
{
  // Compute the corners in slice coordinates
  Vector2f corner[2];
  GetSystemROICorners(corner);

  // Convert the location of the events into slice u,v coordinates
  Vector2f uvSliceNow = 
    m_Parent->MapWindowToSlice(event.XSpace.extract(2));
  Vector2f uvSlicePress = 
    m_Parent->MapWindowToSlice(pressEvent.XSpace.extract(2));

  // Get the corresponding positions in the image coordinates
  Vector3f xImageNow = m_Parent->MapSliceToImage(uvSliceNow);
  Vector3f xImagePress = m_Parent->MapSliceToImage(uvSlicePress);

  // Get the current bounds and extents of the region of interest 
  Vector3f xCornerImage[2] = 
  {
    m_Parent->MapSliceToImage(corner[0]),
    m_Parent->MapSliceToImage(corner[1])
  };

  // TODO: For dragging entire region, the clamps should be just image extents
  // Compute the clamps for each of the corners
  Vector3f clamp[2][2] = 
  {
    {
      Vector3f(0.0f,0.0f,0.0f),
      xCornerImage[1] - Vector3f(1.0f,1.0f,1.0f)
    },
    {
      xCornerImage[0] + Vector3f(1.0f,1.0f,1.0f),
      to_float( m_Driver->GetCurrentImageData()->GetVolumeExtents())
    }
  };

  // For each highlighted edge, update the coordinates of the affected vertex
  // by clamping to the maximum range
  for (unsigned int dir=0;dir<2;dir++)
    {
    for (unsigned int i=0;i<2;i++)
      {
      if (m_EdgeHighlighted[dir][i])
        {
        verbose << i << ", " << dir << ", " << uvSliceNow << ", " << uvSlicePress << endl;

        // Horizontal edge affects the y of the vertex and vice versa
        corner[i](1-dir) =
          m_CornerDragStart[i](1-dir) + uvSliceNow(1-dir) - uvSlicePress(1-dir);

        // Map the affected vertex to image space
        Vector3f vImage = m_Parent->MapSliceToImage(corner[i]);

        // Clamp the affected vertex in image space
        Vector3f vImageClamped = vImage.clamp(clamp[i][0],clamp[i][1]);

        // Map the affected vertex back into slice space
        corner[i] = m_Parent->MapImageToSlice(vImageClamped);
        }
      }
    }

  // Update the region of interest in the system
  for (unsigned int i=0;i<2;i++)
    {
    // Convert the corners to integers
    Vector3f xImage = m_Parent->MapSliceToImage(corner[i]);
    Vector3i xVoxel = to_int(xImage);

    // Get the system's current ROI corner
    Vector3i xSystem = m_GlobalState->GetROICorner(i);

    // The slice z-direction coordinate in xVoxel is equal to the slice
    // number and thus we use the xSystem's value
    xVoxel(m_Parent->m_Id) = xSystem(m_Parent->m_Id);

    // Update the system's ROI corner
    m_GlobalState->SetROICorner(i,xVoxel);
    }  

  // Cause a system redraw
  m_ParentUI->RedrawWindows();
}

int RegionInteractionMode
::OnMouseDrag(const FLTKEvent &event, const FLTKEvent &pressEvent)
{
  // Only do something if there is a highlight
  if(m_IsAnyEdgeHighlighted)
    {
    // Update the corners in response to the dragging
    UpdateCorners(event,pressEvent);
    
    // Event has been handled
    return 1;
    }
  
  return 0;
}

int RegionInteractionMode
::OnMouseRelease(const FLTKEvent &event, const FLTKEvent &pressEvent)
{
  // Only do something if there is a highlight
  if(m_IsAnyEdgeHighlighted)
    {
    // Update the corners in response to the dragging
    UpdateCorners(event,pressEvent);
    
    // Clear highlights of the connected edges
    for(unsigned int i=0;i<2;i++)
      m_EdgeHighlighted[0][i] = m_EdgeHighlighted[1][i] = false;

    // Clear the summary highlight flag
    m_IsAnyEdgeHighlighted = false;
    
    // Event has been handled
    return 1;
    }
  
  // Event has not been handled
  return 0;
}

void
RegionInteractionMode
::OnDraw()
{
  // The region of interest should be in effect
  assert(m_GlobalState->GetIsValidROI());

  // Compute the corners in slice coordinates
  Vector2f corner[2];
  GetSystemROICorners(corner);

  // Check that the current slice is actually within the bounding box
  int slice = m_Parent->m_SliceIndex;
  int dim = m_Parent->m_Id;
  int bbMin = m_GlobalState->GetROICorner(0)(dim);
  int bbMax = m_GlobalState->GetROICorner(1)(dim);

  // And if so, return without painting anything
  if(bbMin > slice || bbMax < slice)
    return;
  
  // Set line properties
  glPushAttrib(GL_LINE_BIT);
  glLineWidth(1);
  glEnable(GL_LINE_STIPPLE);
  glLineStipple(2,0xAAAA);

  // Start drawing the lines
  glBegin(GL_LINES);
  
  // Draw each of the edges
  for(unsigned int dir=0;dir<2;dir++)
  {
    for(unsigned int i=0;i<2;i++)
    {
      // Select color according to edge state
      if(m_EdgeHighlighted[dir][i])
        glColor3f(1,1,0);
      else
        glColor3f(1,0,0);

      // Compute the vertices of the edge
      Vector2f x0,x1;
      GetEdgeVertices(dir,i,x0,x1,corner);

      // Draw the line
      glVertex2f(x0[0],x0[1]);
      glVertex2f(x1[0],x1[1]);
    }
  }

  glEnd();
  glPopAttrib();
}

