/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPImageData.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "itkGroupSpatialObject.h"
#include "itkEllipseSpatialObject.h"
#include "itkSpatialObjectToImageFilter.h"
#include "itkMaximumImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"

#include "GlobalState.h"
#include "EdgePreprocessingImageFilter.h"
#include "IRISVectorTypesToITKConversion.h"
#include "LabelImageWrapper.h"
#include "LevelSetImageWrapper.h"
#include "SignedDistanceFilter.h"
#include "SmoothBinaryThresholdImageFilter.h"
#include "SNAPImageData.h"
#include "SNAPLevelSetDriver.h"
#include "SpeedImageWrapper.h"

using namespace itk;


SNAPImageData
::SNAPImageData()
{
  m_SpeedWrapper = NULL;
  m_SnakeInitializationWrapper = NULL;
  m_SnakeWrapper = NULL;
  m_LevelSetDriver = NULL;
  m_SnakeColorLabel = 0;
}

SNAPImageData
::~SNAPImageData() 
{
  if(m_LevelSetDriver)
    delete m_LevelSetDriver;
  ClearSnake();
  ClearSnakeInitialization();
  ClearSpeed();
}

void 
SNAPImageData
::InitializeSpeed()
{
  // The Grey image wrapper should be present
  assert(m_GreyWrapper);

  // Remove the existing speed image
  ClearSpeed();

  // Intialize the speed based on the current grey image
  m_SpeedWrapper = new SpeedImageWrapper;
  m_SpeedWrapper->InitializeToImage(m_GreyWrapper->GetImage());

  // Propagate the geometry to the image wrappers
  for(unsigned int iSlice = 0;iSlice < 3;iSlice ++)
    {
    m_SpeedWrapper->SetImageToDisplayTransform(
      iSlice,m_ImageGeometry.GetImageToDisplayTransform(iSlice));
    }

  // Copy in the cursor position and transform info
  m_SpeedWrapper->SetSliceIndex(m_GreyWrapper->GetSliceIndex());  
}

void 
SNAPImageData
::DoEdgePreprocessing(const EdgePreprocessingSettings &settings,
                      itk::Command *progressCallback)
{ 
  // Make sure that the speed image exists (don't call initialize speed 
  // because we don't need to allocate the image
  if(!IsSpeedLoaded())
    m_SpeedWrapper = new SpeedImageWrapper;
    
  // Define an edge filter to use for preprocessing
  typedef EdgePreprocessingImageFilter<
    GreyImageWrapper::ImageType,SpeedImageWrapper::ImageType> FilterType;
  
  // Configure the edge filter
  FilterType::Pointer filter = FilterType::New();
  
  // Pass the settings to the filter
  filter->SetEdgePreprocessingSettings(settings);

  // Set the filter's input
  filter->SetInput(m_GreyWrapper->GetImage());

  // Provide a progress callback (if one is provided)
  if(progressCallback)
    filter->AddObserver(ProgressEvent(),progressCallback);

  // Run the filter on the whole image
  filter->UpdateLargestPossibleRegion();
  
  // Pass the output of the filter to the speed wrapper
  m_SpeedWrapper->SetImage(filter->GetOutput());
  
  // Dismantle this pipeline
  m_SpeedWrapper->GetImage()->DisconnectPipeline();
}

void 
SNAPImageData
::DoInOutPreprocessing(const ThresholdSettings &settings,
                       itk::Command *progressCallback)
{
  // Make sure that the speed image exists (don't call initialize speed 
  // because we don't need to allocate the image
  if(!IsSpeedLoaded())
    m_SpeedWrapper = new SpeedImageWrapper;
    
  // Define an edge filter to use for preprocessing
  typedef SmoothBinaryThresholdImageFilter<
    GreyImageWrapper::ImageType,SpeedImageWrapper::ImageType> FilterType;
  
  // Create an edge filter for whole-image preprocessing
  FilterType::Pointer filter = FilterType::New();
  
  // Pass the settings to the filter
  filter->SetThresholdSettings(settings);

  // Set the filter's input
  filter->SetInput(m_GreyWrapper->GetImage());

  // Provide a progress callback (if one is provided)
  if(progressCallback)
    filter->AddObserver(ProgressEvent(),progressCallback);

  // Run the filter
  filter->UpdateLargestPossibleRegion();
  
  // Pass the output of the filter to the speed wrapper
  m_SpeedWrapper->SetImage(filter->GetOutput());
  
  // Dismantle this pipeline
  m_SpeedWrapper->GetImage()->DisconnectPipeline();
}

