#ifndef __AffineRegistrator_txx
#define __AffineRegistrator_txx

#include "AffineRegistrator.h"

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
  
  m_OptimizerMethod = GRADIENT;
  m_OptimizerNumberOfIterations = 100 ;
  m_OptimizerScales.set_size(15) ; 
  m_OptimizerScales[0] = 200; // rotations
  m_OptimizerScales[1] = 200;
  m_OptimizerScales[2] = 200;
  m_OptimizerScales[3] = 1;  // offset
  m_OptimizerScales[4] = 1;
  m_OptimizerScales[5] = 1;
  m_OptimizerScales[6] = 300;  // scale
  m_OptimizerScales[7] = 300;
  m_OptimizerScales[8] = 300;
  m_OptimizerScales[9] = 400; // skew
  m_OptimizerScales[10] = 400;
  m_OptimizerScales[11] = 400;
  m_OptimizerScales[12] = 400;
  m_OptimizerScales[13] = 400;
  m_OptimizerScales[14] = 400;
  
  this->SetMetric(MetricType::New());
  m_MetricNumberOfSpatialSamples = 40000 ;
  }

template< class TImage >
AffineRegistrator< TImage >
::~AffineRegistrator()
  {
  }

template< class TImage >
void
AffineRegistrator< TImage >
::SetOptimizerToOnePlusOne() 
  {
  m_OptimizerMethod = ONEPLUSONE;
  }

template< class TImage >
void
AffineRegistrator< TImage >
::SetOptimizerToGradient() 
  {
  m_OptimizerMethod = GRADIENT;
  }

template< class TImage >
void
AffineRegistrator< TImage >
::SetOptimizerToRegularGradient() 
  {
  m_OptimizerMethod = REGULARGRADIENT;
  }

template< class TImage >
void
AffineRegistrator< TImage >
::SetOptimizerToConjugateGradient() 
  {
  m_OptimizerMethod = CONJUGATEGRADIENT;
  }

template< class TImage >
void
AffineRegistrator< TImage >
::Initialize() throw(ExceptionObject)
  {
  this->GetInterpolator()->SetInputImage( this->GetMovingImage() ) ;

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
AffineRegistrator< TImage >
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
::PrintError(ExceptionObject &e)
{
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in AffineRegistrator:" << std::endl;
  std::cout << e << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
}

#endif //__AffineRegistrator_txx
