


#include "LiverTumorSegmentationBase.h"


LiverTumorSegmentationBase
::LiverTumorSegmentationBase()
{
  m_VolumeReader      = VolumeReaderType::New();

  m_RescaleIntensity  = RescaleIntensityFilterType::New();
  m_RescaleIntensity->SetOutputMaximum( itk::NumericTraits< VisualizationPixelType >::max() );
  m_RescaleIntensity->SetOutputMinimum( itk::NumericTraits< VisualizationPixelType >::min() );
  m_ITK2VTKAdaptor    = ITK2VTKAdaptorFilterType::New();
  m_ITK2VTKAdaptor->SetInput( m_RescaleIntensity->GetOutput() );
  
  m_SegmentedVolumeRescaleIntensity  = RescaleIntensityFilterType::New();
  m_SegmentedVolumeRescaleIntensity->SetOutputMaximum( itk::NumericTraits< VisualizationPixelType >::max() );
  m_SegmentedVolumeRescaleIntensity->SetOutputMinimum( itk::NumericTraits< VisualizationPixelType >::min() );
  m_SegmentedVolumeITK2VTKAdaptor    = ITK2VTKAdaptorFilterType::New();
  m_SegmentedVolumeITK2VTKAdaptor->SetInput( m_SegmentedVolumeRescaleIntensity->GetOutput() );

  m_ThresholdVolumeFilter = ThresholdFilterType::New();

  m_SeedValue = 0.0f;

    m_Writer = WriterType::New();
}




LiverTumorSegmentationBase
::~LiverTumorSegmentationBase()
{
}


bool LiverTumorSegmentationBase::Load( const char *filename )
{
  if( !filename  || strlen(filename) == 0 )
  {
    return false;
  }

  m_VolumeReader->SetFileName( filename );
  m_VolumeReader->Update();
  m_LoadedVolume = m_VolumeReader->GetOutput();

  return true;
}


void  
LiverTumorSegmentationBase::SetSeedPoint( double x, double y, double z )
{
  if (m_LoadedVolume) 
  {
    itk::Point< double, 3 > point;
    point[0] = x;  
    point[1] = y;
    point[2] = z;

    // compute the index in the image
    VisualizationVolumeType::IndexType index; 
    m_LoadedVolume->TransformPhysicalPointToIndex( point, index );

    // If the point is outside the volume, do not change seed selection
    if( m_LoadedVolume->GetLargestPossibleRegion().IsInside( index ) )    
    {
      m_SeedPoint[0] = x;
      m_SeedPoint[1] = y;
      m_SeedPoint[2] = z;

      m_SeedValue = m_LoadedVolume->GetPixel( index );
      
      m_SeedIndex[0] = index[0];
      m_SeedIndex[1] = index[1];
      m_SeedIndex[2] = index[2];
    }
  }  
}


void 
LiverTumorSegmentationBase::GetSeedPoint(double data[3])
{
  for(int i=0; i<3; i++)
  {
    data[i] = m_SeedPoint[i];
  }
}

