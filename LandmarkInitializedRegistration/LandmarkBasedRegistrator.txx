#ifndef __LandmarkBasedRegistrator_txx
#define __LandmarkBasedRegistrator_txx

#include "LandmarkBasedRegistrator.h"

template< class TInputImage, class TOutputImage, class TTransform >
LandmarkBasedRegistrator<TInputImage, TOutputImage, TTransform >
::LandmarkBasedRegistrator()
{
  m_FixedPointSet = PointSetType::New();
  m_MovingPointSet = PointSetType::New();
  m_Input = InputImageType::New();
  m_Output = OutputImageType::New();
  m_Optimizer = OptimizerType::New();
  m_Generator = NormalGeneratorType::New();
  m_Transform = TransformType::New();
  m_Resampler = ResampleFilterType::New();
  m_CostFunction = CostFunctionType::New();

  m_NumberOfIterations = 500;

  m_Scales.resize(m_Transform->GetNumberOfParameters());
  m_Scales.Fill(1);

  m_InitialPosition.resize(m_Transform->GetNumberOfParameters());
  m_InitialPosition.Fill(0) ;
}

template< class TInputImage, class TOutputImage, class TTransform >
LandmarkBasedRegistrator<TInputImage, TOutputImage, TTransform >
::~LandmarkBasedRegistrator()
{

}

template< class TInputImage, class TOutputImage, class TTransform >
void
LandmarkBasedRegistrator<TInputImage, TOutputImage, TTransform >
::StartRegistration()
{ 
  double initialization[3] = {1.05,1.05,0.95};
  
  m_Generator->Initialize(1289);

  if( m_FixedPointSet->Size() != m_MovingPointSet->Size() )
    {
    itk::ExceptionObject e("LandmarkBasedRegistrator.txx",77);
    e.SetLocation("LandmarkBasedRegistrator<TInputImage,TOutputImage,TTransform>::Register()");
    e.SetDescription("cannot register point sets that do not have the same number of elements");
    throw(e);
    }

  try
    {
    m_CostFunction->SetFixedPointSet(m_FixedPointSet);
    m_CostFunction->SetMovingPointSet(m_MovingPointSet);

    m_Optimizer->Initialize(initialization[0],initialization[1],initialization[2]);
    m_Optimizer->SetInitialPosition(m_InitialPosition);
    m_Optimizer->SetScales(m_Scales);
    m_Optimizer->SetCostFunction(m_CostFunction);
    m_Optimizer->SetNormalVariateGenerator(m_Generator);
    m_Optimizer->SetMaximumIteration(m_NumberOfIterations);
    m_Optimizer->StartOptimization();
    }
  catch( itk::ExceptionObject &e )
    {
    std::cout<<"-------------------------------------------------"<<std::endl;
    std::cout<<"Exception caught in LandmarkBasedRegistrator:"<<std::endl;
    std::cout<<e<<std::endl;
    std::cout<<"-------------------------------------------------"<<std::endl;
    }
  catch( ... )
    {
    std::cout<<"-------------------------------------------------"<<std::endl;
    std::cout<<"Exception caught in LandmarkBasedRegistrator:"<<std::endl;
    std::cout<<"unknown exception caught !!!"<<std::endl;
    std::cout<<"-------------------------------------------------"<<std::endl;
    }

  m_Transform = TransformType::New();
  m_Transform->SetParameters(m_Optimizer->GetCurrentPosition());

  m_Resampler->SetInput(m_Input);
  m_Resampler->SetTransform(m_Transform.GetPointer());
  m_Resampler->Update();

  m_Output = m_Resampler->GetOutput();
}

template< class TInputImage, class TOutputImage, class TTransform >
void 
LandmarkBasedRegistrator<TInputImage, TOutputImage, TTransform >
::PrintSelf( std::ostream &os, Indent indent ) const
{
  os<<"-------------------------------------"<<std::endl;
  Superclass::PrintSelf(os,indent);
  os<<"FixedPointSet: "<<m_FixedPointSet<<std::endl;
  os<<"MovingPointSet: "<<m_MovingPointSet<<std::endl;
  os<<"Cost Function: "<<m_CostFunction<<std::endl;
  os<<"Initial Paremeters: "<<m_InitialPosition<<std::endl;
  os<<"Scales: "<<m_Scales<<std::endl;
  os<<"Transform: "<<m_Transform<<std::endl;
  os<<"Optimizer: "<<m_Optimizer<<std::endl;
}

template< class TInputImage, class TOutputImage, class TTransform >
void 
LandmarkBasedRegistrator<TInputImage, TOutputImage, TTransform >
::CopyPointSet( PointSetPointer source, PointSetPointer dest ) const
{
  unsigned int i;
  unsigned int size= source->Size();
  
  dest->Initialize();

  std::cout<<"CopyPointSet("<<size<<")"<<std::endl;

  for( i=0; i<size; i++ )
    {
    dest->InsertElement(i,source->GetElement(i));
    std::cout<<"Source["<<i<<"] :"<<source->GetElement(i)<<std::endl;
    std::cout<<"Dest["<<i<<"] :"<<dest->GetElement(i)<<std::endl;
    }

  std::cout<<"Size after copy: "<<dest->Size()<<std::endl;

}

#endif //__LandmarkBasedRegistrator_txx

