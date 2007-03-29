/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageRegistrationApp.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __ImageRegistrationApp_txx
#define __ImageRegistrationApp_txx

#include "ImageRegistrationApp.h"

#include "itkVersor.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"

#include "itkCommand.h"
#include "itkSingleValuedNonLinearOptimizer.h"

class ImageRegistrationAppViewer
: public itk::Command
{
  public :
    typedef ImageRegistrationAppViewer  Self;
    typedef itk::Command                Superclass;
    typedef itk::SmartPointer<Self>     Pointer;

    itkNewMacro( ImageRegistrationAppViewer );
    itkTypeMacro( ImageRegistrationAppViewer, itk::Command );
    
    typedef itk::SingleValuedNonLinearOptimizer  OptimizerType;

    itkSetMacro(DontShowParameters, bool);

    void Execute( itk::Object * caller, const itk::EventObject & event )
      {
      Execute( (const itk::Object *)caller, event );
      }
    void Execute( const itk::Object * object, const itk::EventObject & event )
      {
      if( typeid( event ) != typeid( itk::IterationEvent ) || object == NULL)
        {
        return;
        }

      const OptimizerType * opt = dynamic_cast<const OptimizerType *>(object);

      if(!m_DontShowParameters)
        {
        std::cout << opt->GetCurrentPosition() << " : "
                  << opt->GetValue( opt->GetCurrentPosition() )
                  << std::endl;
        }
      else
        {
        std::cout << opt->GetValue( opt->GetCurrentPosition() )
                  << std::endl;
        }
      }

    void Update()
      {
      this->Execute ( (const itk::Object *)NULL, itk::IterationEvent() );
      }

  protected:
    bool m_DontShowParameters;
    ImageRegistrationAppViewer() { m_DontShowParameters = false; };
    ~ImageRegistrationAppViewer() {};

};

template< class TImage >
ImageRegistrationApp< TImage >
::ImageRegistrationApp()
  {
  m_NoneRegTransform = NoneRegTransformType::New();
  m_NoneRegTransform->SetIdentity();
  m_NoneAffineTransform = AffineTransformType::New();
  m_NoneAffineTransform->SetIdentity();
  m_NoneRegValid = false;

  m_CenterRegTransform = CenterRegTransformType::New();
  m_CenterRegTransform->SetIdentity();
  m_CenterAffineTransform = AffineTransformType::New();
  m_CenterAffineTransform->SetIdentity();
  m_CenterRegValid = false;

  m_MassRegTransform = MassRegTransformType::New();
  m_MassRegTransform->SetIdentity();
  m_MassAffineTransform = AffineTransformType::New();
  m_MassAffineTransform->SetIdentity();
  m_MassRegValid = false;

  m_MomentRegTransform = MomentRegTransformType::New();
  m_MomentRegTransform->SetIdentity();
  m_MomentAffineTransform = AffineTransformType::New();
  m_MomentAffineTransform->SetIdentity();
  m_MomentRegValid = false;

  m_LandmarkScales.set_size(6) ; 
  m_LandmarkScales[0] = 200;
  m_LandmarkScales[1] = 200;
  m_LandmarkScales[2] = 200;
  m_LandmarkScales[3] = 1;
  m_LandmarkScales[4] = 1;
  m_LandmarkScales[5] = 1;
  m_LandmarkNumberOfIterations = 2000 ;
  m_LandmarkRegTransform = LandmarkRegTransformType::New() ;
  m_LandmarkRegTransform->SetIdentity() ;
  m_LandmarkAffineTransform = AffineTransformType::New() ;
  m_LandmarkAffineTransform->SetIdentity() ;
  m_LandmarkRegValid = false;
  
  m_LoadedRegTransform = LoadedRegTransformType::New();
  m_LoadedRegTransform->SetIdentity();
  m_LoadedAffineTransform = AffineTransformType::New();
  m_LoadedAffineTransform->SetIdentity();
  m_LoadedDeformableTransform = DeformableTransformType::New();
  m_LoadedRegValid = false;
  m_LoadedDefValid = false;

  m_RigidNumberOfIterations = 500 ;
  m_RigidNumberOfSpatialSamples = 20000 ;
  m_RigidScales.set_size(6);
  m_RigidScales[0] = 200;
  m_RigidScales[1] = 200;
  m_RigidScales[2] = 200;
  m_RigidScales[3] = 1;
  m_RigidScales[4] = 1;
  m_RigidScales[5] = 1;
  m_RigidRegTransform = RigidRegTransformType::New() ;
  m_RigidRegTransform->SetIdentity() ;
  m_RigidAffineTransform = AffineTransformType::New() ;
  m_RigidAffineTransform->SetIdentity() ;
  m_RigidRegValid = false;
  m_RigidMetricValue = 0;

  m_AffineNumberOfIterations = 500 ;
  m_AffineNumberOfSpatialSamples = 40000 ;
  m_AffineScales.set_size(15) ;
  m_AffineScales[0] = 200; // rotations
  m_AffineScales[1] = 200;
  m_AffineScales[2] = 200;
  m_AffineScales[3] = 1;  // offset
  m_AffineScales[4] = 1;
  m_AffineScales[5] = 1;
  m_AffineScales[6] = 300;  // scale
  m_AffineScales[7] = 300;
  m_AffineScales[8] = 300;
  m_AffineScales[9] = 400; // skew
  m_AffineScales[10] = 400;
  m_AffineScales[11] = 400;
  m_AffineScales[12] = 400;
  m_AffineScales[13] = 400;
  m_AffineScales[14] = 400;
  m_AffineRegTransform = AffineRegTransformType::New() ;
  m_AffineRegTransform->SetIdentity();
  m_AffineAffineTransform = AffineTransformType::New();
  m_AffineAffineTransform->SetIdentity();
  m_AffineRegValid = false;
  m_AffineMetricValue = 0;

  m_DeformableNumberOfIterations = 1000 ;
  m_DeformableNumberOfSpatialSamples = 80000 ;
  m_DeformableNumberOfControlPoints = 8 ;
  m_DeformableRegTransform = DeformableTransformType::New();
  m_DeformableRegValid = false;
  m_DeformableMetricValue = 0;
  
  m_FixedImage = NULL;
  m_MovingImage = NULL;

  m_PriorRegistrationMethod = NONE;
  m_OptimizerMethod = ONEPLUSONEPLUSGRADIENT;
  }

