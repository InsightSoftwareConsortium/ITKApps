/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    GenericSliceWindow.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "GenericSliceWindow.h"

#include "CrosshairsInteractionMode.h"
#include "GlobalState.h"
#include "IRISApplication.h"
#include "IRISImageData.h"
#include "OpenGLSliceTexture.h"
#include "UserInterfaceLogic.h"
#include "ZoomPanInteractionMode.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>

#include "itkConstantPadImageFilter.h"

using namespace itk;
using namespace std;

GenericSliceWindow
::GenericSliceWindow(int x, int y, int w, int h, const char *l) 
: FLTKCanvas(x, y, w, h, l)
{
  // Start with a blank ID
  m_Id = -1;  

  // Initialize the interaction modes
  m_CrosshairsMode = new CrosshairsInteractionMode(this);
  m_ZoomPanMode = new ZoomPanInteractionMode(this);

  // Zero out the registered flags
  m_IsRegistered = false;
  m_IsSliceInitialized = false;

  // Initialize the Grey slice texture
  m_GreyTexture = new GreyTextureType;
  
  // Initialize the Segmentation slice texture (not default)
  m_LabelTexture = new LabelTextureType;
  m_LabelTexture->SetGlComponents(4);
  m_LabelTexture->SetGlFormat(GL_RGBA);

  // Initalize the margin
  m_Margin = 2;
}

GenericSliceWindow
::~GenericSliceWindow()
{
  // Delete the interaction modes
  delete m_CrosshairsMode;
  delete m_ZoomPanMode;

  // Delete textures
  delete m_GreyTexture;
  delete m_LabelTexture;
}

void
GenericSliceWindow
::Register(int index, UserInterfaceLogic *ui)
{
  // Copy parent pointers
  m_ParentUI = ui;
  m_Driver = m_ParentUI->GetDriver();
  m_GlobalState = m_Driver->GetGlobalState();    

  // This array describes the conjugate axes for the three display orientations
  // static const unsigned int linkedAxes[3][2] = {{1,2},{0,2},{0,1}};
  // static const unsigned int linkedAxes[3][2] = {{2,1},{0,2},{0,1}};

  // Initialize the axes indices (these indices map u,v coordinates of the 
  // slice to the x,y,z coordinates of the display space
  m_Id = index;
  // m_DisplayAxes[0] = linkedAxes[m_Id][0];
  // m_DisplayAxes[1] = linkedAxes[m_Id][1];
  // m_DisplayAxes[2] = m_Id;

  // Register the interaction modes
  m_CrosshairsMode->Register();
  m_ZoomPanMode->Register();

  // We have been registered
  m_IsRegistered = true;
}    

void 
GenericSliceWindow
::InitializeSlice(IRISImageData *imageData)
{
  // Register should have been called already
  assert(m_IsRegistered);

  // Store the image data pointer
  m_ImageData = imageData;

  // Initialize the grey slice texture
  m_GreyTexture->SetImage(
    m_ImageData->GetGrey()->GetDisplaySlice(m_Id));

  // Initialize the segmentation slice texture
  m_LabelTexture->SetImage(
    m_ImageData->GetSegmentation()->GetDisplaySlice(m_Id));

  // Store the transforms between the display and image spaces
  m_ImageToDisplayTransform = 
    imageData->GetImageGeometry().GetImageToDisplayTransform(m_Id);
  m_DisplayToImageTransform =
    imageData->GetImageGeometry().GetDisplayToImageTransform(m_Id);
  m_DisplayToAnatomyTransform = 
    imageData->GetImageGeometry().GetAnatomyToDisplayTransform(m_Id).Inverse();
  
  // Get the volume extents & voxel scale factors
  Vector3ui imageSizeInImageSpace = m_ImageData->GetVolumeExtents();
  Vector3f imageScalingInImageSpace = m_ImageData->GetVoxelScaleFactor();

  // Initialize quantities that depend on the image and its transform
  for(unsigned int i = 0;i < 3;i++) 
    {    
    // Get the direction in image space that corresponds to the i'th
    // direction in slice space
    m_ImageAxes[i] = m_DisplayToImageTransform.GetCoordinateIndexZeroBased(i);

    // Record the size and scaling of the slice
    m_SliceSize[i] = imageSizeInImageSpace[m_ImageAxes[i]];
    m_SliceSpacing[i] = imageScalingInImageSpace[m_ImageAxes[i]]; // TODO: Reverse sign by orientation?
    }

  // No information about the current slice available yet
  m_ImageSliceIndex = -1;
  m_DisplayAxisPosition = 0.0f;

  // We have been initialized
  m_IsSliceInitialized = true;
  
  // If the is no current interaction mode, enter the crosshairs mode
  if(GetInteractionModeCount() == 0)
    PushInteractionMode(m_CrosshairsMode);
  
  // setup default view - fit to window
  ResetViewToFit();
}

