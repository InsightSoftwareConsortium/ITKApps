/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPLevelSetDriver.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "SNAPLevelSetDriver.h"
#include "IRISVectorTypesToITKConversion.h"

#include "itkCommand.h"
#include "itkNarrowBandLevelSetImageFilter.h"
#include "itkParallelSparseFieldLevelSetImageFilter.h"
#include "itkDenseFiniteDifferenceImageFilter.h"

using namespace itk;

class FastSNAPLevelSetFunction : 
  public SNAPLevelSetFunction<itk::Image<float, 3> >
{
public:
  
  /** Standard class typedefs. */
  typedef FastSNAPLevelSetFunction Self;
  typedef itk::Image<float,3> ImageType;
  typedef SNAPLevelSetFunction<ImageType> Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
                                                                                
  /** Method for creation through the object factory. */
  itkNewMacro(FastSNAPLevelSetFunction);
                                                                                
  /** Run-time type information (and related methods) */
  itkTypeMacro( SNAPLevelSetFunction, itk::LevelSetFunction );
                                                                                
  /** Extract some parameters from the superclass. */
  typedef Superclass::ImageType ImageType;
  typedef ImageType::Pointer ImagePointer;
  typedef Superclass::NeighborhoodType NeighborhoodType;
  typedef Superclass::ScalarValueType ScalarValueType;
  typedef Superclass::RadiusType RadiusType;
  typedef Superclass::FloatOffsetType FloatOffsetType;
  
  typedef Superclass::VectorType VectorType;
  typedef itk::Image<VectorType,ImageDimension> VectorImageType;
  typedef VectorImageType::Pointer VectorImagePointer;

  typedef Superclass::TimeStepType TimeStepType;
  typedef Superclass::GlobalDataStruct GlobalDataStruct;

  /** Our own initialize method: computes all offsets */
  virtual void SetSpeedImage(ImageType *speed);

  /** Our own compute update method: fast and furiuos */
  virtual float ComputeUpdate(const NeighborhoodType &it, void *globalData,
                              const FloatOffsetType& offset);
  
  /** Image offsets for fast neighborhood operations */
  int m_OffsetFX;
  int m_OffsetFY;
  int m_OffsetFZ;
  int m_OffsetBX;
  int m_OffsetBY;
  int m_OffsetBZ;
  int m_OffsetFXFY;
  int m_OffsetFYFZ;
  int m_OffsetFZFX;
  int m_OffsetBXFY;
  int m_OffsetBYFZ;
  int m_OffsetBZFX;
  int m_OffsetFXBY;
  int m_OffsetFYBZ;
  int m_OffsetFZBX;
  int m_OffsetBXBY;
  int m_OffsetBYBZ;
  int m_OffsetBZBX;
};

void 
FastSNAPLevelSetFunction
::SetSpeedImage(ImageType *speed)
{
  // Let the parent do his work
  Superclass::SetSpeedImage(speed);

  // Compute all the offsets
  m_OffsetFX = speed->GetOffsetTable()[0];
  m_OffsetFY = speed->GetOffsetTable()[1];
  m_OffsetFZ = speed->GetOffsetTable()[2];

  m_OffsetBX = -m_OffsetFX;
  m_OffsetBY = -m_OffsetFY;
  m_OffsetBZ = -m_OffsetFZ;

  m_OffsetFXFY = m_OffsetFX + m_OffsetFY;
  m_OffsetFYFZ = m_OffsetFY + m_OffsetFZ;
  m_OffsetFZFX = m_OffsetFZ + m_OffsetFX;

  m_OffsetBXFY = m_OffsetBX + m_OffsetFY;
  m_OffsetBYFZ = m_OffsetBY + m_OffsetFZ;
  m_OffsetBZFX = m_OffsetBZ + m_OffsetFX;

  m_OffsetFXBY = m_OffsetFX + m_OffsetBY;
  m_OffsetFYBZ = m_OffsetFY + m_OffsetBZ;
  m_OffsetFZBX = m_OffsetFZ + m_OffsetBX;

  m_OffsetBXBY = m_OffsetBX + m_OffsetBY;
  m_OffsetBYBZ = m_OffsetBY + m_OffsetBZ;
  m_OffsetBZBX = m_OffsetBZ + m_OffsetBX;
}

