#ifndef __ImageRegistrationApp_txx
#define __ImageRegistrationApp_txx

#include "ImageRegistrationApp.h"

template< class TImage, class TRealImage >
ImageRegistrationApp< TImage, TRealImage >
::ImageRegistrationApp()
{
  m_RigidNumberOfIterations = 200 ;
  m_RigidLearningRate = 0.0001 ;
  m_RigidFixedImageStandardDeviation = 0.4 ;
  m_RigidMovingImageStandardDeviation = 0.4 ;
  m_RigidNumberOfSpatialSamples = 50 ;

  m_RigidScales.resize(7) ;
  m_RigidScales.Fill(100);
  m_RigidScales[0] = 1;
  m_RigidScales[1] = 1;
  m_RigidScales[2] = 1;
  m_RigidScales[3] = 10000000; // no scale

  m_AffineNumberOfIterations = 200 ;
  m_AffineLearningRate = 0.0001 ;
  m_AffineFixedImageStandardDeviation = 0.4 ;
  m_AffineMovingImageStandardDeviation = 0.4 ;
  m_AffineNumberOfSpatialSamples = 50 ;

  m_AffineScales.resize(12) ;
  m_AffineScales[0] = 1 ; // scale for M11
  m_AffineScales[1] = 1 ; // scale for M12
  m_AffineScales[2] = 1 ; // scale for M13
  m_AffineScales[3] = 1 ; // scale for M21
  m_AffineScales[4] = 1 ; // scale for M22
  m_AffineScales[5] = 1 ; // scale for M23
  m_AffineScales[6] = 1 ; // scale for M31
  m_AffineScales[7] = 1 ; // scale for M32
  m_AffineScales[8] = 1 ; // scale for M33
  
  m_AffineScales[9] = 0.0001 ;// scale for translation on X
  m_AffineScales[10] = 0.0001 ; // scale for translation on Y
  m_AffineScales[11] = 0.0001 ; // scale for translation on Z

  m_AffineTransform = AffineTransformType::New() ;

  m_LandmarkTransform = AffineTransformType::New() ;
  m_CenteredLandmarkTransform = AffineTransformType::New() ;

  m_LandmarkTransform->SetIdentity() ;
  m_CenteredLandmarkTransform->SetIdentity() ;
  
  m_LandmarkRigidTransform = RigidTransformType::New() ;
  m_CenteredLandmarkRigidTransform = RigidTransformType::New() ;

  RigidTransformType::ParametersType params = 
    RigidTransformType::ParametersType(7);
  params.Fill( 0.0 );
  params[3] = 1.0;
  
  m_LandmarkRigidTransform->SetParameters(params) ;
  m_CenteredLandmarkRigidTransform->SetParameters(params) ;

  // for multi-resolution registration
  m_FixedImagePyramid = ImagePyramidType::New() ;
  m_MovingImagePyramid = ImagePyramidType::New() ;

//   m_FixedImageNormalizer = NormalizerType::New() ;
//   m_MovingImageNormalizer = NormalizerType::New() ;
//   m_FixedImageCenterer = CentererType::New() ;
//   m_MovingImageCenterer = CentererType::New() ;

  //  m_AffineTransform = AffineTransformType::New() ;
//   std::cout << "BUG: ImageRegistrationApp constructor = " 
//             << m_AffineTransform << std::endl ;
}

template< class TImage, class TRealImage >
ImageRegistrationApp< TImage, TRealImage >
::~ImageRegistrationApp()
{
}

