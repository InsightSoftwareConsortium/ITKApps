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
#include "SNAPImageData.h"
#include "SNAPLevelSetDriver.h"
#include "GlobalState.h"

#include "SpeedImageWrapperImplementation.h"
#include "LabelImageWrapperImplementation.h"
#include "LevelSetImageWrapperImplementation.h"
#include "IRISVectorTypesToITKConversion.h"

#include "itkGroupSpatialObject.h"
#include "itkEllipseSpatialObject.h"
#include "itkSpatialObjectToImageFilter.h"
#include "itkMaximumImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"


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
  m_SpeedWrapper = new SpeedImageWrapperImplementation;
  m_SpeedWrapper->InitializeToImage(m_GreyWrapper->GetImage());

  // Copy in the cursor position
  m_SpeedWrapper->SetSliceIndex(m_GreyWrapper->GetSliceIndex());
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

// Create an inverting functor
class InvertFunctor {
public:
  unsigned char operator()(unsigned char input) { 
    return input == 0 ? 1 : 0; 
  }  
};

unsigned int 
SNAPImageData
::InitializeLevelSet(int snakeMode, const SnakeParameters &parameters,
                     Bubble *bubbles, unsigned int nBubbles, 
                     unsigned int labelColor) 
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

    for(unsigned int i=0;i<3;i++)
      {
      ptLower[i] = bubbles[iBubble].center[i] - bubbles[iBubble].radius;
      ptUpper[i] = bubbles[iBubble].center[i] + bubbles[iBubble].radius;
      ptCenter[i] = bubbles[iBubble].center[i];
      }
    
    // Index locations corresponding to the extents of the bubble
    BubbleImageType::IndexType idxLower, idxUpper;     
    imgBubbles->TransformPhysicalPointToIndex(ptLower,idxLower);
    imgBubbles->TransformPhysicalPointToIndex(ptUpper,idxUpper);
    
    // Create a region
    BubbleImageType::SizeType szBubble;
    szBubble[0] = idxUpper[0] - idxLower[0];
    szBubble[1] = idxUpper[1] - idxLower[1];
    szBubble[2] = idxUpper[2] - idxLower[2];
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
  if (nInitVoxels == 0)
    {
    return 0;
    }

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
/*
  // Create spatial objects corresponding to the bubbles
  typedef GroupSpatialObject<3> GroupType;
  typedef EllipseSpatialObject<3> EllipseType;

  GroupType::Pointer objGroup = GroupType::New();
  for(unsigned int iBubble=0; iBubble < nBubbles; iBubble++)
    {
    EllipseType::Pointer objBubble = EllipseType::New();
    objBubble->SetRadius(bubbles[iBubble].radius);

    EllipseType::TransformType::OffsetType offset;
    offset[0] = bubbles[iBubble].center[0];
    offset[1] = bubbles[iBubble].center[1];
    offset[2] = bubbles[iBubble].center[2];
    objBubble->GetObjectToParentTransform()->SetOffset(offset);
      
    objGroup->AddSpatialObject(objBubble);
    }

  objGroup->ComputeObjectToWorldTransform();
  objGroup->SetBoundingBoxChildrenDepth(1);
  objGroup->ComputeBoundingBox();

  // Create a pipeline that converts the input segmentation and bubbles into 
  // a level set image                                                      
  
  // 1. Bubble to image filter
  typedef SpatialObjectToImageFilter<GroupType,BubbleImageType> FillFilterType;
  FillFilterType::Pointer fltFillBubbles = FillFilterType::New();

  fltFillBubbles->SetInput(objGroup);
  fltFillBubbles->SetInsideValue((LabelType)labelColor);
  fltFillBubbles->SetOutsideValue(0);
  fltFillBubbles->SetChildrenDepth(2);
  fltFillBubbles->SetSize(
    m_GreyWrapper->GetImage()->GetLargestPossibleRegion().GetSize());
  fltFillBubbles->SetSpacing(
    m_GreyWrapper->GetImage()->GetSpacing());
  fltFillBubbles->ReleaseDataFlagOn();
  fltFillBubbles->Update();
  
  // 2. Merge bubble image with segmentation image
  typedef MaximumImageFilter<
    BubbleImageType,BubbleImageType,BubbleImageType> MergeFilterType;
  MergeFilterType::Pointer fltMerge = MergeFilterType::New();
  fltMerge->SetInput1(fltFillBubbles->GetOutput());
  fltMerge->SetInput2(m_LabelWrapper->GetImage());
  fltMerge->ReleaseDataFlagOn();
*/

  // 3. Create the inverse of the bubble image
  typedef UnaryFunctorImageFilter<BubbleImageType,BubbleImageType,
    InvertFunctor> InvertFilterType;
  InvertFilterType::Pointer fltInvert = InvertFilterType::New();
  fltInvert->SetInput(imgBubbles);
  fltInvert->ReleaseDataFlagOn();

  // 4. Compute the signed distance function from the bubble image
  typedef DanielssonDistanceMapImageFilter
    <BubbleImageType,FloatImageType> DistanceFilterType;
  DistanceFilterType::Pointer fltDistanceOutside = DistanceFilterType::New();
  fltDistanceOutside->SetInput(imgBubbles);
  fltDistanceOutside->SetInputIsBinary(true);
  fltDistanceOutside->GetOutput()->SetRequestedRegion(regInitialization);
  fltDistanceOutside->ReleaseDataFlagOn();  
  
  // 5. Compute the second distance function
  DistanceFilterType::Pointer fltDistanceInside = DistanceFilterType::New();
  fltDistanceInside->SetInput(fltInvert->GetOutput());
  fltDistanceInside->SetInputIsBinary(true);
  fltDistanceInside->GetOutput()->SetRequestedRegion(regInitialization);
  fltDistanceInside->ReleaseDataFlagOn();

  // Compute the distance transforms
  fltDistanceOutside->Update();
  fltDistanceInside->Update(); 

  // 6. Subtract the inside from the outside, forming a signed distance map
  typedef SubtractImageFilter<FloatImageType,
    FloatImageType,FloatImageType> SubtractFilterType;
  SubtractFilterType::Pointer fltSubtract = SubtractFilterType::New();
  fltSubtract->SetInput1(fltDistanceOutside->GetDistanceMap());
  fltSubtract->SetInput2(fltDistanceInside->GetDistanceMap());

  // Make sure the update only applies to a region of the image
  FloatImageType::Pointer imgDistance = fltSubtract->GetOutput();
  imgDistance->SetRequestedRegion(regInitialization);

  // Update this filter.  Now we have a distance transform image
  fltSubtract->Update();
  
  // Now, the output of fltSubtract is only a sub-region of the total 
  // initialization image.  We need to create a new image, initialize it 
  // with some large positive value and copy in the results of fltSubtract
  FloatImageType::Pointer imgLevelSet = FloatImageType::New();
  imgLevelSet->SetRegions(imgBubbles->GetLargestPossibleRegion());
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

  // Create an image wrapper to hold this initialization
  m_SnakeInitializationWrapper = new LevelSetImageWrapperImplementation();
  m_SnakeInitializationWrapper->SetImage(imgLevelSet);  

  // Make sure that the correct color label is being used
  m_SnakeInitializationWrapper->SetColorLabel(m_ColorLabels[m_SnakeColorLabel]);

  // Copy in the cursor position
  m_SnakeInitializationWrapper->SetSliceIndex(m_LabelWrapper->GetSliceIndex());

  // Initialize the snake driver
  InitalizeSnakeDriver(snakeMode,parameters);

  /*  
  // Todo: initialize seg image on demand?
  Vector3i ul,lr;
  ul[0] = ul[1] = ul[2] = 0;
  lr = GetVolumeExtents();

  // Why ?
  lr[0] -= 1;lr[1] -= 1;lr[2] -= 1;

  // start with the seg data
  ClearSnakeInitialization();
  m_SnakeInitializationWrapper = 
    new ImageWrapperImplementation<float>(*m_LabelWrapper);

  // Copy in the cursor position
  m_SnakeInitializationWrapper->SetSliceIndex(m_LabelWrapper->GetSliceIndex());


  

  // put in the bubbles
  LabelType segLabel = (LabelType)labelColor;

  Vector3i center;

  bool foundlabel = false;
  unsigned int nLabeledVoxels = 0;

  float xscale,yscale,zscale;
  const double *spacing = m_GreyWrapper->GetImage()->GetSpacing();
  xscale = spacing[0]*spacing[0];
  yscale = spacing[1]*spacing[1];
  zscale = spacing[2]*spacing[2];

  // TODO: this code is dreadful.  Looping over all voxels and inside that 
  // over all bubbles is just horrible.  There are better ways to scan convert
  // a sphere

  //for each voxel in the snake init image
  //note: go through this even if no bubbles are specified
  //      because we have to check if there is any initialization
  //      from segImageData if there are no bubbles, anyway
  for (int k = 0; k < m_Size[2]; k++)
    {
    for (int j = 0; j < m_Size[1]; j++)
      {
      for (int i = 0; i < m_Size[0]; i++)
        {
        //set the voxel to zero if it is some other label
        //besides the current label (snake label)
        LabelType &val = m_SnakeInitializationWrapper->GetVoxel(i,j,k);

        //if it's already set to label, do nothing
        //(no need to compare to bubbles)
        if (val != segLabel)
          {
          val = 0;
          //now check if the voxel is inside a bubble
          for (int b = 0; b < nBubbles; b++)
            {
            center = bubbles[b].center;
            if ((center[0] - i)*(center[0] - i)*xscale +
              (center[1] - j)*(center[1] - j)*yscale +
              (center[2] - k)*(center[2] - k)*zscale <=
              bubbles[b].radius*bubbles[b].radius)
              {
              //set the voxel to the snake label
              val = segLabel;
              nLabeledVoxels++;
              break;
              }
            }
          } 
        else
          {
          nLabeledVoxels++;
          }
        }
      }
    }

  if (nLabeledVoxels == 0)
    {
    ClearSnakeInitialization();
    return 0;
    } 
  else
    {
    // Copy initialization image into snake image
    ClearSnake();    
    m_SnakeWrapper = 
      new LabelImageWrapperImplementation(*m_SnakeInitializationWrapper);

    // Copy in the cursor position
    m_SnakeWrapper->SetSliceIndex(m_LabelWrapper->GetSliceIndex());

    return nLabeledVoxels;
    }
*/
  return 1;
}