float 
FastSNAPLevelSetFunction
::ComputeUpdate(const NeighborhoodType &it, 
                void *globalData,const FloatOffsetType& offset)
{
  // Get the central pixel location
  const float *pixel = it.GetCenterPointer();

  // Global data structure
  GlobalDataStruct *gd = (GlobalDataStruct *)globalData;

  // Get the neighboring pixel values
  float v  = *(pixel);  
  float fx = *(pixel + m_OffsetFX);
  float bx = *(pixel + m_OffsetBX);
  float fy = *(pixel + m_OffsetFY);
  float by = *(pixel + m_OffsetBY);
  float fz = *(pixel + m_OffsetFZ);
  float bz = *(pixel + m_OffsetBZ);  

  // Compute the second derivatives
  float vv = v + v;
  float uxx = fx + bx - vv;
  float uyy = fy + by - vv;
  float uzz = fz + bz - vv;

  // Forward and backward differences
  float uxf = fx - v;
  float uyf = fy - v;
  float uzf = fz - v;
  float uxb = v - bx;
  float uyb = v - by;
  float uzb = v - bz;

  // Compute the central first derivatives
  float uxc = 0.5f * (fx - bx);
  float uyc = 0.5f * (fy - by);
  float uzc = 0.5f * (fz - bz);

  // Compute the squared central first derivatives 
  float uxc2 = uxc*uxc;
  float uyc2 = uyc*uyc;
  float uzc2 = uzc*uzc;

  // Compute the Hessian matrix and various other derivatives.  Some of these
  // derivatives may be used by overloaded virtual functions.
  gd->m_GradMagSqr = 1.0e-6 + uxc2 + uyc2 + uzc2;

  // Compute the curvature term
  float curvature_term = 0.0f;
  if(m_CurvatureWeight != 0.0f)
    {
    // More terms to compute
    float fxfy = *(pixel + m_OffsetFXFY);
    float fyfz = *(pixel + m_OffsetFYFZ);
    float fzfx = *(pixel + m_OffsetFZFX);
    
    float bxfy = *(pixel + m_OffsetBXFY);
    float byfz = *(pixel + m_OffsetBYFZ);
    float bzfx = *(pixel + m_OffsetBZFX);
    
    float fxby = *(pixel + m_OffsetFXBY);
    float fybz = *(pixel + m_OffsetFYBZ);
    float fzbx = *(pixel + m_OffsetFZBX);
                          
    float bxby = *(pixel + m_OffsetBXBY);
    float bybz = *(pixel + m_OffsetBYBZ);
    float bzbx = *(pixel + m_OffsetBZBX);

    // Compute the mixed derivatives
    float uxy = 0.25f * (fxfy + bxby - bxfy - fxby);
    float uyz = 0.25f * (fyfz + bybz - byfz - fybz);
    float uzx = 0.25f * (fzfx + bzbx - bzfx - fzbx);

    curvature_term = m_CurvatureWeight * 
      ((uxc2+uyc2)*uzz + (uyc2+uzc2)*uxx + (uzc2+uxc2)*uyy
       - 2*( uxy*uxc*uyc + uyz*uyc*uzc + uzx*uzc*uxc ))
       /  gd->m_GradMagSqr;

    curvature_term *= this->CurvatureSpeed(it, offset);
    }

  // Compute the advection term
  float advection_term = 0.0f;  
  if (m_AdvectionWeight != 0.0f)
    {    
    VectorType advection_field = this->AdvectionField(it, offset, gd);
    float ax = advection_field[0] * m_AdvectionWeight;
    float ay = advection_field[1] * m_AdvectionWeight;
    float az = advection_field[2] * m_AdvectionWeight;
    
    advection_term = 
      ((ax > 0) ? uxb * ax : uxf * ax) +
      ((ay > 0) ? uyb * ay : uyf * ay) + 
      ((az > 0) ? uzb * az : uzf * az);

    // Compute the maximal advection change
    float maxaxay = (ax > ay) ? ax : ay;
    float maxazam = (az > gd->m_MaxAdvectionChange) ? az : gd->m_MaxAdvectionChange;
    gd->m_MaxAdvectionChange = (maxaxay > maxazam) ? maxaxay : maxazam;
    }

  float propagation_term = 0.0f;
  if (m_PropagationWeight != 0.0f)
    {
    // Get the propagation speed
    propagation_term = m_PropagationWeight * this->PropagationSpeed(it, offset, gd);

    float propagation_gradient = (propagation_term > 0) ? 
      vnl_math_sqr( vnl_math_max(uxb, 0.0f) ) + vnl_math_sqr( vnl_math_min(uxf, 0.0f) ) + 
      vnl_math_sqr( vnl_math_max(uyb, 0.0f) ) + vnl_math_sqr( vnl_math_min(uyf, 0.0f) ) + 
      vnl_math_sqr( vnl_math_max(uzb, 0.0f) ) + vnl_math_sqr( vnl_math_min(uzf, 0.0f) ) 
      :      
      vnl_math_sqr( vnl_math_min(uxb, 0.0f) ) + vnl_math_sqr( vnl_math_max(uxf, 0.0f) ) + 
      vnl_math_sqr( vnl_math_min(uyb, 0.0f) ) + vnl_math_sqr( vnl_math_max(uyf, 0.0f) ) + 
      vnl_math_sqr( vnl_math_min(uzb, 0.0f) ) + vnl_math_sqr( vnl_math_max(uzf, 0.0f) );
    
    // Collect energy change from propagation term.  This will be used in
    // calculating the maximum time step that can be taken for this iteration.
    gd->m_MaxPropagationChange = 
      vnl_math_max(gd->m_MaxPropagationChange,vnl_math_abs(propagation_term));
    
    // Scale the propagation term by gradient magnitude
    propagation_term *= vcl_sqrt( propagation_gradient );
    }

  float laplacian_term = 0.0f;
  if(m_LaplacianSmoothingWeight != 0.0f)
    {
    laplacian_term = (uxx + uyy + uzz)
     * m_LaplacianSmoothingWeight * LaplacianSmoothingSpeed(it,offset, gd);
    }

  // Return the combination of all the terms.
  return curvature_term - propagation_term - advection_term - laplacian_term;
}

