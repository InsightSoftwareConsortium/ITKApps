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

#include "IRISTypes.h"

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
    EDGE_SNAKE, IN_OUT_SNAKE
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

  // Define a comparison operator
  bool operator ==(const SnakeParameters &p);

  /** Time step in level snake propagation (TODO: compute automatically) */
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

  /** Type of equation (well known parameter sets) */
  irisGetMacro(Type,ConstraintsType);
  irisSetMacro(Type,ConstraintsType);

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

  ConstraintsType m_Type;
  bool m_Clamp;

  float m_PropagationWeight;
  int m_PropagationSpeedExponent;

  float m_CurvatureWeight;
  int m_CurvatureSpeedExponent;

  float m_LaplacianWeight;
  int m_LaplacianSpeedExponent;

  float m_AdvectionWeight;
  int m_AdvectionSpeedExponent;       
};

#endif // __SnakeParameters_h_
