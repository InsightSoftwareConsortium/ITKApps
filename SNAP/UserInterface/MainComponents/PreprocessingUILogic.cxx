/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    PreprocessingUILogic.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "PreprocessingUILogic.h"

#include "GlobalState.h"
#include "EdgePreprocessingImageFilter.h"
#include "IRISApplication.h"
#include "IRISImageData.h"
#include "SmoothBinaryThresholdImageFilter.h"
#include "SNAPImageData.h"
#include "ThresholdSettings.h"
#include "UserInterfaceLogic.h"

#include "itkImage.h"
#include "itkEventObject.h"

using namespace itk;

void 
PreprocessingUILogic
::Register(UserInterfaceLogic *parent)
{
  m_ParentUI = parent;
  m_Driver = parent->GetDriver();
  m_GlobalState = parent->GetDriver()->GetGlobalState();
  m_ShowProgress = false;
}

void 
PreprocessingUILogic
::DisplayEdgeWindow()
{
  // Get the threshold parameters
  EdgePreprocessingSettings settings = 
    m_GlobalState->GetEdgePreprocessingSettings();

  // Set the slider values
  m_InEdgeScale->value(
    m_InEdgeScale->clamp(settings.GetGaussianBlurScale()));

  m_InEdgeKappa->value(
    m_InEdgeScale->clamp(settings.GetRemappingSteepness()));

  m_InEdgeExponent->value(
    m_InEdgeScale->clamp(settings.GetRemappingExponent()));

  // Position the window and show it
  m_WinEdge->position(
    m_ParentUI->m_WinMain->x() + 200,
    m_ParentUI->m_WinMain->y() + 480);

  // Initialize the filter used for the preprocessing
  m_EdgeFilter = EdgeFilterType::New();
  m_EdgeFilter->SetInput(
    m_Driver->GetSNAPImageData()->GetGrey()->GetImage());

  // Set up the progress bar to update when the filter updated
  typedef SimpleMemberCommand<PreprocessingUILogic> CommandType;
  CommandType::Pointer callback = CommandType::New();
  callback->SetCallbackFunction(this,&PreprocessingUILogic::OnEdgeProgress);

  // Add an event listener to the edge filter
  m_EdgeFilter->AddObserver(ProgressEvent(),callback);

  // Get a handle to the snap image data
  SNAPImageData *snapData = m_Driver->GetSNAPImageData();

  // Initialize the speed image if necessary
  if(!snapData->IsSpeedLoaded())
    snapData->InitializeSpeed();

  // Set the intensity mapping mode for the speed image
  snapData->GetSpeed()->SetModeToEdgeSnake();

  // Pipe the filter into the speed image
  snapData->GetSpeed()->SetImage(m_EdgeFilter->GetOutput());

  // Apply the automatic preview preference
  m_InEdgePreview->value(
    m_GlobalState->GetShowPreprocessedEdgePreview() ? 1 : 0);

  OnEdgePreviewChange();

  // Compute the plot to be displayed
  UpdateEdgePlot();

  // Set up the plot range, etc
  FunctionPlot2DSettings &plotSettings = 
    m_BoxEdgeFunctionPlot->GetPlotter().GetSettings();
  plotSettings.SetPlotRangeMin(Vector2f(0.0f));

  // Show the window
  m_WinEdge->show();

  // Explicitly show the plotting box  
  m_BoxEdgeFunctionPlot->show();
}

