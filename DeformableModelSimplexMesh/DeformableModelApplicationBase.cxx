


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

  PointType::ValueType scaleInit[3] = {10,10,10};
  VectorType scale = scaleInit;
  m_SphereMeshSource->SetCenter(center);
  m_SphereMeshSource->SetResolution(2); 
  m_SphereMeshSource->SetScale(scale);

  m_SimplexFilter  = SimplexFilterType::New();
  m_SimplexFilter->SetInput( m_SphereMeshSource->GetOutput() );

  //deformation stuff
  m_DeformFilter = DeformFilterType::New();

  m_GradientFilter = GradientFilterType::New();

  m_GradientFilter->SetInput( m_VolumeReader->GetOutput() );
  m_GradientFilter->SetSigma( 5.0 );
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