SpeedImageWrapper* 
SNAPImageData
::GetSpeed() 
{
  // Make sure it exists
  assert(m_SpeedWrapper);

  return m_SpeedWrapper;
}

void 
SNAPImageData
::ClearSpeed() 
{
  if(m_SpeedWrapper)
    delete m_SpeedWrapper;
  m_SpeedWrapper = NULL;
}

bool 
SNAPImageData
::IsSpeedLoaded() 
{
  return (m_SpeedWrapper != NULL);
}

LevelSetImageWrapper* 
SNAPImageData
::GetSnakeInitialization() 
{
  assert(m_SnakeInitializationWrapper);
  return m_SnakeInitializationWrapper;
}

void 
SNAPImageData
::ClearSnakeInitialization() 
{
  if(m_SnakeInitializationWrapper)
    delete m_SnakeInitializationWrapper;
  m_SnakeInitializationWrapper = NULL;
}

bool 
SNAPImageData
::IsSnakeInitializationLoaded() 
{
  return (m_SnakeInitializationWrapper != NULL);
}

LevelSetImageWrapper* 
SNAPImageData
::GetSnake() 
{
  assert(m_SnakeWrapper);
  return m_SnakeWrapper;
}

void 
SNAPImageData
::ClearSnake() 
{
  if(m_SnakeWrapper)
    delete m_SnakeWrapper;
  m_SnakeWrapper = NULL;

}

bool 
SNAPImageData
::IsSnakeLoaded() 
{
  return (m_SnakeWrapper != NULL);
}

