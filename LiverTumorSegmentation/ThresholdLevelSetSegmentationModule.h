/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ThresholdLevelSetSegmentationModule.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ThresholdLevelSetSegmentationModule_h__
#define __ThresholdLevelSetSegmentationModule_h__

#include "itkConfidenceConnectedImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkThresholdSegmentationLevelSetImageFilter.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkProgressAccumulator.h"
#include "itkProcessObject.h"


#include "macros.h"

/** 

  \brief Module for Liver Tumor segmentation 

  This module encapsulates all the components required for performing
  segmentation of Liver Tumors.

  The input images for this module should be isotropic and the
  intensity levels be windowed for the range of intensities of the
  liver and the tumor.


*/

namespace itk {
  class Notifier : public ProcessObject
  {
  protected:
    Notifier() {};
    ~Notifier() {};
  public:
    typedef Notifier Self;
    typedef ProcessObject Superclass;
    typedef SmartPointer < Self > Pointer;
    typedef SmartPointer < const Self> ConstPointer;
    itkNewMacro( Self );
  };
};


namespace ISIS {

class ThresholdLevelSetSegmentationModule 
{

public:


  /**  The expected input images for this module are stored
       in 8 bits and are the output of a resampling and intensity
       windowing preprocessing.  */

  typedef   unsigned char     InputPixelType;


  /**  Use float pixel type for internal computations.  */  
  typedef   float             InternalPixelType;

  typedef   unsigned char     OutputPixelType;

  typedef   unsigned char     MaskPixelType;


  /**  Image dimension. This module is specific for 3D images */
  itkStaticConstMacro( Dimension, unsigned int, 3 );
  


  /** Type of the image to be used for internal computation
      actually this is the image type to be used in the threshold level set.  */
  typedef itk::Image< InternalPixelType, Dimension >   InternalImageType;


  /** Type of the image to be used as input to the module. */
  typedef itk::Image< InputPixelType,    Dimension >   InputImageType;


  /** Image type used for the output. This is the mask where the 
      segmented regions have been marked.  */
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;


  /** Image Type for the output of the Confidence Connected filter */
  typedef itk::Image< MaskPixelType,   Dimension > MaskImageType;



  typedef  itk::ConfidenceConnectedImageFilter< 
                                  InputImageType, 
                                  MaskImageType > ConnectedFilterType;


  typedef  itk::RescaleIntensityImageFilter<
                                  MaskImageType,
                                  InternalImageType > RescaleIntensityFilterType;


  typedef  itk::CastImageFilter<  InputImageType,
                                  InternalImageType  > CastFeatureFilterType;


  typedef  itk::ThresholdSegmentationLevelSetImageFilter< 
                                  InternalImageType, 
                                  InternalImageType > ThresholdLevelSetFilterType;


  typedef  itk::RescaleIntensityImageFilter<
                                  ThresholdLevelSetFilterType::OutputImageType,
                                  OutputImageType > OutputRescaleIntensityFilterType;

  /** Progress accumulator object */
  typedef itk::ProgressAccumulator::Pointer           AccumulatorPointer;


  typedef itk::Notifier                 NotifierType;            

public:
    
    ThresholdLevelSetSegmentationModule();

    virtual ~ThresholdLevelSetSegmentationModule();

    void SetInput( const InputImageType * image );

    const OutputImageType * GetOutput();

    void SetSeedPoint( int x, int y, int z );

    void Execute();

   int GetElapsedIterations( void );

   const OutputImageType * GetInitialSegmentationOutput();

  NotifierType*   GetNotifier( void );

  GetMacro( LowerThreshold, float );
  GetMacro( UpperThreshold, float );
  GetMacro( CurvatureScaling, float );
  GetMacro( PropagationScaling, float );
  GetMacro( AdvectionScaling, float );
  GetMacro( MaximumRMSError, float );
  GetMacro( MaximumIterations, int );

  GetMacro( Multiplier, float );
  GetMacro( InitialNeighborhoodRadius, float );
  GetMacro( NumberOfIterations, unsigned int );

  SetMacro( LowerThreshold, float );
  SetMacro( UpperThreshold, float );
  SetMacro( CurvatureScaling, float );
  SetMacro( PropagationScaling, float );
  SetMacro( AdvectionScaling, float );
  SetMacro( MaximumRMSError, float );
  SetMacro( MaximumIterations, unsigned int );

  SetMacro( Multiplier, float );
  SetMacro( InitialNeighborhoodRadius, unsigned int );
  SetMacro( NumberOfIterations, unsigned int );


private:

    ConnectedFilterType::Pointer                  m_ConfidenceConnectedFilter;

    RescaleIntensityFilterType::Pointer           m_RescaleIntensityFilter;

    ThresholdLevelSetFilterType::Pointer          m_ThresholdLevelSetFilter;

    OutputRescaleIntensityFilterType::Pointer     m_RescaleOutputFilter;

    CastFeatureFilterType::Pointer                m_CastFeatureFilter;
       
   /** Progress tracking object */
   AccumulatorPointer        m_ProgressAccumulator;

   NotifierType::Pointer             m_Notifier;

   /** Parameters for the Threshold filter */
   float          m_LowerThreshold, m_UpperThreshold;
   float          m_CurvatureScaling;
   float          m_PropagationScaling;
   float          m_AdvectionScaling;
   float          m_MaximumRMSError;
   unsigned int      m_MaximumIterations;
   /** Parameters for the ConfidenceConnected filter */
   float          m_Multiplier;
   unsigned int   m_NumberOfIterations;
   unsigned int   m_InitialNeighborhoodRadius;

};




} // end namespace ISIS


#endif