template< class TImage, class TRealImage >
void
ImageRegistrationApp< TImage, TRealImage >
::NormalizeAndCenter()
{
  typename NormalizerType::Pointer fixedImageNormalizer = 
    NormalizerType::New() ;
  typename NormalizerType::Pointer movingImageNormalizer = 
    NormalizerType::New() ; 
  typename CentererType::Pointer fixedImageCenterer = CentererType::New() ;
  typename CentererType::Pointer movingImageCenterer = CentererType::New() ;

  fixedImageNormalizer->SetInput( m_FixedImage );
  fixedImageCenterer->SetInput( fixedImageNormalizer->GetOutput() );
  fixedImageCenterer->CenterImageOn();
  fixedImageCenterer->Update();
  
  m_RealFixedImage = fixedImageCenterer->GetOutput() ;
  
  movingImageNormalizer->SetInput( m_MovingImage );
  movingImageCenterer->SetInput( movingImageNormalizer->GetOutput() );
  movingImageCenterer->CenterImageOn();
  movingImageCenterer->Update();
  
  m_RealMovingImage = movingImageCenterer->GetOutput() ;
  
  for ( unsigned int i = 0 ; i < TImage::ImageDimension ; i++ )
    {
      m_FixedImageOffset[i] = m_RealFixedImage->GetOrigin()[i] ;
      m_MovingImageOffset[i] = m_RealMovingImage->GetOrigin()[i] ;
    }

//   typename AffineTransformType::OffsetType offset = 
//     m_LandmarkTransform->GetOffset() ;
  
//   typename RigidTransformType::OffsetType offsetRigid =
//     m_LandmarkRigidTransform->GetOffset() ;

//   std::cout << "DEBUG: offsets before centering" << std::endl ;
//   std::cout << offset << std::endl ;
//   std::cout << offsetRigid << std::endl ;
//   for ( unsigned int i = 0 ; i < TImage::ImageDimension ; i++ )
//     {
//       offset[i] += m_FixedImageOffset[i] - m_MovingImageOffset[i] ;
//       offsetRigid[i] += m_FixedImageOffset[i] - m_MovingImageOffset[i] ;
//     }

//   std::cout << "DEBUG: offsets after centering" << std::endl ;
//   std::cout << offset << std::endl ;
//   std::cout << offsetRigid << std::endl ;
  
//   for ( unsigned int i = 0 ; i < TImage::ImageDimension ; i++ )
//     {
//       offset[i] = m_FixedImageOffset[i] + 
//         m_LandmarkTransform->GetOffset()[i] - m_MovingImageOffset[i] ;
//       offsetRigid[i] = m_FixedImageOffset[i] + 
//         m_LandmarkRigidTransform->GetOffset()[i] - m_MovingImageOffset[i] ;
//     }
//   m_CenteredLandmarkTransform->SetParameters
//     (m_LandmarkTransform->GetParameters()) ;
//   m_CenteredLandmarkTransform->SetOffset(offset) ;

//   std::cout << "DEBUG: landmark transform ===== " << std::endl ;
//   std::cout << m_LandmarkTransform << std::endl ;

//   std::cout << "DEBUG: landmark transform ===== " << std::endl ;
//   std::cout << m_LandmarkRigidTransform << std::endl ;

//   std::cout << "DEBUG: centered landmark transform ===== " << std::endl ;
//   std::cout << m_CenteredLandmarkTransform << std::endl ;

  AffineTransformType::Pointer tempMoving = AffineTransformType::New() ;
  tempMoving->SetIdentity() ;
  tempMoving->SetOffset(m_MovingImageOffset) ;
  AffineTransformType::Pointer tempFixed = AffineTransformType::New() ;
  tempFixed->SetIdentity() ;
  tempFixed->SetOffset(m_FixedImageOffset) ;
  AffineTransformType::Pointer tempResult = AffineTransformType::New() ;
  tempResult->SetIdentity() ;
  tempResult->Compose(tempMoving->Inverse(), true) ;
  tempResult->Compose(m_LandmarkTransform, false) ;
  tempResult->Compose(tempFixed, false) ;

  m_CenteredLandmarkTransform->SetParameters(tempResult->GetParameters()) ;

  m_CenteredLandmarkRigidTransform->SetRotation
    (m_LandmarkRigidTransform->GetRotation()) ;
  m_CenteredLandmarkRigidTransform->SetOffset
    (m_CenteredLandmarkTransform->GetOffset()) ;

//   // debug centering
//   typename AffineTransformType::Pointer tempTransform = AffineTransformType::New() ;
//   tempTransform->SetMatrix (m_CenteredLandmarkRigidTransform->GetRotationMatrix());
//   tempTransform->SetOffset(m_CenteredLandmarkRigidTransform->GetOffset());

//   typedef itk::ImageFileWriter< TRealImage > WriterType ;
//   typename WriterType::Pointer writer = WriterType::New() ;
//   writer->SetFileName("fixed_image.mhd") ;
//   writer->SetInput(m_RealFixedImage) ;
//   writer->Update() ;
  
//   typedef itk::ResampleImageFilter< TRealImage, TRealImage > 
//     ResampleImageFilterType ;
//   // resample the moving image 
//   typename InterpolatorType::Pointer interpolator = 
//     InterpolatorType::New();
//   interpolator->SetInputImage(m_RealMovingImage);
  
//   typename ResampleImageFilterType::Pointer resample = 
//     ResampleImageFilterType::New();
//   resample->SetInput(m_RealMovingImage.GetPointer());
//   resample->SetInterpolator(interpolator.GetPointer());
//   resample->SetSize(m_RealMovingImage->GetLargestPossibleRegion().GetSize());
//   resample->SetOutputOrigin(m_RealMovingImage->GetOrigin());
//   resample->SetOutputSpacing(m_RealMovingImage->GetSpacing());
//   resample->SetTransform(tempTransform);
//   resample->Update();
  
//   typename WriterType::Pointer writer2 = WriterType::New() ;
//   writer2->SetFileName("moving_image.mhd") ;
//   writer2->SetInput(resample->GetOutput()) ;
//   writer2->Update() ;
}

