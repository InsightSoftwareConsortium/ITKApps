/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    GlobalState.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __GlobalState_h_
#define __GlobalState_h_

#include "IRISTypes.h"
#include "EdgePreprocessingSettings.h"
#include "MeshOptions.h"
#include "SnakeParameters.h"
#include "ThresholdSettings.h"
#include "itkImageRegion.h"

enum ToolbarModeType 
{
  POLYGON_DRAWING_MODE,
  NAVIGATION_MODE,
  PAINT3D_MODE,
  CROSSHAIRS_MODE,
  ROI_MODE
};

enum CoverageModeType 
{
  PAINT_OVER_ALL,
  PAINT_OVER_ONE,
  PAINT_OVER_COLORS
};

enum SnakeType 
{
  IN_OUT_SNAKE,
  EDGE_SNAKE
};

enum ConstraintsType 
{
  SAPIRO,
  SCHLEGEL,
  TURELLO,
  USER
};
                     
/**
 * \class GlobalState
 * \brief Contains global variables describing the state of the application.
 */
class GlobalState 
{
public:
  // Region of interest definition
  typedef itk::ImageRegion<3> RegionType;

  GlobalState();
  virtual ~GlobalState();
  
  /** Get color label used to draw polygons */
  irisSetMacro(DrawingColorLabel,unsigned char );

  /** Set color label used to draw polygons */
  irisGetMacro(DrawingColorLabel,unsigned char );
  
  /** Get color label over which we can draw */
  irisSetMacro(OverWriteColorLabel,unsigned char );

  /** Set color label over which we can draw */
  irisGetMacro(OverWriteColorLabel,unsigned char );
  
  /** Get whether the grey image display uses linear interpolation */
  irisSetMacro(InterpolateGrey,bool );

  /** Set whether the grey image display uses linear interpolation */
  irisGetMacro(InterpolateGrey,bool );

  /** Get whether the segmentation image uses linear interpolation */
  irisSetMacro(InterpolateSegmentation,bool );

  /** Set whether the segmentation image uses linear interpolation */
  irisGetMacro(InterpolateSegmentation,bool );

  /** Get whether polygons drawn are inverted or not */
  irisSetMacro(PolygonInvert,bool );

  /** Set whether polygons drawn are inverted or not */
  irisGetMacro(PolygonInvert,bool );

  /** Get the transparency of the segmentation overlay */
  irisSetMacro(SegmentationAlpha,unsigned char );

  /** Set the transparency of the segmentation overlay */
  irisGetMacro(SegmentationAlpha,unsigned char );

  /** Get the current crosshairs position */
  irisSetMacro(CrosshairsPosition,Vector3i );

  /** Set the current crosshairs position */
  irisGetMacro(CrosshairsPosition,Vector3i );

  /** Get the current toolbar mode */
  irisSetMacro(ToolbarMode,ToolbarModeType );

  /** Set the current toolbar mode */
  irisGetMacro(ToolbarMode,ToolbarModeType );

  /** Get whether the slice requires an update or not (TODO: obsolete?) */
  irisSetMacro(UpdateSliceFlag,int );

  /** Set whether the slice requires an update or not (TODO: obsolete?) */
  irisGetMacro(UpdateSliceFlag,int );

  /** Get current mode of polygon/snake painting (over all, over label) */
  irisSetMacro(CoverageMode,CoverageModeType );

  /** Set current mode of polygon/snake painting (over all, over label) */
  irisGetMacro(CoverageMode,CoverageModeType );

  /** Get whether the region of interest is valid */
  irisSetMacro(IsValidROI,bool );

  /** Set whether the region of interest is valid */
  irisGetMacro(IsValidROI,bool );

  /** Get whether the region of interest is visible */
  irisSetMacro(ShowROI,bool );

  /** Set whether the region of interest is visible */
  irisGetMacro(ShowROI,bool );

  /** Get whether the region of interest is being dragged */
  irisSetMacro(DraggingROI,bool );

  /** Set whether the region of interest is being dragged */
  irisGetMacro(DraggingROI,bool );

  /** Get whether SNAP is currently active */
  irisSetMacro(SNAPActive,bool );

  /** Set whether SNAP is currently active */
  irisGetMacro(SNAPActive,bool );

  /** Get whether the speed (preprocessing) image is visible */
  irisSetMacro(ShowSpeed,bool );

  /** Set whether the speed (preprocessing) image is visible */
  irisGetMacro(ShowSpeed,bool );

  /** Get whether the speed (preprocessing) image is valid */
  irisSetMacro(SpeedValid,bool );

  /** Set whether the speed (preprocessing) image is valid */
  irisGetMacro(SpeedValid,bool );

  /** Get whether the zero level of the speed image is being displayed */
  irisSetMacro(SpeedViewZero,bool );