void 
PreprocessingUILogic
::DisplayInOutWindow(void)
{
  // Get the threshold parameters
  ThresholdSettings settings = m_GlobalState->GetThresholdSettings();

  // Shorthands
  unsigned int lower = settings.GetLowerThreshold();
  unsigned int upper = settings.GetUpperThreshold();

  // Make sure that the specified range is valid
  assert(lower <= upper);

  // Set the ranges for the two thresholds.  These ranges do not require the
  // lower slider to be less than the upper slider, that will be corrected
  // dynamically as the user moves the sliders
  GreyType iMin = m_Driver->GetCurrentImageData()->GetGrey()->GetImageMin();
  GreyType iMax = m_Driver->GetCurrentImageData()->GetGrey()->GetImageMax();

  m_InLowerThreshold->minimum(iMin);
  m_InLowerThreshold->maximum(iMax);

  m_InUpperThreshold->minimum(iMin);
  m_InUpperThreshold->maximum(iMax);

  m_InThresholdSteepness->minimum(1);
  m_InThresholdSteepness->maximum(iMax-iMin);

  // Make sure the current values of the upper and lower threshold are 
  // within the bounds (Nathan Moon)
  m_InLowerThreshold->value(m_InLowerThreshold->clamp(lower));
  m_InUpperThreshold->value(m_InUpperThreshold->clamp(upper));
  m_InThresholdSteepness->value(
    m_InThresholdSteepness->clamp(settings.GetSmoothness()));

  // Set the radio buttons
  if(settings.IsLowerThresholdEnabled() && 
    settings.IsUpperThresholdEnabled())
    {
    m_RadioThresholdBoth->setonly();
    }
  else if(settings.IsLowerThresholdEnabled())
    {
    m_RadioThresholdBelow->setonly();
    }
  else
    {
    m_RadioThresholdAbove->setonly();
    }

  // Position the window and show it
  m_WinInOut->position(
    m_ParentUI->m_WinMain->x() + 200,
    m_ParentUI->m_WinMain->y() + 480);

  // Get a handle to the snap image data
  SNAPImageData *snapData = m_Driver->GetSNAPImageData();

  // Initialize the filter used for the preprocessing
  m_InOutFilter = InOutFilterType::New();
  m_InOutFilter->SetThresholdSettings(settings);
  m_InOutFilter->SetInput(snapData->GetGrey()->GetImage());

  // Set up the progress bar to update when the filter updated
  typedef SimpleMemberCommand<PreprocessingUILogic> CommandType;
  CommandType::Pointer callback = CommandType::New();
  callback->SetCallbackFunction(this,&PreprocessingUILogic::OnThresholdProgress);

  // Add an event listener to the edge filter
  m_InOutFilter->AddObserver(ProgressEvent(),callback);

  // Initialize the speed image if necessary
  if(!snapData->IsSpeedLoaded())
    snapData->InitializeSpeed();

  // Set the speed image to In/Out mode
  snapData->GetSpeed()->SetModeToInsideOutsideSnake();

  // Pipe the filter into the speed image
  snapData->GetSpeed()->SetImage(m_InOutFilter->GetOutput());

  // Apply the automatic preview preference
  m_InThresholdPreview->value(
    m_GlobalState->GetShowPreprocessedInOutPreview() ? 1 : 0);
  OnThresholdPreviewChange();

  // Use a callback method to enable / disable sliders
  OnThresholdDirectionChange();

  // Apply the overlay settings (this is a personal preference, but I like 
  // to disable the color overlay when the window comes up initially)
  m_InThresholdOverlay->value(0);
  OnThresholdOverlayChange();

  // Compute the plot to be displayed
  UpdateThresholdPlot();

  // Set up the plot range, etc
  FunctionPlot2DSettings &plotSettings = 
    m_BoxThresholdFunctionPlot->GetPlotter().GetSettings();
  plotSettings.SetPlotRangeMin(Vector2f(iMin,-1.0f));
  plotSettings.SetPlotRangeMax(Vector2f(iMax,1.0f));

  // Show the window
  m_WinInOut->show();

  // Explicitly show the plotting box  
  m_BoxThresholdFunctionPlot->show();
}

void 
PreprocessingUILogic
::OnThresholdDirectionChange()
{
  // Enable and disable the state of the sliders based on the
  // current button settings
  if(m_RadioThresholdBoth->value())
    {
    m_InLowerThreshold->activate();
    m_InUpperThreshold->activate();
    }
  else if(m_RadioThresholdAbove->value())
    {
    m_InLowerThreshold->deactivate();
    m_InUpperThreshold->activate();
    }
  else
    {
    m_InLowerThreshold->activate();
    m_InUpperThreshold->deactivate();
    }

  // The settings have changed, so call that method
  OnThresholdSettingsChange();
}