void
GenericSliceWindow
::ResetViewToFit()
{
  // Should be fully initialized
  assert(m_IsRegistered && m_IsSliceInitialized);

  // Compute slice size in spatial coordinates
  Vector2f worldSize(
    m_SliceSize[0] * m_SliceSpacing[0],
    m_SliceSize[1] * m_SliceSpacing[1]);

  // Set the view position (position of the center of the image?)
  m_ViewPosition = worldSize * 0.5f;

  // Reduce the width and height of the slice by the margin
  Vector2i szCanvas = Vector2i(w(),h()) - Vector2i(2 * m_Margin);
  
  // Compute the ratios of window size to slice size
  Vector2f ratios(
    szCanvas(0) / worldSize(0),
    szCanvas(1) / worldSize(1));

  // The zoom factor is the bigger of these ratios, the number of pixels 
  // on the screen per millimeter in world space
  m_OptimalZoom = m_ViewZoom = ratios.min_value();

  // Cause a redraw of the window
  redraw();
}

Vector3f 
GenericSliceWindow
::MapSliceToImage(const Vector3f &xSlice) 
{
  assert(m_IsSliceInitialized);

  // Get corresponding position in display space
  return m_DisplayToImageTransform.TransformPoint(xSlice);
}

/**
 * Map a point in image coordinates to slice coordinates
 */
Vector3f 
GenericSliceWindow
::MapImageToSlice(const Vector3f &xImage) 
{
  assert(m_IsSliceInitialized);

  // Get corresponding position in display space
  return  m_ImageToDisplayTransform.TransformPoint(xImage);
}


Vector2f 
GenericSliceWindow
::MapSliceToWindow(const Vector3f &xSlice)
{
  assert(m_IsSliceInitialized);

  // Adjust the slice coordinates by the scaling amounts
  Vector2f uvScaled(
    xSlice(0) * m_SliceSpacing(0),xSlice(1) * m_SliceSpacing(1));

  // Compute the window coordinates
  Vector2f uvWindow = 
    m_ViewZoom * (uvScaled - m_ViewPosition) + Vector2f(0.5f*w(),0.5f*h());
  
  // That's it, the projection matrix is set up in the scaled-slice coordinates
  return uvWindow;
}

Vector3f 
GenericSliceWindow
::MapWindowToSlice(const Vector2f &uvWindow)
{
  assert(m_IsSliceInitialized && m_ViewZoom > 0);

  // Compute the scaled slice coordinates
  Vector2f uvScaled = 
    m_ViewPosition + (uvWindow - Vector2f(0.5f*w(),0.5f*h())) / m_ViewZoom;
  
  // The window coordinates are already in the scaled-slice units
  Vector3f uvSlice(
    uvScaled(0) / m_SliceSpacing(0),
    uvScaled(1) / m_SliceSpacing(1),
    m_DisplayAxisPosition);

  // Return this vector
  return uvSlice;
}

int 
GenericSliceWindow
::handle(int eventID)
{
  // When mouse enters the window, request focus
  if(eventID == FL_ENTER)
    {
    this->take_focus();
    return 1;
    }

  // Handle the focus event properly
  if(eventID == FL_FOCUS)
    {
    return 1;
    }

  // Normal processing
  return FLTKCanvas::handle(eventID);
}

