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

SNAPImageData::SnakeWrapperType* 
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

SNAPImageData::SnakeWrapperType* 
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

unsigned int 
SNAPImageData
::InitializeSnakeImage(Bubble *bubbles, int nBubbles, unsigned int labelColor) 
{
  assert(m_GreyWrapper && m_LabelWrapper);

  // Store the label color
  m_SnakeColorLabel = labelColor;

  // Todo: initialize seg image on demand?
  Vector3i ul,lr;
  ul[0] = ul[1] = ul[2] = 0;
  lr = GetVolumeExtents();

  // Why ?
  lr[0] -= 1;lr[1] -= 1;lr[2] -= 1;

  // start with the seg data
  ClearSnakeInitialization();
  m_SnakeInitializationWrapper = 
    new LabelImageWrapperImplementation(*m_LabelWrapper);

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
}

void 
SNAPImageData
::InitalizeSnakeDriver(int snakeMode, const SnakeParameters &p) 
{
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
  m_LevelSetDriver->SetSnakeParameters(&p);
}

/**
 * Restart snake propagation
 */
void SNAPImageData
::RestartSnake() 
{
  // Restart the snake driver
  InitalizeSnakeDriver(m_CurrentSnakeMode,m_CurrentSnakeParameters);

  // Clear the snake image
  ClearSnake();

  // Copy from initialization
  m_SnakeWrapper = new LabelImageWrapperImplementation(*m_SnakeInitializationWrapper);    
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
  SpeedImageWrapper::ImageType *imgLevelSet 
    = m_LevelSetDriver->GetCurrentState();

  // Create an iterator through the level set image
  ImageRegionConstIterator<SpeedImageWrapper::ImageType> 
    itSource(imgLevelSet,imgLevelSet->GetLargestPossibleRegion());

  // Create an iterator through the snake image
  ImageRegionIterator<SnakeWrapperType::ImageType> 
    itTarget(m_SnakeWrapper->GetImage(),
             m_SnakeWrapper->GetImage()->GetLargestPossibleRegion());

  // Map the negative values in the distance transform to label values in the
  // snake image
  while(!itTarget.IsAtEnd())
    {    
    itTarget.Value() = itSource.Value() <= 0 ? m_SnakeColorLabel : 0;
    
    ++itTarget;
    ++itSource;
    }

  // The snake image has been updated!
  m_SnakeWrapper->GetImage()->Modified();
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