/** An interface that let's us have a common pointer to 
 * extensions of the FiniteDifferenceFilter objects defined
 * by LevelSetExtensionFilter */
class LevelSetExtensionFilterInterface 
{
public:
  virtual void SetIterationsUntilPause(unsigned int iterations) = 0;
  virtual void RequestStop() = 0;
  virtual void SetPauseCommand(Command *callback) = 0;
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
  irisSetMacro(PauseCommand,Command *);
  
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
  }
  
  ~LevelSetExtensionFilter() {}
  
  /** Just a print method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  }

  /** Supplies the halting criteria for this class of filters.  The
   * algorithm will stop after a user-specified number of iterations. */
  virtual bool Halt() 
  {
    bool firstTimeInLoop = true;
    while(m_IterationsUntilPause == 0 && !m_StopRequested)
      {
      if(m_PauseCommand) 
        // Here we sit and call the pause command in a loop, until in the pause
        // command someone requests more iterations to run, or requests a stop
        // to occur.
        if(firstTimeInLoop) 
          {
          // Generate an iteration event, since some work has happened up to
          // this point
          m_PauseCommand->Execute(this,IterationEvent());

          // Clear the flag
          firstTimeInLoop = false;
          }
        else
          {
          // Generate a dummy event, since no more changes occurred
          m_PauseCommand->Execute(this,NoEvent());
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
    return false;
  }

private:
  LevelSetExtensionFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  unsigned int m_IterationsUntilPause;
  bool m_StopRequested;
  Command::Pointer m_PauseCommand;
};

// Create an inverting functor
class InvertFunctor {
public:
  unsigned char operator()(unsigned char input) { 
    return input == 0 ? 1 : 0; 
  }  
};

SNAPLevelSetDriver
::SNAPLevelSetDriver(FloatImageType *init, FloatImageType *speed,
                     const SnakeParameters &sparms)
{
  // Create the level set function
  m_LevelSetFunction = LevelSetFunctionType::New();
  // m_LevelSetFunction = FastSNAPLevelSetFunction::New();

  // Pass the speed image to the function
  m_LevelSetFunction->SetSpeedImage(speed);  

  // Remember the input and output images for later initialization
  m_InitializationImage = init;

  // Pass the parameters to the level set function
  AssignParametersToPhi(sparms,true);

  // Create the filter
  DoCreateLevelSetFilter();
}

void 
SNAPLevelSetDriver
::AssignParametersToPhi(const SnakeParameters &p, bool irisNotUsed(firstTime))
{
  // Set up the level set function
  m_LevelSetFunction->SetAdvectionWeight(p.GetAdvectionWeight());
  m_LevelSetFunction->SetAdvectionSpeedExponent(p.GetAdvectionSpeedExponent());
  m_LevelSetFunction->SetCurvatureWeight(p.GetCurvatureWeight());
  m_LevelSetFunction->SetCurvatureSpeedExponent(p.GetCurvatureSpeedExponent()+1);  
  m_LevelSetFunction->SetPropagationWeight(p.GetPropagationWeight());
  m_LevelSetFunction->SetPropagationSpeedExponent(p.GetPropagationSpeedExponent());  
  m_LevelSetFunction->SetLaplacianSmoothingWeight(p.GetLaplacianWeight());
  m_LevelSetFunction->SetLaplacianSmoothingSpeedExponent(p.GetLaplacianSpeedExponent());  
  
  // We only need to recompute the internal images if the exponents to those
  // images have changed
  m_LevelSetFunction->CalculateInternalImages();
  
  // Call the initialize method
  m_LevelSetFunction->Initialize(to_itkSize(Vector3i(1)));

  // Set the time step
  m_LevelSetFunction->SetTimeStep(
    p.GetAutomaticTimeStep() ? 0.0 : p.GetTimeStep());

  // Remember the parameters
  m_Parameters = p;
}

void
SNAPLevelSetDriver
::DoCreateLevelSetFilter()
{
  // In this method we have the flexibility to create a level set filter
  // of any ITK solver type.  This way, we can plug in different solvers:
  // NarrowBand, ParallelSparseField, even Dense.  
  if(m_Parameters.GetSolver() == SnakeParameters::PARALLEL_SPARSE_FIELD_SOLVER)
    {
    // Define an extension to the appropriate filter class
    typedef ParallelSparseFieldLevelSetImageFilter<
      FloatImageType,FloatImageType> LevelSetFilterType;
    typedef LevelSetExtensionFilter<LevelSetFilterType> ExtensionFilterType;
      
    // Create a new extended filter
    ExtensionFilterType::Pointer filter = ExtensionFilterType::New();

    // Cast this specific filter down to the lowest common denominator that is
    // a filter
    m_LevelSetFilter = filter.GetPointer();

    // Cast the specific filter to a generic interface, so we can call the 
    // extended operations without knowing exactly what the filter is (this 
    // is the beauty of polymorphism!)
    m_ExtensionView = filter.GetPointer();
    
    // Perform the special configuration tasks on the filter
    filter->SetInput(m_InitializationImage);
    filter->SetNumberOfLayers(3);
    filter->SetIsoSurfaceValue(0.0f);
    filter->SetDifferenceFunction(m_LevelSetFunction);
    }
  else if(m_Parameters.GetSolver() == SnakeParameters::NARROW_BAND_SOLVER)
    {
    // Define an extension to the appropriate filter class
    typedef NarrowBandLevelSetImageFilter<
      FloatImageType,FloatImageType> LevelSetFilterType;
    typedef LevelSetExtensionFilter<LevelSetFilterType> ExtensionFilterType;
    
    // Create a new extended filter
    ExtensionFilterType::Pointer filter = ExtensionFilterType::New();

    // Cast this specific filter down to the lowest common denominator that is
    // a filter
    m_LevelSetFilter = filter.GetPointer();

    // Cast the specific filter to a generic interface, so we can call the 
    // extended operations without knowing exactly what the filter is (this 
    // is the beauty of polymorphism!)
    m_ExtensionView = filter.GetPointer();
    
    // Perform the special configuration tasks on the filter
    filter->SetSegmentationFunction(m_LevelSetFunction);
    filter->SetInput(m_InitializationImage);
    filter->SetNarrowBandTotalRadius(5);
    filter->SetNarrowBandInnerRadius(3);
    filter->SetFeatureImage(m_LevelSetFunction->GetSpeedImage());  
    }
  else if(m_Parameters.GetSolver() == SnakeParameters::DENSE_SOLVER)
    {
    // Define an extension to the appropriate filter class
    typedef DenseFiniteDifferenceImageFilter<
      FloatImageType,FloatImageType> LevelSetFilterType;
    typedef LevelSetExtensionFilter<LevelSetFilterType> ExtensionFilterType;
    
    // Create a new extended filter
    ExtensionFilterType::Pointer filter = ExtensionFilterType::New();

    // Cast this specific filter down to the lowest common denominator that is
    // a filter
    m_LevelSetFilter = filter.GetPointer();

    // Cast the specific filter to a generic interface, so we can call the 
    // extended operations without knowing exactly what the filter is (this 
    // is the beauty of polymorphism!)
    m_ExtensionView = filter.GetPointer();
    
    // Perform the special configuration tasks on the filter
    filter->SetInput(m_InitializationImage);
    filter->SetDifferenceFunction(m_LevelSetFunction);
    }
}

void
SNAPLevelSetDriver
::RequestRestart()
{ 
  // Makes no sense to call this if not in update cycle
  assert(this->IsInUpdateLoop());

  // Request a stop in the update
  m_ExtensionView->RequestStop();

  // Tell the method that called Update that it needs to call DoRestart()
  m_CommandAfterUpdate = SelfCommandType::New();
  m_CommandAfterUpdate->SetCallbackFunction(this,&SNAPLevelSetDriver::DoRestart);
}

void
SNAPLevelSetDriver
::DoRestart()
{
  // Update the image currently in the filter
  m_LevelSetFilter->SetInput(m_InitializationImage);

  // Reset the filter, so the next time it is Updated, it will run again, even
  // if the input image has not changed
  m_LevelSetFilter->Modified();
}

void 
SNAPLevelSetDriver
::BeginUpdate(Command *pauseCallback)
{
  // This call may not nest
  assert(!this->IsInUpdateLoop());

  // This call loops until the filter returns from an update with no consequent
  // request specified
  while(true)
    {
    // Tell the filter how many iterations to perform
    m_ExtensionView->SetIterationsUntilPause(0);

    // Tell the filter to call back to the command passed in here.  If the 
    // command is NULL, the filter will return after executing the iterations
    m_ExtensionView->SetPauseCommand(pauseCallback);

    // Clear the post-update command
    m_CommandAfterUpdate = NULL;
    
    // Run the filter (at this point, we're stuck in this method, and rely on
    // the callback function to handle user interaction)
    m_LevelSetFilter->UpdateLargestPossibleRegion();

    // The control returns here after the filter finishes updating.  
    if(m_CommandAfterUpdate)
      {          
      // The callback may have set the post-update command pointer, asking us to 
      // execute another action before returning control to the parent
      m_CommandAfterUpdate->Execute(m_LevelSetFilter,AnyEvent());
      }      
    else
      {
      // There was no subsequent command requested, hence there was no  
      // reason for aborting Update() other that the user wants us to quit and
      // return control
      break;
      }      
    }
}

bool
SNAPLevelSetDriver
::IsInUpdateLoop()
{
  return ((m_ExtensionView != NULL) && m_ExtensionView->IsUpdating());
}

void 
SNAPLevelSetDriver
::RequestEndUpdate()
{
  // Makes no sense to call this if not in update cycle
  assert(this->IsInUpdateLoop());

  // Tell the filter it has to stop
  m_ExtensionView->RequestStop();
}


void 
SNAPLevelSetDriver
::RequestIterations(int nIterations)
{
  // This method should only be called once the filter is updating, from the
  // pause callback
  assert(this->IsInUpdateLoop());
    
  // Since the filter is already running, so Run is being called from the 
  // pause callback.  In this case, we just tell the filter to run for more 
  // iterations once the pause callback returns
  m_ExtensionView->SetIterationsUntilPause(nIterations);
}

SNAPLevelSetDriver::FloatImageType * 
SNAPLevelSetDriver
::GetCurrentState()
{
  // Return the filter's output
  return m_LevelSetFilter->GetOutput();
}

void 
SNAPLevelSetDriver
::CleanUp()
{
  // This method should not be called within the pause callback, or else
  // we would trash memory
  assert(!this->IsInUpdateLoop());

  // Basically, the filter is finished, and we can finally return 
  // from running the filter.  Let's clear the level set and the 
  // function to free memory
  m_LevelSetFilter = NULL;
  m_LevelSetFunction = NULL;
}

void
SNAPLevelSetDriver
::SetSnakeParameters(const SnakeParameters &sparms)
{
  // Parameter setting can be destructive or passive.  If the solver has 
  // has changed, then it's destructive, otherwise it's passive
  bool destructive = sparms.GetSolver() != m_Parameters.GetSolver();

  // First of all, pass the parameters to the phi function, which may or
  // may not cause it to recompute it's images
  AssignParametersToPhi(sparms,false);

  if(destructive)
    {
    // We need to reinitialize the internal filter.  However, if the filter
    // is already running, all we can do is schedule that update
    if(this->IsInUpdateLoop())
      {
      // Tell the filter to stop when it regains control from the pause callback
      m_ExtensionView->RequestStop();

      // Schedule a subsequent call to create a new filter
      m_CommandAfterUpdate = SelfCommandType::New();
      m_CommandAfterUpdate->SetCallbackFunction(
        this,&SNAPLevelSetDriver::DoCreateLevelSetFilter);
      }
    else
      {
      // We are not in an update loop, but between updates, so just recreate the 
      // level set filter
      DoCreateLevelSetFilter();
      }
    }
}