void
GenericSliceWindow
::draw()
{
  // Set up the projection if necessary
  if(!valid()) 
  {
    // The window will have coordinates (0,0) to (w,h), i.e. the same as the window
    // coordinates.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,w(),0.0,h());
    glViewport(0,0,w(),h());

    // Establish the model view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  // Clear the display
  glClearColor(0.0,0.0,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

  // Slice should be initialized before display
  if (!m_IsSliceInitialized) 
    return;

  // Compute the position of the cross-hairs in display space
  Vector3ui cursorImageSpace = m_GlobalState->GetCrosshairsPosition();
  Vector3f cursorDisplaySpace = 
    m_ImageToDisplayTransform.TransformPoint(
      to_float(cursorImageSpace) + Vector3f(0.5f));

  // Get the current slice number
  m_ImageSliceIndex = cursorImageSpace[m_ImageAxes[2]];
  m_DisplayAxisPosition = cursorDisplaySpace[2];

  // Set up lighting attributes
  glPushAttrib(GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | 
               GL_PIXEL_MODE_BIT | GL_TEXTURE_BIT );  
  
  
  glDisable(GL_LIGHTING);

  // glDisable(GL_DEPTH);

  // Prepare for overlay drawing.  The model view is set up to correspond
  // to pixel coordinates of the slice
  glPushMatrix();
  glTranslated(0.5 * w(),0.5 * h(),0.0);
  glScalef(m_ViewZoom,m_ViewZoom,1.0);
  glTranslated(-m_ViewPosition(0),-m_ViewPosition(1),0.0);
  glScalef(m_SliceSpacing[0],m_SliceSpacing[1],1.0);
  
  // Make the grey and segmentation image textures up-to-date
  DrawGreyTexture();
  DrawSegmentationTexture();

  // Draw the overlays
  DrawOverlays(false);

  // Draw the zoom locator
  DrawZoomLocator();

  // Clean up the GL state
  glPopMatrix();
  glPopAttrib();

  // Display!
  glFlush();
}

void 
GenericSliceWindow
::DrawGreyTexture(unsigned char r, unsigned char g, unsigned char b) 
{
  // We should have a slice to return
  assert(m_ImageData->IsGreyLoaded() && m_ImageSliceIndex >= 0);

  // Paint the grey texture
  m_GreyTexture->Draw(r, g, b);
}

void 
GenericSliceWindow
::DrawSegmentationTexture() 
{
  // We should have a slice to return
  assert(m_ImageData->IsSegmentationLoaded() 
    && m_ImageSliceIndex >= 0);

  // Update the texture memory
  m_LabelTexture->DrawTransparent(m_GlobalState->GetSegmentationAlpha());
}

void
GenericSliceWindow
::DrawZoomLocator()
{
  // Draw the little version of the image in the corner of the window
  if(m_ViewZoom > m_OptimalZoom)
    {
    double w = m_SliceSize[0];
    double h = m_SliceSize[1];

    glPushMatrix();
    glLoadIdentity();
    glTranslated(5.0, 5.0, 0.0);

    glScalef(0.334 * m_OptimalZoom,0.334 * m_OptimalZoom,1.0);
    
    glPushMatrix();
    glScalef(m_SliceSpacing[0],m_SliceSpacing[1],1.0);
    // glTranslated(w * 0.1111, h * 0.1111, 0.0);
    
    // Draw the grey scale image
    DrawGreyTexture(255, 224, 32);

    // Draw the crosshairs and stuff
    DrawOverlays(true);

    // Draw the line around the image
    glColor3d(0.8, 0.8, 0.2);
    glBegin(GL_LINE_LOOP);
    glVertex2d(0,0);
    glVertex2d(0,h);
    glVertex2d(w,h);
    glVertex2d(w,0);
    glEnd();

    // Draw a box representing the current zoom level
    glPopMatrix();
    glTranslated(m_ViewPosition[0],m_ViewPosition[1],0.0);
    w = this->w() * 0.5 / m_ViewZoom;
    h = this->h() * 0.5 / m_ViewZoom;


    glColor3d(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex2d(-w,-h);
    glVertex2d(-w, h);
    glVertex2d( w, h);
    glVertex2d( w,-h);
    glEnd();
    
    glPopMatrix();
    }
}

void 
GenericSliceWindow
::DrawOverlays(bool inZoomLocator) 
{
  if(!inZoomLocator) 
    {
    // Display the letters (RAI)
    DrawOrientationLabels();
  
    // Draw the zoom mode (does't really draw, repaints a UI widget)
    m_ZoomPanMode->OnDraw();
    }

  // Draw the crosshairs
  m_CrosshairsMode->OnDraw(); 
}

void 
GenericSliceWindow
::DrawOrientationLabels()
{
  // The letter labels
  static const char *letters[3][2] = {{"R","L"},{"A","P"},{"I","S"}};
  const char *labels[2][2];

  // Repeat for X and Y directions
  for(unsigned int i=0;i<2;i++) 
    {
    // Which axis are we on in anatomy space?
    unsigned int anatomyAxis = 
      m_DisplayToAnatomyTransform.GetCoordinateIndexZeroBased(i);

    // Which direction is the axis facing (returns -1 or 1)
    unsigned int anatomyAxisDirection = 
      m_DisplayToAnatomyTransform.GetCoordinateOrientation(i);

    // Map the direction onto 0 or 1
    unsigned int letterIndex = (1 + anatomyAxisDirection) >> 1;

    // Compute the two labels for this axis
    labels[i][0] = letters[anatomyAxis][1-letterIndex];
    labels[i][1] = letters[anatomyAxis][letterIndex];
    }

  glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  glLoadIdentity();

  //glEnable(GL_LIGHTING);
  //glEnable(GL_DEPTH);
/*
  glBegin(GL_LINE_STRIP);
  glVertex2d(320,320);
  glVertex2d(320,100);
  glVertex2d(100,320);
  glEnd();
*/
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(1,1,0,0.5);

  gl_font(FL_COURIER_BOLD, 14);
  
  gl_draw(labels[0][0],0,0,32,h(),FL_ALIGN_LEFT);
  gl_draw(labels[0][1],w() - 33,0,32,h(),FL_ALIGN_RIGHT);
  gl_draw(labels[1][0],0,0,w(),32,FL_ALIGN_BOTTOM);
  gl_draw(labels[1][1],0,h() - 33,w(),32,FL_ALIGN_TOP);

  glPopMatrix();
  glPopAttrib();
}
  
void 
GenericSliceWindow
::EnterCrosshairsMode()
{
  // Place the mode on the stack
  ClearInteractionStack();
  PushInteractionMode(m_CrosshairsMode);
}

void 
GenericSliceWindow
::EnterZoomPanMode()
{
  // Place the mode on the stack
  ClearInteractionStack();
  PushInteractionMode(m_CrosshairsMode);
  PushInteractionMode(m_ZoomPanMode);
}

void 
GenericSliceWindow
::SetViewZoom(float newZoom)
{
  // Update the zoom
  m_ViewZoom = newZoom;  
  // cout << m_Id << " : " << newZoom 

  // Repaint the window
  redraw();
}



