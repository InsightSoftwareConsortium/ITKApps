/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    LevelSetExtensionFilter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __LevelSetExtensionFilter_h_
#define __LevelSetExtensionFilter_h_

#include "itkFiniteDifferenceFunction.h" 
#include "itkFiniteDifferenceImageFilter.h" 
#include "itkMutexLock.h"
#include "itkConditionVariable.h"
#include "itkBarrier.h"
#include "itkCommand.h"

/** An interface that let's us have a common pointer to 
 * extensions of the FiniteDifferenceFilter objects defined
 * by LevelSetExtensionFilter */
class LevelSetExtensionFilterInterface 
{
public:
  virtual void SetIterationsUntilPause(unsigned int iterations) = 0;
  virtual void RequestStop() = 0;
  virtual void SetPauseCommand(itk::Command *callback) = 0;
  virtual bool IsUpdating() = 0;
};

/** A generic extension of a filter (intended to be a 
 * FiniteDifferenceImageFilter) that let's use control it
 * in a VCR (play-stop-step-rewind) fashion */
template <class TFilter>
class LevelSetExtensionFilter : public virtual LevelSetExtensionFilterInterface, 
  public TFilter  
{
public:
  
  /** Standard class typedefs. */
  typedef LevelSetExtensionFilter<TFilter> Self;
  typedef TFilter Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Run-time type information. */
  itkTypeMacro(LevelSetExtensionFilter,TFilter);

  /** Capture information from the superclass. */
  typedef typename Superclass::InputImageType   InputImageType;
  typedef typename Superclass::OutputImageType  OutputImageType;

  /** Dimensionality of input and output data is assumed to be the same.
   * It is inherited from the superclass. */
  itkStaticConstMacro(ImageDimension, unsigned int,Superclass::ImageDimension);

  /** ITK new macro */
  itkNewMacro(LevelSetExtensionFilter);

  /** The pixel type of the output image will be used in computations.
   * Inherited from the superclass. */
  typedef typename Superclass::PixelType PixelType;
  typedef typename Superclass::TimeStepType TimeStepType;

  /** Set a callback to make while pausing */
  irisSetMacro(PauseCommand,itk::Command *);
  
  /** Set the number of iterations that the filter will run until pausing */
  irisSetMacro(IterationsUntilPause,unsigned int);

  /** Request the filter to halt */
  void RequestStop() { m_StopRequested = true; }

  /** Are we updating */
  bool IsUpdating() 
    { 
    return m_Updating; 
    }

protected:
  LevelSetExtensionFilter() 
  {
    m_IterationsUntilPause = 0;
    m_StopRequested = false;
    m_Barrier = itk::Barrier::New();
    m_Condition = itk::ConditionVariable::New();
    m_SharedReturnCode = false;

    m_Barrier->Initialize(this->GetNumberOfThreads());
  }
  
  ~LevelSetExtensionFilter() {}
  
  /** Just a print method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  }

  /** Makes sure that only one thread calls the Halt method, and that the rest
   * return a shared result */
  virtual bool ThreadedHalt(void *threadInfo);

  /** Supplies the halting criteria for this class of filters.  The
   * algorithm will stop after a user-specified number of iterations. */
  virtual bool Halt();

private:
  LevelSetExtensionFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  unsigned int m_IterationsUntilPause;
  bool m_StopRequested;
  
  itk::Command::Pointer m_PauseCommand;
  itk::SimpleMutexLock m_Mutex;
  itk::ConditionVariable::Pointer m_Condition;
  itk::Barrier::Pointer m_Barrier;
  
  bool m_SharedReturnCode;
};

template<class TFilter>
bool
LevelSetExtensionFilter<TFilter>
::ThreadedHalt(void *threadInfo)
{
  // Make sure that all the threads are onboard
  m_Barrier->Wait();

  // Get the thread ID from the thread info
  int threadID = ((itk::MultiThreader::ThreadInfoStruct *) threadInfo)->ThreadID;

  if(threadID == 0)
    {
    // If the thread ID is zero, call the regular Halt method
    m_SharedReturnCode = Halt();

    // Tell all the waiting threads that they can proceed
    m_Condition->Broadcast();
    }
  else
    {
    // Wait for thread zero to finish
    m_Condition->Wait(&m_Mutex);
    }


  // Make sure there is "No Thread Left Behind"
  m_Barrier->Wait();

  // All threads return the same return code.
  return m_SharedReturnCode;
}

template<class TFilter>
bool
LevelSetExtensionFilter<TFilter>
::Halt()
{
  // This is the first thread to reach this point
  bool firstTimeInLoop = true;
  
  while(m_IterationsUntilPause == 0 && !m_StopRequested)
    {
    if(m_PauseCommand) 
      {
      // Here we sit and call the pause command in a loop, until in the pause
      // command someone requests more iterations to run, or requests a stop
      // to occur.
      if(firstTimeInLoop) 
        {
        // Generate an iteration event, since some work has happened up to
        // this point
        m_PauseCommand->Execute(this,itk::IterationEvent());

        // Clear the flag
        firstTimeInLoop = false;
        }
      else
        {
        // Generate a dummy event, since no more changes occurred
        m_PauseCommand->Execute(this,itk::NoEvent());
        }
      }      
    else 
      {
      // Having a NULL pause callback is equivalent to asking the update to
      // stop once the iteration limit has been reached.  This makes sense, 
      // because with no callback to call, we would put the program in an 
      // infinite loop!
      m_StopRequested = true;
      }        
    }

  // Correct the requested region in the output image
  this->GetOutput()->SetRequestedRegionToLargestPossibleRegion();

  // If the stop was requested, stop (let update complete itself)
  if(m_StopRequested) 
    {
    m_StopRequested = false;
    return true;
    }

  // Otherwise, reduce the number of iterations until we pause
  m_IterationsUntilPause--;

  // Return the common return code
  return false;
}

#endif // __LevelSetExtensionFilter_h_