// template< class TImage, class TRealImage >
// void
// ImageRegistrationApp< TImage, TRealImage >
// ::NormalizeAndCenter(AffineTransformType* initialAffineTransform,
//                      AffineTransformType* outputTransform,
//                      OffsetType& fixedImageOffset,
//                      OffsetType& movingImageOffset)
// {
//   m_FixedImageNormalizer->SetInput( m_FixedImage );
//   m_FixedImageCenterer->SetInput( m_FixedImageNormalizer->GetOutput() );
//   m_FixedImageCenterer->CenterImageOn();
//   m_FixedImageCenterer->Update();
  
//   m_RealFixedImage = m_FixedImageCenterer->GetOutput() ;
  
//   std::cout << "DEBUG: normcenter fixed image output2 = " 
//             << m_RealFixedImage << std::endl ;

//   m_MovingImageNormalizer->SetInput( m_MovingImage );
//   m_MovingImageCenterer->SetInput( m_MovingImageNormalizer->GetOutput() );
//   m_MovingImageCenterer->CenterImageOn();
//   m_MovingImageCenterer->Update();
  
//   m_RealMovingImage = m_MovingImageCenterer->GetOutput() ;
  
//   std::cout << "DEBUG: normcenter moving image output2 = " 
//             << m_RealMovingImage << std::endl ;

//   std::cout << "DEBUG: image centered." << std::endl ;

//   for ( unsigned int i = 0 ; i < TImage::ImageDimension ; i++ )
//     {
//       fixedImageOffset[i] = m_RealFixedImage->GetOrigin()[i] ;
//       movingImageOffset[i] = m_RealMovingImage->GetOrigin()[i] ;
//     }

//   std::cout << "DEBUG: NORMCENTER adjusting offset" << std::endl ;

//   typename AffineTransformType::OffsetType offset = 
//     initialAffineTransform->GetOffset() ;

//   for ( unsigned int i = 0 ; i < TImage::ImageDimension ; i++ )
//     {
//       offset += fixedImageOffset[i] - movingImageOffset[i] ;
//     }
  
//   std::cout << "DEBUG: NORMCENTER set offset" << std::endl ;
//   outputTransform->SetOffset(offset) ;
//   std::cout << "DEBUG: NORMCENTER end" << std::endl ;

// //   if ( m_UseMultiResolution )
// //     {
// //       this->GenerateImagePyramid( m_CenteredNormalizedFixedImage, 
// //                                   m_FixedImagePyramid, 
// //                                   m_NumberOfLevels,
// //                                   m_FixedImageShrinkFactors ) ;
        
// //       this->GenerateImagePyramid( m_CenteredNormalizedMovingImage, 
// //                                   m_MovingImagePyramid, 
// //                                   m_NumberOfLevels,
// //                                   m_MovingImageShrinkFactors ) ;
// //     }

// //   if ( m_UseDeformableRegistration )
// //     {
// //       this->MatchIntensity( m_NormalizedFixedImage, m_NormalizedMovingImage,
// //                            m_MatchedNormalizedMovingImage ) ;

// //       this->GenerateImagePyramid( m_NormalizedFixedImage, 
// //                                   m_MatchedFixedImagePyramid, 
// //                                   m_NumberOfLevels,
// //                                   m_FixedImageShrinkFactors ) ;
        
// //       this->GenerateImagePyramid( m_MatchedNormalizedMovingImage, 
// //                                   m_MatchedMovingImagePyramid, 
// //                                   m_NumberOfLevels,
// //                                   m_MovingImageShrinkFactors ) ;
// //     }
// }