bool
SNAPImageData
::InitializeSegmentationPipeline(
  const SnakeParameters &parameters,Bubble *bubbles, 
  unsigned int nBubbles, unsigned int labelColor) 
{
  assert(m_GreyWrapper && m_LabelWrapper);
  
  // Store the label color
  m_SnakeColorLabel = labelColor;

  // Types of images used here
  typedef Image<LabelType,3> BubbleImageType;
  typedef Image<float,3> FloatImageType;

  // Make a copy of the segmentation image in case that it contains some
  // initialization data
  BubbleImageType::Pointer imgInput = this->GetSegmentation()->GetImage();
  BubbleImageType::Pointer imgBubbles = BubbleImageType::New();
  imgBubbles->SetRegions(imgInput->GetLargestPossibleRegion());
  imgBubbles->SetSpacing(imgInput->GetSpacing());
  imgBubbles->Allocate();
  
  // Create iterators to perform the copy
  typedef ImageRegionConstIterator<BubbleImageType> BubbleImageConstIterator;
  typedef ImageRegionIterator<BubbleImageType> BubbleImageIterator;
  
  BubbleImageConstIterator itInput(imgInput,
                                   imgInput->GetLargestPossibleRegion());
  
  BubbleImageIterator itBubbles(imgBubbles,
                                imgBubbles->GetLargestPossibleRegion());

  // During the copy loop, compute the extents of the initialization
  Vector3l bbLower(
    reinterpret_cast<const long *>(
      imgInput->GetLargestPossibleRegion().GetSize().GetSize()));
  Vector3l bbUpper(
    reinterpret_cast<const long *>(
      imgInput->GetLargestPossibleRegion().GetIndex().GetIndex()));

  unsigned long nInitVoxels = 0;

  // The copy loop
  while(!itBubbles.IsAtEnd())
    {
    LabelType pixSource = itInput.Value();
    if(pixSource == m_SnakeColorLabel)
      {
      // Expand the bounding box accordingly
      Vector3l point(itBubbles.GetIndex().GetIndex());
      bbLower = vector_min(bbLower,point);
      bbUpper = vector_max(bbUpper,point);
      
      // Increase the number of initialization voxels
      nInitVoxels++;
      }

    itBubbles.Value() = pixSource;
    ++itBubbles;
    ++itInput;
    }

  // Now we know whether there are initialization voxels and their range in
  // the image.  We can fill in the bubbles now  
  for(unsigned int iBubble=0; iBubble < nBubbles; iBubble++)
    {

    // Compute the extents of the bubble
    typedef Point<float,3> PointType;
    PointType ptLower,ptUpper,ptCenter;

    // Compute the physical position of the bubble center
    imgBubbles->TransformIndexToPhysicalPoint(
      to_itkIndex(bubbles[iBubble].center),
      ptCenter);

    for(unsigned int i=0;i<3;i++)
      {
      ptLower[i] = ptCenter[i] - bubbles[iBubble].radius;
      ptUpper[i] = ptCenter[i] + bubbles[iBubble].radius;
      }
    
    // Index locations corresponding to the extents of the bubble
    BubbleImageType::IndexType idxLower, idxUpper;     
    imgBubbles->TransformPhysicalPointToIndex(ptLower,idxLower);
    imgBubbles->TransformPhysicalPointToIndex(ptUpper,idxUpper);
    
    // Create a region
    BubbleImageType::SizeType szBubble;
    szBubble[0] = 1 + idxUpper[0] - idxLower[0];
    szBubble[1] = 1 + idxUpper[1] - idxLower[1];
    szBubble[2] = 1 + idxUpper[2] - idxLower[2];
    BubbleImageType::RegionType regBubble(idxLower,szBubble);
    regBubble.Crop(imgBubbles->GetLargestPossibleRegion());

    // Stretch the overall bounding box if necessary
    bbLower = vector_min(bbLower,Vector3l(idxLower.GetIndex()));
    bbUpper = vector_max(bbUpper,Vector3l(idxUpper.GetIndex()));

    // Create an iterator with an index to fill out the bubble
    ImageRegionIteratorWithIndex<BubbleImageType> 
      itThisBubble(imgBubbles,regBubble);

    // Need the squared radius for this
    float r2 = bubbles[iBubble].radius * bubbles[iBubble].radius;

    // Fill in the bubble
    while(!itThisBubble.IsAtEnd())
      {
      PointType pt; 
      imgBubbles->TransformIndexToPhysicalPoint(itThisBubble.GetIndex(),pt);
      
      if(pt.SquaredEuclideanDistanceTo(ptCenter) <= r2)
        {
        itThisBubble.Value() = m_SnakeColorLabel;
        nInitVoxels++;
        }

      ++itThisBubble;
      }
    }

  // At this point, we should have an initialization image and a bounding
  // box in bbLower and bbUpper.  End the routine if there are no initialization
  // voxels
  if (nInitVoxels == 0) return false;

  // Create the initialization level set image
  FloatImageType::Pointer imgLevelSet = FloatImageType::New();
  imgLevelSet->SetRegions(imgBubbles->GetLargestPossibleRegion());
  imgLevelSet->SetSpacing(imgBubbles->GetSpacing());
  imgLevelSet->Allocate();

  // Use iterators to set it pixels to positive and negative values. We are 
  // not computing the distance transform because the level set filter will
  // do that itself
  typedef ImageRegionConstIterator<BubbleImageType> BubbleImageConstIterator;
  typedef ImageRegionIterator<FloatImageType> FloatImageIterator;  
  BubbleImageConstIterator itBubblesToLevel(
    imgBubbles,imgBubbles->GetLargestPossibleRegion());
  FloatImageIterator itLevelSet(
    imgLevelSet,imgBubbles->GetLargestPossibleRegion());

  // Perform the transfer
  while(!itBubblesToLevel.IsAtEnd())
    {
    itLevelSet.Value() = itBubblesToLevel.Value() == 0 ? 1.0f : -1.0f;
    ++itLevelSet;
    ++itBubblesToLevel;
    }

  /*

  // The bounding box is used to compute a region for distance computation
  assert(bbLower[0] < bbUpper[0]);
  assert(bbLower[1] < bbUpper[1]);
  assert(bbLower[2] < bbUpper[2]);

  // Convert the bounding box to a region
  BubbleImageType::RegionType regInitialization;
  regInitialization.SetIndex(to_itkIndex(bbLower));
  regInitialization.SetSize(to_itkSize(bbUpper-bbLower));

  // Grow this region by four voxels
  regInitialization.PadByRadius(4);

  // Crop by the largest possible region
  regInitialization.Crop(imgBubbles->GetLargestPossibleRegion());

  // Construct and run the signed distance filter
  typedef SignedDistanceFilter<BubbleImageType,FloatImageType> DistanceFilterType;
  DistanceFilterType::Pointer fltDistance = DistanceFilterType::New();
  fltDistance->SetInput(imgBubbles);
  FloatImageType::Pointer imgDistance = fltDistance->GetOutput();
  imgDistance->SetRequestedRegion(regInitialization);
  fltDistance->Update();

  // Now, the output of fltSubtract is only a sub-region of the total 
  // initialization image.  We need to create a new image, initialize it 
  // with some large positive value and copy in the results of fltSubtract
  FloatImageType::Pointer imgLevelSet = FloatImageType::New();
  imgLevelSet->SetRegions(imgBubbles->GetLargestPossibleRegion());
  imgLevelSet->SetSpacing(imgBubbles->GetSpacing());
  imgLevelSet->Allocate();
  
  // Compute a sufficiently large positive value
  BubbleImageType::SizeType szTotal = 
    imgBubbles->GetLargestPossibleRegion().GetSize();
  float slpv = 1.0f + szTotal[0] + szTotal[1] + szTotal[2];
  
  // Use the value to fill the buffer
  imgLevelSet->FillBuffer(slpv);
  
  // Copy the signed d.t. into this image
  typedef ImageRegionConstIterator<FloatImageType> FloatImageConstIterator;
  typedef ImageRegionIterator<FloatImageType> FloatImageIterator;  
  FloatImageConstIterator itDistance(imgDistance,regInitialization);
  FloatImageIterator itLevelSet(imgLevelSet,regInitialization);

  while(!itDistance.IsAtEnd())
    {
    itLevelSet.Value() = itDistance.Value();
    ++itLevelSet;
    ++itDistance;
    }
  */  
    

  // Create an image wrapper to hold this initialization
  m_SnakeInitializationWrapper = new LevelSetImageWrapper();
  m_SnakeInitializationWrapper->SetImage(imgLevelSet);  

  // Make sure that the correct color label is being used
  m_SnakeInitializationWrapper->SetColorLabel(m_ColorLabels[m_SnakeColorLabel]);

  // Copy the transform info
  for(unsigned int iSlice = 0;iSlice < 3;iSlice ++)
    {
    m_SnakeInitializationWrapper->SetImageToDisplayTransform(
      iSlice,m_ImageGeometry.GetImageToDisplayTransform(iSlice));
    }  

  // Copy the cursor position
  m_SnakeInitializationWrapper->SetSliceIndex(m_LabelWrapper->GetSliceIndex());  
  
  // Initialize the snake driver
  InitalizeSnakeDriver(parameters);

  // Success
  return true;
}

