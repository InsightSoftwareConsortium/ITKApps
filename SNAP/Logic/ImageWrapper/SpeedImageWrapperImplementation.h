/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SpeedImageWrapperImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SpeedImageWrapperImplementation_h_
#define __SpeedImageWrapperImplementation_h_

#include "ImageWrapperImplementation.h"
#include "SpeedImageWrapper.h"

// Forward references to ITK
namespace itk {
  template <class TInput,class TOutput,class TFunctor> 
    class UnaryFunctorImageFilter;
};

// Disable 'inheritance by dominance' warining in VC6
#ifdef _WIN32
  #pragma warning (disable: 4250)
#endif

/**
 * \class SpeedImageWrapperImplementation
 * \brief Implementation of SpeedImageWrapper
 *
 * \sa SpeedImageWrapper
 */
class SpeedImageWrapperImplementation 
: public ImageWrapperImplementation<float>, public virtual SpeedImageWrapper 
{
public:

  // Inherit type definitions from parent
  typedef SpeedImageWrapper::DisplaySliceType DisplaySliceType;
  typedef SpeedImageWrapper::DisplaySlicePointer DisplaySlicePointer;
  typedef SpeedImageWrapper::OverlayPixelType OverlayPixelType; 
  typedef SpeedImageWrapper::OverlaySliceType OverlaySliceType;
  typedef SpeedImageWrapper::OverlaySlicePointer OverlaySlicePointer;

  /**
   * Indicate that this image is a In/Out speed image that has a 
   * range of -1 to +1.  
   */
  void SetModeToInsideOutsideSnake()
    {
    m_IsModeInsideOutside = true;
    }
  
  /**
   * Indicate that this image is a Edge speed image that has a 
   * range of 0 to 1.  
   */
  void SetModeToEdgeSnake()
    {
    m_IsModeInsideOutside = false;
    }

  /**
   * Check if the image is in the Inside/Outside or Edge mode
   */
  bool IsModeInsideOutsideSnake() const
    {
    return m_IsModeInsideOutside;
    }
    
  /**
   * Check if the image is in the Inside/Outside or Edge mode
   */
  bool IsModeEdgeOutsideSnake() const
    {
    return !m_IsModeInsideOutside;
    }
  
  /**
   * Get the display slice in a given direction.  To change the
   * display slice, call parent's MoveToSlice() method
   */
  DisplaySlicePointer GetDisplaySlice(unsigned int dim);

  /**
   * Get an overlay mask slice for displaying on top of the greylevel
   * segmentation image.  Such slices are used for example to overlay
   * the thresholding result over the grey slice to give users feedback 
   * of the segmentation
   */
  OverlaySlicePointer GetOverlaySlice(unsigned int dim);

  /**
   * Set the overlay cutoff range.  The intensities above the cutoff will
   * be included in the overlay
   */
  void SetOverlayCutoff(float cutoff);  
  float GetOverlayCutoff() const;

  typedef itk::ImageSource<ImageType> PreviewFilterType;
  typedef itk::SmartPointer<PreviewFilterType> PreviewFilterPointer;
  
  /**
   * Set the color for overlay drawing
   */
  void SetOverlayColor(const OverlayPixelType &color);
  OverlayPixelType GetOverlayColor() const;
  
  /** Constructor initializes mappers */
  SpeedImageWrapperImplementation();

  /** Destructor */
  ~SpeedImageWrapperImplementation();

private:
  /**
   * A very simple functor used to map intensities
   */
  class MappingFunctor 
  {
  public:
    float operator()(float in);
  };  
  
  /**
   * A functor used for overlay mapping
   */
  class OverlayFunctor 
  {
  public:
    /** Operator used to map pixels to color */
    OverlayPixelType operator()(float in);

    /** Overlay cutoff */
    float m_Cutoff;

    /** Overlay color */
    OverlayPixelType m_Color;
  };  
  
  // Type of the display intensity mapping filter used when the 
  // input is a in-out image
  typedef itk::UnaryFunctorImageFilter<
    ImageWrapper<float>::SliceType,DisplaySliceType,MappingFunctor> 
    IntensityFilterType;
  typedef itk::SmartPointer<IntensityFilterType> IntensityFilterPointer;

  // A mapping filter used to construct overlay images.  This filter assigns
  // an opaque color to pixels over the cutoff threshold
  typedef itk::UnaryFunctorImageFilter<
    ImageWrapper<float>::SliceType,OverlaySliceType,OverlayFunctor> 
    OverlayFilterType;
  typedef itk::SmartPointer<OverlayFilterType> OverlayFilterPointer;
    
  /** Whether or not the image is in edge or in-out mode */
  bool m_IsModeInsideOutside;

  /** 
   * The filters used to remap internal speed image that can be 
   * in range of -1 to 1 to a display image in range 0 to 1
   */
  IntensityFilterPointer m_DisplayFilter[3];

  /**
   * The filters used to create overlay slices
   */
  OverlayFilterPointer m_OverlayFilter[3];

  /** The currently used overlay functor */
  OverlayFunctor m_OverlayFunctor;
};

#endif // __SpeedImageWrapperImplementation_h_
