/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageRegistrationConsoleBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef IMAGEREGISTRATIONCONSOLEBASE
#define IMAGEREGISTRATIONCONSOLEBASE

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include "itkCastImageFilter.h"
#include <itkResampleImageFilter.h>
#include <itkImageRegistrationMethod.h>

#include "itkImageToVTKImageFilter.h"  //file located in the directory
                    // InsightApplications/Auxiliary/vtk

// 
// files for vtk image processing
//
#include "vtkImageCast.h"

#include "itkVTKImageToImageFilter.h"



//
// files for noise addition
//
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkNormalVariateGenerator.h"


#include "itkNormalizeImageFilter.h" 

#include "itkNormalizedMutualInformationHistogramImageToImageMetric.h"
#include "itkMeanSquaresImageToImageMetric.h"

#include <itkRegularStepGradientDescentOptimizer.h>

#include "itkLinearInterpolateImageFunction.h"

#include "itkAffineTransform.h"
#include "itkCenteredRigid2DTransform.h"
#include "itkSimilarity2DTransform.h"


#include "CommandIterationUpdate.h"
/**
 * \brief ImageRegistrationConsoleBase class that instantiate
 * the elements required for a registration method without GUI
 *
 */
class ImageRegistrationConsoleBase
{
public:

 /** Identifiers for the different types of Metrics */
  typedef enum {
    mutualInformation,
  meanSquares
  } MetricIdentifier;

   /** Dimension of the images to be registered */ 
  enum { ImageDimension = 2 };

  /** Pixel type to be used internally during registration */
  typedef   float                            InternalPixelType;
  typedef   unsigned short              InputPixelType; 
    //may need to change to float if too much degeneration in reading input image

  typedef   unsigned char                         OutputPixelType;
  

  /** Type of Input Images*/
  typedef   itk::Image<InputPixelType,ImageDimension>     InputImageType;

  /** Type of the Internal image */
  typedef   itk::Image<InternalPixelType,ImageDimension>  InternalImageType;
  
  /** Type of the Moving image */
  typedef   itk::Image<InternalPixelType,ImageDimension>  MovingImageType;

  /** Type of the Fixed image */
  typedef   itk::Image<InternalPixelType,ImageDimension>  FixedImageType;

  typedef itk::Image< OutputPixelType, ImageDimension >   OutputImageType;
  
  typedef itk::CastImageFilter< 
                        InputImageType,
                        OutputImageType > CastFilterType;
                    
  /**  Registration methods to use */
  typedef   itk::ImageRegistrationMethod< MovingImageType,
                                          FixedImageType > 
                                                  ImageRegistrationMethodType;

  typedef itk::CenteredRigid2DTransform< double > TransformType;


  typedef   itk::ImageFileReader< InputImageType >    FixedImageReaderType;

  typedef   itk::ImageFileReader< InputImageType >    MovingImageReaderType;

  typedef  itk::ImageToVTKImageFilter<InputImageType>    ItkToVtkConnectorType;

  typedef  itk::VTKImageToImageFilter<InternalImageType>    VtkToItkConnectorType;

  typedef   itk::ImageFileWriter< OutputImageType >   ImageWriterType;
  
  typedef   itk::AffineTransform< double,
                                  ImageDimension >    AffineTransformType;

  typedef   itk::Statistics::NormalVariateGenerator   GeneratorType;
  typedef   itk::ImageRegionConstIterator<InternalImageType>  ConstIteratorType;
  typedef   itk::ImageRegionIterator<InternalImageType>       IteratorType;

  typedef   itk::NormalizedMutualInformationHistogramImageToImageMetric< 
                                  FixedImageType,
                                  MovingImageType 
                                         > MutualInformationMetricType;



  typedef itk::MeanSquaresImageToImageMetric< 
                                    FixedImageType, 
                                    MovingImageType >    MeanSquaresMetricType;



  typedef MutualInformationMetricType::ScalesType     ScalesType;


  typedef   itk::ResampleImageFilter< InputImageType,
                                      InputImageType>  ResampleFilterType;

  typedef   itk::ResampleImageFilter< InternalImageType,
                                      InternalImageType> InternalResampleFilterType;


  typedef   itk::LinearInterpolateImageFunction< 
                       InternalImageType, double >  InterpolatorType;

  typedef   CommandIterationUpdate::OptimizerType   OptimizerType;
  
  typedef   itk::NormalizeImageFilter< InternalImageType, InternalImageType > NormalizeFilterType;

public:
  ImageRegistrationConsoleBase();
  virtual ~ImageRegistrationConsoleBase();

  virtual void LoadFixedImage(void)=0;
  virtual void LoadFixedImage(const char * filename);
  virtual void LoadMovingImage(void)=0;
  virtual void LoadMovingImage(const char * filename);
  
  virtual void SaveRegisteredImage(void)=0;
  virtual void SaveRegisteredImage(const char * filename);
  
  virtual void ShowStatus(const char * text);

  virtual void Execute(void);
  virtual void Stop(void);

  virtual void GenerateRegisteredMovingImage(void);

  virtual void GenerateFixedImage(void);
  virtual void GenerateNormalizedInputMovingImage(void);
  virtual void GenerateTransformedMovingImage(void);
  virtual void GenerateMovingImage(void);

  virtual void SelectMutualInformationMetric(void);
  virtual void SelectMeanSquaresMetric(void);
    
protected:

  FixedImageReaderType::Pointer           m_FixedImageReader;

  MovingImageReaderType::Pointer          m_MovingImageReader;

  ItkToVtkConnectorType::Pointer      m_FixedImageItkToVtkConnector;

  ItkToVtkConnectorType::Pointer      m_MovingImageItkToVtkConnector;

  VtkToItkConnectorType::Pointer      m_FixedImageVtkToItkConnector;

  VtkToItkConnectorType::Pointer      m_MovingImageVtkToItkConnector;

  CastFilterType::Pointer                 m_Caster;

  ImageWriterType::Pointer                m_RegisteredImageWriter;

  NormalizeFilterType::Pointer        m_NormalizedInputMovingImageNormalizeFilter;

  NormalizeFilterType::Pointer        m_TransformedMovingImageNormalizeFilter;

  NormalizeFilterType::Pointer        m_MovingImageNormalizeFilter;

  NormalizeFilterType::Pointer        m_FixedImageNormalizeFilter;

  ResampleFilterType::Pointer             m_ResampleMovingImageFilter;

  InternalResampleFilterType::Pointer     m_ResampleNormalizedInputMovingImageFilter;
  
  AffineTransformType::Pointer            m_InputTransform;

  TransformType::Pointer          m_Transform;

  InterpolatorType::Pointer               m_InputInterpolator;

  InterpolatorType::Pointer               m_Interpolator;

  OptimizerType::Pointer          m_Optimizer;

  ImageRegistrationMethodType::Pointer    m_ImageRegistrationMethod;
  
  bool                                    m_FixedImageIsLoaded;

  bool                                    m_MovingImageIsLoaded;

  MetricIdentifier                        m_SelectedMetric;


  AffineTransformType::OutputVectorType   m_Offset;
  AffineTransformType::ScalarType         m_Angle;

  float                    m_MovingImageNoiseMean;
  float                    m_MovingImageNoiseStandardDeviation;

  unsigned short              m_NumberOfIterations;
  double                  m_MinimumStepLength;
  double                  m_MaximumStepLength;
  double                    m_RotationScale;
  double                  m_TranslationScale;
  double                  m_ScalingScale;
  
  unsigned int                m_MutualInformationNumberOfBins;

};



#endif