template< class TImage, class TRealImage >
void
ImageRegistrationApp< TImage, TRealImage >
::CenterLandmarks(LandmarksType* fixedImageLandmarks,
                  LandmarksType* movingImageLandmarks,
                  LandmarksType* centeredFixedImageLandmarks,
                  LandmarksType* centeredMovingImageLandmarks,
                  PointType& fixedImageLandmarkCenter,
                  PointType& movingImageLandmarkCenter)
{
  std::cout << "DEBUG: CenterLandmarks() " << std::endl ;
  PointType point;

  point.Fill(0);
  
  fixedImageLandmarkCenter.Fill(0);
  movingImageLandmarkCenter.Fill(0);
  
  // calculate the center of landmarks
  
  for( unsigned int i = 0 ; i < fixedImageLandmarks->Size() ; i++ )
    {
      for ( unsigned int j = 0 ; j < PointType::PointDimension ; j++ )
        {
          fixedImageLandmarkCenter[j] += fixedImageLandmarks->GetElement(i)[j];
          movingImageLandmarkCenter[j] += movingImageLandmarks->GetElement(i)[j];
        }
    }
  
  for ( unsigned int j = 0 ; j < PointType::PointDimension ; j++ )
    {
      fixedImageLandmarkCenter[j] = fixedImageLandmarkCenter[j] / 
        fixedImageLandmarks->Size() ;
      movingImageLandmarkCenter[j] = movingImageLandmarkCenter[j] / 
        fixedImageLandmarks->Size() ;
    }
  
  
  // center both pointset on (0,0,0)...
  
  for( unsigned int i = 0 ; i < fixedImageLandmarks->Size() ; i++ )
    {
      for ( unsigned int j = 0 ; j < PointType::PointDimension ; j++ )
        {
          point[j] = fixedImageLandmarks->GetElement(i)[j] - 
            fixedImageLandmarkCenter[j];
        }
      
      centeredFixedImageLandmarks->InsertElement(i,point);
      
      for ( unsigned int j = 0 ; j < PointType::PointDimension ; j++ )
        {
          point[j] = movingImageLandmarks->GetElement(i)[j] - 
            movingImageLandmarkCenter[j];
        }
      
      centeredMovingImageLandmarks->InsertElement(i,point);
    }
}