void 
SNAPImageData
::InitalizeSnakeDriver(int snakeMode, const SnakeParameters &p) 
{
  // Create a new level set driver
  if (m_LevelSetDriver) 
    delete m_LevelSetDriver;
  m_LevelSetDriver = new SNAPLevelSetDriver;

  // Copy the configuration parameters
  m_CurrentSnakeMode = snakeMode;
  m_CurrentSnakeParameters = p;

  // This is a good place to check that the parameters are valid
  if(snakeMode == IN_OUT_SNAKE)
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

  // Initialize the snake driver and pass the parameters
  m_LevelSetDriver->Initialize(m_SnakeInitializationWrapper->GetImage(),
                               m_SpeedWrapper->GetImage(),&p);

  // Initialize the snake wrapper, currently empty
  if(m_SnakeWrapper)
    delete m_SnakeWrapper;
  m_SnakeWrapper = new LevelSetImageWrapperImplementation();

  // Make sure that the correct color label is being used
  m_SnakeWrapper->SetColorLabel(m_ColorLabels[m_SnakeColorLabel]);

  // Copy in the cursor position
  m_SnakeWrapper->SetSliceIndex(m_LabelWrapper->GetSliceIndex());

  // Wrap the output of the driver
  m_SnakeWrapper->SetImage(m_LevelSetDriver->GetCurrentState());
}

