#ifndef __AffineRegistrator_txx
#define __AffineRegistrator_txx

#include "AffineRegistrator.h"
#include "itkStatisticsImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"

template< class TImage >
AffineRegistrator< TImage >
::AffineRegistrator()
:ImageRegistrationMethod<TImage, TImage>()
  {
  this->SetTransform(TransformType::New());
  this->GetTypedTransform()->SetIdentity();
  this->SetInitialTransformParameters(
                  this->GetTypedTransform()->GetParameters());

  this->SetInterpolator(InterpolatorType::New());
  
  this->SetOptimizer(OptimizerType::New());
  this->GetTypedOptimizer()->MaximizeOn();
  m_OptimizerNumberOfIterations = 1000 ;
  m_OptimizerScales.resize(15) ; 
  m_OptimizerScales[0] = 500; // rotations
  m_OptimizerScales[1] = 500;
  m_OptimizerScales[2] = 500;
  m_OptimizerScales[3] = 500;  // center of rotation
  m_OptimizerScales[4] = 500;
  m_OptimizerScales[5] = 500;
  m_OptimizerScales[6] = 500;  // offset
  m_OptimizerScales[7] = 500;
  m_OptimizerScales[8] = 500;
  m_OptimizerScales[9] = 200;
  m_OptimizerScales[10] = 200;
  m_OptimizerScales[11] = 200;
  m_OptimizerScales[12] = 10;
  m_OptimizerScales[13] = 10;
  m_OptimizerScales[14] = 10;
  this->GetTypedOptimizer()->SetNormalVariateGenerator(
                             OptimizerNormalGeneratorType::New());
  
  this->SetMetric(MetricType::New());
  m_MetricNumberOfSpatialSamples = 50 ;
  m_MetricMovingImageStandardDeviation = 0.4;
  m_MetricFixedImageStandardDeviation = 0.4;

  // Base class handles
  // m_FixedImage = NULL;
  // m_MovingImage = NULL;
  // m_FixedImageRegionDefined = false;
  m_MovingImageRegionDefined = false;
  }

template< class TImage >
AffineRegistrator< TImage >
::~AffineRegistrator()
  {
  }

template< class TImage >
void
AffineRegistrator< TImage >
::SetMovingImageRegion(RegionType & region)
  {
  m_MovingImageRegionDefined = true;
  m_MovimgImageRegion = region;
  }
  
template< class TImage >
void
AffineRegistrator< TImage >
::Initialize() throw(ExceptionObject)
  {
  try
    {
    Superclass::Initialize();
    }
  catch(ExceptionObject e)
    {
    throw(e);
    }

  // Base class handles
  // m_Interpolator->SetInputImage(m_MovingImage) ;

  this->GetTypedOptimizer()->SetScales( m_OptimizerScales );
  this->GetTypedOptimizer()->SetMaximumIteration(m_OptimizerNumberOfIterations);
  this->GetTypedOptimizer()->Initialize(4.0, 1.1, 0.9); // Initial search radius
  this->GetTypedOptimizer()->SetEpsilon(0.00000001);

  this->GetTypedMetric()->SetNumberOfSpatialSamples( 
                          m_MetricNumberOfSpatialSamples );
  
  typedef itk::StatisticsImageFilter< TImage > StatsFilterType;
  typename StatsFilterType::Pointer statsFilter = StatsFilterType::New();
  typedef itk::RegionOfInterestImageFilter<TImage, TImage> ROIFilterType;
  typename ROIFilterType::Pointer roiFilter = ROIFilterType::New();
  if(this->GetMovingImageRegionDefined())
    {
    roiFilter->SetInput(this->GetMovingImage());
    roiFilter->SetRegionOfInterest(this->GetMovingImageRegion());
    statsFilter->SetInput(roiFilter->GetOutput());
    }
  else
    {
    statsFilter->SetInput(this->GetMovingImage());
    }
  statsFilter->Update();
  double stdDev;
  stdDev = m_MetricMovingImageStandardDeviation * statsFilter->GetSigma();
  this->GetTypedMetric()->SetMovingImageStandardDeviation( stdDev );

  if(this->GetFixedImageRegionDefined())
    {
    roiFilter->SetInput(this->GetFixedImage());
    roiFilter->SetRegionOfInterest(this->GetFixedImageRegion());
    statsFilter->SetInput(roiFilter->GetOutput());
    }
  else
    {
    statsFilter->SetInput(this->GetFixedImage());
    }
  statsFilter->Update();
  stdDev = m_MetricFixedImageStandardDeviation * statsFilter->GetSigma();
  this->GetTypedMetric()->SetFixedImageStandardDeviation( stdDev );
  }

template< class TImage >
void
AffineRegistrator< TImage >
::StartRegistration()
  {
  try
    {   
    Superclass::StartRegistration();
    }
  catch(itk::ExceptionObject &e)
    {
    this->PrintError(e) ;
    }
  catch(...)
    {
    this->PrintUncaughtError() ;
    }
  }

template< class TImage >
void
AffineRegistrator< TImage >
::PrintUncaughtError()
{
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in AffineRegistrator:" << std::endl;
  std::cout << "unknown exception caught !!!" << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
}

template< class TImage >
void
AffineRegistrator< TImage >
::PrintError(itk::ExceptionObject &e)
{
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in AffineRegistrator:" << std::endl;
  std::cout << e << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
}

#endif //__AffineRegistrator_txx
