/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    UserInterfaceBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __UserInterfaceBase__h_
#define __UserInterfaceBase__h_

#include "SNAPCommonUI.h"

// TODO: this is a hack.  Clean all code out of GUI!!!
#include "GlobalState.h"

/**
 * \class UserInterfaceBase
 * \brief Base class for the main user interface.
 */
class UserInterfaceBase {
public:

  // Methods for switching between SNAP and IRIS modes
  virtual void ShowIRIS() = 0;
  virtual void ShowSNAP() = 0;
  
  // Method for exiting IRIS
  virtual void OnMainWindowCloseAction() = 0;

  // Methods that set the IRIS Toolbar and 3D Toolbar modes
  virtual void SetToolbarMode(ToolbarModeType mode) = 0;
  virtual void SetToolbarMode3D(ToolbarMode3DType mode) = 0;
  
  // Menu item callbacks
  virtual void OnMenuOpenProject() = 0;
  virtual void OnMenuSaveProject() = 0;
  virtual void OnMenuCloseProject() = 0;
  virtual void OnMenuLoadGrey() = 0;
  virtual void OnMenuSaveGreyROI() = 0;
  virtual void OnMenuLoadSegmentation() = 0;
  virtual void OnMenuSaveSegmentation() = 0;
  virtual void OnMenuLoadPreprocessed() = 0;  
  virtual void OnMenuSavePreprocessed() = 0; 
  virtual void OnMenuLoadLabels() = 0;
  virtual void OnMenuSaveLabels() = 0;
  virtual void OnMenuWriteVoxelCounts() = 0;
  virtual void OnMenuIntensityCurve() = 0;  
  virtual void OnMenuShowDisplayOptions() = 0;
  virtual void OnMenuQuit() = 0;
  
  // IRIS: Slice selection actions  
  virtual void OnSliceSliderChange(int id) = 0;
  virtual void RedrawWindows() = 0;
  virtual void ResetScrollbars() = 0;
  virtual void UpdateImageProbe() = 0;
  virtual void UpdatePositionDisplay(int id) = 0;

  // IRIS: Zoom/pan interaction callbacks
  virtual void OnResetView2DAction(unsigned int window) = 0;
  virtual void OnResetAllViews2DAction() = 0;
  virtual void OnLinkedZoomChange() = 0;
  virtual void OnZoomPercentageChange() = 0;
  
  // IRIS: Color label selection and editing callbacks
  virtual void UpdateColorChips() = 0;
  virtual void OnDrawingLabelUpdate() = 0;
  virtual void OnDrawOverLabelUpdate() = 0;
  virtual void ChangeLabelsCallback() = 0;
  virtual void UpdateEditLabelWindow() = 0;
  
  // IRIS: Polygon buttons callbacks
  virtual void OnAcceptPolygonAction(unsigned int window) = 0;
  virtual void OnDeletePolygonSelectedAction(unsigned int window) = 0;
  virtual void OnInsertIntoPolygonSelectedAction(unsigned int window) = 0;
  virtual void OnPastePolygonAction(unsigned int window) = 0;
  
  // IRIS: ROI manipulation callbacks
  virtual void OnResetROIAction() = 0;
  virtual void OnSnakeStartAction() = 0;
  
  // SNAP Preprocessing page actions
  virtual void OnInOutSnakeSelect() = 0;
  virtual void OnEdgeSnakeSelect() = 0;
  virtual void OnPreprocessAction() = 0;
  virtual void OnPreprocessClose() = 0;
  virtual void OnLoadPreprocessedImageAction() = 0;
  virtual void OnAcceptPreprocessingAction() = 0;

  // SNAP Initialization page actions
  virtual void OnAddBubbleAction() = 0;
  virtual void OnRemoveBubbleAction() = 0;
  virtual void OnActiveBubblesChange() = 0;
  virtual void OnBubbleRadiusChange() = 0;
  virtual void OnAcceptInitializationAction() = 0;
  
  // SNAP Segmentation page actions
  virtual void OnRestartInitializationAction() = 0;
  virtual void OnSnakeParametersAction() = 0;
  virtual void OnAcceptSegmentationAction() = 0;
  virtual void OnSnakeRewindAction() = 0;
  virtual void OnSnakeStopAction() = 0;
  virtual void OnSnakePlayAction() = 0;
  virtual void OnSnakeStepAction() = 0;
  virtual void OnSnakeStepSizeChange() = 0;
  virtual void OnRestartPreprocessingAction() = 0;
  virtual void OnCancelSegmentationAction() = 0;
  
  // SNAP display interaction actions  
  virtual void OnSNAPViewOriginalSelect() = 0;
  virtual void OnViewPreprocessedSelect() = 0;
  
  virtual void MakeSegTexturesCurrent() = 0;
  virtual void UpdateMainLabel() = 0;

  // Display options callbacks
  virtual void OnSliceAnatomyOptionsChange(unsigned int order) = 0;
  virtual void OnDisplayOptionsCancelAction() = 0;
  virtual void OnDisplayOptionsOkAction() = 0;
  virtual void OnDisplayOptionsApplyAction() = 0;
  
  // Opacity sliders  
  virtual void OnIRISLabelOpacityChange() = 0;
  virtual void OnSNAPLabelOpacityChange() = 0;

  // 3D window related callbacks  
  virtual void OnContinuousViewUpdateChange() = 0;
  virtual void Activate3DAccept(bool on) = 0;

  // Help related callbacks
  virtual void OnLaunchTutorialAction() = 0;

protected:
    GlobalState *m_GlobalState;
};

#endif // __UserInterfaceBase__h_
