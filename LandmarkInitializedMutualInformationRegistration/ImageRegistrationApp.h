#ifndef __ImageRegistrationApp_h
#define __ImageRegistrationApp_h

#include "itkObject.h"
#include "itkSmartPointer.h"
#include "itkImage.h"
#include <itkLinearInterpolateImageFunction.h>

#include "itkNormalizeImageFilter.h"
#include "itkChangeInformationImageFilter.h"
#include "itkMinimumMaximumImageFilter.h"
#include "itkHistogramMatchingImageFilter.h"

//#include "LandmarkRegistration.h"
#include "LandmarkBasedRegistrator.h"
#include "itkImageRegistrationMethod.h"
#include "itkQuaternionRigidTransform.h"
#include <itkMutualInformationImageToImageMetric.h>
#include <itkQuaternionRigidTransformGradientDescentOptimizer.h>
#include "itkAffineTransform.h"

//#include "itkNormalVariateGenerator.h"

#include "itkMultiResolutionImageRegistrationMethod.h"
#include "itkGradientDescentOptimizer.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkRecursiveMultiResolutionPyramidImageFilter.h"

#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"

// including all the type of transform that could possibly be used
// as templated type.

template< class TImage, class TRealImage >
class ImageRegistrationApp : public itk::Object
{
public:
  ImageRegistrationApp() ;
  virtual ~ImageRegistrationApp() ;

  typedef typename TImage::PixelType ImagePixelType ;
  typedef typename TImage::RegionType RegionType ;

  typedef itk::LinearInterpolateImageFunction< TRealImage, double > 
  InterpolatorType ;

  //  typedef itk::NormalizedMutualInformationImageToImageMetric< TRealImage, TRealImage > 
  //  MutualInformationMatricType ;
  typedef itk::MutualInformationImageToImageMetric< TRealImage, TRealImage > 
  MutualInformationMatricType ;

  /** preprocessing related typedefs */
  typedef itk::NormalizeImageFilter< TImage, TRealImage > NormalizerType ;
  typedef itk::ChangeInformationImageFilter< TRealImage > CentererType ;

  /** single-resolution related typedefs */
  typedef itk::ImageRegistrationMethod< TRealImage, TRealImage > 
  SingleResolutionRegistrationMethodType ;

  /** multi-resolution related typedefs */
  typedef itk::MultiResolutionImageRegistrationMethod< TRealImage, 
                                                       TRealImage > 
  MultiResolutionRegistrationMethodType;

  typedef itk::RecursiveMultiResolutionPyramidImageFilter< TRealImage,
                                                           TRealImage >
  ImagePyramidType;

  typedef itk::FixedArray< unsigned int, 3 > ShrinkFactorsArrayType ;

  /** rigid registration related typedefs */
  typedef itk::QuaternionRigidTransform<double> RigidTransformType ;
  typedef itk::QuaternionRigidTransformGradientDescentOptimizer 
  RigidOptimizerType ;
  typedef RigidOptimizerType::ParametersType RigidParametersType ;

  typedef typename RigidOptimizerType::ScalesType RigidScalesType ;

  /** affine registration related typedefs */
  typedef itk::AffineTransform< double, 3 > AffineTransformType;
  typedef typename AffineTransformType::Pointer AffineTransformPointer ;
  typedef typename AffineTransformType::OffsetType OffsetType ;
  typedef itk::GradientDescentOptimizer AffineOptimizerType;
    // typedef itk::OnePlusOneEvolutionaryOptimizer AffineOptimizerType;
  typedef AffineOptimizerType::ScalesType AffineScalesType;
  typedef AffineOptimizerType::ParametersType AffineParametersType ;

  /** landmark based registration related typedefs */
  typedef itk::Point< double, 3 > PointType ;
  typedef itk::LandmarkBasedRegistrator<  TImage, TImage, RigidTransformType > 
  LandmarkRegistratorType;
  typedef itk::Array<double> DoubleArrayType ;
  typedef typename LandmarkRegistratorType::PointSetType LandmarksType ;
  typedef DoubleArrayType LandmarkScalesType ;

  void RegisterUsingLandmarks(LandmarksType* fixedImageLandmarks,
                              LandmarksType* movingImageLandmarks) ;

  void RegisterUsingRigidMethod() ;

  void RegisterUsingAffineMethod() ;

  void RegisterUsingDeformableMethod() ;

  void SetFixedImage(TImage* image)
  { m_FixedImage = image ; }
  
  void SetMovingImage(TImage* image)
  { m_MovingImage = image ; }

  itkGetObjectMacro(AffineTransform, AffineTransformType) ;
  itkGetObjectMacro(LandmarkTransform, AffineTransformType) ;
  itkGetObjectMacro(CenteredLandmarkTransform, AffineTransformType) ;
  itkGetObjectMacro(LandmarkRigidTransform, RigidTransformType) ;
  itkGetObjectMacro(CenteredLandmarkRigidTransform, RigidTransformType) ;

  itkSetMacro(LandmarkNumberOfIterations, unsigned int) ;
  itkGetConstMacro(LandmarkNumberOfIterations, unsigned int) ;

  itkSetMacro(LandmarkScales, RigidScalesType) ;
  itkGetConstMacro(LandmarkScales, RigidScalesType) ;

  itkSetMacro(RigidNumberOfIterations, unsigned int) ;
  itkGetConstMacro(RigidNumberOfIterations, unsigned int) ;

  itkSetMacro(RigidLearningRate, double) ;
  itkGetConstMacro(RigidLearningRate, double) ;

