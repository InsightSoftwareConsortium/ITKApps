#ifndef __AffineRegistrator_h
#define __AffineRegistrator_h

// MUST CALL IN ORDER (especially first 3)
// SetFixedImage
// SetMovingImage
// SetLearningRate and other parameters
// SetFixedImageRegion()
// StartRegistration()
// GetTransform()

#include "itkImage.h"

#include "itkImageRegistrationMethod.h"
#include "itkCenteredAffineTransform.h"
#include "itkMutualInformationImageToImageMetric.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkNormalVariateGenerator.h"
#include "itkLinearInterpolateImageFunction.h"

template< class TImage >
class AffineRegistrator : public itk::ImageRegistrationMethod < TImage, TImage >
  {
  public:
    typedef AffineRegistrator Self;
    typedef itk::ImageRegistrationMethod< TImage, TImage> Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkNewMacro(Self);
    itkTypeMacro(AffineRegistrator, itk::ImageRegistrationmethod);

  
    typedef typename TImage::PixelType PixelType ;
    typedef typename TImage::RegionType RegionType ;

    /** preprocessing related typedefs */
    typedef itk::CenteredAffineTransform<double> TransformType ;
    typedef itk::OnePlusOneEvolutionaryOptimizer OptimizerType ;
    typedef itk::Statistics::NormalVariateGenerator  OptimizerNormalGeneratorType;
    typedef OptimizerType::ParametersType ParametersType ;
    typedef typename OptimizerType::ScalesType ScalesType ;
    typedef itk::LinearInterpolateImageFunction< TImage, double > 
                 InterpolatorType ;
    typedef itk::MutualInformationImageToImageMetric< 
                 TImage, TImage > MetricType ;

    void StartRegistration() ;

    TransformType * GetTransform(void)
      {
      return static_cast<TransformType *>(Superclass::GetTransform());
      }

    OptimizerType * GetOptimizer(void)
      {
      return static_cast<OptimizerType *>(Superclass::GetOptimizer());
      }

    itkSetMacro(OptimizerNumberOfIterations, unsigned int) ;
    itkGetConstMacro(OptimizerNumberOfIterations, unsigned int) ;
    itkSetMacro(OptimizerScales, ScalesType) ;
    itkGetConstMacro(OptimizerScales, ScalesType) ;

    MetricType * GetMetric(void)
      {
      return static_cast<MetricType *>(Superclass::GetMetric());
      }

    itkSetMacro(MetricNumberOfSpatialSamples, unsigned int) ;
    itkGetConstMacro(MetricNumberOfSpatialSamples, unsigned int) ;

    void SetMovingImageRegion(RegionType & region);
    itkGetConstReferenceMacro(MovingImageRegion, RegionType);
    itkGetMacro(MovingImageRegionDefined, bool);
    itkSetMacro(MovingImageRegionDefined, bool);

    itkSetMacro(MetricMovingImageStandardDeviation, double);
    itkGetMacro(MetricMovingImageStandardDeviation, double);
    itkSetMacro(MetricFixedImageStandardDeviation, double);
    itkGetMacro(MetricFixedImageStandardDeviation, double);

  protected:
    AffineRegistrator() ;
    virtual ~AffineRegistrator() ;

    virtual void Initialize() throw(ExceptionObject);

    void PrintUncaughtError() ;

    void PrintError(itk::ExceptionObject &e) ;

  private:
    // Base Class Defines
    // TImage *      m_FixedImage ;
    // TImage *      m_MovingImage ;

    // typename MetricType::Pointer m_Metric ;
    // typename OptimizerType::Pointer m_Optimizer ;
    // typename TransformType::Pointer m_Transform ;
    // typename InterpolatorType::Pointer m_Interpolator ;

    unsigned int  m_OptimizerNumberOfIterations;
    double        m_OptimizerLearningRate;
    ScalesType    m_OptimizerScales;

    unsigned int  m_MetricNumberOfSpatialSamples;
    double        m_MetricMovingImageStandardDeviation;
    double        m_MetricFixedImageStandardDeviation;

    bool          m_MovingImageRegionDefined;
    RegionType    m_MovingImageRegion;
  } ; // end of class


#ifndef ITK_MANUAL_INSTANTIATION
#include "AffineRegistrator.txx"
#endif

#endif //__AffineRegistrator_H

