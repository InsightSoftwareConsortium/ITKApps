/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    LabelImageWrapperImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __LabelImageWrapperImplementation_h_
#define __LabelImageWrapperImplementation_h_

#include "LabelImageWrapper.h"
#include "ImageWrapperImplementation.h"
#include "UnaryFunctorCache.h"
#include "itkRGBAPixel.h"

// Disable 'inheritance by dominance' warining in VC6
#ifdef _WIN32
    #pragma warning (disable: 4250)
#endif

/**
 * \class LabelImageWrapperImplementation
 * \brief Implementation for LabelImageWrapper.
 *
 * \sa LabelImageWrapper
 */
class LabelImageWrapperImplementation :   public virtual LabelImageWrapper, 
  public ImageWrapperImplementation<LabelType>
{
public:

  // Type of color slice returned by this class
  // typedef vnl_vector_fixed<unsigned char,4> DisplayPixelType;
  typedef itk::RGBAPixel<unsigned char> DisplayPixelType;
  typedef itk::Image<DisplayPixelType,2> DisplaySliceType;
  typedef itk::SmartPointer<DisplaySliceType> DisplaySlicePointer;

  // Implementation of LabelImageWrapper abstract methods
  void SetLabelColorTable(ColorLabel *labels);
  ColorLabel *GetLabelColorTable() const;
  void UpdateColorMappingCache();
  
  DisplaySlicePointer GetDisplaySlice(unsigned int dim)
  {
    return m_IntensityFilter[dim]->GetOutput();
  }


  /** Constructor initializes mapper */
  LabelImageWrapperImplementation();

  /** Constructor that copies another wrapper */
  LabelImageWrapperImplementation(const LabelImageWrapper &source);

  /** Destructor */
  ~LabelImageWrapperImplementation();  

private:
  /**
   * Functor used for display caching.  This class keeps a pointer to 
   * the table of colors and maps colors to RGBA Pixels
   */
  class IntensityFunctor {
  public:    
      /** The pointer to the label table */
      ColorLabel *m_ColorLabelTable;

      /** The operator that maps label to color */
      DisplayPixelType operator()(const LabelType &x) const;
  };

  // Type of intensity function used to map 3D volume intensity into
  // 2D slice intensities
  typedef 
    UnaryFunctorCache<LabelType,DisplayPixelType,IntensityFunctor> CacheType;  
  typedef itk::SmartPointer<CacheType> CachePointer;
  typedef CacheType::CachingFunctor CacheFunctor;

  // Filter applied to slices
  typedef itk::Image<LabelType,2> LabelSliceType;
  typedef 
    itk::UnaryFunctorImageFilter<LabelSliceType,DisplaySliceType,CacheFunctor>
    IntensityFilterType;
  typedef IntensityFilterType::Pointer IntensityFilterPointer;


  /**
   * An instance of the private intensity mapper (this mapper wraps the passed
   * in list of labels
   */
  IntensityFunctor m_IntensityFunctor;

  /**
   * A cache used for the intensity mapping function
   */
  CachePointer m_IntensityMapCache;

  /**
   * Filters used to remap the intensity of the slices in this image
   * into unsigned char images
   */
  IntensityFilterPointer m_IntensityFilter[3];
};

#endif // __LabelImageWrapperImplementation_h_
