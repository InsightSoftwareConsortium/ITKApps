#ifndef __LandmarkBasedRegistrator_h
#define __LandmarkBasedRegistrator_h

#include "itkLeastSquareDistanceCostFunction.h"
#include "itkResampleImageFilter.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkNormalVariateGenerator.h"
#include "itkArray.h"
#include "itkObject.h"

namespace itk
{
  template< class TInputImage = itk::Image<double,3>, 
            class TOutputImage = itk::Image<double,3>,
            class TTransform = itk::QuaternionRigidTransform<double> >
  class LandmarkBasedRegistrator
  : public Object
  {

  public:

    typedef LandmarkBasedRegistrator< TInputImage,
                            TOutputImage,
                            TTransform >                  Self;
    typedef itk::Object                                   Superclass;
    typedef SmartPointer<Self>                            Pointer;
    typedef SmartPointer<const Self>                      ConstPointer;

    typedef itk::LeastSquareDistanceCostFunction<
                                      TTransform >        CostFunctionType;
    typedef typename CostFunctionType::Pointer            CostFunctionPointer;

    typedef itk::ResampleImageFilter< TInputImage,
                                      TOutputImage >      ResampleFilterType;
    typedef typename ResampleFilterType::Pointer          ResampleFilterPointer;

    typedef typename CostFunctionType::PointType PointType ;
    typedef typename CostFunctionType::PointSetType       PointSetType;
    typedef typename PointSetType::Pointer                PointSetPointer;

    typedef TInputImage                                   InputImageType;
    typedef typename InputImageType::Pointer              InputImagePointer;

    typedef TOutputImage                                  OutputImageType;
    typedef typename OutputImageType::Pointer             OutputImagePointer;

    typedef TTransform                                    TransformType;
    typedef typename TransformType::Pointer               TransformPointer;

    typedef itk::OnePlusOneEvolutionaryOptimizer          OptimizerType;
    typedef typename OptimizerType::Pointer               OptimizerPointer;

    typedef itk::Statistics::NormalVariateGenerator       NormalGeneratorType;
    typedef typename NormalGeneratorType::Pointer         NormalGeneratorPointer;

    typedef itk::Array<double>                            DoubleArrayType;

    itkNewMacro(LandmarkBasedRegistrator);

    itkTypeMacro(LandmarkBasedRegistrator,Object);

    itkSetMacro(Input,InputImagePointer);

    itkGetConstMacro(Input,InputImagePointer);

    itkSetMacro(CostFunction,CostFunctionPointer);

    itkGetConstMacro(CostFunction,CostFunctionPointer);

    itkSetMacro(InitialPosition,DoubleArrayType);

    itkGetConstMacro(InitialPosition,DoubleArrayType);

    itkSetMacro(Transform,TransformPointer);

    itkSetMacro(Scales,DoubleArrayType &);

    itkGetConstReferenceMacro(Scales,DoubleArrayType);

    itkGetConstMacro(Transform,TransformPointer);

    itkGetConstMacro(Output,OutputImagePointer);

    itkSetMacro(FixedPointSet,PointSetPointer);

    itkGetConstMacro(FixedPointSet,PointSetPointer);

    itkSetMacro(MovingPointSet,PointSetPointer);

    itkGetConstMacro(MovingPointSet,PointSetPointer);

    itkSetMacro(NumberOfIterations,unsigned int);

    itkGetMacro(NumberOfIterations,unsigned int);

    void StartRegistration();

  protected:

    virtual void PrintSelf( std::ostream &os, Indent indent ) const;

    LandmarkBasedRegistrator();

    ~LandmarkBasedRegistrator();

    void CopyPointSet( PointSetPointer source, PointSetPointer dest ) const;

    unsigned int            m_NumberOfIterations;
    PointSetPointer         m_FixedPointSet;
    PointSetPointer         m_MovingPointSet;
    CostFunctionPointer     m_CostFunction;
    InputImagePointer       m_Input;
    OutputImagePointer      m_Output;
    DoubleArrayType         m_InitialPosition;
    TransformPointer        m_Transform;
    ResampleFilterPointer   m_Resampler;
    OptimizerPointer        m_Optimizer;
    NormalGeneratorPointer  m_Generator;
    DoubleArrayType         m_Scales;


  private:

    LandmarkBasedRegistrator(const Self&);  //purposely not implemented
    void operator=(const Self&);  //purposely not implemented
  };

} //end of namespace

#ifndef ITK_MANUAL_INSTANTIATION
  #include "LandmarkBasedRegistrator.txx"
#endif

#endif //__LandmarkBasedRegistrator_h

