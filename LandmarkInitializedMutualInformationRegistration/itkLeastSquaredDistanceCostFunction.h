/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLeastSquaredDistanceCostFunction.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _itkLeastSquaredDistanceCostFunction__h
#define _itkLeastSquaredDistanceCostFunction__h

#include <itkSingleValuedCostFunction.h>
#include <itkLandmarkSpatialObject.h>
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

    typedef itk::LandmarkSpatialObject<3>         PointSetType;
    typedef PointSetType::SpatialObjectPointType  PointType;
    typedef PointSetType::Pointer                 PointSetPointer;

    typedef TTransform                            TransformType;
    typedef typename TransformType::Pointer       TransformPointer;
    typedef typename TransformType::InputPointType
                                                  TransformCenterPointType;

    typedef double                                MeasureType;

    itkNewMacro(Self);

    itkTypeMacro(LeastSquaredDistanceCostFunction,SingleValuedCostFunction);
  
    void SetFixedPointSet( PointSetPointer fixedPointSet );

    void SetMovingPointSet( PointSetPointer movingPointSet );

    void SetCenter(const TransformCenterPointType & pnt );

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