void
SNAPImageData
::SetExternalAdvectionField( FloatImageType *imgX, 
  FloatImageType *imgY, FloatImageType *imgZ)
{
  m_ExternalAdvectionField = VectorImageType::New();
  m_ExternalAdvectionField->SetRegions(
    m_SpeedWrapper->GetImage()->GetBufferedRegion());
  m_ExternalAdvectionField->Allocate();
  m_ExternalAdvectionField->SetSpacing(
    m_GreyWrapper->GetImage()->GetSpacing());
  m_ExternalAdvectionField->SetOrigin(
    m_GreyWrapper->GetImage()->GetOrigin());

  typedef itk::ImageRegionConstIterator<FloatImageType> Iterator;
  Iterator itX(imgX,imgX->GetBufferedRegion());
  Iterator itY(imgY,imgY->GetBufferedRegion());
  Iterator itZ(imgZ,imgZ->GetBufferedRegion());

  typedef itk::ImageRegionIterator<VectorImageType> Vectorator;
  Vectorator itTarget(
    m_ExternalAdvectionField,
    m_ExternalAdvectionField->GetBufferedRegion());

  while(!itTarget.IsAtEnd())
    {
    VectorType v;
    
    v[0] = itX.Get();
    v[1] = itY.Get();
    v[2] = itZ.Get();

    itTarget.Set(v);
    
    ++itTarget; 
    ++itX; ++itY; ++itZ;
    }
}
  

