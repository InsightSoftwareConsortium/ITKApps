/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    GenericSliceWindow.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __GenericSliceWindow_h_
#define __GenericSliceWindow_h_

#include "FLTKCanvas.h"
#include "IRISTypes.h"
#include "GreyImageWrapper.h"
#include "LabelImageWrapper.h"

// Forward references to parent classes
class IRISApplication;
class IRISImageData;
class GlobalState;
class UserInterfaceLogic;  

// Forward references to interaction modes that work with this window
class CrosshairsInteractionMode;
class ZoomPanInteractionMode;

// Forward reference to Gl texture object
template <class TPixel> class OpenGLSliceTexture;


/**
 * \class GenericSliceWindow
 * \brief A window used to display a 2D slice either in SNAP or in IRIS mode.
 *
 * A generic slice window, that is neither fitted to IRIS nor to SNAP.  The
 * IRIS and SNAP windows are children of this slice window class.  This class
 * provides support for display space to image space transforms, for texture
 * display and management, and for interaction mode plugins.
 *
 * The generic window supports two types of interaction modes: crosshairs mode
 * and zoom/pan mode.  
 */
class GenericSliceWindow : public FLTKCanvas
{
public:
  
  GenericSliceWindow(int x,int y,int w,int h,const char *l=0);
  virtual ~GenericSliceWindow();

  /**
   * Register the window with its parent UI.  This method assigns an Id to 
   * the window, which is equal to the coordinate direction in display space
   * along which the window displays slices.
   */
  virtual void Register(int id, UserInterfaceLogic *parentUI);

  /** Enter the cross-hairs mode of operation */
  virtual void EnterCrosshairsMode();
  
  /** Enter the zoom/pan mode of operation */
  virtual void EnterZoomPanMode();

  /**
   * Initialize the window's attributes (size, view position, etc.)
   * This method should called when the image dimensions and transforms
   * get updated.
   */
  virtual void InitializeSlice(IRISImageData *imageData);

  /**
   * Reset the view parameters of the window (zoom, view position) to
   * defaults
   */
  virtual void ResetView();

  /**
   * The FLTK draw method (paints the window)
   */
  void draw();

  /**
   * Map a point in window coordinates to a point in slice coordinates
   * (Window coordinates are the ones stored in FLTKEvent.xSpace)
   */
  Vector2f MapWindowToSlice(const Vector2f &xWindow); 
  
  /**
   * Map a point in slice coordinates to a point in window coordinates
   * (Window coordinates are the ones stored in FLTKEvent.xSpace)
   */
  Vector2f MapSliceToWindow(const Vector2f &uvSlice);
  
  /**
   * Map a point in slice coordinates to a point in the image coordinates
   */
  Vector3f MapSliceToImage(const Vector2f &uvSlice);

  /**
   * Map a point in image coordinates to slice coordinates
   */
  Vector2f MapImageToSlice(const Vector3f &xImage);

  /**
   * A parent class from which all the Fl event handlers associated
   * with this class should be derived
   */
  class EventHandler : public InteractionMode {
  public:
    EventHandler(GenericSliceWindow *parent) {
      m_Parent = parent;
    }    
    void Register() 
    {
      m_Driver = m_Parent->m_Driver;
      m_ParentUI = m_Parent->m_ParentUI;
      m_GlobalState = m_Parent->m_GlobalState;
    }
  protected:
    GenericSliceWindow *m_Parent;
    GlobalState *m_GlobalState;
    UserInterfaceLogic *m_ParentUI;
    IRISApplication *m_Driver;
  };

  // Allow friendly access by interactors
  friend class EventHandler;
  friend class BubblesInteractionMode;
  friend class CrosshairsInteractionMode;
  friend class ZoomPanInteractionMode;
  friend class RegionInteractionMode;
  friend class PolygonInteractionMode;

protected:

  /** Pointer to the application driver for this UI object */
  IRISApplication *m_Driver;

  /** Pointer to the global state object (shorthand) */
  GlobalState *m_GlobalState;

  /** Pointer to GUI that contains this Window3D object */
  UserInterfaceLogic *m_ParentUI;   

  /** The image data object that is displayed in this window */
  IRISImageData *m_ImageData;

  /** Interaction mode used to position the crosshairs */
  CrosshairsInteractionMode *m_CrosshairsMode;

  /** Interaction mode used to zoom and pan the image */
  ZoomPanInteractionMode *m_ZoomPanMode;

  /** Whether or not we have been registered with the parent UI */
  bool m_IsRegistered;

  /** 
   * Whether or not the sizes have been initialized 
   * (synched with the image data)
   */
  bool m_IsSliceInitialized;

  // Window id, equal to the direction in display space along which the window
  // shows slices
  int m_Id;       

  // Current slice number
  int m_SliceIndex;

  // The index of the display space axes that correspond to the u,v,w of the 
  // window id==0 => (1,2,0); id==1 => (0,2,1); id==2 => (0,1,2)
  int m_DisplayAxes[3];
  
  // The index of the image space axes corresponding to the u,v,w of the window
  // (computed by applying a transform to the DisplayAxes)
  int m_ImageAxes[3]; 

  // Dimensions of the current slice (the third component is the size of the
  // image in the slice direction)
  Vector3i m_SliceSize;             

  // Pixel dimensions for the slice.  (the thirs component is the pixel width
  // in the slice direction)
  Vector3f m_SliceScale;
  
  // Position of visible window in slice space coordinates
  Vector2f m_ViewPosition;            

  // The current zoom factor
  float m_ViewZoom;                         

  // Grey texture object typedefs
  typedef OpenGLSliceTexture<unsigned char> GreyTextureType;

  // Label texture object typedefs
  typedef LabelImageWrapper::DisplayPixelType RGBAType;
  typedef OpenGLSliceTexture<RGBAType> LabelTextureType;
    
  // Grey texture object
  GreyTextureType *m_GreyTexture;

  // Label texture object
  LabelTextureType *m_LabelTexture;
  
  // This method is called in draw() to paint the grey slice
  virtual void DrawGreyTexture();

  // This method is called in draw() to paint the segmentation slice
  virtual void DrawSegmentationTexture();

  // This method is called after the grey and segmentation images have
  // been drawn.  It calls the draw method of each of the interaction modes
  virtual void DrawOverlays();

  /** This method draws the RAI labels at the four sides of the slice */
  void DrawOrientationLabels();
};

#endif // __GenericSliceWindow_h_
