/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPLevelSetDriver.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SNAPLevelSetDriver_h_
#define __SNAPLevelSetDriver_h_

#include "IRISTypes.h"
#include "SnakeParameters.h"
#include "SNAPLevelSetFunction.h"
#include "SNAPLevelSetStopAndGoFilter.h"

/**
 * \class SNAPLevelSetDriver
 * \brief A generic interface between the SNAP application and ITK level set
 * framework.
 *
 * This interface allows the SNAP code to exist independently of the way stop-and-go
 * level set evolution is implemented in ITK.  This gives the software a bit of 
 * modularity.  As far as SNAP cares, the public methods declared in this class are
 * the only ways to control level set evolution.
 */
class SNAPLevelSetDriver
{
public:
  /** Floating point image type used internally */
  typedef itk::Image<float,3> FloatImageType;
  typedef itk::Image<LabelType,3> BubbleImageType;

  /** Initialize the level set driver.  Note that the type of snake (in/out
    or edge) is determined entirely by the speed image and by the values
    of the parameters */
  void Initialize(FloatImageType *initialLevelSet,
                  FloatImageType *speed,
                  const SnakeParameters *parms);

  /** Set snake parameters */
  void SetSnakeParameters(const SnakeParameters *parms);
  
  /** Run snake for N iterations */
  void Run(int nIterations);

  /** Restart the snake */
  void Restart(FloatImageType *init);

  /** Get the current state of the snake (level set and narrow band) */
  FloatImageType *GetCurrentState();

  /** Clean up the snake's state */
  void CleanUp();
  
private:

  /** Type definition for the level set filter */
  typedef SNAPLevelSetStopAndGoFilter<
    FloatImageType,FloatImageType> LevelSetFilterType;

  /** Type definition for the level set function */
  typedef SNAPLevelSetFunction<FloatImageType> LevelSetFunctionType;
  
  /** Level set filter wrapped by this object */
  LevelSetFilterType::Pointer m_LevelSetFilter;

  /** Level set function used by the level set filter */
  LevelSetFunctionType::Pointer m_Phi;

  /** Assign the values of snake parameters to a snake function */
  void AssignParametersToPhi(const SnakeParameters *parms);
};

#endif // __SNAPLevelSetDriver_h_
