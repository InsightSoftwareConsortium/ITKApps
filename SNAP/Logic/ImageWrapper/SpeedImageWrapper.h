/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SpeedImageWrapper.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SpeedImageWrapper_h_
#define __SpeedImageWrapper_h_

#include "ImageWrapper.h"
#include "IRISTypes.h"
#include "itkRGBAPixel.h"

namespace itk {  
  template<class TPixel,unsigned int VDimension> class Image;
  template<class TInput,class TOutput> class FunctionBase;
  template<class TImage> class ImageSource;
};

/**
 * \class SpeedImageWrapper
 * \brief Image wraper for speed images in SNAP
 *
 * This wrapper remaps floating point slices to byte slices differently
 * depending on if it's in InOut snake mode (some speed values are negative) or
 * in Edge snake mode (speed values are nonnegative).
 *
 * \sa ImageWrapper, SpeedImageWrapperImplementation
 */
class SpeedImageWrapper : virtual public ImageWrapper<float> 
{
public:
  // Basics
  typedef SpeedImageWrapper Self;
  typedef ImageWrapper<float> Superclass;
  typedef Superclass::ImageType ImageType;

  // The type definition for the image used to display speed slices
  typedef ImageWrapper<float>::SliceType DisplaySliceType;
  typedef itk::SmartPointer<DisplaySliceType> DisplaySlicePointer;

  // The type definition for the image used to display overlays based on
  // speed images
  typedef itk::RGBAPixel<unsigned char> OverlayPixelType;
  typedef itk::Image<OverlayPixelType,2> OverlaySliceType;
  typedef itk::SmartPointer<OverlaySliceType> OverlaySlicePointer;

  /**
   * Set the preview source for the slices.  This means that the slices
   * will be generated not from the internal image but from external 
   * images (outputs of some preprocessing filters)
   */
  virtual void SetSliceSourceForPreview(unsigned int slice,ImageType *source) = 0;

  /** 
   * Unset the preview sources for all slices.  The slices will be now
   * generated from the internal image 
   */
  virtual void RemoveSliceSourcesForPreview() = 0;

  /**
   * Indicate that this image is a In/Out speed image that has a 
   * range of -1 to +1.  
   */
  virtual void SetModeToInsideOutsideSnake() = 0;
  
  /**
   * Indicate that this image is a Edge speed image that has a 
   * range of 0 to 1.  
   */
  virtual void SetModeToEdgeSnake() = 0;

  /**
   * Check if the image is in the Inside/Outside or Edge mode
   */
  virtual bool IsModeInsideOutsideSnake() const = 0;
    
  /**
   * Check if the image is in the Inside/Outside or Edge mode
   */
  virtual bool IsModeEdgeOutsideSnake() const = 0;

  /**
   * Get the display slice in a given direction.  To change the
   * display slice, call parent's MoveToSlice() method
   */
  virtual DisplaySlicePointer GetDisplaySlice(unsigned int dim) = 0;

  /**
   * Get an overlay mask slice for displaying on top of the greylevel
   * segmentation image.  Such slices are used for example to overlay
   * the thresholding result over the grey slice to give users feedback 
   * of the segmentation
   */
  virtual OverlaySlicePointer GetOverlaySlice(unsigned int dim) = 0;

  /**
   * Set the overlay cutoff range.  The intensities above the cutoff will
   * be included in the overlay
   */
  virtual void SetOverlayCutoff(float cutoff) = 0;
  virtual float GetOverlayCutoff() const = 0;

  /**
   * Set the color for overlay drawing
   */
  virtual void SetOverlayColor(const OverlayPixelType &color) = 0;
  virtual OverlayPixelType GetOverlayColor() const = 0;

  typedef itk::ImageSource<ImageType> PreviewFilterType;
  typedef itk::SmartPointer<PreviewFilterType> PreviewFilterPointer;

  /** Get a 'preview' voxel, i.e., a voxel from the previewing slices.  For
   * the results to be valid, the voxel has to be on one of the previewing
   * slices, and this method is intended for getting the voxel at the
   * cross-hairs position */
  virtual float GetPreviewVoxel(const Vector3i &index) const = 0;

};

#endif // __SpeedImageWrapper_h_