template< class TImage >
ImageRegistrationApp< TImage >
::~ImageRegistrationApp()
  {
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::SetFixedImage(TImage * image)
  {
  m_FixedImage = image;
  m_NoneRegValid = false;
  m_MassRegValid = false;
  m_MomentRegValid = false;
  m_LandmarkRegValid = false;
  m_LoadedRegValid = false;
  m_LoadedDefValid = false;
  m_RigidRegValid = false;
  m_AffineRegValid = false;
  m_PriorRegistrationMethod = NONE;
  m_OptimizerMethod = ONEPLUSONE;//PLUSGRADIENT;
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::SetMovingImage(TImage * image)
  {
  m_MovingImage = image;
  m_NoneRegValid = false;
  m_MassRegValid = false;
  m_MomentRegValid = false;
  m_LandmarkRegValid = false;
  m_LoadedRegValid = false;
  m_LoadedDefValid = false;
  m_RigidRegValid = false;
  m_AffineRegValid = false;
  m_PriorRegistrationMethod = NONE;
  m_OptimizerMethod = ONEPLUSONE;//PLUSGRADIENT;
  }


template< class TImage >
void
ImageRegistrationApp< TImage >
::InitRigidParameters(RigidParametersType & p, itk::Point<double, 3> & center)
  {
  p.Fill(0);
  center.Fill(0);

  switch(m_PriorRegistrationMethod)
    {
    default:
    case NONE:
      {
      if(m_NoneRegValid)
        {
        center = m_NoneRegTransform->GetCenter();
        }
      break;
      }
    case CENTER:
      {
      if(m_CenterRegValid)
        {
        p[3] = m_CenterRegTransform->GetTranslation()[0];
        p[4] = m_CenterRegTransform->GetTranslation()[1];
        p[5] = m_CenterRegTransform->GetTranslation()[2];
        center = m_CenterRegTransform->GetCenter();
        }
      break;
      }
    case MASS:
      {
      if(m_MassRegValid)
        {
        p[3] = m_MassRegTransform->GetTranslation()[0];
        p[4] = m_MassRegTransform->GetTranslation()[1];
        p[5] = m_MassRegTransform->GetTranslation()[2];
        center = m_MassRegTransform->GetCenter();
        }
      break;
      }
    case MOMENT:
      {
      if(m_MomentRegValid)
        {
        itk::Versor<double> v;
        v.Set(m_MomentRegTransform->GetMatrix());
        p[0] = v.GetX();
        p[1] = v.GetY();
        p[2] = v.GetZ();
        p[3] = m_MomentRegTransform->GetTranslation()[0];
        p[4] = m_MomentRegTransform->GetTranslation()[1];
        p[5] = m_MomentRegTransform->GetTranslation()[2];
        center = m_MomentRegTransform->GetCenter();
        }
      break;
      }
    case LANDMARK:
      {
      if(m_LandmarkRegValid)
        {
        LandmarkParametersType lmkParams;
        lmkParams = m_LandmarkRegTransform->GetParameters();
        p[0] = lmkParams[0];
        p[1] = lmkParams[1];
        p[2] = lmkParams[2];
        p[3] = lmkParams[3];
        p[4] = lmkParams[4];
        p[5] = lmkParams[5];
        center = m_LandmarkRegTransform->GetCenter();
        }
      break;
      }
    case LOADED:
      {
      if(m_LoadedRegValid)
        {
        itk::Versor<double> v;
        v.Set(m_LoadedRegTransform->GetMatrix());
        p[0] = v.GetX();
        p[1] = v.GetY();
        p[2] = v.GetZ();
        p[3] = m_LoadedRegTransform->GetTranslation()[0];
        p[4] = m_LoadedRegTransform->GetTranslation()[1];
        p[5] = m_LoadedRegTransform->GetTranslation()[2];
        center = m_LoadedRegTransform->GetCenter();
        }
      break;
      }
    case RIGID:
      {
      if(m_RigidRegValid)
        {
        RigidParametersType rigidParams;
        rigidParams = m_RigidRegTransform->GetParameters();
        p[0] = rigidParams[0];
        p[1] = rigidParams[1];
        p[2] = rigidParams[2];
        p[3] = rigidParams[3];
        p[4] = rigidParams[4];
        p[5] = rigidParams[5];
        center = m_RigidRegTransform->GetCenter();
        }
      break;
      }
    case AFFINE:
      {
      if(m_AffineRegValid)
        {
        AffineParametersType affineParams;
        affineParams = m_AffineRegTransform->GetParameters();
        p[0] = affineParams[0];
        p[1] = affineParams[1];
        p[2] = affineParams[2];
        p[3] = affineParams[3];
        p[4] = affineParams[4];
        p[5] = affineParams[5];
        center = m_AffineRegTransform->GetCenter();
        }
      break;
      }
    }
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::SetOptimizerToOnePlusOne()
  {
  m_OptimizerMethod = ONEPLUSONE;
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::SetOptimizerToGradient()
  {
  m_OptimizerMethod = GRADIENT;
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::SetOptimizerToOnePlusOnePlusGradient()
  {
  m_OptimizerMethod = ONEPLUSONEPLUSGRADIENT;
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::RegisterUsingNone()
  {
  m_NoneRegValid = true;
  m_NoneRegTransform->SetIdentity();
  m_NoneAffineTransform->SetIdentity();

  AffineParametersType params = m_NoneAffineTransform->GetParameters();
  itk::Point<double, 3> center;
  this->InitRigidParameters(params, center);
  m_NoneAffineTransform->SetCenter(center);

  m_FinalTransform = m_NoneAffineTransform;
  m_PriorRegistrationMethod = NONE;
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::RegisterUsingCenters()
  {
  typename CenterRegistratorType::Pointer registrator = 
                                            CenterRegistratorType::New();

  registrator->SetMovingImage ( m_MovingImage ) ; // Unlike Rigid and Affine
  registrator->SetFixedImage( m_FixedImage ) ; // Isa moving to fixed transform
  registrator->SetNumberOfMoments(0);

  m_PriorRegistrationMethod = NONE;

  try
    {
    registrator->StartRegistration();
    }
  catch( ExceptionObject &e )
    {
    this->PrintError(e) ;
    }
  catch(...)
    {
    this->PrintUncaughtError() ;
    }
  
  m_CenterRegValid = true;
  m_CenterRegTransform->SetParameters( registrator->GetTypedTransform()
                                                      ->GetParameters() ) ;
  m_CenterRegTransform->SetFixedParameters( registrator->GetTypedTransform()
                                                      ->GetFixedParameters() ) ;
  m_CenterAffineTransform->SetIdentity();
  m_CenterAffineTransform->SetCenter(m_CenterRegTransform->GetCenter());
  m_CenterAffineTransform->SetOffset(m_CenterRegTransform->GetOffset());
  m_FinalTransform = m_CenterAffineTransform;
  m_PriorRegistrationMethod = CENTER;

  //std::cout << "DEBUG: Center: FINAL affine transform: " << std::endl
            //<< m_FinalTransform->GetParameters() << std::endl
            //<< "   Offset = " << m_FinalTransform->GetOffset() << std::endl
            //<< "   Center = " << m_FinalTransform->GetCenter() << std::endl;
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::RegisterUsingMass()
  {
  typename MassRegistratorType::Pointer registrator = 
                                            MassRegistratorType::New();

  registrator->SetMovingImage( m_MovingImage ) ; // Unlike Rigid and Affine
  registrator->SetFixedImage( m_FixedImage ) ; // Isa moving to fixed transform
  registrator->SetNumberOfMoments(1);

  m_PriorRegistrationMethod = NONE;

  try
    {
    registrator->StartRegistration();
    }
  catch( ExceptionObject &e )
    {
    this->PrintError(e) ;
    }
  catch(...)
    {
    this->PrintUncaughtError() ;
    }
  
  m_MassRegValid = true;
  m_MassRegTransform->SetParameters( registrator->GetTypedTransform()
                                                      ->GetParameters() ) ;
  m_MassRegTransform->SetFixedParameters( registrator->GetTypedTransform()
                                                      ->GetFixedParameters() ) ;
  m_MassAffineTransform->SetIdentity();
  m_MassAffineTransform->SetCenter(m_MassRegTransform->GetCenter());
  m_MassAffineTransform->SetOffset(m_MassRegTransform->GetOffset());
  m_FinalTransform = m_MassAffineTransform;
  m_PriorRegistrationMethod = MASS;

  //std::cout << "DEBUG: Mass: FINAL affine transform: " << std::endl
            //<< m_FinalTransform->GetParameters() << std::endl
            //<< "   Offset = " << m_FinalTransform->GetOffset() << std::endl
            //<< "   Center = " << m_FinalTransform->GetCenter() << std::endl;
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::RegisterUsingMoments()
  {
  typename MomentRegistratorType::Pointer registrator = 
                                            MomentRegistratorType::New();

  registrator->SetMovingImage( m_MovingImage ) ; // Unlike Rigid & Affine
  registrator->SetFixedImage( m_FixedImage ) ; // isa moving to fixed transform
  registrator->SetNumberOfMoments(2);

  m_PriorRegistrationMethod = NONE;

  try
    {
    registrator->StartRegistration();
    }
  catch( ExceptionObject &e )
    {
    this->PrintError(e) ;
    }
  catch(...)
    {
    this->PrintUncaughtError() ;
    }
  
  m_MomentRegValid = true;
  m_MomentRegTransform->SetParameters( registrator->GetTypedTransform()
                                                      ->GetParameters() ) ;
  m_MomentRegTransform->SetFixedParameters( registrator->GetTypedTransform()
                                                      ->GetFixedParameters() ) ;
  m_MomentAffineTransform->SetIdentity();
  m_MomentAffineTransform->SetCenter(m_MomentRegTransform->GetCenter());
  m_MomentAffineTransform->SetMatrix(m_MomentRegTransform->GetMatrix());
  m_MomentAffineTransform->SetOffset(m_MomentRegTransform->GetOffset());
  m_FinalTransform = m_MomentAffineTransform;
  m_PriorRegistrationMethod = MOMENT;

  //std::cout << "DEBUG: Moment: FINAL affine transform: " << std::endl
            //<< m_FinalTransform->GetParameters() << std::endl
            //<< "   Offset = " << m_FinalTransform->GetOffset() << std::endl
            //<< "   Center = " << m_FinalTransform->GetCenter() << std::endl;
  }


template< class TImage >
void
ImageRegistrationApp< TImage >
::RegisterUsingLandmarks(LandmarkSetType* fixedImageLandmarks,
                         LandmarkSetType* movingImageLandmarks)
  {
  typename LandmarkRegistratorType::Pointer registrator = 
                                            LandmarkRegistratorType::New();

  registrator->SetFixedLandmarkSet( fixedImageLandmarks );
  registrator->SetMovingLandmarkSet( movingImageLandmarks );
  registrator->SetOptimizerScales( m_LandmarkScales );
  registrator->SetOptimizerNumberOfIterations( m_LandmarkNumberOfIterations );


  m_PriorRegistrationMethod = NONE;

  try
    {
      registrator->StartRegistration();
    }
  catch( ExceptionObject &e )
    {
      this->PrintError(e) ;
    }
  catch(...)
    {
      this->PrintUncaughtError() ;
    }
  
  m_LandmarkRegValid = true;
  m_LandmarkRegTransform->SetParameters( registrator->GetTypedTransform()
                                                      ->GetParameters() ) ;
  m_LandmarkRegTransform->SetFixedParameters( registrator->GetTypedTransform()
                                                      ->GetFixedParameters() ) ;
  m_LandmarkAffineTransform->SetIdentity();
  m_LandmarkAffineTransform->SetCenter(m_LandmarkRegTransform->GetCenter());
  m_LandmarkAffineTransform->SetMatrix(
                                   m_LandmarkRegTransform->GetRotationMatrix());
  m_LandmarkAffineTransform->SetOffset(m_LandmarkRegTransform->GetOffset());
  m_FinalTransform = m_LandmarkAffineTransform;
  m_PriorRegistrationMethod = LANDMARK;

  //std::cout << "DEBUG: Landmark: FINAL affine transform: " << std::endl
            //<< m_FinalTransform->GetParameters() << std::endl
            //<< "   Offset = " << m_FinalTransform->GetOffset() << std::endl
            //<< "   Center = " << m_FinalTransform->GetCenter() << std::endl;
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::SetLoadedTransform(const LoadedRegTransformType & tfm)
  {
  m_LoadedRegTransform->SetIdentity();
  m_LoadedRegTransform->SetCenter(tfm.GetCenter());
  m_LoadedRegTransform->SetMatrix(tfm.GetMatrix());
  m_LoadedRegTransform->SetOffset(tfm.GetOffset());

  m_LoadedAffineTransform->SetIdentity();
  m_LoadedAffineTransform->SetCenter(tfm.GetCenter());
  m_LoadedAffineTransform->SetMatrix(tfm.GetMatrix());
  m_LoadedAffineTransform->SetOffset(tfm.GetOffset());

  m_LoadedRegValid = true;
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::SetLoadedDeformableTransform(const DeformableTransformType & tfm)
  {
  m_LoadedDeformableTransform->SetGridRegion( tfm.GetGridRegion() );
  m_LoadedDeformableTransform->SetGridSpacing( tfm.GetGridSpacing() );
  m_LoadedDeformableTransform->SetGridOrigin( tfm.GetGridOrigin() );
  m_LoadedDeformableTransform->SetParameters(tfm.GetParameters());
  
  m_FinalParameters = tfm.GetParameters();
  m_LoadedDefValid = true;
  }
  
template< class TImage >
void
ImageRegistrationApp< TImage >
::CompositeLoadedTransform(const LoadedRegTransformType & tfm)
  {
  m_LoadedRegTransform->SetIdentity();
  m_LoadedRegTransform->SetCenter(tfm.GetCenter());
  m_LoadedRegTransform->SetMatrix(tfm.GetMatrix());
  m_LoadedRegTransform->SetOffset(tfm.GetOffset());

  m_LoadedAffineTransform->Compose(m_LoadedRegTransform, false);

  m_LoadedRegTransform->SetIdentity();
  m_LoadedRegTransform->SetCenter(m_LoadedAffineTransform->GetCenter());
  m_LoadedRegTransform->SetMatrix(m_LoadedAffineTransform->GetMatrix());
  m_LoadedRegTransform->SetOffset(m_LoadedAffineTransform->GetOffset());

  m_LoadedRegValid = true;
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::RegisterUsingLoadedTransform()
  {
  m_LoadedRegValid = true;
  m_FinalTransform = m_LoadedAffineTransform;
  
  m_PriorRegistrationMethod = LOADED;
  
  if(m_LoadedDefValid == true)
    {
    m_FinalDeformableTransform = m_LoadedDeformableTransform;
    m_PriorRegistrationMethod = DEFLOADED;
    }
    

  //std::cout << "DEBUG: Loaded: FINAL affine transform: " << std::endl
            //<< m_FinalTransform->GetParameters() << std::endl
            //<< "   Offset = " << m_FinalTransform->GetOffset() << std::endl
            //<< "   Center = " << m_FinalTransform->GetCenter() << std::endl;
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::RegisterUsingRigid()
  {
  typename RigidRegistratorType::Pointer registrator = 
                                         RigidRegistratorType::New();

  registrator->SetMovingImage( m_MovingImage ) ;   // ITK transforms the fixed 
  registrator->SetFixedImage( m_FixedImage ) ; //   image into the moving image
  registrator->SetFixedImageRegion( m_FixedImageRegion ) ;
  registrator->SetOptimizerScales( m_RigidScales );
  registrator->SetOptimizerNumberOfIterations(m_RigidNumberOfIterations);
  switch(m_OptimizerMethod)
    {
    case ONEPLUSONE:
      {
      registrator->SetOptimizerToOnePlusOne();
      break;
      }
    case GRADIENT:
      {
      registrator->SetOptimizerToGradient();
      break;
      }
    case ONEPLUSONEPLUSGRADIENT:
      {
      registrator->SetOptimizerToOnePlusOnePlusGradient();
      break;
      }
    }

  RigidParametersType params = m_RigidRegTransform->GetParameters();
  itk::Point<double, 3> center;
  this->InitRigidParameters(params, center);
  registrator->GetTypedTransform()->SetCenter(center);
  registrator->SetInitialTransformParameters(params);

  ImageRegistrationAppViewer::Pointer viewer =
    ImageRegistrationAppViewer::New();
  registrator->SetObserver( viewer );

  //std::cout << "DEBUG: Rigid: INITIAL versor transform: "
            //<< std::endl
            //<< registrator->GetInitialTransformParameters() << std::endl ;

  try
    {   
    registrator->StartRegistration();
    }
  catch(ExceptionObject &e)
    {
    this->PrintError(e) ;
    }
  catch(...)
    {
    this->PrintUncaughtError() ;
    }

  m_RigidRegValid = true;
  m_RigidRegTransform->SetParameters( registrator->GetTypedTransform()
                                                     ->GetParameters() );
  m_RigidRegTransform->SetFixedParameters( registrator->GetTypedTransform()
                                                     ->GetFixedParameters() );
  m_RigidMetricValue = registrator->GetTypedMetric()->GetValue(
                                      m_RigidRegTransform->GetParameters());
  m_RigidAffineTransform->SetIdentity();
  m_RigidAffineTransform->SetCenter(m_RigidRegTransform->GetCenter());
  m_RigidAffineTransform->SetMatrix( m_RigidRegTransform->GetRotationMatrix());
  m_RigidAffineTransform->SetOffset(m_RigidRegTransform->GetOffset());
  m_FinalTransform = m_RigidAffineTransform;
  m_PriorRegistrationMethod = RIGID;

  //std::cout << "DEBUG: Rigid: FINAL affine transform: "  << std::endl
            //<< m_FinalTransform->GetParameters() << std::endl
            //<< "   Offset = " << m_FinalTransform->GetOffset() << std::endl
            //<< "   Center = " << m_FinalTransform->GetCenter() << std::endl;
  }


template< class TImage >
void
ImageRegistrationApp< TImage >
::RegisterUsingAffine()
  {
  typename AffineRegistratorType::Pointer registrator = 
                                         AffineRegistratorType::New();

  registrator->SetMovingImage( m_MovingImage ) ; // ITK transforms fixed image
  registrator->SetFixedImage( m_FixedImage ) ;  // into the moving
  registrator->SetFixedImageRegion( m_FixedImageRegion ) ;
  registrator->SetOptimizerScales( m_AffineScales );
  registrator->SetOptimizerNumberOfIterations(m_AffineNumberOfIterations);

  switch(m_OptimizerMethod)
    {
    case ONEPLUSONE:
      {
      registrator->SetOptimizerToOnePlusOne();
      break;
      }
    case GRADIENT:
      {
      registrator->SetOptimizerToGradient();
      break;
      }
    case ONEPLUSONEPLUSGRADIENT:
      {
      registrator->SetOptimizerToOnePlusOnePlusGradient();
      break;
      }
    }

  AffineParametersType params = m_AffineRegTransform->GetParameters();
  itk::Point<double, 3> center;
  this->InitRigidParameters(params, center);
  params[6] = 1;
  params[7] = 1;
  params[8] = 1;
  registrator->GetTypedTransform()->SetCenter(center);
  registrator->SetInitialTransformParameters(params);

  ImageRegistrationAppViewer::Pointer viewer =
    ImageRegistrationAppViewer::New();
  registrator->SetObserver( viewer );

  //std::cout << "DEBUG: Affine: INITIAL versor/offset/scale/skew transform: "
            //<< std::endl 
            //<< registrator->GetInitialTransformParameters() << std::endl ;

  try
    {   
    registrator->StartRegistration();
    }
  catch(ExceptionObject &e)
    {
    this->PrintError(e) ;
    }
  catch(...)
    {
    this->PrintUncaughtError() ;
    }

  m_AffineRegTransform->SetParameters( registrator->GetTypedTransform()
                                                      ->GetParameters() ) ;
  m_AffineRegTransform->SetFixedParameters( registrator->GetTypedTransform()
                                                      ->GetFixedParameters() ) ;
  m_AffineMetricValue = registrator->GetTypedMetric()->GetValue(
                                      m_AffineRegTransform->GetParameters());
  m_AffineAffineTransform->SetIdentity();
  m_AffineAffineTransform->SetCenter(m_AffineRegTransform->GetCenter());
  m_AffineAffineTransform->SetMatrix(m_AffineRegTransform->GetMatrix());
  m_AffineAffineTransform->SetOffset(m_AffineRegTransform->GetOffset());
  m_FinalTransform = m_AffineAffineTransform;
  m_PriorRegistrationMethod = AFFINE;

  //std::cout << "DEBUG: Affine: FINAL affine transform: "  << std::endl
            //<< m_FinalTransform->GetParameters() << std::endl
            //<< "   Offset = " << m_FinalTransform->GetOffset() << std::endl
            //<< "   Center = " << m_FinalTransform->GetCenter() << std::endl;
  }

template< class TImage >
void 
ImageRegistrationApp< TImage >
::RegisterUsingDeformable()
{
  typename DeformableRegistratorType::Pointer registrator = 
                                         DeformableRegistratorType::New();

  registrator->SetFixedImage( m_FixedImage ) ;
  registrator->SetMovingImage( m_MovingImage ) ;

  //Update the ROI
  TImage::RegionType::SizeType size;
  TImage::RegionType region;
  
  TImage::RegionType::SizeType movingSize = 
                            m_MovingImage->GetLargestPossibleRegion().GetSize();
  TImage::RegionType::SizeType fixedSize = 
                            m_FixedImage->GetLargestPossibleRegion().GetSize();

  registrator->SetFixedImageRegion( m_FixedImageRegion );
  registrator->SetOptimizerNumberOfIterations( m_DeformableNumberOfIterations );
  registrator->SetNumberOfControlPoints( m_DeformableNumberOfControlPoints );

  registrator->GetTypedTransform()->SetBulkTransform( m_FinalTransform );
  
  registrator->SetOptimizerToLBFGS();
  
  ImageRegistrationAppViewer::Pointer viewer =
    ImageRegistrationAppViewer::New();
  viewer->SetDontShowParameters(true);
  registrator->SetObserver( viewer );

  //std::cout << "DEBUG: Deformable: "
            //<< std::endl 
            //<< registrator->GetInitialTransformParameters() << std::endl ;

  try
    {   
    registrator->StartRegistration();
    }
  catch(ExceptionObject &e)
    {
    this->PrintError(e) ;
    }
  catch(...)
    {
    this->PrintUncaughtError() ;
    }

  m_DeformableRegTransform->SetFixedParameters( registrator
                                                  ->GetTypedTransform()
                                                    ->GetFixedParameters() ) ;
  m_DeformableRegTransform->SetParametersByValue( registrator
                                             ->GetLastTransformParameters() );
  m_DeformableRegTransform->SetBulkTransform( m_FinalTransform );

  m_DeformableMetricValue = registrator->GetTypedMetric()->GetValue(
                                    m_DeformableRegTransform->GetParameters());
                                      
  m_FinalParameters = registrator->GetLastTransformParameters();

  m_FinalDeformableTransform = m_DeformableRegTransform;

  m_PriorRegistrationMethod = DEFORMABLE;
}

template< class TImage >
typename ImageRegistrationApp< TImage >::ImagePointer
ImageRegistrationApp< TImage >
::GetNoneRegisteredMovingImage()
  {
  return m_ResampleUsingTransform(this->GetNoneAffineTransform(),
                                  m_MovingImage, m_FixedImage);
  }

template< class TImage >
typename ImageRegistrationApp< TImage >::ImagePointer
ImageRegistrationApp< TImage >
::GetCenterRegisteredMovingImage()
  {
  return m_ResampleUsingTransform(this->GetCenterAffineTransform(),
                                  m_MovingImage, m_FixedImage);
  }

template< class TImage >
typename ImageRegistrationApp< TImage >::ImagePointer
ImageRegistrationApp< TImage >
::GetMassRegisteredMovingImage()
  {
  return m_ResampleUsingTransform(this->GetMassAffineTransform(),
                                  m_MovingImage, m_FixedImage);
  }

template< class TImage >
typename ImageRegistrationApp< TImage >::ImagePointer
ImageRegistrationApp< TImage >
::GetMomentRegisteredMovingImage()
  {
  return m_ResampleUsingTransform(this->GetMomentAffineTransform(),
                                  m_MovingImage, m_FixedImage);
  }

template< class TImage >
typename ImageRegistrationApp< TImage >::ImagePointer
ImageRegistrationApp< TImage >
::GetLandmarkRegisteredMovingImage()
  {
  return m_ResampleUsingTransform(this->GetLandmarkAffineTransform(),
                                  m_MovingImage, m_FixedImage);
  }

template< class TImage >
typename ImageRegistrationApp< TImage >::ImagePointer
ImageRegistrationApp< TImage >
::GetLoadedRegisteredMovingImage()
  {
  return m_ResampleUsingTransform(this->GetLoadedAffineTransform(),
                                  m_MovingImage, m_FixedImage);
  }

template< class TImage >
typename ImageRegistrationApp< TImage >::ImagePointer
ImageRegistrationApp< TImage >
::GetRigidRegisteredMovingImage()
  {
  return m_ResampleUsingTransform(this->GetRigidAffineTransform(),
                                  m_MovingImage, m_FixedImage);
  }

template< class TImage >
typename ImageRegistrationApp< TImage >::ImagePointer
ImageRegistrationApp< TImage >
::GetAffineRegisteredMovingImage()
  {
  return m_ResampleUsingTransform(this->GetAffineAffineTransform(),
                                  m_MovingImage, m_FixedImage);
  }

template< class TImage >
typename ImageRegistrationApp< TImage >::ImagePointer
ImageRegistrationApp< TImage >
::GetFinalRegisteredMovingImage()
  {
  if ((m_PriorRegistrationMethod == DEFORMABLE)
      || (m_PriorRegistrationMethod == DEFLOADED))
    {
    return m_ResampleUsingDeformableTransform( 
                                  this->GetFinalDeformableTransform(),
                                  m_MovingImage, m_FixedImage);
    }
  else
    {
    return m_ResampleUsingTransform(this->GetFinalTransform(),
                                  m_MovingImage, m_FixedImage);
    }
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::PrintUncaughtError()
  {
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in ImageRegistrationApp:" << std::endl;
  std::cout << "unknown exception caught !!!" << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::PrintError(ExceptionObject &e)
  {
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in ImageRegistrationApp:" << std::endl;
  std::cout << e << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
  }

template< class TImage >
typename ImageRegistrationApp< TImage >::ImagePointer
ImageRegistrationApp< TImage >
::m_ResampleUsingTransform(AffineTransformType * transform,
                           ImageType * moving, ImageType * fixed)
  {
  typedef itk::ResampleImageFilter<ImageType, ImageType>
          ResampleImageFilterType;
  typedef itk::LinearInterpolateImageFunction<ImageType, double>
          InterpolatorType;

  typename InterpolatorType::Pointer interpolator = 
      InterpolatorType::New();
  interpolator->SetInputImage(moving);
  
  typename ResampleImageFilterType::Pointer resample = 
    ResampleImageFilterType::New();
    
  resample->SetInput(moving);
  resample->SetInterpolator(interpolator.GetPointer());
  resample->SetSize(fixed->GetLargestPossibleRegion().GetSize());
  resample->SetOutputOrigin(fixed->GetOrigin());
  resample->SetOutputSpacing(fixed->GetSpacing());

  resample->SetTransform(transform);
  resample->Update();

  return resample->GetOutput();
  }

template< class TImage >
typename ImageRegistrationApp< TImage >::ImagePointer
ImageRegistrationApp< TImage >
::m_ResampleUsingDeformableTransform(DeformableTransformType * transform,
                                     ImageType * moving, ImageType * fixed)
  {
  typedef itk::ResampleImageFilter<ImageType, ImageType>
          ResampleImageFilterType;
  typedef itk::LinearInterpolateImageFunction<ImageType, double>
          InterpolatorType;

  typename InterpolatorType::Pointer interpolator = 
      InterpolatorType::New();
  
  
  interpolator->SetInputImage(moving);
  
  typename ResampleImageFilterType::Pointer resample = 
           ResampleImageFilterType::New();

  resample->SetTransform( transform );
  resample->SetInput(moving);
  resample->SetInterpolator(interpolator.GetPointer());

  resample->SetSize(fixed->GetLargestPossibleRegion().GetSize());
  resample->SetOutputOrigin(fixed->GetOrigin());
  resample->SetOutputSpacing(fixed->GetSpacing());
  resample->SetDefaultPixelValue( 0 );

  try
    {
    resample->Update();
    }
  catch(ExceptionObject &e)
    {
    this->PrintError(e) ;
    }
  catch(...)
    {
    this->PrintUncaughtError() ;
    }
    
  
  return resample->GetOutput();
  }

    
#endif //__ImageRegistrationApp_txx
