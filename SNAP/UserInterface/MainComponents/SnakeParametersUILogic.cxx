/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SnakeParametersUILogic.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "SnakeParametersUILogic.h"

#include "GlobalState.h"
#include "IRISApplication.h"
#include "IRISImageData.h"
#include "SNAPImageData.h"
#include "ThresholdSettings.h"
#include "SystemInterface.h"
#include "UserInterfaceLogic.h"
#include "SnakeParametersPreviewPipeline.h"

#include "itkImage.h"
#include "itkEventObject.h" 

#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"
#include "itkPNGImageIO.h"
#include "itkRGBPixel.h"

using namespace itk;

void 
SnakeParametersUILogic
::OnAdvectionExponentChange(Fl_Valuator *input)
{
  float clamped = input->clamp(input->value());
  m_Parameters.SetAdvectionSpeedExponent(clamped);
  this->OnParameterUpdate();
}

void 
SnakeParametersUILogic
::OnAdvectionWeightChange(Fl_Valuator *input)
{
  m_Parameters.SetAdvectionWeight(input->value());
  this->OnParameterUpdate();
}

void 
SnakeParametersUILogic
::OnCurvatureExponentChange(Fl_Valuator *input)
{
  float clamped = input->clamp(input->value());
  m_Parameters.SetCurvatureSpeedExponent(clamped);
  this->OnParameterUpdate();
}

void 
SnakeParametersUILogic
::OnCurvatureWeightChange(Fl_Valuator *input)
{
  m_Parameters.SetCurvatureWeight(input->value());
  this->OnParameterUpdate();
}

void 
SnakeParametersUILogic
::OnPropagationExponentChange(Fl_Valuator *input)
{
  float clamped = input->clamp(input->value());
  m_Parameters.SetPropagationSpeedExponent(clamped);
  this->OnParameterUpdate();
}

void 
SnakeParametersUILogic
::OnPropagationWeightChange(Fl_Valuator *input)
{
  m_Parameters.SetPropagationWeight(input->value());
  this->OnParameterUpdate();
}

void 
SnakeParametersUILogic
::OnTimeStepChange(Fl_Valuator *input)
{
  m_Parameters.SetTimeStep(input->value());
  this->OnParameterUpdate();
}

void 
SnakeParametersUILogic
::OnSmoothingWeightChange(Fl_Valuator *input)
{
  m_Parameters.SetLaplacianWeight(input->value());
  this->OnParameterUpdate();
}

void 
SnakeParametersUILogic
::OnLegacyClampChange(Fl_Check_Button *input)
{
  m_Parameters.SetClamp(input->value() > 0);
  this->OnParameterUpdate();
}

void 
SnakeParametersUILogic
::OnLegacyGroundChange(Fl_Valuator *input)
{ 
  m_Parameters.SetGround(input->value());
  this->OnParameterUpdate();
}

void 
SnakeParametersUILogic
::OnSolverChange()
{
  if(m_WarnOnSolverUpdate) 
    {
    int rc = 
      fl_choice("Changing the algorithm while level set evolution is running\n"
                "will cause the results to be lost!  Are you sure?",
                "Yes, change it","No",NULL);
    if(rc == 1) 
      {
      this->OnParameterUpdate();
      return;
      }      
    }

  SnakeParameters::SolverType solver;
  switch(m_InSolver->value()) 
    {
    case 0: solver = SnakeParameters::PARALLEL_SPARSE_FIELD_SOLVER; break;
    case 1: solver = SnakeParameters::NARROW_BAND_SOLVER; break;
    case 2: solver = SnakeParameters::DENSE_SOLVER; break;
    case 3: solver = SnakeParameters::LEGACY_SOLVER; break;
    default: solver = SnakeParameters::PARALLEL_SPARSE_FIELD_SOLVER;
    }

  if(solver != SnakeParameters::LEGACY_SOLVER)
    m_Parameters.SetAutomaticTimeStep(true);

  m_Parameters.SetSolver(solver);
  this->OnParameterUpdate();
}

void
SnakeParametersUILogic
::OnTimeStepAutoAction()
{
  m_Parameters.SetAutomaticTimeStep(m_BtnTimeStepAuto[0]->value() == 1);
  this->OnParameterUpdate();
}

