#ifndef __AffineRegistrator_h
#define __AffineRegistrator_h

#include "itkImage.h"

#include "itkImageRegistrationMethod.h"
#include "itkScaleSkewVersor3DTransform.h"
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
class AffineRegistrator : public ImageRegistrationMethod < TImage, TImage >
  {
  public:
    typedef AffineRegistrator Self;
    typedef ImageRegistrationMethod< TImage, TImage> Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkTypeMacro(AffineRegistrator, ImageRegistrationMethod);

    itkNewMacro(Self);
  
    typedef typename TImage::PixelType PixelType ;
    typedef typename TImage::RegionType RegionType ;

    typedef ScaleSkewVersor3DTransform<double> TransformType ;

    typedef typename Superclass::OptimizerType           OptimizerType;

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

    typedef typename Superclass::OptimizerType OptimizerType;
    typedef typename OptimizerType::Pointer    OptimizerPointer;

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
    AffineRegistrator() ;
    virtual ~AffineRegistrator() ;

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

    OptimizerPointer        m_SecondaryOptimizer;

  } ; // end of class


#ifndef ITK_MANUAL_INSTANTIATION
#include "AffineRegistrator.txx"
#endif

} // end namespace itk

#endif //__AffineRegistrator_H