template< class TImage, class TRealImage >
void
ImageRegistrationApp< TImage, TRealImage >
::RegisterUsingLandmarks(LandmarksType* fixedImageLandmarks,
                         LandmarksType* movingImageLandmarks,
                         LandmarkScalesType& scales,
                         unsigned int numberOfIterations)
{
  typename LandmarksType::Pointer centeredFixedImageLandmarks = 
    LandmarksType::New() ;
  typename LandmarksType::Pointer centeredMovingImageLandmarks =
    LandmarksType::New() ;

  PointType fixedImageLandmarkCenter ;
  PointType movingImageLandmarkCenter ;

  // center landmarks

  PointType point;

  point.Fill(0);
  
  fixedImageLandmarkCenter.Fill(0);
  movingImageLandmarkCenter.Fill(0);
  
  // calculate the center of landmarks
//   std::cout << "DEBUG: REGLAND # of landmarks = " 
//             << fixedImageLandmarks->Size() << " "
//             << movingImageLandmarks->Size() << std::endl ;

  for( unsigned int i = 0 ; i < fixedImageLandmarks->Size() ; i++ )
    {
      for ( unsigned int j = 0 ; j < PointType::PointDimension ; j++ )
        {
          fixedImageLandmarkCenter[j] += fixedImageLandmarks->GetElement(i)[j];
          movingImageLandmarkCenter[j] += movingImageLandmarks->GetElement(i)[j];
        }
    }

  for ( unsigned int j = 0 ; j < PointType::PointDimension ; j++ )
    {
      fixedImageLandmarkCenter[j] = fixedImageLandmarkCenter[j] / 
        fixedImageLandmarks->Size() ;
      movingImageLandmarkCenter[j] = movingImageLandmarkCenter[j] / 
        fixedImageLandmarks->Size() ;
    }
  
//   std::cout << "DEBUG: center of fixed image landmarks = " 
//             << fixedImageLandmarkCenter << std::endl ;

//   std::cout << "DEBUG: center of moving image landmarks = " 
//             << movingImageLandmarkCenter << std::endl ;

  
  // center both pointset on (0,0,0)...
  
  for( unsigned int i = 0 ; i < fixedImageLandmarks->Size() ; i++ )
    {
      for ( unsigned int j = 0 ; j < PointType::PointDimension ; j++ )
        {
          point[j] = fixedImageLandmarks->GetElement(i)[j] - 
            fixedImageLandmarkCenter[j];
        }
      
      centeredFixedImageLandmarks->InsertElement(i,point);
      
      for ( unsigned int j = 0 ; j < PointType::PointDimension ; j++ )
        {
          point[j] = movingImageLandmarks->GetElement(i)[j] - 
            movingImageLandmarkCenter[j];
        }
      
      centeredMovingImageLandmarks->InsertElement(i,point);

//       std::cout << "DEBUG: centered image landmark  " << std::endl ;
//       std::cout << centeredFixedImageLandmarks->GetElement(i) << ", " 
//                 << centeredMovingImageLandmarks->GetElement(i) << std::endl ;
    }
//   this->CenterLandmarks(fixedImageLandmarks, movingImageLandmarks,
//                         centeredFixedImageLandmarks.GetPointer(),
//                         centeredMovingImageLandmarks.GetPointer(),
//                         fixedImageLandmarkCenter,
//                         movingImageLandmarkCenter) ;

  typename LandmarkRegistratorType::Pointer registrator = 
    LandmarkRegistratorType::New();

  DoubleArrayType initialPosition(7);
  initialPosition.Fill(0);
  initialPosition[3]=1;

  registrator->SetInitialPosition(initialPosition) ;
  registrator->SetFixedPointSet( fixedImageLandmarks );
  registrator->SetMovingPointSet( movingImageLandmarks );
  registrator->SetInput( m_FixedImage );
  registrator->SetScales( scales );
  registrator->SetNumberOfIterations( numberOfIterations );
  
  std::cout << "DEBUG: start registration ."
            << std::endl ;
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
  
  // compute the pre-registration transform, which is composed of a
  // translation to center the moving image to the origin, 
  // of the landmark registration
  // transform, and of the translation to re-center it 
  // with the center of the fixed image.
  // It is necessary to center both the fixed, 
  // and moving point sets on (0,0,0) otherwise
  // the registration will be biased.
      
  AffineTransformType::OffsetType fixedImageOffset;
      
  for ( unsigned int i = 0 ; i < PointType::PointDimension ; i++ )
    {
      fixedImageOffset[i] = -fixedImageLandmarkCenter[i];
    }
      
  AffineTransformType::Pointer preTransform = AffineTransformType::New();
  preTransform->SetIdentity();
  preTransform->SetOffset(fixedImageOffset);

  AffineTransformType::OffsetType movingImageOffset;

  for ( unsigned int i = 0 ; i < PointType::PointDimension ; i++ )
    {
      movingImageOffset[i] = movingImageLandmarkCenter[i];
    }
      
  AffineTransformType::Pointer postTransform = AffineTransformType::New();
  postTransform->SetIdentity();
  postTransform->SetOffset(movingImageOffset);
      

  typename RigidTransformType::Pointer regTransform = 
    registrator->GetTransform() ;
  typename AffineTransformType::Pointer 
    affineTransform = AffineTransformType::New() ;
 
  affineTransform->SetMatrix (regTransform->GetRotationMatrix());
  affineTransform->SetOffset(regTransform->GetOffset());
  
//   affineTransform->SetMatrix (regTransform->Inverse()->GetRotationMatrix());
//   affineTransform->SetOffset(regTransform->Inverse()->GetOffset());

//   AffineTransformType::Pointer resultTransform =
//     AffineTransformType::New() ;
//   std::cout << "DEBUG: m_AffineTransform = " 
//             << resultTransform << std::endl ;
//   outputTransform->SetIdentity();
//   outputTransform->Compose(preTransform,true);
//   outputTransform->Compose(affineTransform2,false);
//   outputTransform->Compose(postTransform,false);
  
  m_LandmarkTransform->SetIdentity();
  m_LandmarkTransform->Compose(postTransform,true);
  m_LandmarkTransform->Compose(affineTransform,true);
  m_LandmarkTransform->Compose(preTransform,true);

  m_LandmarkRigidTransform->SetRotation(regTransform->GetRotation()) ;
  m_LandmarkRigidTransform->SetOffset(m_LandmarkTransform->GetOffset()) ;

//   std::cout << "DEBUG: before inverse" << std::endl ;
//   std::cout << affineTransform2 << std::endl ;
//   outputTransform->SetParameters(affineTransform2->Inverse()->GetParameters()) ;
//   std::cout << "DEBUG: after inverse" << std::endl ;
//   std::cout << affineTransform2->Inverse() << std::endl ;

//   outputRigidTransform->SetRotation(regTransform->GetRotation().inverse()) ;
//   outputRigidTransform->SetOffset(outputTransform->GetOffset()) ;

//   outputRigidTransform->SetRotation(regTransform->Inverse()->GetRotation()) ;
//   outputRigidTransform->SetOffset(outputTransform->Inverse()->GetOffset()) ;
//   std::cout << "DEBUG: landmark registration result = " 
//             << resultTransform << std::endl ;
//   return resultTransform ;
  std::cout << "DEBUG: landmark registration result transform: " << m_LandmarkTransform << std::endl ;
}


