#ifndef __RigidRegistrator_txx
#define __RigidRegistrator_txx

#include "RigidRegistrator.h"

template< class TImage >
RigidRegistrator< TImage >
::RigidRegistrator()
:ImageRegistrationMethod<TImage, TImage>()
  {
  this->SetTransform(TransformType::New());
  this->GetTypedTransform()->SetIdentity();
  this->SetInitialTransformParameters(
                  this->GetTypedTransform()->GetParameters());

  this->SetInterpolator(InterpolatorType::New());

  this->SetOptimizer(OptimizerType::New());
  m_OptimizerNumberOfIterations = 100 ;
  m_OptimizerScales.set_size(6) ; 
  m_OptimizerScales[0] = 200; // rotations
  m_OptimizerScales[1] = 200;
  m_OptimizerScales[2] = 200;
  m_OptimizerScales[3] = 1;  // offset
  m_OptimizerScales[4] = 1;
  m_OptimizerScales[5] = 1;
  this->GetTypedOptimizer()->SetNormalVariateGenerator(
                OptimizerNormalGeneratorType::New());

  this->SetMetric(MetricType::New());
  m_MetricNumberOfSpatialSamples = 20000 ;

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
  m_MovingImageRegion = region;
  }

template< class TImage >
void
RigidRegistrator< TImage >
::Initialize() throw (ExceptionObject)
  {
  this->GetInterpolator()->SetInputImage(this->GetMovingImage()) ;

  this->GetTypedMetric()->SetNumberOfSpatialSamples( 
                          m_MetricNumberOfSpatialSamples );

  this->GetTypedOptimizer()->SetScales( m_OptimizerScales );
  this->GetTypedOptimizer()->SetMaximumIteration(m_OptimizerNumberOfIterations);
  this->GetTypedOptimizer()->Initialize(1.01); // Initial search radius
  this->GetTypedOptimizer()->SetEpsilon(0.0000000000001);

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
RigidRegistrator< TImage >
::StartRegistration()
  {
  try
    {   
    Superclass::StartRegistration();
    }
  catch(ExceptionObject &e)
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
::PrintError(ExceptionObject &e)
{
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in RigidRegistrator:" << std::endl;
  std::cout << e << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
}

#endif //__RigidRegistrator_txx
