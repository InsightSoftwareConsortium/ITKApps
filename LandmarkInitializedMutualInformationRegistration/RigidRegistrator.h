#ifndef __RigidRegistrator_h
#define __RigidRegistrator_h

#include "itkImage.h"

#include "itkImageRegistrationMethod.h"
#include "itkVersorRigid3DTransform.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNormalVariateGenerator.h"


template< class TImage >
class RigidRegistrator : public itk::ImageRegistrationMethod < TImage, TImage >
  {
  public:
    typedef RigidRegistrator Self;
    typedef itk::ImageRegistrationMethod< TImage, TImage> Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkTypeMacro(RigidRegistrator, ImageRegistrationmethod);
    itkNewMacro(Self);
  
    typedef typename TImage::PixelType PixelType ;
    typedef typename TImage::RegionType RegionType ;

    /** preprocessing related typedefs */
    typedef itk::VersorRigid3DTransform<double> TransformType ;
    typedef itk::OnePlusOneEvolutionaryOptimizer OptimizerType ;
    typedef itk::Statistics::NormalVariateGenerator  OptimizerNormalGeneratorType;
    typedef TransformType::ParametersType ParametersType ;
    typedef TransformType::ParametersType ScalesType ;
    typedef itk::LinearInterpolateImageFunction< TImage, double > 
                 InterpolatorType ;
    typedef itk::MattesMutualInformationImageToImageMetric< 
                 TImage, TImage > MetricType ;

    void StartRegistration() ;

    TransformType * GetTypedTransform(void)
      {
      return static_cast<TransformType *>(Superclass::GetTransform());
      }

    OptimizerType * GetTypedOptimizer(void)
      {
      return static_cast<OptimizerType *>(Superclass::GetOptimizer());
      }

    itkSetMacro(OptimizerNumberOfIterations, unsigned int) ;
    itkGetConstMacro(OptimizerNumberOfIterations, unsigned int) ;
    itkSetMacro(OptimizerScales, ScalesType) ;
    itkGetConstMacro(OptimizerScales, ScalesType) ;

    MetricType * GetTypedMetric(void)
      {
      return static_cast<MetricType *>(Superclass::GetMetric());
      }


    itkSetMacro(MetricNumberOfSpatialSamples, unsigned int) ;
    itkGetConstMacro(MetricNumberOfSpatialSamples, unsigned int) ;

    void SetMovingImageRegion(RegionType & region);
    itkGetConstReferenceMacro(MovingImageRegion, RegionType);
    itkGetMacro(MovingImageRegionDefined, bool);
    itkSetMacro(MovingImageRegionDefined, bool);

  protected:
    RigidRegistrator() ;
    virtual ~RigidRegistrator() ;

    virtual void Initialize() throw(ExceptionObject);

    void PrintUncaughtError() ;

    void PrintError(itk::ExceptionObject &e) ;

  private:
    unsigned int  m_OptimizerNumberOfIterations;
    ScalesType    m_OptimizerScales;

    unsigned int  m_MetricNumberOfSpatialSamples;

    bool          m_MovingImageRegionDefined;
    RegionType    m_MovingImageRegion;
  } ; // end of class


#ifndef ITK_MANUAL_INSTANTIATION
#include "RigidRegistrator.txx"
#endif

#endif //__RigidRegistrator_H

