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

#include "IRISTypes.h"

// TODO: this is a hack.  Clean all code out of GUI!!!
#include "GlobalState.h"

/**
 * \class UserInterfaceBase
 * \brief Base class for the main user interface.
 */
class UserInterfaceBase {
public:

  // virtual void LoadGreyFileCallback() = 0;
  virtual void ShowIRIS() = 0;
  virtual void OnResetROIAction() = 0;
  virtual void OnShowROISelect() = 0;
  virtual void OnSnakeStartAction() = 0;
  // virtual void ApplyInOutPreprocessing() = 0;
  virtual void OnPreprocessAction() = 0;
  virtual void OnPreprocessClose() = 0;
  virtual void OnAddBubbleAction() = 0;
  virtual void OnRemoveBubbleAction() = 0;
  virtual void OnActiveBubblesChange() = 0;
  virtual void OnBubbleRadiusChange() = 0;
  virtual void OnSnakeParametersAction() = 0;
  virtual void OnAcceptSegmentationAction() = 0;
  virtual void OnCancelSegmentationAction() = 0;
  virtual void OnContinuousViewUpdateChange() = 0;
  virtual void OnSliceSliderChange(int id) = 0;
  virtual void OnInOutSnakeSelect() = 0;
  virtual void OnEdgeSnakeSelect() = 0;
  virtual void OnAcceptPreprocessingAction() = 0;
  virtual void OnAcceptInitializationAction() = 0;
  virtual void OnRestartInitializationAction() = 0;
  virtual void OnRestartPreprocessingAction() = 0;
  virtual void OnSnakeRewindAction() = 0;
  virtual void OnSnakeStopAction() = 0;
  virtual void OnSnakePlayAction() = 0;
  virtual void OnSnakeStepAction() = 0;
  virtual void OnSnakeStepSizeChange() = 0;
  virtual Bubble* getBubbles() = 0;
  virtual int getNumberOfBubbles() = 0;

  // Zoom/pan interaction callbacks
  virtual void OnResetView2DAction(unsigned int window) = 0;
  virtual void OnResetAllViews2DAction() = 0;
  virtual void OnLinkedZoomChange() = 0;
  virtual void OnZoomPercentageChange() = 0;

  // Label IO callbacks
  virtual void OnLoadLabelsBrowseAction() = 0;
  virtual void OnLoadLabelsCancelAction() = 0;
  virtual void OnLoadLabelsOkAction() = 0;

  // virtual void LoadPreprocSelectCallback() = 0;
  // virtual void SelectPreprocFileCallback() = 0;
  // virtual void LoadPreprocessedDataCallback() = 0;
  // virtual void SavePreprocessedData_Callback() = 0;
  // virtual void OkaySavePreproc_button_Callback() = 0;
  virtual void OnSNAPViewOriginalSelect() = 0;
  virtual void OnViewPreprocessedSelect() = 0;
  
  // Method called when user tries to close the window
  virtual void OnMainWindowCloseAction() = 0;
  virtual void shutdown() = 0;

  virtual void TweakROI(Vector3i &pt1, Vector3i &pt2) = 0;
  virtual void init() = 0;
  virtual void ShowSNAP() = 0;
  // virtual int RunSnake() = 0;
  
  // Color label callbacks
  virtual void InitColorMap() = 0;
  virtual void UpdateColorChips() = 0;
  virtual void OnDrawingLabelUpdate() = 0;
  virtual void OnDrawOverLabelUpdate() = 0;
  
  virtual void RedrawWindows() = 0;
  virtual void ResetScrollbars() = 0;
  virtual void MakeSegTexturesCurrent() = 0;
  virtual void UpdateImageProbe() = 0;
  virtual void UpdateMainLabel() = 0;
  // virtual void SelectGreyFileCallback() = 0;
  // virtual void SelectSegFileCallback() = 0;
  // virtual void LoadSegFileCallback() = 0;
  virtual void UpdateEditLabelWindow() = 0;
  virtual void ChangeLabelsCallback() = 0;
  virtual void UpdatePositionDisplay(int id) = 0;
  
  // Polygon buttons callbacks
  virtual void OnAcceptPolygonAction(unsigned int window) = 0;
  virtual void OnDeletePolygonSelectedAction(unsigned int window) = 0;
  virtual void OnInsertIntoPolygonSelectedAction(unsigned int window) = 0;
  virtual void OnPastePolygonAction(unsigned int window) = 0;
  
  virtual int CheckOrient(const char *txt, Vector3i &RAI) = 0;
  virtual void Activate3DAccept(bool on) = 0;
  virtual void SaveLabelsCallback() = 0;
  // virtual void SaveSegFileCallback() = 0;
  virtual void PrintVoxelCountsCallback() = 0;

  // Menu item callbacks
  virtual void OnMenuLoadGrey() = 0;
  virtual void OnMenuLoadSegmentation() = 0;
  virtual void OnMenuLoadLabels() = 0;
  virtual void OnMenuSaveGreyROI() = 0;
  virtual void OnMenuSaveSegmentation() = 0;
  virtual void OnMenuSaveLabels() = 0;
  virtual void OnMenuIntensityCurve() = 0;  
  virtual void OnMenuLoadPreprocessed() = 0;  
  virtual void OnMenuSavePreprocessed() = 0; 
  virtual void OnLoadPreprocessedImageAction() = 0;

  // Display options callbacks
  virtual void OnMenuShowDisplayOptions() = 0;
  virtual void OnSliceAnatomyOptionsChange(unsigned int order) = 0;
  virtual void OnDisplayOptionsCancelAction() = 0;
  virtual void OnDisplayOptionsOkAction() = 0;
  virtual void OnDisplayOptionsApplyAction() = 0;
  
  // Opacity sliders  
  virtual void OnSNAPLabelOpacityChange() = 0;
  virtual void OnIRISLabelOpacityChange() = 0;

  // SNAP pipeline step buttons
  virtual void OnSNAPStepPreprocess() = 0;
  virtual void OnSNAPStepInitialize() = 0;
  virtual void OnSNAPStepSegment() = 0;

  // Help related callbacks
  virtual void OnLaunchTutorialAction() = 0;

  virtual void SetToolbarMode(ToolbarModeType mode) = 0;
  

protected:
    GlobalState *m_GlobalState;
};

#endif // __UserInterfaceBase__h_
