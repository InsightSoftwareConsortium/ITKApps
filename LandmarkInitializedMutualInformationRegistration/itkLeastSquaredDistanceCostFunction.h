#ifndef _itkLeastSquaredDistanceCostFunction__h
#define _itkLeastSquaredDistanceCostFunction__h

#include <itkSingleValuedCostFunction.h>
#include <itkPoint.h>
#include <itkArray.h>
#include <itkVectorContainer.h>
#include <itkVersorRigid3DTransform.h>

namespace itk
{

/**
 *
 * This class calculate the sum of of the distances between two set of points.
 *
 */

  template< class TTransform = itk::VersorRigid3DTransform<double> >
  class LeastSquaredDistanceCostFunction
  : public SingleValuedCostFunction
  {

  public:

    typedef LeastSquaredDistanceCostFunction       Self;
    typedef SingleValuedCostFunction              Superclass;
    typedef SmartPointer<Self>                    Pointer;
    typedef SmartPointer< const Self >            ConstPointer;

    typedef itk::Point<double,3>                  PointType;
    typedef itk::VectorContainer<int,PointType >  PointSetType;
    typedef PointSetType::Pointer                 PointSetPointer;

    typedef TTransform                            TransformType;
    typedef typename TransformType::Pointer       TransformPointer;

    typedef double                                MeasureType;

    itkNewMacro(Self);

    itkTypeMacro(LeastSquaredDistanceCostFunction,SingleValuedCostFunction);
  
    void SetFixedPointSet( PointSetPointer fixedPointSet );

    void SetMovingPointSet( PointSetPointer movingPointSet );

    double GetValue( const ParametersType &parameters ) const;

    void GetDerivative( const ParametersType &parameters,
                        DerivativeType &derivative ) const;

    unsigned int GetNumberOfParameters(void) const;

  protected:

    bool                m_Valid;
    TransformPointer    m_Transform;

    PointSetPointer     m_FixedPointSet;
    PointSetPointer     m_MovingPointSet;

    LeastSquaredDistanceCostFunction();

    virtual ~LeastSquaredDistanceCostFunction();

    virtual void PrintSelf( std::ostream &os, Indent indent ) const;

  private:

   LeastSquaredDistanceCostFunction(const Self&);  //purposely not implemented
   void operator=(const Self&);                   //purposely not implemented

  };

} //end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
  #include "itkLeastSquaredDistanceCostFunction.txx"
#endif

#endif //_itkLeastSquaredDistanceCostFunction__h