bool  LiverTumorSegmentationBase::DoSegmentation( SegmentationModuleType sType )
  {
  if (!m_LoadedVolume) return false;
  
  try 
    {
    switch( sType )
      {
      case THRESHOLD:
        m_ThresholdVolumeFilter->SetInput( m_LoadedVolume );
        m_ThresholdVolumeFilter->SetOutsideValue( 0 );
        m_ThresholdVolumeFilter->ThresholdOutside( 
          static_cast<PixelType>( m_ThresholdLevelSetModule.GetLowerThreshold() ), 
          static_cast<PixelType>( m_ThresholdLevelSetModule.GetUpperThreshold() ));
        m_ThresholdVolumeFilter->Update();
        m_SegmentedVolume = m_ThresholdVolumeFilter->GetOutput();  
        break;

      case CONFIDENCE_CONNECTED:
        m_ConfidenceConnectedModule.SetInput( m_LoadedVolume );
        printf("Seed Index -> ( %d  %d  %d )\n", m_SeedIndex[0], m_SeedIndex[1], m_SeedIndex[2] );
        m_ConfidenceConnectedModule.SetSeedPoint( m_SeedIndex[0], m_SeedIndex[1], m_SeedIndex[2] );
        m_ConfidenceConnectedModule.Execute();
        m_SegmentedVolume = m_ConfidenceConnectedModule.GetOutput(); 
       break;

      case CONNECTED_THRESHOLD:
        m_ConnectedThresholdModule.SetInput( m_LoadedVolume );
        printf("Seed Index -> ( %d  %d  %d )\n", m_SeedIndex[0], m_SeedIndex[1], m_SeedIndex[2] );
        m_ConnectedThresholdModule.SetSeedPoint( m_SeedIndex[0], m_SeedIndex[1], m_SeedIndex[2] );
        m_ConnectedThresholdModule.Execute();
        m_SegmentedVolume = m_ConnectedThresholdModule.GetOutput(); 
        break;

      case THRESHOLD_LEVEL_SET:
        m_ThresholdLevelSetModule.SetInput( m_LoadedVolume );
        printf("Seed Index -> ( %d  %d  %d )\n", m_SeedIndex[0], m_SeedIndex[1], m_SeedIndex[2] );
        m_ThresholdLevelSetModule.SetSeedPoint( m_SeedIndex[0], m_SeedIndex[1], m_SeedIndex[2] );
        m_ThresholdLevelSetModule.Execute();
        m_SegmentedVolume = m_ThresholdLevelSetModule.GetOutput(); 
        break;
        
      case WATERSHED:
        
      case GEODESIC_ACTIVE_CONTOUR:

      default:
        fl_message("The selected segmentation method is not fully implemented");
     }
     
     //**************** For debugging ( R E M O V E       L A T E R  )
//     m_Writer->SetFileName( "confidence_connected.gipl" );
//     m_Writer->SetInput( m_ThresholdLevelSetModule.GetInitialSegmentationOutput() );
//     m_Writer->Update();
     // End 
     
     return true;
  }
  catch( std::exception & e )
    {
    std::cerr << e.what() << std::endl;
    std::cerr << "exception in Liver Tumor Segmentation Base Segmentation Method " << std::endl;
    return false;
    }
}



void LiverTumorSegmentationBase::WriteSegmentedVolume( const char *fname )
{
  if (m_SegmentedVolume)
  {
    m_Writer->SetFileName( fname );
    m_Writer->SetInput( m_SegmentedVolume );
    m_Writer->Update();
  }
}

#include "itkBinaryThresholdImageFilter.h"

void LiverTumorSegmentationBase::WriteBinarySegmentedVolume( const char *fname )
{
  typedef itk::BinaryThresholdImageFilter< VolumeType,
    VolumeType  >                BinaryThresholdImageFilterType;

  BinaryThresholdImageFilterType::Pointer m_BinaryThresholdImageFilterType;

  m_BinaryThresholdImageFilterType = BinaryThresholdImageFilterType::New();
  if (m_SegmentedVolume)
  {
    m_BinaryThresholdImageFilterType->SetUpperThreshold( 255 );
    m_BinaryThresholdImageFilterType->SetLowerThreshold( 1 );

    m_BinaryThresholdImageFilterType->SetInsideValue( 1 );
    m_BinaryThresholdImageFilterType->SetOutsideValue( 0 );

    // Connect the input images
    m_BinaryThresholdImageFilterType->SetInput( m_SegmentedVolume ); 
    m_Writer->SetFileName( fname );
    m_Writer->SetInput( m_BinaryThresholdImageFilterType->GetOutput() );
    m_Writer->Update();
  }
}

/*
bool LiverTumorSegmentationBase::DoThreshold( double lower, double upper )
{
  if (!m_LoadedVolume) return false;

  try 
   {
    m_ThresholdVolumeFilter->SetInput( m_LoadedVolume );
    m_ThresholdVolumeFilter->SetOutsideValue( 0 );
    m_ThresholdVolumeFilter->ThresholdOutside( static_cast<PixelType>( lower ), 
                                               static_cast<PixelType>( upper ) );
    m_ThresholdVolumeFilter->Update();
    m_ThresholdedVolume = m_ThresholdVolumeFilter->GetOutput();  
    return true;
  }
  catch( std::exception & ex )
  {
    std::cerr << ex.what() << std::endl;
    std::cerr << "exception in Liver Tumor Segmentation Test " << std::endl;
    return false;
  }
}
*/


