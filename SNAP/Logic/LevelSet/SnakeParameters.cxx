/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SnakeParameters.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "SnakeParameters.h"


SnakeParameters 
SnakeParameters
::GetDefaultEdgeParameters() 
{
  SnakeParameters p;

  p.m_TimeStep = 0.0f;
  p.m_Ground = 5.0;

  p.m_Type = USER;
  p.m_Clamp = true;

  p.m_PropagationWeight = -5.0;
  p.m_PropagationSpeedExponent = 1;

  p.m_CurvatureWeight = 1.0;
  p.m_CurvatureSpeedExponent = 0;

  p.m_LaplacianWeight = 0.0f;
  p.m_LaplacianSpeedExponent = 0;

  p.m_AdvectionWeight = 0;
  p.m_AdvectionSpeedExponent = 0;       

  return p;
}

SnakeParameters 
SnakeParameters
::GetDefaultInOutParameters() 
{
  SnakeParameters p;

  p.m_TimeStep = 0.0f;
  p.m_Ground = 5.0;

  p.m_Type = USER;
  p.m_Clamp = true;

  p.m_PropagationWeight = -5.0;
  p.m_PropagationSpeedExponent = 1;

  p.m_CurvatureWeight = 1.0;
  p.m_CurvatureSpeedExponent = -1;

  p.m_LaplacianWeight = 0.0f;
  p.m_LaplacianSpeedExponent = 0;

  p.m_AdvectionWeight = 0;
  p.m_AdvectionSpeedExponent = 0;       

  return p;
}

bool 
SnakeParameters
::operator == (const SnakeParameters &p) 
{
  return(
        m_TimeStep == p.m_TimeStep &&
        m_Ground == p.m_Ground &&
        m_Type == p.m_Type &&
        m_Clamp == p.m_Clamp &&
        m_PropagationWeight == p.m_PropagationWeight &&
        m_PropagationSpeedExponent == p.m_PropagationSpeedExponent &&
        m_CurvatureWeight == p.m_CurvatureWeight &&
        m_CurvatureSpeedExponent == p.m_CurvatureSpeedExponent &&
        m_LaplacianWeight == p.m_LaplacianWeight &&
        m_LaplacianSpeedExponent == p.m_LaplacianSpeedExponent &&
        m_AdvectionWeight == p.m_AdvectionWeight &&
        m_AdvectionSpeedExponent == p.m_AdvectionSpeedExponent);
}
