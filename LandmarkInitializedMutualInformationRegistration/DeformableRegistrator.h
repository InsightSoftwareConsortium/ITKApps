/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DeformableRegistrator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __DeformableRegistrator_h
#define __DeformableRegistrator_h

#include "itkImage.h"
#include "itkCommand.h"

#include "itkImageRegistrationMethod.h"
#include "itkDeformationFieldSource.h"
#include "itkBSplineDeformableTransform.h"
#include "itkMattesMutualInformationImageToImageMetric.h"

#include "itkOnePlusOneEvolutionaryOptimizer.h"

#include <itkLBFGSBOptimizer.h>
#include "itkNormalVariateGenerator.h"
#include "itkFRPROptimizer.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkStatisticsImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"

namespace itk
{

template< class TImage >
class DeformableRegistrator : public ImageRegistrationMethod < TImage, TImage >
  {
  public:
    typedef DeformableRegistrator Self;
    typedef ImageRegistrationMethod< TImage, TImage> Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkTypeMacro(DeformableRegistrator, ImageRegistrationMethod);

    itkNewMacro(Self);
  
    typedef typename TImage::PixelType PixelType ;
    typedef typename TImage::RegionType RegionType ;

    typedef itk::BSplineDeformableTransform<double, 3, 3> TransformType ;

    typedef enum { ONEPLUSONE,  LBFGS }         OptimizerMethodType;

    typedef OnePlusOneEvolutionaryOptimizer     OnePlusOneOptimizerType ;
    typedef itk::LBFGSBOptimizer                LBFGSOptimizerType;
    typedef Statistics::NormalVariateGenerator  OptimizerNormalGeneratorType;
    typedef TransformType::ParametersType       ParametersType ;
    typedef TransformType::ParametersType       ScalesType ;
    typedef LinearInterpolateImageFunction< TImage, double > 
                                                InterpolatorType ;
    typedef MattesMutualInformationImageToImageMetric< TImage, TImage >
                                                MetricType ;

    typedef typename Superclass::OptimizerType  OptimizerType;
    typedef typename OptimizerType::Pointer     OptimizerPointer;
    
    typedef itk::Vector< double, 3 >            VectorType;
    typedef itk::Image< VectorType, 3 >         DeformationFieldType;
    typedef itk::DeformationFieldSource< DeformationFieldType >              
                                                 DeformationSourceType;
                   
    void StartRegistration() ;

    TransformType * GetTypedTransform(void)
      {
      return static_cast<TransformType *>(Superclass::GetTransform());
      }

    void SetOptimizerToOnePlusOne();
    void SetOptimizerToLBFGS();

    itkSetMacro(OptimizerNumberOfIterations, unsigned int) ;
    itkGetConstMacro(OptimizerNumberOfIterations, unsigned int) ;
    itkSetMacro(NumberOfControlPoints, unsigned int);
    itkGetConstMacro(NumberOfControlPoints, unsigned int);

    MetricType * GetTypedMetric(void)
      {
      return static_cast<MetricType *>(Superclass::GetMetric());
      }

    itkSetMacro(MetricNumberOfSpatialSamples, unsigned int) ;
    itkGetConstMacro(MetricNumberOfSpatialSamples, unsigned int) ;

    itkSetObjectMacro(Observer, Command);

  protected:
    DeformableRegistrator() ;
    virtual ~DeformableRegistrator() ;

    virtual void Initialize() throw(ExceptionObject);

    void PrintUncaughtError() ;

    void PrintError(ExceptionObject &e) ;


  private:

    Command::Pointer        m_Observer;

    OptimizerMethodType     m_OptimizerMethod;
    unsigned int            m_OptimizerNumberOfIterations;

    unsigned int            m_MetricNumberOfSpatialSamples;
    
    unsigned int            m_NumberOfControlPoints;
    

  } ; // end of class


#ifndef ITK_MANUAL_INSTANTIATION
#include "DeformableRegistrator.txx"
#endif

} // end namespace itk

#endif //__DeformableRegistrator_H

