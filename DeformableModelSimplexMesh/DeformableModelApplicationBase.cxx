


#include "DeformableModelApplicationBase.h"


DeformableModelApplicationBase
::DeformableModelApplicationBase()
{
  m_VolumeReader      = VolumeReaderType::New();
  m_RescaleIntensity  = RescaleIntensityFilterType::New();

  m_ITK2VTKAdaptor    = ITK2VTKAdaptorFilterType::New();
  m_ITK2VTKAdaptor->SetInput( m_RescaleIntensity->GetOutput() );

  m_RescaleIntensity->SetOutputMaximum( itk::NumericTraits< VisualizationPixelType >::max() );
  m_RescaleIntensity->SetOutputMinimum( itk::NumericTraits< VisualizationPixelType >::min() );

  m_SphereMeshSource        = SphereMeshSourceType::New();
  PointType center; 
  center.Fill(0);
 
  VectorType sphereRadius;
  sphereRadius.Fill( 5.0 );

  m_SphereMeshSource->SetCenter(center);
  m_SphereMeshSource->SetScale( sphereRadius );
  m_SphereMeshSource->SetResolution(3); 

  m_SimplexFilter  = SimplexFilterType::New();
  m_SimplexFilter->SetInput( m_SphereMeshSource->GetOutput() );

  //deformation stuff
  m_DeformFilter = DeformFilterType::New();

  m_GradientFilter = GradientFilterType::New();

  m_GradientFilter->SetInput( m_VolumeReader->GetOutput() );
  m_GradientFilter->SetSigma( 5.0 );

  m_IterationObserver = IterationObserverType::New();
  m_IterationObserver->SetCallbackFunction( this, & DeformableModelApplicationBase::IterationCallback );

  m_DeformFilter->AddObserver( itk::IterationEvent(), m_IterationObserver );
  m_DeformFilter->AddObserver( itk::ProgressEvent(), m_IterationObserver );

}




DeformableModelApplicationBase
::~DeformableModelApplicationBase()
{
}




void  
DeformableModelApplicationBase::SetSeedPoint( double x, double y, double z )
{
  m_SeedPoint[0] = x;
  m_SeedPoint[1] = y;
  m_SeedPoint[2] = z;
 
}


void 
DeformableModelApplicationBase::GetSeedPoint(double data[3])
{
  for(int i=0; i<3; i++)
  {
    data[i] = m_SeedPoint[i];
  }
}


void 
DeformableModelApplicationBase::IterationCallback()
{
  std::cout << "Iteration " << m_DeformFilter->GetProgress() << std::endl;
}

