/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SmoothBinaryThresholdImageFilter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SmoothBinaryThresholdImageFilter_h_
#define __SmoothBinaryThresholdImageFilter_h_

#include "itkCommand.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkProgressAccumulator.h"
#include "ThresholdSettings.h"

/**
 * A functor used for the smooth threshold operation on images.  
 * Used in conjuction with itk::UnaryFunctorImageFilter.
 */
template<class TInput, class TOutput>
class SmoothBinaryThresholdFunctor 
{
public:

  /**
   * Initialize the function
   */
  void SetParameters(TInput imageMin,TInput imageMax,
                     const ThresholdSettings &settings)
  {
    // At least one threshold should be used
    assert(settings.IsLowerThresholdEnabled() || 
           settings.IsUpperThresholdEnabled());

    // Set the factor by which the input is multiplied
    // m_ScalingFactor = 1.0f / (smoothness * (imageMax-imageMin));
    m_ScalingFactor = 1.0f / settings.GetSmoothness();

    // Store the upper and lower bounds
    m_LowerThreshold = settings.GetLowerThreshold();
    m_UpperThreshold = settings.GetUpperThreshold();

    // Combine the usage and inversion flags to get the scaling factors    
    m_FactorLower = settings.IsLowerThresholdEnabled() ? 1.0f : 0.0f;
    m_FactorUpper = settings.IsUpperThresholdEnabled() ? 1.0f : 0.0f;

    // Compute the shift
    m_Shift = 1.0f - (m_FactorLower + m_FactorUpper);
  }

  /**
   * Apply the function to image intensity
   */
  TOutput operator()(const TInput &x)
  {
    // Cast the input to float
    float z = static_cast<float>(x);
    
    // Compute the left component
    float yLower = m_FactorLower * tanh((z-m_LowerThreshold) * m_ScalingFactor);

    // Compute the right component
    float yUpper = m_FactorUpper * tanh((m_UpperThreshold-z) * m_ScalingFactor);

    // Return the result (TODO: hack)
    return static_cast<TOutput>(yLower + yUpper + m_Shift);
  }

private:
  // The lower threshold in intensity units
  float m_LowerThreshold;

  // The upper threshold in intensity units
  float m_UpperThreshold;

  // The scaling factor that incorporates the smoothness parameter and the
  // intensity range of the input image
  float m_ScalingFactor;

  // The multiplier applied to the left/right threshold.  This can be set to 
  // 0 and 1 depending on the other threshold
  float m_FactorLower;
  float m_FactorUpper;

  // The shift amount added to the sum of the left and right 
  // thresholded intensities to force them into [-1,1]
  float m_Shift;
};

/**
 * \class SmoothBinaryThresholdFunctor
 * \brief A filter used to perform binary thresholding to produce SNAP speed images.
 * 
 * This filter uses a sigmoid function as a smooth threshold
 */
template <typename TInputImage,typename TOutputImage = TInputImage>
class SmoothBinaryThresholdImageFilter: 
  public itk::ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  
  /** Standard class typedefs. */
  typedef SmoothBinaryThresholdImageFilter                         Self;
  typedef itk::ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef itk::SmartPointer<Self>                               Pointer;
  typedef itk::SmartPointer<const Self>                    ConstPointer;  
  
  /** Pixel Type of the input image */
  typedef TInputImage                                    InputImageType;
  typedef typename InputImageType::PixelType             InputPixelType;

  /** Pixel Type of the output image */
  typedef TOutputImage                                  OutputImageType;
  typedef typename OutputImageType::PixelType           OutputPixelType;

  /** Functor type used for thresholding */
  typedef SmoothBinaryThresholdFunctor<
    InputPixelType,OutputPixelType>                          FunctorType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self)
    
  /** Image dimension. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  /** Assign threshold settings */
  void SetThresholdSettings(const ThresholdSettings &settings);
  
protected:

  SmoothBinaryThresholdImageFilter();
  virtual ~SmoothBinaryThresholdImageFilter() {};
  void PrintSelf(std::ostream& os, itk::Indent indent) const;
  
  /** Generate Data */
  void GenerateData( void );

private:

  /** The unary functor filter type used for remapping */
  typedef itk::UnaryFunctorImageFilter<
    TInputImage,TOutputImage,FunctorType>            ThresholdFilterType;
  typedef typename ThresholdFilterType::Pointer   ThresholdFilterPointer;
  
  /** The min / max calculator used to compute input range */
  typedef itk::MinimumMaximumImageCalculator<TInputImage> CalculatorType;
  typedef typename CalculatorType::Pointer             CalculatorPointer;

  /** Progress accumulator object */
  typedef itk::ProgressAccumulator::Pointer           AccumulatorPointer;

  ThresholdFilterPointer    m_ThresholdFilter;
  CalculatorPointer         m_Calculator;                                               
  ThresholdSettings         m_ThresholdSettings;
  AccumulatorPointer        m_ProgressAccumulator;
};

#ifndef ITK_MANUAL_INSTANTIATION
#include "SmoothBinaryThresholdImageFilter.txx"
#endif

#endif // __SmoothBinaryThresholdImageFilter_h_