void 
PreprocessingUILogic
::OnThresholdLowerChange()
{
  // There may be a need to shift the upper bound
  if(m_InUpperThreshold->value() < m_InLowerThreshold->value())
    {
    m_InUpperThreshold->value(m_InLowerThreshold->value());
    }

  // Call the generic callback
  OnThresholdSettingsChange();
}

void 
PreprocessingUILogic
::OnThresholdUpperChange()
{
  // There may be a need to shift the lower bound
  if( m_InUpperThreshold->value() < m_InLowerThreshold->value())
    {
    m_InLowerThreshold->value(m_InUpperThreshold->value());
    }

  // Call the generic callback
  OnThresholdSettingsChange();
}

void 
PreprocessingUILogic
::OnEdgeSettingsChange()
{
  // Pass the current GUI settings to the filter
  EdgePreprocessingSettings settings;
  settings.SetGaussianBlurScale(m_InEdgeScale->value());
  settings.SetRemappingSteepness(m_InEdgeKappa->value());
  settings.SetRemappingExponent(m_InEdgeExponent->value());

  // Store the settings globally
  m_GlobalState->SetEdgePreprocessingSettings(settings);

  // Apply the settings to the filter if in preview mode
  if(m_GlobalState->GetShowPreprocessedEdgePreview())
    {
    m_EdgeFilter->SetEdgePreprocessingSettings(settings);
    }

  // Update the plotter
  UpdateEdgePlot();

  // Repaint the slice windows
  m_ParentUI->RedrawWindows();
}

void 
PreprocessingUILogic
::OnThresholdSettingsChange()
{
  // Pass the current GUI settings to the filter
  ThresholdSettings settings;
  settings.SetLowerThreshold((unsigned int)m_InLowerThreshold->value());
  settings.SetUpperThreshold((unsigned int)m_InUpperThreshold->value());
  settings.SetSmoothness((unsigned int)m_InThresholdSteepness->value());
  settings.SetLowerThresholdEnabled(m_InLowerThreshold->active());
  settings.SetUpperThresholdEnabled(m_InUpperThreshold->active());  

  // Store the settings globally
  m_GlobalState->SetThresholdSettings(settings);

  // Apply the settings to the filter but only if we are in preview mode
  if(m_GlobalState->GetShowPreprocessedInOutPreview())
    {    
    m_InOutFilter->SetThresholdSettings(settings);

    // Repaint the slice windows
    m_ParentUI->RedrawWindows();
    }  

  // Compute the plot to be displayed
  UpdateThresholdPlot();
}

void 
PreprocessingUILogic
::OnEdgePreviewChange(void)
{  
  bool preview = (m_InEdgePreview->value() == 1);

  // Store the value of the flag globally
  m_GlobalState->SetShowPreprocessedEdgePreview(preview);

  // Entering preview mode means that we can draw the speed image
  m_GlobalState->SetShowSpeed(preview);

  // Repaint the slice windows
  m_ParentUI->RedrawWindows();
}

void 
PreprocessingUILogic
::OnThresholdPreviewChange(void)
{
  bool preview = (m_InThresholdPreview->value() == 1);

  // Store the value of the flag globally
  m_GlobalState->SetShowPreprocessedInOutPreview(preview);

  // Entering preview mode means that we can draw the speed image
  m_GlobalState->SetShowSpeed(preview);

  // Repaint the slice windows
  m_ParentUI->RedrawWindows();
}

void 
PreprocessingUILogic
::OnEdgeOk()
{
  // Apply the preprocessing on the whole image
  OnEdgeApply();

  // Run the same code as when the window is closed
  OnEdgeClose();
}

