#ifndef __ImageRegistrationApp_h
#define __ImageRegistrationApp_h

#include "itkImage.h"

#include "LandmarkRegistrator.h"
#include "RigidRegistrator.h"
#include "AffineRegistrator.h"

#include "itkAffineTransform.h"

// including all the type of transform that could possibly be used
// as templated type.

namespace itk
{

template< class TImage >
class ImageRegistrationApp : public Object
  {
  public:
    typedef ImageRegistrationApp Self;
    typedef Object Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;
  
    itkNewMacro(Self);
    itkTypeMacro(ImageRegistrationApp, Object);
  
    typedef TImage                        ImageType;
    typedef typename ImageType::Pointer   ImagePointer;
    typedef typename TImage::PixelType    ImagePixelType ;
    typedef typename TImage::RegionType   RegionType ;
    typedef typename TImage::OffsetType   OffsetType ;
  
    typedef LandmarkRegistrator                       LandmarkRegistratorType;
    typedef LandmarkRegistratorType::LandmarkType     LandmarkType;
    typedef LandmarkRegistratorType::LandmarkSetType  LandmarkSetType;
    typedef LandmarkRegistratorType::ParametersType   LandmarkParametersType;
    typedef LandmarkRegistratorType::ScalesType       LandmarkScalesType;
    typedef LandmarkRegistratorType::TransformType    LandmarkRegTransformType;
  
    typedef RigidRegistrator< TImage >                    RigidRegistratorType;
    typedef typename RigidRegistratorType::ParametersType RigidParametersType;
    typedef typename RigidRegistratorType::ScalesType     RigidScalesType;
    typedef typename RigidRegistratorType::TransformType  RigidRegTransformType;
  
    typedef AffineRegistrator< TImage >                   AffineRegistratorType;
    typedef typename AffineRegistratorType::ParametersType 
                                                          AffineParametersType;
    typedef typename AffineRegistratorType::ScalesType    AffineScalesType;
    typedef typename AffineRegistratorType::TransformType 
                                            AffineRegTransformType;
  
    typedef AffineTransform<double, 3>                    AffineTransformType;
  
    void RegisterUsingLandmarks(LandmarkSetType* fixedImageLandmarks,
                                LandmarkSetType* movingImageLandmarks) ;
  
    void RegisterUsingRigidMethod() ;
  
    void RegisterUsingAffineMethod() ;
  
    void SetFixedImage(TImage* image);
    
    void SetMovingImage(TImage* image);
  
    itkGetObjectMacro(LandmarkRegTransform, LandmarkRegTransformType) ;
    itkGetObjectMacro(RigidRegTransform, RigidRegTransformType) ;
    itkGetObjectMacro(AffineRegTransform, AffineRegTransformType) ;
    itkGetObjectMacro(LandmarkAffineTransform, AffineTransformType) ;
    itkGetObjectMacro(RigidAffineTransform, AffineTransformType) ;
    itkGetObjectMacro(AffineAffineTransform, AffineTransformType) ;
    itkGetObjectMacro(FinalTransform, AffineTransformType);
  
    itkSetMacro(LandmarkNumberOfIterations, unsigned int) ;
    itkGetConstMacro(LandmarkNumberOfIterations, unsigned int) ;
    itkSetMacro(LandmarkScales, LandmarkScalesType) ;
    itkGetConstMacro(LandmarkScales, LandmarkScalesType) ;
  
    itkSetMacro(RigidNumberOfIterations, unsigned int) ;
    itkGetConstMacro(RigidNumberOfIterations, unsigned int) ;
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
    itkSetMacro(AffineFixedImageStandardDeviation, double) ;
    itkGetConstMacro(AffineFixedImageStandardDeviation, double) ;
    itkSetMacro(AffineMovingImageStandardDeviation, double) ;
    itkGetConstMacro(AffineMovingImageStandardDeviation, double) ;
    itkSetMacro(AffineNumberOfSpatialSamples, unsigned int) ;
    itkGetConstMacro(AffineNumberOfSpatialSamples, unsigned int) ;
    itkSetMacro(AffineScales, AffineScalesType) ;
    itkGetConstMacro(AffineScales, AffineScalesType) ;
  
    itkSetMacro(MovingImageRegion, RegionType) ;
    itkGetConstMacro(MovingImageRegion, RegionType) ;
  
    ImagePointer GetLandmarkRegisteredMovingImage();
    ImagePointer GetRigidRegisteredMovingImage();
    ImagePointer GetAffineRegisteredMovingImage();
    ImagePointer GetFinalRegisteredMovingImage();
  
  protected:
    ImageRegistrationApp() ;
    virtual ~ImageRegistrationApp() ;
  
    void PrintUncaughtError() ;
  
    void PrintError(ExceptionObject &e) ;
  
  private:
    typename LandmarkRegTransformType::Pointer   m_LandmarkRegTransform ;
    typename AffineTransformType::Pointer        m_LandmarkAffineTransform ;
    typename RigidRegTransformType::Pointer      m_RigidRegTransform ;
    typename AffineTransformType::Pointer        m_RigidAffineTransform ;
    typename AffineRegTransformType::Pointer     m_AffineRegTransform ;
    typename AffineTransformType::Pointer        m_AffineAffineTransform ;
    typename AffineTransformType::Pointer        m_FinalTransform;
  
    TImage*             m_FixedImage ;
    TImage*             m_MovingImage ;
    
    RegionType          m_MovingImageRegion ;
  
    unsigned int        m_LandmarkNumberOfIterations ;
    LandmarkScalesType  m_LandmarkScales ;
    bool                m_LandmarkRegValid;
  
    unsigned int        m_RigidNumberOfIterations ;
    double              m_RigidFixedImageStandardDeviation ;
    double              m_RigidMovingImageStandardDeviation ;
    unsigned int        m_RigidNumberOfSpatialSamples ;
    RigidScalesType     m_RigidScales ;
    bool                m_RigidRegValid;
  
    unsigned int        m_AffineNumberOfIterations ;
    double              m_AffineFixedImageStandardDeviation ;
    double              m_AffineMovingImageStandardDeviation ;
    unsigned int        m_AffineNumberOfSpatialSamples ;
    AffineScalesType    m_AffineScales ;
    bool                m_AffineRegValid;
  
    ImagePointer        m_ResampleUsingTransform(
                                        AffineTransformType * transform,
                                        ImageType * input,
                                        ImageType * output);
  }; // end of class

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "ImageRegistrationApp.txx"
#endif

#endif //__ImageRegistrationApp_h

