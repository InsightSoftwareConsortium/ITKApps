/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    LandmarkRegistrator.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "LandmarkRegistrator.h"

#include "vnl/vnl_vector.h"
#include "vnl/vnl_c_vector.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"

namespace itk
{

LandmarkRegistrator::LandmarkRegistrator()
  {
  m_FixedLandmarkSet = LandmarkSetType::New();
  m_MovingLandmarkSet = LandmarkSetType::New();
  m_Optimizer = OptimizerType::New();
  m_Generator = NormalGeneratorType::New();
  m_Transform = TransformType::New();
  m_Metric = MetricType::New();

  m_Transform->SetIdentity();
  m_InitialTransformParameters = m_Transform->GetParameters();

  m_OptimizerNumberOfIterations = 5000;

  m_OptimizerScales.set_size(6);
  m_OptimizerScales[0] = 200; // rotation
  m_OptimizerScales[1] = 200;
  m_OptimizerScales[2] = 200;
  m_OptimizerScales[3] = 1;   // offset
  m_OptimizerScales[4] = 1;
  m_OptimizerScales[5] = 1;
  }

LandmarkRegistrator
::~LandmarkRegistrator()
  {
  }

void
LandmarkRegistrator
::StartRegistration()
  { 
  m_Generator->Initialize(1289);

  if( m_FixedLandmarkSet->GetNumberOfPoints() 
      != m_MovingLandmarkSet->GetNumberOfPoints() )
    {
    itk::ExceptionObject e("LandmarkRegistrator.txx",77);
    e.SetLocation("LandmarkRegistrator::Register()");
    e.SetDescription("Landmark sets do not have the same number of elements");
    throw(e);
    }

  itk::Point<double, 3> fixedCenter;
  fixedCenter.Fill(0);
  itk::Point<double, 3> movingCenter;
  movingCenter.Fill(0);
  for( unsigned int i=0; i<m_MovingLandmarkSet->GetNumberOfPoints(); i++)
    {
    for( unsigned int j=0; j<3; j++)
      {
      fixedCenter[j] += m_FixedLandmarkSet->GetPoint(i)->GetPosition()[j];
      movingCenter[j] += m_MovingLandmarkSet->GetPoint(i)->GetPosition()[j];
      }
    }
  for( unsigned int j=0; j<3; j++)
    {
    fixedCenter[j] /= m_FixedLandmarkSet->GetNumberOfPoints();
    movingCenter[j] /= m_MovingLandmarkSet->GetNumberOfPoints();
    }
  m_InitialTransformParameters[3] = movingCenter[0]-fixedCenter[0];
  m_InitialTransformParameters[4] = movingCenter[1]-fixedCenter[1];
  m_InitialTransformParameters[5] = movingCenter[2]-fixedCenter[2];

  vnl_vector<double> v1(3);
  vnl_vector<double> v2(3);
  vnl_vector<double> vTemp(3);
  vnl_matrix<double> m(3,3);
  vnl_matrix<double> mTemp(3,3);
  m.set_identity();

  itk::Vector<double, 3> vct;
  itk::Versor<double> vsr;
  itk::Versor<double> vsrTemp;
  itk::Matrix<double, 3, 3> mat;

  mat = m;
  vsr.Set(mat);
  double weight = 0.5;
  for( int count = 0; count < 5; count++)
    {
    for( unsigned int i=0; i<m_MovingLandmarkSet->GetNumberOfPoints(); i++)
      {
      v1 = (m_MovingLandmarkSet->GetPoint(i)->GetPosition().GetVnlVector()
            - movingCenter.GetVnlVector());
      v1.normalize();
      
      v2 = (m_FixedLandmarkSet->GetPoint(i)->GetPosition().GetVnlVector()
            - fixedCenter.GetVnlVector());
      v2.normalize();
      
      vTemp = m * v2;
      vTemp.normalize();
      mat = outer_product(v1, vTemp);
      vsrTemp.Set(mat);

      vct[0] = vsr.GetX() + weight * vsrTemp.GetX();
      vct[1] = vsr.GetY() + weight * vsrTemp.GetY();
      vct[2] = vsr.GetZ() + weight * vsrTemp.GetZ();
      if(vct.GetNorm() > 1) 
        {
        vct.Normalize();
        }
      std::cout << "vct = " << vct << std::endl;
      std::cout << "vct.norm = " << vct.GetNorm() << std::endl;
      vsr.Set(vct);
      std::cout << "   done" << std::endl;

      m = vsr.GetMatrix().GetVnlMatrix();
      }
    weight *= 0.75;
    }

  std::cout << "Landmark quality :" << std::endl;
  for( unsigned int i=0; i<m_MovingLandmarkSet->GetNumberOfPoints(); i++)
    {
    v1 = (m_MovingLandmarkSet->GetPoint(i)->GetPosition().GetVnlVector()
          - movingCenter.GetVnlVector());
    v1.normalize();
    
    v2 = (m_FixedLandmarkSet->GetPoint(i)->GetPosition().GetVnlVector()
          - fixedCenter.GetVnlVector());
    v2.normalize();

    vTemp = vsr.Transform(v1);
    vTemp.normalize();
    std::cout << "Landmark " << i << " : fit as T(m)*f = "
              << dot_product(vTemp, v2) << std::endl;
    }


  m_InitialTransformParameters[0] = vsr.GetRight()[0];
  m_InitialTransformParameters[1] = vsr.GetRight()[1];
  m_InitialTransformParameters[2] = vsr.GetRight()[2];


  std::cout << "LandmarkRegistrator: InitialParameters = " << std::endl
            << m_InitialTransformParameters << std::endl;
  try
    {
    m_Metric->SetFixedPointSet(m_FixedLandmarkSet);
    m_Metric->SetMovingPointSet(m_MovingLandmarkSet);
    m_Metric->SetCenter(fixedCenter);

    m_Optimizer->SetInitialPosition(m_InitialTransformParameters);
    m_Optimizer->SetScales(m_OptimizerScales);
    m_Optimizer->SetCostFunction(m_Metric);
    m_Optimizer->SetNormalVariateGenerator(m_Generator);
    m_Optimizer->SetMaximumIteration(m_OptimizerNumberOfIterations);
    m_Optimizer->Initialize(4.0, 1.1, 0.9); // Initial search radius
    m_Optimizer->SetEpsilon(0.00000001);
    m_Optimizer->StartOptimization();
    }
  catch( itk::ExceptionObject &e )
    {
    std::cout<<"-------------------------------------------------"<<std::endl;
    std::cout<<"Exception caught in LandmarkRegistrator:"<<std::endl;
    std::cout<<e<<std::endl;
    std::cout<<"-------------------------------------------------"<<std::endl;
    }
  catch( ... )
    {
    std::cout<<"-------------------------------------------------"<<std::endl;
    std::cout<<"Exception caught in LandmarkRegistrator:"<<std::endl;
    std::cout<<"unknown exception caught !!!"<<std::endl;
    std::cout<<"-------------------------------------------------"<<std::endl;
    }

  m_Transform = TransformType::New();
  m_Transform->SetParameters(m_Optimizer->GetCurrentPosition());
  m_Transform->SetCenter(fixedCenter);
  }

void 
LandmarkRegistrator
::PrintSelf( std::ostream &os, itk::Indent indent ) const
  {
  os<<"-------------------------------------"<<std::endl;
  Superclass::PrintSelf(os,indent);
  os<<"FixedLandmarkSet: "<<m_FixedLandmarkSet<<std::endl;
  os<<"MovingLandmarkSet: "<<m_MovingLandmarkSet<<std::endl;
  os<<"Metric Function: "<<m_Metric<<std::endl;
  os<<"OptimizerScales: "<<m_OptimizerScales<<std::endl;
  os<<"Transform: "<<m_Transform<<std::endl;
  os<<"Optimizer: "<<m_Optimizer<<std::endl;
  }

void 
LandmarkRegistrator
::CopyLandmarkSet( LandmarkSetType::Pointer source,
                   LandmarkSetType::Pointer dest ) const
  {
  dest->Initialize();

  dest->SetPoints( source->GetPoints() );
  }

} // end namespace itk

