#ifndef __RigidRegistrator_txx
#define __RigidRegistrator_txx

#include "RigidRegistrator.h"
#include "itkStatisticsImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"

template< class TImage >
RigidRegistrator< TImage >
::RigidRegistrator()
:ImageRegistrationMethod<TImage, TImage>()
  {
  this->SetTransform(TransformType::New());
  this->GetTransform()->SetIdentity();
  this->SetInitialTransformParameters(this->GetTransform()->GetParameters());

  this->SetInterpolator(InterpolatorType::New());

  this->SetOptimizer(OptimizerType::New());
  this->GetOptimizer()->MaximizeOn();
  m_OptimizerNumberOfIterations = 1000 ;
  m_OptimizerScales.resize(9) ; 
  m_OptimizerScales[0] = 500; // rotations
  m_OptimizerScales[1] = 500;
  m_OptimizerScales[2] = 500;
  m_OptimizerScales[3] = 200;  // center of rotation
  m_OptimizerScales[4] = 200;
  m_OptimizerScales[5] = 200;
  m_OptimizerScales[6] = 10;  // offset
  m_OptimizerScales[7] = 10;
  m_OptimizerScales[8] = 10;
  this->GetOptimizer()->SetNormalVariateGenerator(OptimizerNormalGeneratorType::New());

  typename MetricType::Pointer newMetric = MetricType::New();
  this->SetMetric(newMetric);
  m_MetricNumberOfSpatialSamples = 50 ;
  m_MetricMovingImageStandardDeviation = 0.4;
  m_MetricFixedImageStandardDeviation = 0.4;

  // Base class handles
  //m_FixedImage = NULL;
  //m_MovingImage = NULL;
  //m_FixedImageRegionDefined = false;
  m_MovingImageRegionDefined = false;
  }

template< class TImage >
RigidRegistrator< TImage >
::~RigidRegistrator()
  {
  }

template< class TImage >
void
RigidRegistrator< TImage >
::SetMovingImageRegion(RegionType & region)
  {
  m_MovingImageRegionDefined = true;
  m_MovimgImageRegion = region;
  }

template< class TImage >
void
RigidRegistrator< TImage >
::Initialize() throw (ExceptionObject)
  {
  try
    {
    Superclass::Initialize();
    }
  catch(ExceptionObject e)
    {
    throw(e);
    }
  
  std::cout << "RigidRegistrator: Initialize" << std::endl;

  // Base class handles
  // m_Interpolator->SetInputImage(m_MovingImage) ;

  this->GetOptimizer()->SetScales( m_OptimizerScales );
  this->GetOptimizer()->SetMaximumIteration(m_OptimizerNumberOfIterations);
  this->GetOptimizer()->Initialize(4.0, 1.1, 0.9); // Initial search radius
  this->GetOptimizer()->SetEpsilon(0.00000001);
  
  this->GetMetric()->SetNumberOfSpatialSamples( m_MetricNumberOfSpatialSamples );
  
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
  this->GetMetric()->SetMovingImageStandardDeviation( stdDev );

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
  this->GetMetric()->SetFixedImageStandardDeviation( stdDev );
  }

template< class TImage >
void
RigidRegistrator< TImage >
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
RigidRegistrator< TImage >
::PrintUncaughtError()
{
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in RigidRegistrator:" << std::endl;
  std::cout << "unknown exception caught !!!" << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
}

template< class TImage >
void
RigidRegistrator< TImage >
::PrintError(itk::ExceptionObject &e)
{
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in RigidRegistrator:" << std::endl;
  std::cout << e << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
}

#endif //__RigidRegistrator_txx