void 
SNAPImageData
::InitalizeSnakeDriver(const SnakeParameters &p) 
{
  // Create a new level set driver
  if (m_LevelSetDriver) 
    {
    // Better not call this while we are in the middle of an update loop
    assert(!m_LevelSetDriver->IsInUpdateLoop());
    delete m_LevelSetDriver;
    }
    
  // This is a good place to check that the parameters are valid
  if(p.GetSnakeType()  == SnakeParameters::REGION_SNAKE)
    {
    // There is no advection 
    assert(p.GetAdvectionWeight() == 0);

    // There is no curvature speed
    assert(p.GetCurvatureSpeedExponent() == -1);

    // Propagation is modulated by probability
    assert(p.GetPropagationSpeedExponent() == 1);

    // There is no smoothing speed
    assert(p.GetLaplacianSpeedExponent() == 0);
    }

  // Copy the configuration parameters
  m_CurrentSnakeParameters = p;

  // Initialize the snake wrapper, currently empty
  if(m_SnakeWrapper)
    delete m_SnakeWrapper;
  m_SnakeWrapper = new LevelSetImageWrapper();

  // Initialize the snake driver and pass the parameters
  m_LevelSetDriver = 
    new SNAPLevelSetDriver3d(m_SnakeInitializationWrapper->GetImage(),
                           m_SpeedWrapper->GetImage(),
                           m_CurrentSnakeParameters,
                           m_ExternalAdvectionField);

  // Initialize the level set wrapper with the image from the level set 
  // driver
  m_SnakeWrapper->SetImage(m_LevelSetDriver->GetCurrentState());
  
  // Make sure that the correct color label is being used
  m_SnakeWrapper->SetColorLabel(m_ColorLabels[m_SnakeColorLabel]);

  // Copy the slice transform into the new wrapper
  for(unsigned int iSlice = 0;iSlice < 3;iSlice ++)
    {
    m_SnakeWrapper->SetImageToDisplayTransform(
      iSlice,m_ImageGeometry.GetImageToDisplayTransform(iSlice));
    }  
  
  // Copy the slice index information
  m_SnakeWrapper->SetSliceIndex(m_LabelWrapper->GetSliceIndex());  
}

bool 
SNAPImageData
::IsSegmentationPipelineInitialized() 
{
  return (m_LevelSetDriver);
}

bool 
SNAPImageData
::IsSegmentationPipelineRunning() 
{
  return (m_LevelSetDriver && m_LevelSetDriver->IsInUpdateLoop());
}


/** A pass-through callback that does some processing before calling the
 * real pause callback */
void 
SNAPImageData
::IntermediatePauseCallback(itk::Object *object,const itk::EventObject &event)
{
  static itk::NoEvent evtIdle;
  static itk::IterationEvent evtUpdate;
  
  // Check the type of the event, since the caller generates iteration events
  // and NoEvents
  if(event.CheckEvent(&evtIdle))
    {
    // Just call the idle function, nothing of importance has happened!
    m_SegmentationIdleCallback->Execute(object,event);
    }

  else if (event.CheckEvent(&evtUpdate))
    {
    // Update the snake wrapper with the contents of this filter
    m_SnakeWrapper->SetImage(m_LevelSetDriver->GetCurrentState());

    // Call the update callback
    m_SegmentationUpdateCallback->Execute(object,event);
    }
  else
    {
    // Unknown event!
    assert(0);
    }
}

void 
SNAPImageData
::StartSegmentationPipeline(
  itk::Command *idleCallback,itk::Command *updateCallback)
{
  // Should be in level set mode
  assert(m_LevelSetDriver);

  // Store the passed in callbacks
  m_SegmentationIdleCallback = idleCallback;
  m_SegmentationUpdateCallback = updateCallback;

  // Create a command pointing to our own callback function
  typedef itk::MemberCommand<SNAPImageData> CommandType;
  CommandType::Pointer intermediateCallback = CommandType::New();
  intermediateCallback->SetCallbackFunction(
    this,&SNAPImageData::IntermediatePauseCallback);
  
  // Call the driver's begin method with our intermediate callback
  m_LevelSetDriver->BeginUpdate(intermediateCallback);
}

