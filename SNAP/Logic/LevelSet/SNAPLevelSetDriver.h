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

template <class TFilter> class LevelSetExtensionFilter;
class LevelSetExtensionFilterInterface;
 
namespace itk {
  template <class TInputImage, class TOutputImage> class ImageToImageFilter;
  template <class TOwner> class SimpleMemberCommand;
  template <class TOwner> class MemberCommand;
  class Command;
};
 
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
  typedef itk::SmartPointer<FloatImageType> FloatImagePointer;

  /** Type definition for the level set function */
  typedef SNAPLevelSetFunction<FloatImageType> LevelSetFunctionType;

  /** Initialize the level set driver.  Note that the type of snake (in/out
   * or edge) is determined entirely by the speed image and by the values
   * of the parameters.  Moreover, the type of solver used is specified in
   * the parameters as well */
  SNAPLevelSetDriver(FloatImageType *initialLevelSet,
                     FloatImageType *speed,
                     const SnakeParameters &parms);

  /** Virtual destructor */
  virtual ~SNAPLevelSetDriver() {}

  /** Set snake parameters */
  void SetSnakeParameters(const SnakeParameters &parms);

  /** Tell the update loop to execute.  Halts until the execution has ended */
  void BeginUpdate(itk::Command *pauseCallback);

  /** Request the update loop to end execution */
  void RequestEndUpdate();

  /** Check whether the driver is currently withing the update loop */
  bool IsInUpdateLoop();
  
  /** This method will run the evolution for N iterations.  Afterwards, it
   * will call the pauseCallback command repeatedly (unless the command is
   * NULL, in which case, it will return to caller).  The callback is provided
   * so that the filter may be updated in stages, with user interation.  You 
   * may call Run repeatedly from the callback or call Restart or CleanUp */
  void RequestIterations(int nIterations);

  /** Restart the snake */
  void RequestRestart();

  /** Get the level set function */
  irisGetMacro(LevelSetFunction,LevelSetFunctionType *);

  /** Get the current state of the snake (level set and narrow band) */
  FloatImageType *GetCurrentState();

  /** Clean up the snake's state */
  void CleanUp();
  
private:

  /** Type definition for the level set filter */
  typedef itk::ImageToImageFilter<FloatImageType,FloatImageType> FilterType;

  /** Level set filter wrapped by this object */
  FilterType::Pointer m_LevelSetFilter;

  /** A pointer to the filter, but veiwed as a level set extension interface */
  LevelSetExtensionFilterInterface *m_ExtensionView;

  /** Level set function used by the level set filter */
  LevelSetFunctionType::Pointer m_LevelSetFunction;

  /** An initialization image */
  FloatImagePointer m_InitializationImage;

  /** Last accepted snake parameters */
  SnakeParameters m_Parameters;

  /** Assign the values of snake parameters to a snake function */
  void AssignParametersToPhi(const SnakeParameters &parms, bool firstTime);

  // A callback type
  typedef itk::SmartPointer<itk::Command> CommandPointer;
  typedef itk::SimpleMemberCommand<SNAPLevelSetDriver> SelfCommandType;
  typedef itk::SmartPointer<SelfCommandType> SelfCommandPointer;

  /** Internal command pointer */
  SelfCommandPointer m_CommandAfterUpdate;

  /** Idle (pause) command pointer */
  CommandPointer m_CommandOnPause;

  /** Internal routines */
  void DoRestart();
  void DoCreateLevelSetFilter();
};

#endif // __SNAPLevelSetDriver_h_
