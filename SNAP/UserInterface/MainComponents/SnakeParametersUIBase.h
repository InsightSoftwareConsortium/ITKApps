/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SnakeParametersUIBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SnakeParametersUIBase_h_
#define __SnakeParametersUIBase_h_

class Fl_Valuator;
class Fl_Check_Button;

/**
 * \class SnakeParametersUIBase
 * \brief Base class for the parameter setting user interface.
 */
class SnakeParametersUIBase 
{
public:
  virtual void OnAdvectionExponentChange(Fl_Valuator *input) = 0;
  virtual void OnAdvectionWeightChange(Fl_Valuator *input) = 0;
  virtual void OnCurvatureExponentChange(Fl_Valuator *input) = 0;
  virtual void OnCurvatureWeightChange(Fl_Valuator *input) = 0;
  virtual void OnPropagationExponentChange(Fl_Valuator *input) = 0;
  virtual void OnPropagationWeightChange(Fl_Valuator *input) = 0;
  virtual void OnHelpAction() = 0 ;
  virtual void OnOkAction() = 0;
  virtual void OnCloseAction() = 0;
  virtual void OnSavePresetAction() = 0;
  virtual void OnSelectPresetAction() = 0;
  virtual void OnAdvancedEquationAction() = 0;  

  // Advanced page
  virtual void OnTimeStepAutoAction() = 0;
  virtual void OnTimeStepChange(Fl_Valuator *input) = 0;
  virtual void OnSmoothingWeightChange(Fl_Valuator *input) = 0;
  virtual void OnLegacyClampChange(Fl_Check_Button *input) = 0;
  virtual void OnLegacyGroundChange(Fl_Valuator *input) = 0;
  virtual void OnSolverChange() = 0;
};

#endif // __SnakeParametersUIBase_h_