void 
SNAPImageData
::RequestSegmentationStep(unsigned int nIterations)
{
  // Should be in level set mode
  assert(m_LevelSetDriver);

  // Pass through to the level set driver
  m_LevelSetDriver->RequestIterations(nIterations);
}

void 
SNAPImageData
::RequestSegmentationRestart()
{
  // Should be in level set mode
  assert(m_LevelSetDriver);

  // Pass through to the level set driver
  m_LevelSetDriver->RequestRestart();
}

void 
SNAPImageData
::RequestSegmentationPipelineTermination()
{
  // Should be in level set mode
  assert(m_LevelSetDriver);

  // Pass through to the level set driver
  m_LevelSetDriver->RequestEndUpdate();
}

void 
SNAPImageData
::SetSegmentationParameters(const SnakeParameters &parameters)
{
  // Should be in level set mode
  assert(m_LevelSetDriver);

  // Pass through to the level set driver
  m_LevelSetDriver->SetSnakeParameters(parameters);
}

void 
SNAPImageData
::ReleaseSegmentationPipeline()
{
  // Should be in level set mode
  assert(m_LevelSetDriver);

  // Should not be in an update loop!
  assert(!m_LevelSetDriver->IsInUpdateLoop());

  // Delete the level set driver and all the problems that go along with it
  delete m_LevelSetDriver; m_LevelSetDriver = NULL;
}

void 
SNAPImageData
::TerminatingPauseCallback()
{
  m_LevelSetDriver->RequestEndUpdate();
}

void 
SNAPImageData
::RunNonInteractiveSegmentation(int nIterations)
{
  // The trick here is to set a callback that will ask the pipeline to stop
  // on its completion
  typedef itk::SimpleMemberCommand<SNAPImageData> CommandType;
  CommandType::Pointer terminatingCallback = CommandType::New();
  terminatingCallback->SetCallbackFunction(
    this,&SNAPImageData::TerminatingPauseCallback);
  
  // Call the driver's begin method with our intermediate callback
  m_LevelSetDriver->BeginUpdate(terminatingCallback);
  m_LevelSetDriver->RequestIterations(nIterations);
}

void
SNAPImageData
::SetCrosshairs(const Vector3ui &crosshairs)
{
  // Call the parent's version
  IRISImageData::SetCrosshairs(crosshairs);
  
  // Set our own cross-hairs
  if(m_SpeedWrapper)
    m_SpeedWrapper->SetSliceIndex(crosshairs);
  
  if(m_SnakeInitializationWrapper)
    m_SnakeInitializationWrapper->SetSliceIndex(crosshairs);
  
  if(m_SnakeWrapper)
    m_SnakeWrapper->SetSliceIndex(crosshairs);
}

void 
SNAPImageData
::SetImageGeometry(const ImageCoordinateGeometry &geometry)
{
  // Call the parent's method
  IRISImageData::SetImageGeometry(geometry);

  // Propagate the geometry to the image wrappers
  for(unsigned int iSlice = 0;iSlice < 3;iSlice ++)
    {
    if(m_SpeedWrapper)
      m_SpeedWrapper->SetImageToDisplayTransform(
        iSlice,m_ImageGeometry.GetImageToDisplayTransform(iSlice));

    if(m_SnakeInitializationWrapper)
      m_SnakeInitializationWrapper->SetImageToDisplayTransform(
        iSlice,m_ImageGeometry.GetImageToDisplayTransform(iSlice));

    if(m_SnakeWrapper)
      m_SnakeWrapper->SetImageToDisplayTransform(
        iSlice,m_ImageGeometry.GetImageToDisplayTransform(iSlice));
    }  
}

SNAPImageData::LevelSetImageType *
SNAPImageData
::GetLevelSetImage()
{
  assert(m_LevelSetDriver);
  return m_LevelSetDriver->GetCurrentState();
}

SNAPLevelSetFunction<SpeedImageWrapper::ImageType> *
SNAPImageData
::GetLevelSetFunction()
{
  return m_LevelSetDriver->GetLevelSetFunction();
}
