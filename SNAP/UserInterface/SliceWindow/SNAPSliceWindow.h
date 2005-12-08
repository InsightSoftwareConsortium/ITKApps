/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPSliceWindow.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SNAPSliceWindow_h_
#define __SNAPSliceWindow_h_

#include "GenericSliceWindow.h"

// Forward references
class SNAPImageData;
class BubblesInteractionMode;
 
/**
 * \class SNAPSliceWindow
 * \brief The window used to display slices in SnAP part of the application.  
 *
 * SnAP not only has the original greyscale image, but also a floating 
 * point preprocessing image.  The overlays for SnAP include bubble display.
 */
class SNAPSliceWindow : public GenericSliceWindow
{
public:

  SNAPSliceWindow(int x,int y,int w,int h,const char *label=0);
  ~SNAPSliceWindow();

  /** The parent's register method is overridden */
  void Register(int id,UserInterfaceLogic *parentUI);

  /** Overrides the parent's method */
  void InitializeSlice(IRISImageData *imageData);

protected:

  // Texture object type associated with the preprocessing (speed) image
  typedef OpenGLSliceTexture<RGBAType> SpeedTextureType;

  // Texture object type associated with the active segmentation image
  typedef OpenGLSliceTexture<RGBAType> SnakeTextureType;
  
  // Another texture object type used to display thresholding overlays, etc
  typedef OpenGLSliceTexture<RGBAType> OverlayTextureType;

  // The interaction mode for bubble drawing
  BubblesInteractionMode *m_BubblesMode;
  
  // SNAP image data object displayed in this window (overrides parent's 
  // m_ImageData
  SNAPImageData *m_ImageData;

  // Preprocessed slice texture object
  SpeedTextureType *m_SpeedTexture;

  // Segmentation slice texture object
  SnakeTextureType *m_SnakeTexture;

  // The overlay texture object
  OverlayTextureType *m_OverlayTexture;

  // Overlay and texture drawing are customized in this class
  void DrawOverlays();
  void DrawGreyTexture();
  void DrawSegmentationTexture();  
};

#endif // SNAP Slice Window