  /** Set whether the zero level of the speed image is being displayed */
  irisGetMacro(SpeedViewZero,bool );

  /** Get the type of the snake being used */
  irisSetMacro(SnakeMode,SnakeType );

  /** Set the type of the snake being used */
  irisGetMacro(SnakeMode,SnakeType );

  /** Get whether the snake is currently active */
  irisSetMacro(SnakeActive,bool );

  /** Set whether the snake is currently active */
  irisGetMacro(SnakeActive,bool );

  /** Set whether the speed preview is valid or not */
  irisSetMacro(SpeedPreviewValid, bool);
  
  /** Get whether the speed preview is valid or not */
  irisGetMacro(SpeedPreviewValid, bool);
  
  /** Set the auto-preview feature of edge-mode preprocessor uses  */
  irisSetMacro(ShowPreprocessedEdgePreview,bool);

  /** Get the auto-preview feature of edge-mode preprocessor uses  */
  irisGetMacro(ShowPreprocessedEdgePreview,bool);

  /** Set the auto-preview feature of edge-mode preprocessor uses  */
  irisSetMacro(ShowPreprocessedInOutPreview,bool);

  /** Get the auto-preview feature of edge-mode preprocessor uses  */
  irisGetMacro(ShowPreprocessedInOutPreview,bool);

  /** Get the current settings for in/out snake processing */
  irisGetMacro(ThresholdSettings,ThresholdSettings);

  /** Set the current settings for in/out snake processing */
  irisSetMacro(ThresholdSettings,ThresholdSettings);  

  /** Set the current settings for edge snake processing */
  irisSetMacro(EdgePreprocessingSettings,EdgePreprocessingSettings);  

  /** Get the current settings for edge snake processing */
  irisGetMacro(EdgePreprocessingSettings,EdgePreprocessingSettings);

  /** Get the current parameters of the snake algorithm */
  irisGetMacro(SnakeParameters,SnakeParameters);

  /** Set the current parameters of the snake algorithm */
  irisSetMacro(SnakeParameters,SnakeParameters);

  /** Get the current mesh rendering options */
  irisGetMacro(MeshOptions,MeshOptions);

  /** Set the current mesh rendering options */
  irisSetMacro(MeshOptions,MeshOptions);

  /** Set the region of interest for the segmentation (drawn by the user) */
  irisSetMacro(SegmentationROI,RegionType);

  /** Get the region of interest for the segmentation (drawn by the user) */
  irisGetMacro(SegmentationROI,RegionType);


#ifdef DRAWING_LOCK
  int GetDrawingLock( short );
  int ReleaseDrawingLock( short );
#endif /* DRAWING_LOCK */

  /** Set the extension of the grey image */
  void SetGreyExtension(char * fname);
  
  /** Get the extension of the grey image */
  void GetGreyExtension(char *& ext);

private:
  /** Color label used to draw polygons */
  unsigned char m_DrawingColorLabel;

  /** Color label over which we can draw */
  unsigned char m_OverWriteColorLabel;

  /** Whether the grey image display uses linear interpolation */
  bool m_InterpolateGrey;

  /** Whether the segmentation image uses linear interpolation */
  bool m_InterpolateSegmentation;

  /** Whether polygons drawn are inverted or not */
  bool m_PolygonInvert;

  /** The transparency of the segmentation overlay */
  unsigned char m_SegmentationAlpha;

  /** The current crosshairs position */
  Vector3i m_CrosshairsPosition;

  /** The current toolbar mode */
  ToolbarModeType m_ToolbarMode;

  /** Whether the slice requires an update or not (TODO: obsolete?) */
  int m_UpdateSliceFlag;

  /** Current mode of polygon/snake painting (over all, over label) */
  CoverageModeType m_CoverageMode;

  /** Whether the region of interest is valid */
  bool m_IsValidROI;

  /** Whether the region of interest is visible */
  bool m_ShowROI;

  /** Whether the region of interest is being dragged */
  bool m_DraggingROI;

  /** Whether SNAP is currently active */
  bool m_SNAPActive;

  /** Whether the speed (preprocessing) image is visible */
  bool m_ShowSpeed;

  /** Whether the speed (preprocessing) image is valid */
  bool m_SpeedValid;

  /** Whether the zero level of the speed image is being displayed */
  bool m_SpeedViewZero;

  /** The type of the snake being used */
  SnakeType m_SnakeMode;

  /** Whether the snake is currently active */
  bool m_SnakeActive;

  /** Grey image file extension */
  char * m_GreyFileExtension; 

  /** The region of interest for the segmentation (drawn by the user) */
  RegionType m_SegmentationROI;
  
  int m_LockHeld; 
  int m_LockOwner;

  // Current settings for threshold preprocessing
  ThresholdSettings m_ThresholdSettings;

  // Current settings for threshold preprocessing
  EdgePreprocessingSettings m_EdgePreprocessingSettings;