template< class TImage, class TRealImage >
void
ImageRegistrationApp< TImage, TRealImage >
::RegisterUsingRigidMethod()
{
  std::cout << "DEBUG: RegisterUsingRigidMethod." << std::endl ;

  typename SingleResolutionRegistrationMethodType::Pointer registration = 
    SingleResolutionRegistrationMethodType::New();
  RigidTransformType::Pointer transform = RigidTransformType::New();
  typename MutualInformationMatricType::Pointer metric = 
    MutualInformationMatricType::New();
  RigidOptimizerType::Pointer optimizer = RigidOptimizerType::New();
  
  typename InterpolatorType::Pointer interpolator =
    InterpolatorType::New() ;

  interpolator->SetInputImage(m_RealMovingImage) ;

  optimizer->SetScales( m_RigidScales );
  optimizer->MaximizeOn();
  optimizer->SetNumberOfIterations(m_RigidNumberOfIterations);
  optimizer->SetLearningRate(m_RigidLearningRate);

  metric->SetMovingImageStandardDeviation
    ( m_RigidMovingImageStandardDeviation );
  metric->SetFixedImageStandardDeviation( m_RigidFixedImageStandardDeviation );
  metric->SetNumberOfSpatialSamples( m_RigidNumberOfSpatialSamples );

  registration->SetFixedImage( m_RealFixedImage ) ;
  registration->SetFixedImageRegion
    ( m_RealFixedImage->GetLargestPossibleRegion() ) ;
  registration->SetMovingImage( m_RealMovingImage ) ;
  registration->SetInterpolator( interpolator.GetPointer() ) ;
  registration->SetMetric( metric );
  registration->SetOptimizer( optimizer ) ;
  registration->SetTransform( transform ) ;

  RigidTransformType::ParametersType params(7) ;
  params[0] = m_CenteredLandmarkRigidTransform->GetRotation().x() ;
  params[1] = m_CenteredLandmarkRigidTransform->GetRotation().y() ;
  params[2] = m_CenteredLandmarkRigidTransform->GetRotation().z() ;
  params[3] = m_CenteredLandmarkRigidTransform->GetRotation().r() ;
  params[4] = m_CenteredLandmarkRigidTransform->GetOffset()[0] ;
  params[5] = m_CenteredLandmarkRigidTransform->GetOffset()[1] ;
  params[6] = m_CenteredLandmarkRigidTransform->GetOffset()[2] ;

  registration->SetInitialTransformParameters
    ( params );

  std::cout << "DEBUG: before registration " << std::endl ;
  try
    {   
      registration->StartRegistration();
    }
  catch(itk::ExceptionObject &e)
    {
      this->PrintError(e) ;
    }
  catch(...)
    {
      this->PrintUncaughtError() ;
    }

  // calculates the result affine transform 
  typename AffineTransformType::Pointer affineTransform = 
    AffineTransformType::New();
  affineTransform->SetMatrix( transform->GetRotationMatrix() );
  affineTransform->SetOffset( transform->GetOffset() );

//   AffineTransformType::OffsetType offset = affineTransform->GetOffset() ;
//   for ( unsigned int i = 0 ; i < TImage::ImageDimension ; i++ )
//     {
//       offset[i] -= m_FixedImageOffset[i] ;
//     }
//   outputTransform->SetOffset(offset) ;

//   outputTransform->SetMatrix(affineTransform->GetMatrix()) ;
//   this->ReverseCenteringTransform(outputTransform, affineTransform, 
//                                   m_FixedImageOffset, m_MovingImageOffset) ;

  m_AffineTransform->SetMatrix(affineTransform->GetMatrix()) ;
  this->ReverseCenteringTransform(m_AffineTransform, affineTransform, 
                                  m_FixedImageOffset, m_MovingImageOffset) ;
}