void 
SnakeParametersUILogic
::OnAdvancedEquationAction()
{
  // Just call the parameters update method
  this->OnParameterUpdate();
}

void SnakeParametersUILogic
::OnParameterUpdate()
{
  // Propagate the values of the parameters to all the controls in 
  // this user interface

  // Activate or disactivate widgets based on the snake type
  if(m_Parameters.GetSnakeType() == SnakeParameters::EDGE_SNAKE)
    {
    m_GrpAdvectionEasy->show();
    m_GrpAdvectionMath->show();

    m_InAdvectionWeightMathText->show();
    m_InAdvectionExponentMathText->show();
    m_InCurvatureExponentMathText->show();
    m_InPropagationExponentMathText->show();

    m_InAdvectionWeightMathSlider->show();
    m_InAdvectionExponentMathSlider->show();
    m_InCurvatureExponentMathSlider->show();
    m_InPropagationExponentMathSlider->show();

    m_InAdvectionWeightClassic->show();
    m_InAdvectionExponentClassic->show();
    m_InCurvatureExponentClassic->show();
    m_InPropagationExponentClassic->show();
    }
  else
    {
    m_GrpAdvectionEasy->hide();
    m_GrpAdvectionMath->hide();

    m_InAdvectionWeightMathText->hide();
    m_InAdvectionExponentMathText->hide();
    m_InCurvatureExponentMathText->hide();
    m_InPropagationExponentMathText->hide();

    m_InAdvectionWeightMathSlider->hide();
    m_InAdvectionExponentMathSlider->hide();
    m_InCurvatureExponentMathSlider->hide();
    m_InPropagationExponentMathSlider->hide();

    m_InAdvectionWeightClassic->hide();
    m_InAdvectionExponentClassic->hide();
    m_InCurvatureExponentClassic->hide();
    m_InPropagationExponentClassic->hide();
    }

  // Curvature weight
  float value = m_Parameters.GetCurvatureWeight();
  m_InCurvatureWeightMathText->value(value);
  m_InCurvatureWeightMathSlider->value(value);
  m_InCurvatureWeightClassic->value(value);
  m_InCurvatureWeightEasy->value(value);

  // Curvature exponent
  value = m_Parameters.GetCurvatureSpeedExponent();
  m_InCurvatureExponentMathText->value(value);
  m_InCurvatureExponentMathSlider->value(value);
  m_InCurvatureExponentClassic->value(value);
  //m_InCurvatureExponentEasy->value(value);
  
  // Propagation weight 
  value = m_Parameters.GetPropagationWeight();
  m_InPropagationWeightMathText->value(value);
  m_InPropagationWeightMathSlider->value(value);
  m_InPropagationWeightClassic->value(value);
  //m_InPropagationWeightEasy->value(value);
  
  // Propagation exponent 
  value = m_Parameters.GetPropagationSpeedExponent();
  m_InPropagationExponentMathText->value(value);
  m_InPropagationExponentMathSlider->value(value);
  m_InPropagationExponentClassic->value(value);
  //m_InPropagationExponentEasy->value(value);
  
  // Advection weight 
  value = m_Parameters.GetAdvectionWeight();
  m_InAdvectionWeightMathText->value(value);
  m_InAdvectionWeightMathSlider->value(value);
  m_InAdvectionWeightClassic->value(value);
  m_InAdvectionWeightEasy->value(value);
  
  // Advection exponent 
  value = m_Parameters.GetAdvectionSpeedExponent();
  m_InAdvectionExponentMathText->value(value);
  m_InAdvectionExponentMathSlider->value(value);
  m_InAdvectionExponentClassic->value(value);
  //m_InAdvectionExponentEasy->value(value);

  // Experimental / normal equation display
  if(m_BtnAdvancedEquation->value() > 0)
    {
    // Show the right equation
    m_WizEquation->value(m_GrpEquationExperimental);

    // Enable all the controls
    m_InAdvectionExponentMathText->show();
    m_InAdvectionExponentMathSlider->show();
    m_InCurvatureExponentMathText->show();
    m_InCurvatureExponentMathSlider->show();
    m_InPropagationExponentMathText->show();    
    m_InPropagationExponentMathSlider->show();    
    m_GrpAdvectionEasy->show();
    }
  else
    {
    m_InAdvectionExponentMathSlider->hide();
    m_InCurvatureExponentMathText->hide();
    m_InCurvatureExponentMathSlider->hide();
    m_InPropagationExponentMathText->hide();    
    m_InPropagationExponentMathSlider->hide();    
    
    if(m_Parameters.GetSnakeType() == SnakeParameters::EDGE_SNAKE)
      {
      // Show the right equation
      m_WizEquation->value(m_GrpEquationEdge);

      m_GrpAdvectionEasy->show();
      m_GrpAdvectionMath->show();
      }
    else
      {
      // Show the right equation
      m_WizEquation->value(m_GrpEquationRegion);

      m_GrpAdvectionEasy->hide();
      m_GrpAdvectionMath->hide();
      }
    }

  // Update the display in the 2D preview boxes
  if(m_Parameters.GetSnakeType() == SnakeParameters::EDGE_SNAKE)
    m_PreviewPipeline->SetSpeedImage(m_ExampleImage[0]);
  else
    m_PreviewPipeline->SetSpeedImage(m_ExampleImage[1]);
  m_PreviewPipeline->SetSnakeParameters(m_Parameters);
    
  for(unsigned int j=0;j<4;j++)
    m_BoxPreview[j]->redraw();

  // Advanced page : solver
  if(m_Parameters.GetSolver() == SnakeParameters::LEGACY_SOLVER) 
    {
    // Show the right solver
    m_InSolver->value(2);

    // Show the options page
    m_WizSolverOptions->value(m_GrpLegacySolverOptions);

    // Fill the option page controls
    m_ChkLegacyClamp->value(m_Parameters.GetClamp() ? 1 : 0);
    m_InLegacyGround->value(m_Parameters.GetGround());

    // Disable the automatic timestep selection
    m_Parameters.SetAutomaticTimeStep(false);
    m_BtnTimeStepAuto[0]->deactivate();
    }
  else
    {
    if(m_Parameters.GetSolver() == SnakeParameters::NARROW_BAND_SOLVER)
      {
      // Show the right solver
      m_InSolver->value(1);

      // Show the options page
      m_WizSolverOptions->value(m_GrpNarrowSolverOptions);
      }
    else if(m_Parameters.GetSolver() == SnakeParameters::DENSE_SOLVER)
      {
      // Show the right solver
      m_InSolver->value(2);

      // Show the options page
      m_WizSolverOptions->value(m_GrpNarrowSolverOptions);
      }
    else
      {
      // Show the right solver
      m_InSolver->value(0);

      // Show the options page
      m_WizSolverOptions->value(m_GrpSparseSolverOptions);
      }

    // Enable the automatic timestep selection
    m_BtnTimeStepAuto[0]->activate();
    }

  // Advanced page : time step
  if(m_Parameters.GetAutomaticTimeStep())
    {
    m_BtnTimeStepAuto[0]->setonly();
    m_InTimeStep->deactivate();
    m_Parameters.SetLaplacianWeight(0);
    m_InSmoothingWeight->deactivate();
    }
  else
    {
    m_BtnTimeStepAuto[1]->setonly();
    m_InTimeStep->activate();
    m_InSmoothingWeight->activate();
    }

  m_InTimeStep->value(m_Parameters.GetTimeStep());
  m_InSmoothingWeight->value(m_Parameters.GetLaplacianWeight());
}
 