  itkSetMacro(RigidFixedImageStandardDeviation, double) ;
  itkGetConstMacro(RigidFixedImageStandardDeviation, double) ;

  itkSetMacro(RigidMovingImageStandardDeviation, double) ;
  itkGetConstMacro(RigidMovingImageStandardDeviation, double) ;

  itkSetMacro(RigidNumberOfSpatialSamples, unsigned int) ;
  itkGetConstMacro(RigidNumberOfSpatialSamples, unsigned int) ;

  itkSetMacro(RigidScales, RigidScalesType) ;
  itkGetConstMacro(RigidScales, RigidScalesType) ;

  itkSetMacro(AffineNumberOfIterations, unsigned int) ;
  itkGetConstMacro(AffineNumberOfIterations, unsigned int) ;

  itkSetMacro(AffineLearningRate, double) ;
  itkGetConstMacro(AffineLearningRate, double) ;

  itkSetMacro(AffineFixedImageStandardDeviation, double) ;
  itkGetConstMacro(AffineFixedImageStandardDeviation, double) ;

  itkSetMacro(AffineMovingImageStandardDeviation, double) ;
  itkGetConstMacro(AffineMovingImageStandardDeviation, double) ;

  itkSetMacro(AffineNumberOfSpatialSamples, unsigned int) ;
  itkGetConstMacro(AffineNumberOfSpatialSamples, unsigned int) ;

  itkSetMacro(AffineScales, AffineScalesType) ;
  itkGetConstMacro(AffineScales, AffineScalesType) ;

  itkSetMacro(FixedImageRegion, RegionType) ;
  itkGetConstMacro(FixedImageRegion, RegionType) ;
  
  void NormalizeAndCenter() ;

  //  void CenterImage(TRealImage* input, typename TRealImage::Pointer output) ;

//   void NormalizeImageToZeroMean(TImage* input, 
//                                 typename TRealImage::Pointer output) ;
protected:
  /** landmark registration related functions */
 
  void CenterLandmarks(LandmarksType* fixedImageLandmarks,
                       LandmarksType* movingImageLandmarks,
                       LandmarksType* centeredFixedImageLandmarks,
                       LandmarksType* centeredMovingImageLandmarks,
                       PointType& fixedImageLandmarkCenter,
                       PointType& movingImageLandmarkCenter) ;


  void NormalizeImageToZeroToOneRange(TImage* input, 
                                      TRealImage* output, 
                                      ImagePixelType min) ;

  void MatchIntensity(TRealImage* fixedImage, 
                      TRealImage* movingImage, 
                      TRealImage* outputMovingImage,
                      unsigned int numberOfHistogramLevels = 256,
                      unsigned int numberOfHistogramMatchPoints = 1) ;

  void GenerateImagePyramid(TRealImage* image, 
                            ImagePyramidType* imagePyramid,
                            unsigned int numberOfLevels,
                            ShrinkFactorsArrayType &shrinkFactors) ;

  void ReverseCenteringTransform(AffineTransformType* outputTransform,
                                 AffineTransformType* registrationResult,
                                 OffsetType& fixedImageOffset,
                                 OffsetType& movingImageOffset) ;

  void PrintUncaughtError() ;

  void PrintError(itk::ExceptionObject &e) ;

private:
  typename AffineTransformType::Pointer m_AffineTransform ;
  typename AffineTransformType::Pointer m_LandmarkTransform ;
  typename AffineTransformType::Pointer m_CenteredLandmarkTransform ;
  typename RigidTransformType::Pointer m_LandmarkRigidTransform ;
  typename RigidTransformType::Pointer m_CenteredLandmarkRigidTransform ;

  bool m_UseMultiResolution ;

  TImage* m_FixedImage ;
  TImage* m_MovingImage ;
  
  typename NormalizerType::Pointer m_FixedImageNormalizer ;
  typename NormalizerType::Pointer m_MovingImageNormalizer ;

  typename CentererType::Pointer m_FixedImageCenterer ;
  typename CentererType::Pointer m_MovingImageCenterer ;

  typename TRealImage::Pointer m_RealFixedImage ;
  typename TRealImage::Pointer m_RealMovingImage ;

  OffsetType m_FixedImageOffset ;
  OffsetType m_MovingImageOffset ;

  unsigned int m_LandmarkNumberOfIterations ;
  RigidScalesType m_LandmarkScales ;

  unsigned int m_RigidNumberOfIterations ;
  double m_RigidLearningRate ;
  double m_RigidFixedImageStandardDeviation ;
  double m_RigidMovingImageStandardDeviation ;
  unsigned int m_RigidNumberOfSpatialSamples ;
  RigidScalesType m_RigidScales ;

  unsigned int m_AffineNumberOfIterations ;
  double m_AffineLearningRate ;
  double m_AffineFixedImageStandardDeviation ;
  double m_AffineMovingImageStandardDeviation ;
  unsigned int m_AffineNumberOfSpatialSamples ;
  AffineScalesType m_AffineScales ;
  RegionType m_FixedImageRegion ;

  typename ImagePyramidType::Pointer m_FixedImagePyramid ;
  typename ImagePyramidType::Pointer m_MovingImagePyramid ;

  ShrinkFactorsArrayType m_FixedImageShrinkFactors ;
  ShrinkFactorsArrayType m_MovingImageShrinkFactors ;
} ; // end of class


#ifndef ITK_MANUAL_INSTANTIATION
#include "ImageRegistrationApp.txx"
#endif

#endif //__ImageRegistrationApp_h

