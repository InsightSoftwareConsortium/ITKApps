#ifndef __ImageRegistrationApp_txx
#define __ImageRegistrationApp_txx

#include "ImageRegistrationApp.h"

template< class TImage >
ImageRegistrationApp< TImage >
::ImageRegistrationApp()
  {
  m_LandmarkScales.resize(9) ; 
  m_LandmarkScales[0] = 500;
  m_LandmarkScales[1] = 500;
  m_LandmarkScales[2] = 500;
  m_LandmarkScales[3] = 200;
  m_LandmarkScales[4] = 200;
  m_LandmarkScales[5] = 200;
  m_LandmarkScales[6] = 10;
  m_LandmarkScales[7] = 10;
  m_LandmarkScales[8] = 10;
  m_LandmarkNumberOfIterations = 5000 ;
  m_LandmarkRegTransform = LandmarkRegTransformType::New() ;
  m_LandmarkRegTransform->SetIdentity() ;
  m_LandmarkAffineTransform = AffineTransformType::New() ;
  m_LandmarkAffineTransform->SetIdentity() ;
  m_LandmarkRegValid = false;
  
  m_RigidNumberOfIterations = 500 ;
  m_RigidFixedImageStandardDeviation = 0.4 ;
  m_RigidMovingImageStandardDeviation = 0.4 ;
  m_RigidNumberOfSpatialSamples = 50 ;
  m_RigidScales.resize(9);
  m_RigidScales[0] = 500;
  m_RigidScales[1] = 500;
  m_RigidScales[2] = 500;
  m_RigidScales[3] = 200;
  m_RigidScales[4] = 200;
  m_RigidScales[5] = 200;
  m_RigidScales[6] = 10;
  m_RigidScales[7] = 10;
  m_RigidScales[8] = 10;
  m_RigidRegTransform = RigidRegTransformType::New() ;
  m_RigidRegTransform->SetIdentity() ;
  m_RigidAffineTransform = AffineTransformType::New() ;
  m_RigidAffineTransform->SetIdentity() ;

  m_AffineNumberOfIterations = 500 ;
  m_AffineFixedImageStandardDeviation = 0.4 ;
  m_AffineMovingImageStandardDeviation = 0.4 ;
  m_AffineNumberOfSpatialSamples = 50 ;
  m_AffineScales.resize(15) ;
  m_AffineScales[0] = 500 ; // scale for M11
  m_AffineScales[1] = 500 ; // scale for M12
  m_AffineScales[2] = 500 ; // scale for M13
  m_AffineScales[3] = 500 ; // scale for M21
  m_AffineScales[4] = 500 ; // scale for M22
  m_AffineScales[5] = 500 ; // scale for M23
  m_AffineScales[6] = 500 ; // scale for M31
  m_AffineScales[7] = 500 ; // scale for M32
  m_AffineScales[8] = 500 ; // scale for M33
  m_AffineScales[9] =  200 ;// scale for translation on X
  m_AffineScales[10] = 200 ; // scale for translation on Y
  m_AffineScales[11] = 200 ; // scale for translation on Z
  m_AffineScales[12] = 10 ;// scale for translation on X
  m_AffineScales[13] = 10 ; // scale for translation on Y
  m_AffineScales[14] = 10 ; // scale for translation on Z
  m_AffineRegTransform = AffineRegTransformType::New() ;
  m_AffineRegTransform->SetIdentity();
  m_AffineAffineTransform = AffineTransformType::New();
  m_AffineAffineTransform->SetIdentity();

  m_FixedImage = NULL;
  m_MovingImage = NULL;
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
  m_LandmarkRegValid = false;
  }

template< class TImage >
void
ImageRegistrationApp< TImage >
::SetMovingImage(TImage * image)
  {
  m_MovingImage = image;
  m_LandmarkRegValid = false;
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

  try
    {
      registrator->StartRegistration();
    }
  catch( itk::ExceptionObject &e )
    {
      this->PrintError(e) ;
    }
  catch(...)
    {
      this->PrintUncaughtError() ;
    }
  
  m_LandmarkRegValid = true;
  m_LandmarkRegTransform = registrator->GetTransform() ;
  m_LandmarkAffineTransform->SetIdentity();
  m_LandmarkAffineTransform->SetMatrix(
                                   m_LandmarkRegTransform->GetRotationMatrix());
  m_LandmarkAffineTransform->SetOffset(m_LandmarkRegTransform->GetOffset());
  m_FinalTransform = m_LandmarkAffineTransform;

  std::cout << "DEBUG: Landmark registration FINAL centered versor transform: "
            << std::endl
            << m_LandmarkRegTransform->GetParameters() << std::endl ;
  std::cout << "DEBUG: Landmark registration FINAL affine transform: "
            << std::endl
            << m_FinalTransform->GetParameters() << std::endl  << std::endl;
  }


