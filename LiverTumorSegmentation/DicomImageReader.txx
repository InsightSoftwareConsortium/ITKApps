
#include "DicomImageReader.h"
#include "itkDICOMImageIO2.h"

//#include <Fl/Fl_Ask.H>
#include <FL/fl_ask.H>

namespace ISIS
{


template < class TVolumeType >
DicomImageReader<TVolumeType>
::DicomImageReader()
{
  m_Reader   = ReaderType::New();
  m_Notifier = itk::Object::New();
}



template < class TVolumeType >
DicomImageReader<TVolumeType>
::~DicomImageReader()
{
}





template < class TVolumeType >
const typename DicomImageReader<TVolumeType>::VolumeType *
DicomImageReader<TVolumeType>
::GetOutput()
{
  return m_Reader->GetOutput();
}




template < class TVolumeType >
void
DicomImageReader<TVolumeType>
::AddObserver( itk::Command * command ) 
{
  m_Notifier->AddObserver( itk::EndEvent(), command );
}






template < class TVolumeType >
void
DicomImageReader<TVolumeType>
::CollectSeriesAndSelectOne()
{

  typedef std::vector<std::string> seriesContainer;
  const  seriesContainer seriesUID = m_FilenamesGenerator->GetSeriesUIDs();

  seriesContainer::const_iterator seriesItr = seriesUID.begin();
  seriesContainer::const_iterator seriesEnd = seriesUID.end();
  
  seriesBrowser->clear();
  
  while( seriesItr != seriesEnd )
    {
    seriesBrowser->add( seriesItr->c_str() );
    seriesItr++;
    }
  
  this->ShowSeriesBrowser();
 
}




template < class TVolumeType >
bool
DicomImageReader<TVolumeType>
::IsVolumeLoaded() const
{
  return m_VolumeIsLoaded;
}




template < class TVolumeType >
void
DicomImageReader<TVolumeType>
::ReadVolume()
{

  m_VolumeIsLoaded = false;

  const unsigned int selectedLine = seriesBrowser->value();

  if( selectedLine == 0 )
    {
    return; 
    }
    
  std::string selectedSerie = seriesBrowser->text( selectedLine );
std::cout << "selectedSerie " << selectedSerie << std::endl;

  seriesBrowserWindow->hide();  // added to hide the series window when loading DICOM data
  
  Fl::check();
  
  m_Reader->SetFileNames( m_FilenamesGenerator->GetFileNames( selectedSerie ) );   

  itk::DICOMImageIO2::Pointer  dicomImageIO =  itk::DICOMImageIO2::New();

  m_Reader->SetImageIO( dicomImageIO );

  try
    {
    m_Reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    fl_alert( excp.GetDescription() );
    return;
    }
  catch( std::exception & excp )
    {
    fl_alert( excp.what() );
    return;
    }
  catch( ... )
    {
    fl_alert( "Problem found during reading. Data was not loaded" );
    return;
    }


  m_VolumeIsLoaded = true;

  m_Notifier->InvokeEvent( itk::EndEvent() );

}



}  // end namespace ISIS


