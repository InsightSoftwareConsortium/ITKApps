#ifndef __LandmarkRegistrator_h
#define __LandmarkRegistrator_h

#include "itkLeastSquareDistanceCostFunction.h"
#include "itkResampleImageFilter.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkNormalVariateGenerator.h"
#include "itkVersorRigid3DTransform.h"
#include "itkArray.h"
#include "itkObject.h"

class LandmarkRegistrator : public itk::Object
  {

  public:

    typedef LandmarkRegistrator                           Self;
    typedef itk::Object                                   Superclass;
    typedef itk::SmartPointer<Self>                       Pointer;
    typedef itk::SmartPointer<const Self>                 ConstPointer;

    typedef itk::VersorRigid3DTransform< double >         TransformType;
    typedef itk::LeastSquareDistanceCostFunction<
                                      TransformType >     MetricType;

    typedef MetricType::PointType                         LandmarkType ;
    typedef MetricType::PointSetType                      LandmarkSetType;

    typedef itk::OnePlusOneEvolutionaryOptimizer          OptimizerType;
    typedef itk::Statistics::NormalVariateGenerator       NormalGeneratorType;
    typedef TransformType::ParametersType                 ParametersType;
    typedef TransformType::ParametersType                 ScalesType;

    itkNewMacro(LandmarkRegistrator);

    itkTypeMacro(LandmarkRegistrator,Object);

    //
    itkSetObjectMacro(Metric,MetricType);
    itkGetConstObjectMacro(Metric,MetricType);

    itkSetMacro(InitialTransformParameters,ParametersType);
    itkGetConstMacro(InitialTransformParameters,ParametersType);

    itkSetObjectMacro(Transform,TransformType);
    itkGetObjectMacro(Transform,TransformType);

    itkSetMacro(OptimizerScales,ScalesType);
    itkGetMacro(OptimizerScales,ScalesType);
    itkSetMacro(OptimizerNumberOfIterations,unsigned int);
    itkGetMacro(OptimizerNumberOfIterations,unsigned int);

    itkSetObjectMacro(FixedLandmarkSet,LandmarkSetType);
    itkGetObjectMacro(FixedLandmarkSet,LandmarkSetType);

    itkSetObjectMacro(MovingLandmarkSet,LandmarkSetType);
    itkGetObjectMacro(MovingLandmarkSet,LandmarkSetType);

    void StartRegistration();

  protected:

    virtual void PrintSelf( std::ostream &os, itk::Indent indent ) const;

    LandmarkRegistrator();

    ~LandmarkRegistrator();

    void CopyLandmarkSet( LandmarkSetType::Pointer source,
                          LandmarkSetType::Pointer dest ) const;

    LandmarkSetType::Pointer    m_FixedLandmarkSet;
    LandmarkSetType::Pointer    m_MovingLandmarkSet;
    MetricType::Pointer         m_Metric;
    TransformType::Pointer      m_Transform;
    ParametersType              m_InitialTransformParameters;
    NormalGeneratorType::Pointer    m_Generator;
    OptimizerType::Pointer      m_Optimizer;
    ScalesType                  m_OptimizerScales;
    unsigned int                m_OptimizerNumberOfIterations;

  private:

    LandmarkRegistrator(const Self&);  //purposely not implemented
    void operator=(const Self&);  //purposely not implemented
  };

#endif //__LandmarkRegistrator_h