void 
SnakeParametersUILogic
::OnHelpAction()
{

}

void SnakeParametersUILogic
::OnOkAction()
{
  m_UserAccepted = true;
  m_Window->hide();
}

void SnakeParametersUILogic
::OnCloseAction()
{
  m_UserAccepted = false;
  m_Window->hide();
}

void SnakeParametersUILogic
::OnSavePresetAction()
{

}

void SnakeParametersUILogic
::OnSelectPresetAction()
{

}

void SnakeParametersUILogic
::SetParameters(const SnakeParameters &parms)
{
  // Set the parameters
  m_Parameters = parms;

   // Update the user interface controls
  OnParameterUpdate();
}

void SnakeParametersUILogic
::Register(UserInterfaceLogic *parent)
{
  // Get the parent's system object
  SystemInterface *system = parent->GetSystemInterface();
  
  // Get the edge and region example image file names
  string fnImage[2];
  fnImage[0] = system->GetFileInRootDirectory("Images2D/EdgeForcesExample.png");
  fnImage[1] = system->GetFileInRootDirectory("Images2D/RegionForcesExample.png");

  // We are converting read data from RGB pixel image
  typedef itk::RGBPixel<unsigned char> RGBPixelType;
  typedef itk::Image<RGBPixelType,2> RGBImageType;

  // Typedefs
  typedef itk::ImageFileReader<RGBImageType> ReaderType;
  typedef itk::PNGImageIO IOType;
  typedef itk::ImageRegionIterator<RGBImageType> RGBIteratorType;
  typedef itk::ImageRegionIterator<ExampleImageType> IteratorType;

  // Scale and shift constants
  const float scale[2] = {1.0f / 255.0f, 2.0f / 255.0f };
  const float shift[2] = {0.0f, -1.0f };

  // Load each of these images
  for(unsigned int i = 0; i < 2; i++) 
    {
    try 
      {
      // Read the image in
      ReaderType::Pointer reader = ReaderType::New();
      IOType::Pointer io = IOType::New();
      reader->SetImageIO(io);
      reader->SetFileName(fnImage[i].c_str());
      reader->Update();

      // Allocate the example image
      m_ExampleImage[i] = ExampleImageType::New();
      m_ExampleImage[i]->SetRegions(reader->GetOutput()->GetBufferedRegion());
      m_ExampleImage[i]->Allocate();

      // Scale the image into the range (image is RGB 0..255)
      RGBIteratorType 
        itColor(reader->GetOutput(),reader->GetOutput()->GetBufferedRegion());
      IteratorType it(m_ExampleImage[i],m_ExampleImage[i]->GetBufferedRegion());

      for(itColor.GoToBegin();!itColor.IsAtEnd();++it,++itColor)
        {
        it.Value() = itColor.Value().GetLuminance() * scale[i] + shift[i];
        }
      }
    catch(...)
      {
      // An exception occurred.  
      fl_alert("Unable to load image %s\n"
               "Force illustration example will not be available.",fnImage[i].c_str());

      // Initialize an image to zeros
      m_ExampleImage[i] = NULL;
      }
    }

  // Load the points from the registry
  std::vector<Vector2d> points;
  string fnPreset = 
    system->GetFileInRootDirectory("Presets/SnakeParameterPreviewCurve.txt");
  try 
    {
    Registry regPoints(fnPreset.c_str());
    points = regPoints.Folder("Points").GetArray(Vector2d(0.0));
    }
  catch(...)
    {
    // An exception occurred.  
    fl_alert("Unable to load file %s\n"
             "Force illustration example will not be available.",
             fnPreset.c_str());
    }
  
  // Assign our previewer to the preview windows
  for(unsigned int i=0;i<4;i++)
    m_BoxPreview[i]->SetPipeline(m_PreviewPipeline);
  
  // If there are some points in there, draw them
  if(points.size() >= 4)
    {
    // Set spline points, etc
    m_PreviewPipeline->SetControlPoints(points);

    // Set which forces to display
    m_BoxPreview[0]->SetForceToDisplay(SnakeParametersPreviewBox::PROPAGATION_FORCE);
    m_BoxPreview[1]->SetForceToDisplay(SnakeParametersPreviewBox::CURVATURE_FORCE);
    m_BoxPreview[2]->SetForceToDisplay(SnakeParametersPreviewBox::ADVECTION_FORCE);
    m_BoxPreview[3]->SetForceToDisplay(SnakeParametersPreviewBox::TOTAL_FORCE);
    }

  // Don't warn by default
  m_WarnOnSolverUpdate = false;
}

void 
SnakeParametersUILogic
::DisplayWindow()
{
  // Show everything
  m_Window->show();
  for(unsigned int j=0;j<4;j++) 
    {
    m_BoxPreview[j]->show();
    }

  // Update parameters
  OnParameterUpdate();
}

SnakeParametersUILogic
::SnakeParametersUILogic()
  : SnakeParametersUI()
{
  // Create a preview pipeline 
  m_PreviewPipeline = new SnakeParametersPreviewPipeline();
}

SnakeParametersUILogic
::~SnakeParametersUILogic()
{
  delete m_PreviewPipeline;
}
