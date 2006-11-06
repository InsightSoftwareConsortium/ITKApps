/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    LandmarkRegistrator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __LandmarkRegistrator_h
#define __LandmarkRegistrator_h

#include "itkLeastSquaredDistanceCostFunction.h"
#include "itkResampleImageFilter.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkNormalVariateGenerator.h"
#include "itkVersorRigid3DTransform.h"
#include "itkArray.h"
#include "itkObject.h"

namespace itk
{

class LandmarkRegistrator : public Object
  {

  public:

    typedef LandmarkRegistrator                           Self;
    typedef Object                                   Superclass;
    typedef SmartPointer<Self>                       Pointer;
    typedef SmartPointer<const Self>                 ConstPointer;

    typedef VersorRigid3DTransform< double >         TransformType;
    typedef LeastSquaredDistanceCostFunction<
                                      TransformType >     MetricType;

    typedef MetricType::PointType                         LandmarkType ;
    typedef MetricType::PointSetType                      LandmarkSetType;

    typedef OnePlusOneEvolutionaryOptimizer          OptimizerType;
    typedef Statistics::NormalVariateGenerator       NormalGeneratorType;
    typedef TransformType::ParametersType                 ParametersType;
    typedef TransformType::ParametersType                 ScalesType;

    itkTypeMacro(LandmarkRegistrator,Object);

    itkNewMacro(LandmarkRegistrator);


    //
    itkSetObjectMacro(Metric,MetricType);
    MetricType * GetTypedMetric(void)
      {
      return m_Metric;
      }

    itkSetMacro(InitialTransformParameters,ParametersType);
    itkGetConstMacro(InitialTransformParameters,ParametersType);

    itkSetObjectMacro(Transform,TransformType);
    TransformType * GetTypedTransform(void)
      {
      return m_Transform;
      }

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

    virtual void PrintSelf( std::ostream &os, Indent indent ) const;

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

} // end namespace itk

#endif //__LandmarkRegistrator_h

