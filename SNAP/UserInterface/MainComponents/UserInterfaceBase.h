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
  virtual void OnSnakeParametersApplyAction() = 0;
  virtual void OnSnakeParametersAction() = 0;
  virtual void OnSnakeParametersSchlegelSelect() = 0;
  virtual void OnSnakeParametersSapiroSelect() = 0;
  virtual void OnSnakeParametersTurelloSelect() = 0;
  virtual void OnSnakeParametersUserDefinedSelect() = 0;
  virtual void OnSnakeParametersClampChange() = 0;
  virtual void OnAcceptSegmentationAction() = 0;
  virtual void OnCancelSegmentationAction() = 0;
  virtual void ResetSNAPScrollbars() = 0;
  virtual void UpdateSNAPPositionDisplay(int id) = 0;
  virtual void UpdateSNAPImageProbe() = 0;
  virtual void OnContinuousViewUpdateChange() = 0;
  virtual void OnIRISSliceSliderChange(int id) = 0;
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
  // virtual void LoadPreprocSelectCallback() = 0;
  // virtual void SelectPreprocFileCallback() = 0;
  // virtual void LoadPreprocessedDataCallback() = 0;
  // virtual void SavePreprocessedData_Callback() = 0;
  // virtual void OkaySavePreproc_button_Callback() = 0;
  virtual void OnSNAPViewOriginalSelect() = 0;
  virtual void OnViewPreprocessedSelect() = 0;
  virtual void shutdown() = 0;
  virtual void TweakROI(Vector3i &pt1, Vector3i &pt2) = 0;
  virtual void init() = 0;
  virtual void ShowSNAP() = 0;
  virtual int RunSnake(int numsteps) = 0;
  virtual void InitColorMap() = 0;
  virtual void RedrawWindows() = 0;
  virtual void ResetScrollbars() = 0;
  virtual void MakeSegTexturesCurrent() = 0;
  virtual void UpdateColorChips() = 0;
  virtual void UpdateImageProbe() = 0;
  virtual void UpdateMainLabel(char *greyImg, char* labelImg) = 0;
  // virtual void SelectGreyFileCallback() = 0;
  // virtual void SelectSegFileCallback() = 0;
  // virtual void LoadSegFileCallback() = 0;
  virtual void LoadLabelsCallback() = 0;
  virtual void UpdateEditLabelWindow() = 0;
  virtual void ChangeLabelsCallback() = 0;
  virtual void PositionSliderCallback(int id) = 0;
  virtual void UpdatePositionDisplay(int id) = 0;
  virtual void AcceptPolygonCallback(int id) = 0;
  virtual int CheckOrient(const char *txt, Vector3i &RAI) = 0;
  virtual void ActivatePaste(int id, bool on) = 0;
  virtual void ActivatePaste(bool on) = 0;
  virtual void ActivateAccept(int id, bool on) = 0;
  virtual void ActivateAccept(bool on) = 0;
  virtual void Activate3DAccept(bool on) = 0;
  virtual void SaveLabelsCallback() = 0;
  // virtual void SaveSegFileCallback() = 0;
  virtual void PrintVoxelCountsCallback() = 0;

  // Menu item callbacks
  virtual void OnMenuLoadGrey() = 0;
  virtual void OnMenuLoadSegmentation() = 0;
  virtual void OnMenuLoadLabels() = 0;
  virtual void OnMenuSaveSegmentation() = 0;
  virtual void OnMenuSaveLabels() = 0;
  virtual void OnMenuIntensityCurve() = 0;  
  virtual void OnMenuLoadPreprocessed() = 0;  
  virtual void OnMenuSavePreprocessed() = 0; 

  // Rendering and display options callbacks
  virtual void OnRenderOptionsChange() = 0;
  virtual void OnRenderOptionsCancel() = 0;
  virtual void OnRenderOptionsOk() = 0;
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
