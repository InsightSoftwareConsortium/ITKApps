/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPImageData.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SNAPImageData_h_
#define __SNAPImageData_h_

#include "IRISImageData.h"
#include "SnakeParameters.h"
#include "SpeedImageWrapper.h"

class SNAPLevelSetDriver;

/**
 * \class SNAPImageData
 * \brief Wrapper around the SNAP automatic segmentation pipelines.
 *
 * This class encapsulates several images used in the SNAP application, 
 * including the speed image, the bubble-initialization image and the
 * segmentation result image.
 */
class SNAPImageData : public IRISImageData 
{
public:

  // Some types that we will deal with
  typedef LabelImageWrapper SnakeWrapperType;

  // The type of the internal level set image
  typedef itk::Image<float,3> LevelSetImageType;

  SNAPImageData();
  ~SNAPImageData();

  /** 
   * Get the preprocessed (speed) image wrapper
   */
  SpeedImageWrapper* GetSpeed();

  /**
   * Initialize the Speed image wrapper to blank data
   */
  void InitializeSpeed();
  
  /**
   * Clear the preprocessed (speed) image (discard data, etc)
   */
  void ClearSpeed();

  /**
   * Check the preprocessed image for validity
   */
  bool IsSpeedLoaded();

  /**
   * Get the snake initialization image
   */
  SnakeWrapperType* GetSnakeInitialization();

  /**
   * Clear the snake initialization image (discard data, etc)
   */
  void ClearSnakeInitialization();

  /**
   * Check the snake initialization image for validity
   */
  bool IsSnakeInitializationLoaded();

  /**
   * Get the current snake image wrapper
   */
  SnakeWrapperType* GetSnake();

  /**
   * Clear the current snake (discard data, etc)
   */
  void ClearSnake();

  /**
   * Check the current snake for validity
   */
  bool IsSnakeLoaded();

  /**
   *    Check if the snake is currently running
   */
  bool IsSnakeInitialized();

  /**
   * Initialize the snake with an array of bubbles.  If successful,
   * this method constructs a snake initialization image.
   * @return Number of voxels of color labelColor in the resulting image
   */
  unsigned int InitializeSnakeImage(Bubble *bubbles, int nBubbles, unsigned int labelColor);

  /**
   * Initialize snake from snake initialization image
   */
  void InitalizeSnakeDriver(int snakeMode, const SnakeParameters &param);

  /**
   * Set current snake parameters
   */
  void SetSnakeParameters(const SnakeParameters &parms);

  /**
   * Restart snake propagation
   */
  void RestartSnake();

  /**
   * Take a step in snake propagation
   */
  void StepSnake(int nSteps);

  /**
   * Check if the snake has converged
   */
  bool IsSnakeConverged();

  /**
   * Update snake image (from the snake pipeline)
   * TODO: This should be unnecessary with ITK pipeline Update command
   */
  void UpdateSnakeImage();

  /**
   * Merge the segmentation result with the segmentation contained in a
   * IRIS image data object.
   */
  void MergeSnakeWithIRIS(IRISImageData *target) const;

  /**
   * Set the cross-hairs position
   */
  void SetCrosshairs(const Vector3i &crosshairs);

  /**
   * Get the level set image currently being evolved
   */
  LevelSetImageType *GetLevelSetImage();
  
private:
  // Speed image adata
  SpeedImageWrapper *m_SpeedWrapper;

  // Snake image data
  SnakeWrapperType *m_SnakeWrapper, *m_SnakeInitializationWrapper;

  // Snake driver
  SNAPLevelSetDriver *m_LevelSetDriver;

  // Label color used for the snake images
  LabelType m_SnakeColorLabel;

  // Current value of snake parameters
  SnakeParameters m_CurrentSnakeParameters;       

  // Current snake mode (in/out or edge)
  int m_CurrentSnakeMode;
};




#endif
