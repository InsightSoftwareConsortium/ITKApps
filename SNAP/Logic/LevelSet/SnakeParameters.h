/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SnakeParameters.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SnakeParameters_h_
#define __SnakeParameters_h_

#include "SNAPCommon.h"

/**
 * \class SnakeParameters
 * \brief Parameters for the Level Set evolution.
 * Most of these parameters correspond to the terms in LevelSetFunction.
 *
 * \sa itk::LevelSetFunction
 */
class SnakeParameters  
{
public:
  enum ConstraintsType {
    SAPIRO, SCHLEGEL, TURELLO, USER
  };

  enum SnakeType {
    EDGE_SNAKE, REGION_SNAKE
  };

  enum SolverType {
    PARALLEL_SPARSE_FIELD_SOLVER, SPARSE_FIELD_SOLVER,
    NARROW_BAND_SOLVER, LEGACY_SOLVER, DENSE_SOLVER
  };


  /**
   * Initialize parameters with default values for snake extraction
   * in Edge images
   */
  static SnakeParameters GetDefaultEdgeParameters();

  /**
   * Initialize parameters with default values for snake extraction
   * in Inside/Outside images
   */
  static SnakeParameters GetDefaultInOutParameters();

  /**
   * Initialize parameters with default values for snake extraction
   * in Inside/Outside images
   */
  static SnakeParameters GetDefaultAllZeroParameters();

  // Define a comparison operator
  bool operator ==(const SnakeParameters &p) const;

  /** Whether we wish to automatically compute optimal time step 
   * in level snake propagation */
  irisGetMacro(AutomaticTimeStep,bool);
  irisSetMacro(AutomaticTimeStep,bool);

  /** Time step in level snake propagation.  This is is only used if the
   * automatic computation is off */
  irisGetMacro(TimeStep,float);
  irisSetMacro(TimeStep,float);

  /** Clamp-to-ground parameter.  Obsolete in ITK implementation, kept for
    backward compatibility and regression testing */
  irisGetMacro(Ground,float);
  irisSetMacro(Ground,float);

  /** Whether to clamp or not.  Obsolete in ITK implementation, kept for
    backward compatibility and regression testing */
  irisGetMacro(Clamp,bool);
  irisSetMacro(Clamp,bool);

  /** Which solver to use to run the equation */
  irisGetMacro(Solver,SolverType);
  irisSetMacro(Solver,SolverType);

  /** Type of equation (well known parameter sets) */
  irisGetMacro(SnakeType,SnakeType);
  irisSetMacro(SnakeType,SnakeType);

  irisGetMacro(PropagationWeight,float);
  irisSetMacro(PropagationWeight,float);
  
  irisGetMacro(PropagationSpeedExponent,int);
  irisSetMacro(PropagationSpeedExponent,int);

  irisGetMacro(CurvatureWeight,float);
  irisSetMacro(CurvatureWeight,float);

  irisGetMacro(CurvatureSpeedExponent,int);
  irisSetMacro(CurvatureSpeedExponent,int);

  irisGetMacro(LaplacianWeight,float);
  irisSetMacro(LaplacianWeight,float);

  irisGetMacro(LaplacianSpeedExponent,int);
  irisSetMacro(LaplacianSpeedExponent,int);

  irisGetMacro(AdvectionWeight,float);
  irisSetMacro(AdvectionWeight,float);

  irisGetMacro(AdvectionSpeedExponent,int);
  irisSetMacro(AdvectionSpeedExponent,int);

private:
  float m_TimeStep;
  float m_Ground;

  SnakeType m_SnakeType;
  bool m_Clamp;

  bool m_AutomaticTimeStep;

  float m_PropagationWeight;
  int m_PropagationSpeedExponent;

  float m_CurvatureWeight;
  int m_CurvatureSpeedExponent;

  float m_LaplacianWeight;
  int m_LaplacianSpeedExponent;

  float m_AdvectionWeight;
  int m_AdvectionSpeedExponent;   

  SolverType m_Solver;
};

#endif // __SnakeParameters_h_
