/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    LevelSetImageWrapperImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __LevelSetImageWrapperImplementation_h_
#define __LevelSetImageWrapperImplementation_h_

#include "ImageWrapperImplementation.h"
#include "LevelSetImageWrapper.h"

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
 * \class LevelSetImageWrapperImplementation
 * \brief Implementation of LevelSetImageWrapper
 *
 * \sa LevelSetImageWrapper
 */
class LevelSetImageWrapperImplementation 
: public ImageWrapperImplementation<float>, public virtual LevelSetImageWrapper 
{
public:

  // Inherit type definitions from parent
  typedef LevelSetImageWrapper::DisplayPixelType DisplayPixelType; 
  typedef LevelSetImageWrapper::DisplaySliceType DisplaySliceType;
  typedef LevelSetImageWrapper::DisplaySlicePointer DisplaySlicePointer;

  /** Set the color label for inside */
  void SetColorLabel(const ColorLabel &label);
  
  /**
   * Get the display slice in a given direction.  To change the
   * display slice, call parent's MoveToSlice() method
   */
  DisplaySlicePointer GetDisplaySlice(unsigned int dim);

  /** Constructor initializes mappers */
  LevelSetImageWrapperImplementation();

  /** Destructor */
  ~LevelSetImageWrapperImplementation();

private:
  /**
   * A very simple functor used to map intensities
   */
  class MappingFunctor 
  {
  public:
    DisplayPixelType operator()(float in);
    DisplayPixelType m_InsidePixel;
    DisplayPixelType m_OutsidePixel;
  };  
  
  // Type of the display intensity mapping filter used when the 
  // input is a in-out image
  typedef itk::UnaryFunctorImageFilter<
    ImageWrapper<float>::SliceType,DisplaySliceType,MappingFunctor> 
    IntensityFilterType;
  typedef itk::SmartPointer<IntensityFilterType> IntensityFilterPointer;

  /** 
   * The filters used to remap internal level set image 
   * to a color display image
   */
  IntensityFilterPointer m_DisplayFilter[3];

  /** The currently used overlay functor */
  MappingFunctor m_MappingFunctor;
};

#endif // __LevelSetImageWrapperImplementation_h_
