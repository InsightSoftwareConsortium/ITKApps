#ifndef __RigidRegistrator_h
#define __RigidRegistrator_h

#include "itkImage.h"

#include "itkImageRegistrationMethod.h"
#include "itkVersorRigid3DTransform.h"
#include "itkMattesMutualInformationImageToImageMetric.h"

#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkNormalVariateGenerator.h"
#include "itkPowellOptimizer.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkStatisticsImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"

namespace itk
{

template< class TImage >
class RigidRegistrator : public ImageRegistrationMethod < TImage, TImage >
  {
  public:
    typedef RigidRegistrator Self;
    typedef ImageRegistrationMethod< TImage, TImage> Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkTypeMacro(RigidRegistrator, ImageRegistrationMethod);

    itkNewMacro(Self);
  
    typedef typename TImage::PixelType PixelType ;
    typedef typename TImage::RegionType RegionType ;

    /** preprocessing related typedefs */
    typedef VersorRigid3DTransform<double>      TransformType ;

    typedef enum { ONEPLUSONE, GRADIENT, ONEPLUSONEPLUSGRADIENT }
                                                OptimizerMethodType;

    typedef OnePlusOneEvolutionaryOptimizer     OnePlusOneOptimizerType ;
    typedef PowellOptimizer                     GradientOptimizerType ;
    typedef Statistics::NormalVariateGenerator  OptimizerNormalGeneratorType;
    typedef TransformType::ParametersType       ParametersType ;
    typedef TransformType::ParametersType       ScalesType ;
    typedef LinearInterpolateImageFunction< TImage, double > 
                                                InterpolatorType ;
    typedef MattesMutualInformationImageToImageMetric< TImage, TImage >
                                                MetricType ;

    void StartRegistration() ;

    TransformType * GetTypedTransform(void)
      {
      return static_cast<TransformType *>(Superclass::GetTransform());
      }

    void SetOptimizerToOnePlusOne();
    void SetOptimizerToGradient();
    void SetOptimizerToOnePlusOnePlusGradient();

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

  protected:
    RigidRegistrator() ;
    virtual ~RigidRegistrator() ;

    virtual void Initialize() throw(ExceptionObject);

    void PrintUncaughtError() ;

    void PrintError(ExceptionObject &e) ;

    itkSetObjectMacro(SecondaryOptimizer, OptimizerType);
    itkGetObjectMacro(SecondaryOptimizer, OptimizerType);


  private:
    OptimizerMethodType     m_OptimizerMethod;
    unsigned int            m_OptimizerNumberOfIterations;
    ScalesType              m_OptimizerScales;

    unsigned int            m_MetricNumberOfSpatialSamples;

    OptimizerType::Pointer  m_SecondaryOptimizer;



  } ; // end of class


#ifndef ITK_MANUAL_INSTANTIATION
#include "RigidRegistrator.txx"
#endif

} // end namespace itk

#endif //__RigidRegistrator_H

