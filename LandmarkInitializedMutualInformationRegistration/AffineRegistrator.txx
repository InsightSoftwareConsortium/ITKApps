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
  m_OptimizerNumberOfIterations = 100 ;
  m_OptimizerScales.resize(18) ; 
  m_OptimizerScales[0] = 200; // rotations
  m_OptimizerScales[1] = 200;
  m_OptimizerScales[2] = 200;
  m_OptimizerScales[3] = 10;  // center of rotation
  m_OptimizerScales[4] = 10;
  m_OptimizerScales[5] = 10;
  m_OptimizerScales[6] = 1;  // offset
  m_OptimizerScales[7] = 1;
  m_OptimizerScales[8] = 1;
  m_OptimizerScales[9] = 300;  // scale
  m_OptimizerScales[10] = 300;
  m_OptimizerScales[11] = 300;
  m_OptimizerScales[12] = 400; // skew
  m_OptimizerScales[13] = 400;
  m_OptimizerScales[14] = 400;
  m_OptimizerScales[15] = 400;
  m_OptimizerScales[16] = 400;
  m_OptimizerScales[17] = 400;
  
  this->GetTypedOptimizer()->SetNormalVariateGenerator(
                             OptimizerNormalGeneratorType::New());
  
  this->SetMetric(MetricType::New());
  m_MetricNumberOfSpatialSamples = 20000 ;

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
  this->GetInterpolator()->SetInputImage( this->GetMovingImage() ) ;

  this->GetTypedOptimizer()->SetScales( m_OptimizerScales );
  this->GetTypedOptimizer()->SetMaximumIteration(m_OptimizerNumberOfIterations);
  this->GetTypedOptimizer()->Initialize(4.0, 1.1, 0.9); // Initial search radius
  this->GetTypedOptimizer()->SetEpsilon(0.00000001);

  this->GetTypedMetric()->SetNumberOfSpatialSamples( 
                          m_MetricNumberOfSpatialSamples );

  try
    {
    Superclass::Initialize();
    }
  catch(ExceptionObject e)
    {
    throw(e);
    }
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