template< class TImage, class TRealImage >
void
ImageRegistrationApp< TImage, TRealImage >
::RegisterUsingAffineMethod()
{
  AffineOptimizerType::Pointer optimizer = AffineOptimizerType::New();

//   typename SingleResolutionRegistrationMethodType::Pointer registration = 
//     SingleResolutionRegistrationMethodType::New();
  typename MultiResolutionRegistrationMethodType::Pointer registration = 
    MultiResolutionRegistrationMethodType::New();
  typename MutualInformationMatricType::Pointer metric = 
    MutualInformationMatricType::New();
  typename AffineTransformType::Pointer transform = 
    AffineTransformType::New() ;

  typename InterpolatorType::Pointer interpolator =
    InterpolatorType::New() ;

  interpolator->SetInputImage(m_RealMovingImage) ;

  registration->SetOptimizer( optimizer );
  registration->SetMetric( metric );

  registration->SetTransform( transform );

  // to do multi-resolution registration uncomment the following three lines
  registration->SetFixedImagePyramid( m_FixedImagePyramid );
  registration->SetMovingImagePyramid( m_MovingImagePyramid );
  registration->SetNumberOfLevels( 4 );

  registration->SetFixedImage( m_RealFixedImage );
  registration->SetMovingImage( m_RealMovingImage ) ;

  registration->SetFixedImageRegion
    ( m_RealFixedImage->GetRequestedRegion() );

  registration->SetInterpolator( interpolator.GetPointer() ) ;

  registration->SetInitialTransformParameters
    ( m_CenteredLandmarkTransform->GetParameters() );

  metric->SetNumberOfSpatialSamples( m_AffineNumberOfSpatialSamples );
  metric->SetFixedImageStandardDeviation
    ( m_AffineFixedImageStandardDeviation ) ;
  metric->SetMovingImageStandardDeviation
    ( m_AffineMovingImageStandardDeviation ) ;

  // common for any optimizer
  optimizer->SetScales( m_AffineScales );
  optimizer->MaximizeOn() ;

  // for gradient descent optimizer
  optimizer->SetNumberOfIterations( m_AffineNumberOfIterations );
  optimizer->SetLearningRate( m_AffineLearningRate ) ;

  try 
    { 
      registration->StartRegistration(); 
    } 
  catch( itk::ExceptionObject &e ) 
    { 
      this->PrintError(e) ;
    } 
  catch( ... )
    {
      this->PrintUncaughtError() ;
    }

  // calculates the result affine transform 

  AffineParametersType finalParameters = 
    registration->GetLastTransformParameters();
  typename AffineTransformType::Pointer affineTransform = 
    AffineTransformType::New();

  affineTransform->SetParameters( finalParameters );

//   this->ReverseCenteringTransform(outputTransform, affineTransform, 
//                                   m_FixedImageOffset, m_MovingImageOffset) ;

  m_AffineTransform->SetParameters(affineTransform->GetParameters()) ;
  this->ReverseCenteringTransform(m_AffineTransform, affineTransform, 
                                  m_FixedImageOffset, m_MovingImageOffset) ;

  std::cout << "DEBUG: centered landmark registration result transform: " << m_CenteredLandmarkTransform << std::endl ;
  std::cout << "DEBUG: full affine registration result transform before recentering: " << affineTransform << std::endl ;
  std::cout << "DEBUG: full affine registration result transform: " << m_AffineTransform << std::endl ;
  std::cout << "DEBUG: final metric value = " << metric->GetValue(finalParameters) << std::endl ;
}

template< class TImage, class TRealImage >
void
ImageRegistrationApp< TImage, TRealImage >
::RegisterUsingDeformableMethod()
{
}

// template< class TImage, class TRealImage >
// void
// ImageRegistrationApp< TImage, TRealImage >
// ::CenterImage(TRealImage* input, typename TRealImage::Pointer output)
// {
//   typedef itk::ChangeInformationImageFilter< TRealImage >  FilterType ;

//   typename FilterType::Pointer filter = FilterType::New();
//   filter->SetInput( input );
//   filter->CenterImageOn();
//   filter->Update();
  
//   output = filter->GetOutput() ;
// }

// template< class TImage, class TRealImage >
// void
// ImageRegistrationApp< TImage, TRealImage >
// ::NormalizeImageToZeroMean(TImage* input, typename TRealImage::Pointer output)
// {
//   typedef itk::NormalizeImageFilter< TImage, TRealImage > FilterType ;