void 
PreprocessingUILogic
::OnThresholdOk(void)
{
  // Apply the preprocessing on the whole image
  OnThresholdApply();

  // Run the same code as when the window is closed
  OnThresholdClose();
}

void 
PreprocessingUILogic
::OnEdgeApply()
{
  // Get a handle to the speed image wrapper
  SpeedImageWrapper *speed = m_Driver->GetSNAPImageData()->GetSpeed();

  // Pass the settings object to the filter (redundant in preview mode)
  m_EdgeFilter->SetEdgePreprocessingSettings(
    m_GlobalState->GetEdgePreprocessingSettings());

  // Enable progress bar 
  m_ShowProgress = true;

  // Update the filter using the largest possible region
  m_EdgeFilter->UpdateLargestPossibleRegion();

  // Disable progress bar 
  m_ShowProgress = false;

  // The preprocessing image is valid
  m_GlobalState->SetSpeedValid(true);

  // Activate some buttons
  // TODO: Fix direct access
  m_ParentUI->m_GrpImageOptions->activate();

  // update the view with the new preprocessed data
  m_ParentUI->m_RadioSNAPViewPreprocessed->setonly();
  m_ParentUI->OnViewPreprocessedSelect();

  // Allow preprocessing to be saved
  m_ParentUI->m_MenuSavePreprocessed->activate();

  // Repaint the slice windows
  m_ParentUI->RedrawWindows();
}

void 
PreprocessingUILogic
::OnThresholdApply()
{
  // Get a handle to the speed image wrapper
  SpeedImageWrapper *speed = m_Driver->GetSNAPImageData()->GetSpeed();

  // Pass the current settings to the filter (redundant in preview mode)
  m_InOutFilter->SetThresholdSettings(m_GlobalState->GetThresholdSettings());

  // Enable progress bar 
  m_ShowProgress = true;

  // Update the filter using the largest possible region
  m_InOutFilter->UpdateLargestPossibleRegion();

  // Disable progress bar 
  m_ShowProgress = false;

  // The preprocessing image is valid
  m_GlobalState->SetSpeedValid(true);

  // Activate some buttons
  // TODO: Fix direct access
  m_ParentUI->m_GrpImageOptions->activate();

  // update the view with the new preprocessed data
  m_ParentUI->m_RadioSNAPViewPreprocessed->setonly();
  m_ParentUI->OnViewPreprocessedSelect();

  // Allow preprocessing to be saved
  m_ParentUI->m_MenuSavePreprocessed->activate();
}

void 
PreprocessingUILogic
::OnThresholdOverlayChange(void)
{  
  if(m_InThresholdOverlay->value())
    {
    // These five lines of code pass the current drawing color to the overlay
    // system in the SpeedImageWrapper
    unsigned char rgbaOverlay[4];
    unsigned int label = m_GlobalState->GetDrawingColorLabel();
    m_Driver->GetSNAPImageData()->GetColorLabel(label).GetRGBAVector(rgbaOverlay);
    SpeedImageWrapper::OverlayPixelType colorOverlay(rgbaOverlay);
    m_Driver->GetSNAPImageData()->GetSpeed()->SetOverlayColor(colorOverlay);

    // Set the cutoff threshold to zero, i.e., positive speed values will be 
    // painted 
    m_Driver->GetSNAPImageData()->GetSpeed()->SetOverlayCutoff(0);

    // Set the appropriate flag
    m_GlobalState->SetSpeedViewZero(true);

    // Repaint
    m_ParentUI->RedrawWindows();
    }
  else 
    {
    // Clear the appropriate flag
    m_GlobalState->SetSpeedViewZero(false);
    }

  // Repaint the slice windows
  m_ParentUI->RedrawWindows();
}

void 
PreprocessingUILogic
::DisconnectSpeedWrapper()
{
  // Take a handle to the speed image
  SpeedImageWrapper::ImagePointer image = 
    m_Driver->GetSNAPImageData()->GetSpeed()->GetImage();

  // Disconnnect the speed image from the pipeline
  image->DisconnectPipeline();

  // Make sure that the image has the correct requested region
  image->SetRequestedRegionToLargestPossibleRegion();
}

