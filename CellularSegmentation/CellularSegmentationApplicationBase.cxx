


#include "CellularSegmentationApplicationBase.h"


CellularSegmentationApplicationBase
::CellularSegmentationApplicationBase()
{
  m_ImageReader      = ImageReaderType::New();

  m_RescaleIntensity  = RescaleIntensityFilterType::New();

  m_ITK2VTKAdaptor    = ITK2VTKAdaptorFilterType::New();

  m_RescaleIntensity->SetOutputMaximum( itk::NumericTraits< VisualizationPixelType >::max() );
  m_RescaleIntensity->SetOutputMinimum( itk::NumericTraits< VisualizationPixelType >::min() );

  m_IterationObserver = IterationObserverType::New();
  m_IterationObserver->SetCallbackFunction( this, & CellularSegmentationApplicationBase::IterationCallback );

}


CellularSegmentationApplicationBase
::~CellularSegmentationApplicationBase()
{
}


void  
CellularSegmentationApplicationBase::SetSeedPoint( double x, double y, double z )
{
  m_SeedPoint[0] = x;
  m_SeedPoint[1] = y;
  m_SeedPoint[2] = z;
 
}


void 
CellularSegmentationApplicationBase::GetSeedPoint(double data[3])
{
  for(int i=0; i<3; i++)
    {
      data[i] = m_SeedPoint[i];
    }
}


void 
CellularSegmentationApplicationBase::IterationCallback()
{
  std::cout << "Progress..." << std::endl;
}