//   typename FilterType::Pointer filter = FilterType::New() ;
//   filter->SetInput( input );
//   filter->Update();

//   std::cout << "DEBUG: NormalizeImage before output = " 
//             << output << std::endl ;
//   output = filter->GetOutput() ;
//   std::cout << "DEBUG: NormalizeImage after output = " 
//             << output << std::endl ;
// }

template< class TImage, class TRealImage >
void
ImageRegistrationApp< TImage, TRealImage >
::NormalizeImageToZeroToOneRange(TImage * input,
                                 TRealImage* output,
                                 ImagePixelType min)
{
  
  typedef itk::MinimumMaximumImageFilter< TImage > MinMaxFilterType;
  typename MinMaxFilterType::Pointer minMaxFilter = MinMaxFilterType::New();
  
  minMaxFilter->SetInput( input );
  minMaxFilter->Update();
  
  min = minMaxFilter->GetMinimum();
  double shift = -1.0 * static_cast<double>( min );
  double scale = static_cast<double>( minMaxFilter->GetMaximum() );
  scale += shift;
  scale = 1.0 / scale;
  
  typedef itk::ShiftScaleImageFilter<TImage, TRealImage> 
    FilterType;
  typename FilterType::Pointer filter = FilterType::New();

  filter->SetInput( input );
  filter->SetShift( shift );
  filter->SetScale( scale );
  filter->Update();

  output = filter->GetOutput();
}

template< class TImage, class TRealImage >
void
ImageRegistrationApp< TImage, TRealImage >
::MatchIntensity(TRealImage* normalizedFixedImage, 
                 TRealImage* normalizedMovingImage,
                 TRealImage* outputMovingImage,
                 unsigned int numberOfHistogramLevels,
                 unsigned int numberOfHistogramMatchPoints)
{
  typedef itk::HistogramMatchingImageFilter< TRealImage, TRealImage>
    FilterType ;
  typename FilterType::Pointer filter = FilterType::New();

  filter->SetInput( normalizedMovingImage );
  filter->SetReferenceImage( normalizedFixedImage );
  filter->SetNumberOfHistogramLevels( numberOfHistogramLevels );
  filter->SetNumberOfMatchPoints( numberOfMatchPoints );
  // uncomment the following line to exclude pixels that has intensity
  // values smaller than mean intensity.
//  filter->ThresholdAtMeanIntensityOn();
  filter->Update();

  outputMovingImage = filter->GetOutput() ;
}

template< class TImage, class TRealImage >
void
ImageRegistrationApp< TImage, TRealImage >
::GenerateImagePyramid(TRealImage* image, 
                       ImagePyramidType* imagePyramid,
                       unsigned int numberOfLevels,
                       ShrinkFactorsArrayType &shrinkFactors)
{
  imagePyramid->SetNumberOfLevels( numberOfLevels );
  imagePyramid->SetStartingShrinkFactors
    (shrinkFactors.GetDataPointer() );
}

template< class TImage, class TRealImage >
void
ImageRegistrationApp< TImage, TRealImage >
::ReverseCenteringTransform(AffineTransformType* outputTransform,
                            AffineTransformType* registrationResult,
                            OffsetType& fixedImageOffset,
                            OffsetType& movingImageOffset)
{
  typename AffineTransformType::Pointer fixedTransform = 
    AffineTransformType::New();
  fixedTransform->SetIdentity() ;
  fixedTransform->SetOffset(fixedImageOffset);

  typename AffineTransformType::Pointer movingTransform = 
    AffineTransformType::New();
  movingTransform->SetIdentity() ;
  movingTransform->SetOffset(movingImageOffset);

  outputTransform->SetIdentity();
//   outputTransform->Compose(movingTransform->Inverse(), true);
//   outputTransform->Compose(registrationResult, true);
//   outputTransform->Compose(fixedTransform, true);
  outputTransform->Compose(registrationResult, true);
  outputTransform->Compose(movingTransform, true);
  outputTransform->Compose(fixedTransform->Inverse(), false);
}

template< class TImage, class TRealImage >
void
ImageRegistrationApp< TImage, TRealImage >
::PrintUncaughtError()
{
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in ImageRegistrationApp:" << std::endl;
  std::cout << "unknown exception caught !!!" << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
}

template< class TImage, class TRealImage >
void
ImageRegistrationApp< TImage, TRealImage >
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