bool 
SNAPImageData
::IsSnakeInitialized() 
{
  return (m_LevelSetDriver != NULL);
}

/**
 * Set current snake parameters
 */
void 
SNAPImageData
::SetSnakeParameters(const SnakeParameters &p) 
{
  assert(m_LevelSetDriver);

  // Set the parameters
  m_LevelSetDriver->SetSnakeParameters(&p);

  // Make sure that the snake wrapper is pointing to the right output
  // TODO: Resolve if the following is necessary!
  // m_SnakeWrapper->SetImage(m_LevelSetDriver->GetCurrentState());
}

/**
 * Restart snake propagation
 */
void SNAPImageData
::RestartSnake() 
{
  // Restart the level set driver
  m_LevelSetDriver->Restart(m_SnakeInitializationWrapper->GetImage());

  // Make sure that the snake wrapper is pointing to the right output
  m_SnakeWrapper->SetImage(m_LevelSetDriver->GetCurrentState());
}

/**
 * Take a step in snake propagation (copy data into the snake image)
 */
void SNAPImageData
::StepSnake(int nSteps) 
{
  assert(m_LevelSetDriver);

  m_LevelSetDriver->Run(nSteps);
}

/**
 * Check if the snake has converged
 */
bool 
SNAPImageData
::IsSnakeConverged() 
{
  assert(m_LevelSetDriver);

  // TODO: Implement convergence checks
  return false;
}

/**
 * Update snake image (from the snake pipeline)
 * TODO: This should be unnecessary with ITK pipeline Update command
 */
void 
SNAPImageData
::UpdateSnakeImage() 
{
  assert(m_LevelSetDriver);

  // Get the level set image 
  // SpeedImageWrapper::ImagePointer imgLevelSet 
  //  = m_LevelSetDriver->GetCurrentState();

  // Assign this image to the snake wrapper
  // m_SnakeWrapper->SetImage(imgLevelSet);
}

void
SNAPImageData
::SetCrosshairs(const Vector3i &crosshairs)
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

SNAPImageData::LevelSetImageType *
SNAPImageData
::GetLevelSetImage()
{
  assert(m_LevelSetDriver);
  return m_LevelSetDriver->GetCurrentState();
}