void 
PreprocessingUILogic
::OnEdgeClose()
{
  // Make sure we are no longer looking in grey/speed overlay mode
  m_GlobalState->SetSpeedViewZero(false);

  // Detach the speed wrapper from the filter
  DisconnectSpeedWrapper();

  // Destroy the filter
  m_EdgeFilter = NULL;

  // Close the window
  m_WinEdge->hide();

  // Notify that we have been closed
  m_ParentUI->OnPreprocessClose();
}

void 
PreprocessingUILogic
::OnThresholdClose(void)
{
  // Make sure we are no longer looking in grey/speed overlay mode
  m_GlobalState->SetSpeedViewZero(false);

  // Detach the speed wrapper from the filter
  DisconnectSpeedWrapper();

  // Destroy the filter
  m_InOutFilter = NULL;

  // Close the window
  m_WinInOut->hide();

  // Notify that we have been closed
  m_ParentUI->OnPreprocessClose();
}

void 
PreprocessingUILogic
::HidePreprocessingWindows()
{
  m_WinInOut->hide();
  m_WinEdge->hide();
}

void 
PreprocessingUILogic
::UpdateEdgePlot()
{
  // Create a functor object used in the filter
  EdgeFilterType::FunctorType functor;

  // Get the global settings
  EdgePreprocessingSettings settings = 
    m_GlobalState->GetEdgePreprocessingSettings();

  // Pass the settings to the functor
  functor.SetParameters(0.0f,1.0f,
    settings.GetRemappingExponent(),
    settings.GetRemappingSteepness());

  // Compute the function for a range of values
  const unsigned int nSamples = 200;
  float x[nSamples];
  float y[nSamples];

  for(int i=0;i<nSamples;i++) 
    {
    x[i] = i * 1.0f / (nSamples-1);
    y[i] = functor(x[i]);
    }

  // Pass the results to the plotter
  m_BoxEdgeFunctionPlot->GetPlotter().SetDataPoints(x,y,nSamples);

  // Redraw the box
  m_BoxEdgeFunctionPlot->redraw();
}

void 
PreprocessingUILogic
::UpdateThresholdPlot()
{
  // Create a functor object used in the filter
  SmoothBinaryThresholdFunctor<float,float> functor;

  // Get the global settings
  ThresholdSettings settings = 
    m_GlobalState->GetThresholdSettings();

  // We need to know the min/max of the image
  float iMin = m_InLowerThreshold->minimum();
  float iMax = m_InUpperThreshold->maximum();

  // Pass the settings to the functor
  functor.SetParameters(iMin,iMax,settings);

  // Compute the function for a range of values
  const unsigned int nSamples = 200;
  float x[nSamples];
  float y[nSamples];

  for(int i=0;i<nSamples;i++) 
    {
    x[i] = iMin + i * (iMax-iMin) / (nSamples-1.0f);
    y[i] = functor(x[i]);
    }

  // Pass the results to the plotter
  m_BoxThresholdFunctionPlot->GetPlotter().SetDataPoints(x,y,nSamples);

  // Redraw the box
  m_BoxThresholdFunctionPlot->redraw();
}

void 
PreprocessingUILogic
::OnEdgeProgress()
{
  // What is the progress of the filter
  float progress = m_EdgeFilter->GetProgress();

  // Only update the bar if we are not in preview mode
  if(m_ShowProgress)
    {
    m_OutEdgeProgress->value(progress);
    Fl::check();
    }
}

void 
PreprocessingUILogic
::OnThresholdProgress()
{
  // What is the progress of the filter
  float progress = m_InOutFilter->GetProgress();

  // Only update the bar if we are not in preview mode
  if(m_ShowProgress)
    {
    m_OutThresholdProgress->value(progress);
    Fl::check();
    }
}

