/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DeformableRegistrator.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __DeformableRegistrator_txx
#define __DeformableRegistrator_txx

#include "DeformableRegistrator.h"

template< class TImage >
DeformableRegistrator< TImage >
::DeformableRegistrator()
:ImageRegistrationMethod<TImage, TImage>()
  {
  this->SetTransform(TransformType::New());
  
  this->SetInterpolator(InterpolatorType::New());
  
  m_OptimizerMethod = LBFGS;
  m_OptimizerNumberOfIterations = 1000 ;
  
  this->SetMetric(MetricType::New());
  m_MetricNumberOfSpatialSamples = 80000 ;

  m_NumberOfControlPoints = 8 ;
  
  m_Observer = 0;
  }

template< class TImage >
DeformableRegistrator< TImage >
::~DeformableRegistrator()
  {
  }

template< class TImage >
void
DeformableRegistrator< TImage >
::SetOptimizerToOnePlusOne() 
  {
  m_OptimizerMethod = ONEPLUSONE;
  }

template< class TImage >
void
DeformableRegistrator< TImage >
::SetOptimizerToLBFGS() 
  {
  m_OptimizerMethod = LBFGS;
  }

template< class TImage >
void
DeformableRegistrator< TImage >
::Initialize() throw(ExceptionObject)
  {
  this->GetInterpolator()->SetInputImage( this->GetMovingImage() ) ;
  this->GetTypedMetric()->SetNumberOfSpatialSamples( 
                          m_MetricNumberOfSpatialSamples );
  
    // Definition of the differents objects used by the B Spline Transform
  TransformType::RegionType bsplineRegion;
  TransformType::RegionType::SizeType gridSizeOnImage;
  TransformType::RegionType::SizeType gridBorderSize;
  TransformType::RegionType::SizeType totalGridSize;

  gridSizeOnImage.Fill( m_NumberOfControlPoints - 3 );  
  gridBorderSize.Fill( 3 );  // Border for spline order = 3 ( 1 lower, 2 upper )

  totalGridSize = gridSizeOnImage + gridBorderSize;

  bsplineRegion.SetSize( totalGridSize );

  TransformType::SpacingType spacing   = this->GetFixedImage()->GetSpacing();
  TransformType::OriginType  origin    = this->GetFixedImage()->GetOrigin();
  
  typename TImage::SizeType fixedImageSize = this->GetFixedImageRegion().GetSize();

  for(unsigned int r=0; r<3; r++)
    {
    spacing[r] *= floor( static_cast<double>(fixedImageSize[r] - 1)  / 
                         static_cast<double>(gridSizeOnImage[r] - 1) );
    origin[r]  -=  spacing[r]; 
    }

  this->GetTypedTransform()->SetGridSpacing( spacing );
  this->GetTypedTransform()->SetGridOrigin( origin );
  this->GetTypedTransform()->SetGridRegion( bsplineRegion );

  const unsigned int numberOfParameters =
               this->GetTypedTransform()->GetNumberOfParameters();
  
  ParametersType params( numberOfParameters );
  params.Fill( 0.0 );
  this->GetTypedTransform()->SetParameters( params );

  this->SetInitialTransformParameters( params );
    
  switch(m_OptimizerMethod)
    {
    case ONEPLUSONE:
      {
      OnePlusOneOptimizerType::Pointer opt = OnePlusOneOptimizerType::New();
      opt->SetNormalVariateGenerator( OptimizerNormalGeneratorType::New() );
      opt->SetMaximumIteration( m_OptimizerNumberOfIterations);
      opt->SetEpsilon(1e-10);
      opt->Initialize(1.01); // Initial search radius
      this->SetOptimizer(opt);
      if(m_Observer)
        {
        opt->AddObserver(itk::IterationEvent(), m_Observer);
        }
      break;
      }
    case LBFGS:
      {
      LBFGSOptimizerType::Pointer opt = LBFGSOptimizerType::New();
      LBFGSOptimizerType::BoundSelectionType boundSelect( numberOfParameters );
      LBFGSOptimizerType::BoundValueType upperBound( numberOfParameters );
      LBFGSOptimizerType::BoundValueType lowerBound( numberOfParameters );
      boundSelect.Fill( 0 );
      upperBound.Fill( 0.0 );
      lowerBound.Fill( 0.0 );
      opt->SetBoundSelection( boundSelect );
      opt->SetUpperBound( upperBound );
      opt->SetLowerBound( lowerBound );

      opt->SetCostFunctionConvergenceFactor( 1e+4 );
      opt->SetProjectedGradientTolerance( 1e-10 );
      opt->SetMaximumNumberOfIterations( m_OptimizerNumberOfIterations );
      opt->SetMaximumNumberOfEvaluations( m_OptimizerNumberOfIterations );
      opt->SetMaximumNumberOfCorrections( 10 );

      this->SetOptimizer(opt);
      if(m_Observer)
        {
        opt->AddObserver(itk::IterationEvent(), m_Observer);
        }
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
DeformableRegistrator< TImage >
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
DeformableRegistrator< TImage >
::PrintUncaughtError()
{
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in DeformableRegistrator:" << std::endl;
  std::cout << "unknown exception caught !!!" << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
}

template< class TImage >
void
DeformableRegistrator< TImage >
::PrintError(ExceptionObject &e)
{
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in DeformableRegistrator:" << std::endl;
  std::cout << e << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
}

#endif //__DeformableRegistrator_txx