template< class TImage >
void
ImageRegistrationApp< TImage >
::RegisterUsingRigidMethod()
  {
  typename RigidRegistratorType::Pointer registrator = 
                                         RigidRegistratorType::New();

  registrator->SetMovingImage(m_MovingImage) ;
  registrator->SetFixedImage( m_FixedImage ) ;
  registrator->SetFixedImageRegion( m_FixedImageRegion ) ;
  registrator->SetOptimizerScales( m_RigidScales );
  registrator->SetOptimizerNumberOfIterations(m_RigidNumberOfIterations);

  registrator->SetMetricMovingImageStandardDeviation
               ( m_RigidMovingImageStandardDeviation );
  registrator->SetMetricFixedImageStandardDeviation
               ( m_RigidFixedImageStandardDeviation );
  registrator->SetMetricNumberOfSpatialSamples
               ( m_RigidNumberOfSpatialSamples );

  if(m_LandmarkRegValid)
    {
    registrator->SetInitialTransformParameters 
                 ( m_LandmarkRegTransform->GetParameters() );
    }
   else
    {
    typename TImage::SizeType size;
    typename TImage::IndexType fixedCenterIndex;
    itk::Point<double, 3> fixedCenterPoint;
    size = m_FixedImage->GetLargestPossibleRegion().GetSize();
    fixedCenterIndex[0] = size[0]/2;
    fixedCenterIndex[1] = size[1]/2;
    fixedCenterIndex[2] = size[2]/2;
    m_FixedImage->TransformIndexToPhysicalPoint(fixedCenterIndex,
                                                fixedCenterPoint);
    typename TImage::IndexType movingCenterIndex;
    itk::Point<double, 3> movingCenterPoint;
    size = m_MovingImage->GetLargestPossibleRegion().GetSize();
    movingCenterIndex[0] = size[0]/2;
    movingCenterIndex[1] = size[1]/2;
    movingCenterIndex[2] = size[2]/2;
    m_MovingImage->TransformIndexToPhysicalPoint(movingCenterIndex,
                                                movingCenterPoint);
    RigidParametersType params;
    params.resize(9);
    params.Fill(0);
    params[3] = movingCenterPoint[0];
    params[4] = movingCenterPoint[1];
    params[5] = movingCenterPoint[2];
    params[6] = fixedCenterPoint[0] - movingCenterPoint[0];
    params[7] = fixedCenterPoint[1] - movingCenterPoint[1];
    params[8] = fixedCenterPoint[2] - movingCenterPoint[2];
    registrator->SetInitialTransformParameters ( params );
    }

  std::cout << "DEBUG: rigid registration INITIAL centered versor transform: "
            << std::endl
            << registrator->GetInitialTransformParameters() << std::endl ;

  try
    {   
      registrator->StartRegistration();
    }
  catch(itk::ExceptionObject &e)
    {
      this->PrintError(e) ;
    }
  catch(...)
    {
      this->PrintUncaughtError() ;
    }

  m_RigidRegTransform = registrator->GetTransform();
  m_RigidAffineTransform->SetMatrix( m_RigidRegTransform->GetRotationMatrix());
  m_RigidAffineTransform->SetOffset(m_RigidRegTransform->GetOffset());
  m_FinalTransform = m_RigidAffineTransform;

  std::cout << "DEBUG: Rigid registration FINAL centered versor transform: " 
            << std::endl 
            << m_RigidRegTransform->GetParameters() << std::endl ;
  std::cout << "DEBUG: Rigid registration FINAL affine transform: "  
            << std::endl
            << m_FinalTransform->GetParameters() << std::endl  << std::endl;
  }


