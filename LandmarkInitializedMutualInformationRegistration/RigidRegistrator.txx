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

  m_OptimizerMethod = GRADIENT;
  m_OptimizerNumberOfIterations = 100 ;
  m_OptimizerScales.set_size(6) ; 
  m_OptimizerScales[0] = 200; // rotations
  m_OptimizerScales[1] = 200;
  m_OptimizerScales[2] = 200;
  m_OptimizerScales[3] = 1;  // offset
  m_OptimizerScales[4] = 1;
  m_OptimizerScales[5] = 1;

  this->SetMetric(MetricType::New());
  m_MetricNumberOfSpatialSamples = 20000 ;
  }

template< class TImage >
RigidRegistrator< TImage >
::~RigidRegistrator()
  {
  }

template< class TImage >
void
RigidRegistrator< TImage >
::SetOptimizerToOnePlusOne() 
  {
  m_OptimizerMethod = ONEPLUSONE;
  }

template< class TImage >
void
RigidRegistrator< TImage >
::SetOptimizerToGradient() 
  {
  m_OptimizerMethod = GRADIENT;
  }

template< class TImage >
void
RigidRegistrator< TImage >
::SetOptimizerToRegularGradient() 
  {
  m_OptimizerMethod = REGULARGRADIENT;
  }

template< class TImage >
void
RigidRegistrator< TImage >
::SetOptimizerToConjugateGradient() 
  {
  m_OptimizerMethod = CONJUGATEGRADIENT;
  }

template< class TImage >
void
RigidRegistrator< TImage >
::Initialize() throw (ExceptionObject)
  {
  this->GetInterpolator()->SetInputImage(this->GetMovingImage()) ;

  this->GetTypedMetric()->SetNumberOfSpatialSamples( 
                          m_MetricNumberOfSpatialSamples );

  switch(m_OptimizerMethod)
    {
    case ONEPLUSONE:
      {
      OnePlusOneOptimizerType::Pointer opt = OnePlusOneOptimizerType::New();
      opt->SetNormalVariateGenerator( OptimizerNormalGeneratorType::New() );
      opt->SetMaximumIteration( m_OptimizerNumberOfIterations);
      opt->SetEpsilon(0.0000000000001);
      opt->Initialize(1.01); // Initial search radius
      opt->SetScales( m_OptimizerScales );
      this->SetOptimizer(opt);
      break;
      }
    case GRADIENT:
      {
      GradientOptimizerType::Pointer opt = GradientOptimizerType::New();
      opt->SetMaximize(false);
      opt->SetLearningRate(0.001);
      opt->SetNumberOfIterations(m_OptimizerNumberOfIterations);
      opt->SetScales( m_OptimizerScales );
      this->SetOptimizer(opt);
      break;
      }
    case REGULARGRADIENT:
      {
      RegularGradientOptimizerType::Pointer opt;
      opt = RegularGradientOptimizerType::New();
      opt->SetMaximize(false);
      //opt->SetLearningRate(0.001);
      opt->SetNumberOfIterations(m_OptimizerNumberOfIterations);
      opt->SetScales( m_OptimizerScales );
      this->SetOptimizer(opt);
      break;
      }
    case CONJUGATEGRADIENT:
      {
      ConjugateGradientOptimizerType::Pointer opt;
      opt = ConjugateGradientOptimizerType::New();
      //opt->SetLearningRate(0.001);
      //opt->SetNumberOfIterations(m_OptimizerNumberOfIterations);
      opt->SetScales( m_OptimizerScales );
      this->SetOptimizer(opt);
      break;
      }
    }


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