  // Current mesh options
  MeshOptions m_MeshOptions;

  // Whether preview is valid or not
  bool m_SpeedPreviewValid;
  
  // Auto-preview state
  bool m_ShowPreprocessedEdgePreview;
  bool m_ShowPreprocessedInOutPreview;

  // Current settings for the snake algorithm
  SnakeParameters m_SnakeParameters;
};

#endif // __GlobalState_h_

/*
 *Log: GlobalState.h
 *Revision 1.2  2003/08/27 04:57:45  pauly
 *FIX: A large number of bugs has been fixed for 1.4 release
 *
 *Revision 1.1  2003/07/12 04:52:23  pauly
 *Initial checkin of SNAP application  to the InsightApplications tree
 *
 *Revision 1.10  2003/07/12 01:34:18  pauly
 *More final changes before ITK checkin
 *
 *Revision 1.9  2003/07/11 23:28:52  pauly
 **** empty log message ***
 *
 *Revision 1.8  2003/06/08 16:11:42  pauly
 *User interface changes
 *Automatic mesh updating in SNAP mode
 *
 *Revision 1.7  2003/06/03 00:06:46  pauly
 *Almost ready for Pittsburgh demo
 *
 *Revision 1.6  2003/05/22 17:36:19  pauly
 *Edge preprocessing settings
 *
 *Revision 1.5  2003/05/14 18:33:58  pauly
 *SNAP Component is working. Double thresholds have been enabled.  Many other changes.
 *
 *Revision 1.4  2003/05/07 19:14:46  pauly
 *More progress on getting old segmentation working in the new SNAP.  Almost there, region of interest and bubbles are working.
 *
 *Revision 1.3  2003/04/23 20:36:23  pauly
 **** empty log message ***
 *
 *Revision 1.2  2003/04/23 06:05:18  pauly
 **** empty log message ***
 *
 *Revision 1.1  2003/03/07 19:29:47  pauly
 *Initial checkin
 *
 *Revision 1.1.1.1  2002/12/10 01:35:36  pauly
 *Started the project repository
 *
 *
 *Revision 1.16  2002/05/08 17:33:34  moon
 *I made some changes Guido wanted in the GUI, including removing
 *Turello/Sapiro/Schlegel options (I only hid them, the code is all
 *still there), changing a bunch of the ranges, defaults, etc. of the
 *snake parameters.
 *
 *Revision 1.15  2002/04/27 18:30:14  moon
 *Finished commenting
 *
 *Revision 1.14  2002/04/27 00:08:43  talbert
 *Final commenting run through . . . no functional changes.
 *
 *Revision 1.13  2002/04/26 17:38:05  moon
 *Added global variable used by the Apply+ button in the in/out dialog so that
 *2D windows know to show zero level visualization rather than seg data.
 *
 *Revision 1.12  2002/04/24 19:51:34  moon
 *Added a flag for when the ROI was being dragged.  The roi dragging is now better in
 *some ways, although it is not completely perfect.
 *
 *Revision 1.11  2002/04/24 14:14:01  moon
 *Implemented separate brightness/contrast settings for grey/preproc data
 *
 *Revision 1.10  2002/04/20 21:57:20  talbert
 *Added some code to access the extension of the grey file.
 *
 *Revision 1.9  2002/04/20 18:20:40  talbert
 *Added functions to the global state which allowed access to the new
 *data member m_GreyFileExtension.
 *
 *Revision 1.8  2002/04/19 23:04:12  moon
 *Changed more stuff to get the snake params state synched with the global state.
 *
 *Revision 1.7  2002/04/19 20:35:43  moon
 *Made preproc dialogs check global state and only preproc if parameters have changed.
 *So no if you hit apply, then ok, it doesn't re process on the ok.
 *Added global state for preproc params and snake params.  Still need to get snake
 *params synched.
 *
 *Revision 1.6  2002/04/18 21:05:20  moon
 *Changed the IRIS window ROI stuff.  Now the ROI is always valid if an image is
 *loaded, but there is a toggle to show it or not.  This will work better with
 *Konstantin's addition of being able to drag the roi box.  Added global state
 *as appropriate.
 *
 *Revision 1.5  2002/04/01 22:31:30  moon
 *Added snakeMode and snakeActive to global state
 *snakeMode is in/out or edge, snakeActive is whether the snake
 *has been initialized, meaning it should be drawn in the windows
 *
 *Revision 1.4  2002/03/26 18:16:40  scheuerm
 *Added loading and display of preprocessed data:
 *- added vtkImageDeepCopy function
 *- added flags indicating which dataset to display in GlobalState
 *- added flag indicating whether to load gray or preprocessed data
 *  in the GUI class
 *
 *Revision 1.3  2002/03/08 14:06:29  moon
 *Added Header and Log tags to all files
 **/
