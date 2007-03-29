/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLeastSquaredDistanceCostFunction.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "itkLeastSquaredDistanceCostFunction.h"

namespace itk
  {

/**
 *
 * Constructor
 *
 */

template< class TTransform >
LeastSquaredDistanceCostFunction<TTransform>
::LeastSquaredDistanceCostFunction()
  {
  m_Valid = 0;
  m_Transform = TransformType::New();
  m_FixedPointSet = PointSetType::New();
  m_MovingPointSet = PointSetType::New();
  }

/**
 *
 * Destructor
 *
 */

template< class TTransform >
LeastSquaredDistanceCostFunction<TTransform>
::~LeastSquaredDistanceCostFunction()
  {

  }

/**
 *
 * Set the fixed point set.
 * If the point set passed as argument is empty, an exception 
 * is generated.
 *
 */

template< class TTransform >
void
LeastSquaredDistanceCostFunction<TTransform>
::SetFixedPointSet( PointSetPointer fixedPointSet )
  {
  if( fixedPointSet->GetNumberOfPoints() == 0 )
    {
    itk::ExceptionObject e("itkLeastSqaureDistanceCostFunction.txx",50);
    e.SetLocation("itk::LeastSquaredDistanceCostFunction::SetFixedPointSet()");
    e.SetDescription("the point set passed as argument is of size 0...");
    throw(e);
    }

  m_FixedPointSet = fixedPointSet;
  
  if( m_MovingPointSet->GetNumberOfPoints() )
    {
    if( m_FixedPointSet->GetNumberOfPoints() 
        == m_MovingPointSet->GetNumberOfPoints() )
      {
      m_Valid = true;
      }
    }
  }

/**
 *
 * Print information about the class.
 *
 */

template< class TTransform >
void
LeastSquaredDistanceCostFunction<TTransform>
::PrintSelf( std::ostream &os, Indent indent ) const
  {
  unsigned int fixedPointSetSize = m_FixedPointSet->GetNumberOfPoints();
  unsigned int movingPointSetSize = m_MovingPointSet->GetNumberOfPoints();

  Superclass::PrintSelf(os,indent);
  
  os<<"Fixed Pointset Size: "<<fixedPointSetSize<<std::endl;
  os<<"Fixed Pointset: "<<std::endl;
  for( unsigned int i=0; i<fixedPointSetSize; i++)
    {
    os<<"   ["<<m_FixedPointSet->GetPoint(i)->GetPosition()[0]<<"]";
    os<<"   ["<<m_FixedPointSet->GetPoint(i)->GetPosition()[1]<<"]";
    os<<"   ["<<m_FixedPointSet->GetPoint(i)->GetPosition()[2]<<"]";
    os<<std::endl;
    }
  
  os<<"Moving Pointset Size: "<<movingPointSetSize<<std::endl;
  os<<"Moving Pointset: "<<std::endl;
  for( unsigned int i=0; i<movingPointSetSize; i++)
    {
    os<<"   ["<<m_MovingPointSet->GetPoint(i)->GetPosition()[0]<<"]";
    os<<"   ["<<m_MovingPointSet->GetPoint(i)->GetPosition()[1]<<"]";
    os<<"   ["<<m_MovingPointSet->GetPoint(i)->GetPosition()[2]<<"]";
    os<<std::endl;
    }
  }

/**
 *
 * Set the moving point set.
 * If the moving point set size is not the same as
 * the fixed image point set, an exception will be 
 * generated, and the moving point set will not be
 * set.
 *
 */

template< class TTransform >
void
LeastSquaredDistanceCostFunction<TTransform>
::SetMovingPointSet( PointSetPointer movingPointSet )
  {
  if( movingPointSet->GetNumberOfPoints() == 0 )
    {
    itk::ExceptionObject e("itkLeastSquaredDistanceCostFunction.txx",121);
    e.SetLocation("itk::LeastSquaredDistanceCostFunction::SetMovingPointSet()");
    e.SetDescription("the point set passed as argument is of size 0...");
    throw(e);
    }
  
  m_MovingPointSet = movingPointSet;

  if( m_FixedPointSet->GetNumberOfPoints() )
    {
    if( m_MovingPointSet->GetNumberOfPoints() 
        == m_FixedPointSet->GetNumberOfPoints() )
      {
      m_Valid = true;
      }
    }
  }

template< class TTransform >
void
LeastSquaredDistanceCostFunction<TTransform>
::SetCenter( const TransformCenterPointType & pnt )
  {
  m_Transform->SetCenter( pnt );
  }

/**
 *
 * Return the sum of the square distances between 
 * each point of the fixed point set, and its corresponding
 * point in the moving point set.
 *
 */

template< class TTransform >
double
LeastSquaredDistanceCostFunction<TTransform>
::GetValue( const ParametersType &parameters ) const
  {
  if( m_Valid )
    {
    double   maxPointSquaredDistance = 0;
    double   pointSquaredDistance = 0;
    double   squaredEuclideanDistance = 0.0;
    PointType     transformedPoint;
    unsigned int  pointSetSize = m_FixedPointSet->GetNumberOfPoints();

    m_Transform->SetParameters(parameters);
    
    for( unsigned int i=0; i<pointSetSize; i++)
      {
      transformedPoint.SetPosition(m_Transform->TransformPoint(
                                      m_FixedPointSet->GetPoint(i)
                                                      ->GetPosition()));
      pointSquaredDistance = 0;
      for( unsigned int j=0; j<3; j++)
        {
        double tf = (m_MovingPointSet->GetPoint(i)->GetPosition()[j] -
                                 transformedPoint.GetPosition()[j]);
        pointSquaredDistance += tf * tf;
        }
      if(pointSquaredDistance > maxPointSquaredDistance)
        {
        maxPointSquaredDistance = pointSquaredDistance;
        }
      squaredEuclideanDistance += pointSquaredDistance;
      }
    squaredEuclideanDistance -= 0.5 * maxPointSquaredDistance;
    return squaredEuclideanDistance;
    }
  else
    {
    itk::ExceptionObject e("itkLeastSquaredDistanceCostFunction.txx",169);
    e.SetLocation("itk::LeastSquaredDistanceCostFunction::GetValue()");
    e.SetDescription("Point sets do not have the same number of elements");
    throw(e);
    }
  }

/**
 *
 * No need to return a derivative for that class.
 * This function is just declared to fit into the API 
 * declared by the upper class.
 *
 */

template< class TTransform >
void
LeastSquaredDistanceCostFunction<TTransform>
::GetDerivative( const ParametersType &, DerivativeType & ) const
  {
  // not implemented yet
  }

/**
 *
 * Return the number of parameters of the transform.
 *
 */

template< class TTransform >
unsigned int 
LeastSquaredDistanceCostFunction<TTransform>
::GetNumberOfParameters(void) const
  {
  return m_Transform->GetNumberOfParameters();
  }

} //namespace