template< class TImage >
void
ImageRegistrationApp< TImage >
::RegisterUsingAffineMethod()
  {
  typename AffineRegistratorType::Pointer registrator = 
                                         AffineRegistratorType::New();

  registrator->SetMovingImage(m_MovingImage) ;
  registrator->SetFixedImage( m_FixedImage ) ;
  registrator->SetFixedImageRegion( m_FixedImageRegion ) ;
  registrator->SetOptimizerScales( m_AffineScales );
  registrator->SetOptimizerNumberOfIterations(m_AffineNumberOfIterations);

  registrator->SetMetricMovingImageStandardDeviation
               ( m_AffineMovingImageStandardDeviation );
  registrator->SetMetricFixedImageStandardDeviation
               ( m_AffineFixedImageStandardDeviation );
  registrator->SetMetricNumberOfSpatialSamples
               ( m_AffineNumberOfSpatialSamples );

  if(m_LandmarkRegValid)
    {
    m_AffineRegTransform->SetIdentity();
    m_AffineRegTransform->SetMatrix( 
                          m_LandmarkRegTransform->GetRotationMatrix());
    m_AffineRegTransform->SetTranslation(
                          m_LandmarkRegTransform->GetTranslation());
    m_AffineRegTransform->SetCenter(
                          m_LandmarkRegTransform->GetCenter());
    registrator->SetInitialTransformParameters 
                 ( m_AffineRegTransform->GetParameters() );
    }
  else
    {
    typename TImage::SizeType size;
    typename TImage::IndexType fixedCenterIndex;
    itk::Point<double, 3> fixedCenterPoint;
    size = m_FixedImage->GetLargestPossibleRegion().GetSize();
    fixedCenterIndex[0] = size[0]/2;
    fixedCenterIndex[1] = size[1]/2;
    fixedCenterIndex[2] = size[2]/2;
    m_FixedImage->TransformIndexToPhysicalPoint(fixedCenterIndex,
                                                fixedCenterPoint);
    typename TImage::IndexType movingCenterIndex;
    itk::Point<double, 3> movingCenterPoint;
    size = m_MovingImage->GetLargestPossibleRegion().GetSize();
    movingCenterIndex[0] = size[0]/2;
    movingCenterIndex[1] = size[1]/2;
    movingCenterIndex[2] = size[2]/2;
    m_MovingImage->TransformIndexToPhysicalPoint(movingCenterIndex,
                                                movingCenterPoint);
    m_AffineRegTransform->SetIdentity();
    AffineParametersType params = m_AffineRegTransform->GetParameters();
    params[9] = movingCenterPoint[0];
    params[10] = movingCenterPoint[1];
    params[11] = movingCenterPoint[2];
    params[12] = fixedCenterPoint[0] - movingCenterPoint[0];
    params[13] = fixedCenterPoint[1] - movingCenterPoint[1];
    params[14] = fixedCenterPoint[2] - movingCenterPoint[2];
    registrator->SetInitialTransformParameters ( params );
    }

  std::cout << "DEBUG: affine registration INITIAL centered versor transform: "
            << std::endl 
            << registrator->GetInitialTransformParameters() << std::endl ;

  try
    {   
      registrator->StartRegistration();
    }
  catch(itk::ExceptionObject &e)
    {
      this->PrintError(e) ;
    }
  catch(...)
    {
      this->PrintUncaughtError() ;
    }

  m_AffineRegTransform = registrator->GetTransform() ;
  m_AffineRegTransform->ComputeOffset();
  m_AffineAffineTransform->SetMatrix( m_AffineRegTransform->GetMatrix());
  m_AffineAffineTransform->SetOffset(m_AffineRegTransform->GetOffset());
  m_FinalTransform = m_AffineAffineTransform;

  std::cout << "DEBUG: Affine registration FINAL centered affine transform: " << std::endl
            << m_AffineRegTransform->GetParameters() << std::endl ;
  std::cout << "DEBUG: Affine registration FINAL affine transform: "  << std::endl
            << m_FinalTransform->GetParameters() << std::endl  << std::endl;
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
::PrintError(itk::ExceptionObject &e)
  {
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in ImageRegistrationApp:" << std::endl;
  std::cout << e << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
  }

#endif //__ImageRegistrationApp_txx
