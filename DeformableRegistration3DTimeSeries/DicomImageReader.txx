
#include "DicomImageReader.h"
#include "itkDICOMImageIO2.h"
#include "itkImageFileWriter.h"
//#include <Fl/Fl_Ask.H>
#include <Fl/fl_ask.H>

namespace ISIS
{
template < class TVolumeType >
DicomImageReader<TVolumeType>
::DicomImageReader()
{
//  m_Reader   = ReaderType::New();
  m_Notifier = itk::Object::New();
  image4D =VolumeType4D::New();
}

template < class TVolumeType >
DicomImageReader<TVolumeType>
::~DicomImageReader()
{
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
::CollectSeriesAndSelect()
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
void
DicomImageReader<TVolumeType>
::AddSelectOne()
{
  SelectedSeries->add( seriesBrowser->text(seriesBrowser->value()));
  this->ShowSeriesBrowser(); 
}


template < class TVolumeType >
void
DicomImageReader<TVolumeType>
::RemoveSelectOne()
{
  SelectedSeries->remove( SelectedSeries->value());

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
  seriesBrowserWindow->hide();  // added to hide the series window when loading DICOM data
  
  Fl::check();

  if( SelectedSeries->size() == 0 )
    {
    return; 
    }
  //typedef itk::Image<unsigned short, 4 >                VolumeType4D; 
  typedef itk::Image<unsigned short, 3 >                  VolumeType3D; 
  typedef itk::ImageSeriesReader< VolumeType3D >          ReaderType3D; 
  typename ReaderType3D::Pointer                                   m_Reader3D=ReaderType3D::New();  
  typedef itk::ImageFileWriter< VolumeType4D >            WriterType4D; 
  typename WriterType4D::Pointer                                   file_writer = WriterType4D::New();
  
  //typename VolumeType4D::Pointer image4D =VolumeType4D::New();
  //Adaugata
  
  typename VolumeType3D::Pointer image =VolumeType3D::New();
  std::string selectedSerie = SelectedSeries->text( 1);
  std::cout<<selectedSerie<<std::endl;
  m_Reader3D->SetFileNames( m_FilenamesGenerator->GetFileNames(selectedSerie));   
  typename itk::DICOMImageIO2::Pointer  dicomImageIO =  itk::DICOMImageIO2::New();
  m_Reader3D->SetImageIO( dicomImageIO );
   m_Reader3D->Update();
  typename VolumeType4D::SizeType size1;
  size1[ 0 ]=m_Reader3D->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
  size1[ 1 ] =m_Reader3D->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
  size1[ 2 ] =m_Reader3D->GetOutput()->GetLargestPossibleRegion().GetSize()[2];
  size1[ 3 ]=SelectedSeries->size();
  image4D->SetRegions( size1 );
  typename VolumeType3D::SpacingType spacing3D;
  typename VolumeType4D::SpacingType spacing4D;
  spacing3D =m_Reader3D->GetOutput()->GetSpacing();
  spacing4D[0]=spacing3D[0];
  spacing4D[1]=spacing3D[1];
  spacing4D[2]=spacing3D[2];
  spacing4D[3]=1;
  image4D->SetSpacing(spacing4D);

  image4D->Allocate();

  typedef itk::ImageRegionIterator< VolumeType4D >  Iterator4D;

  Iterator4D it4( image4D, image4D->GetBufferedRegion() );
  it4.GoToBegin();


  typedef itk::ImageRegionConstIterator< VolumeType3D >  Iterator3D;
  image=m_Reader3D->GetOutput();
  typename VolumeType3D::RegionType region = image->GetLargestPossibleRegion();
  Iterator3D it3( image, region );
  it3.GoToBegin();
  while( !it3.IsAtEnd() )
    {
    it4.Set( it3.Get() );
    ++it3;
    ++it4;
    }
 
  for(int i=2;i<SelectedSeries->size()+1;++i)
  {
    std::string selectedSerie = SelectedSeries->text( i );
    std::cout<<selectedSerie<<std::endl;
    m_Reader3D->SetFileNames( m_FilenamesGenerator->GetFileNames(selectedSerie));   
    m_Reader3D->SetImageIO( dicomImageIO );
    
    try
      {
      m_Reader3D->Update();
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
    image=m_Reader3D->GetOutput();
    //typename VolumeType3D ::RegionType region = image->GetLargestPossibleRegion();
    Iterator3D it3( image, region );
    it3.GoToBegin();
    while( !it3.IsAtEnd() )
      {
      it4.Set( it3.Get() );
      ++it3;
      ++it4;
      }
  }
  file_writer->SetInput(image4D);
  file_writer->SetFileName("rez.mhd");
  file_writer->Update();
  m_VolumeIsLoaded = true;

  m_Notifier->InvokeEvent( itk::EndEvent() );

}



}  // end namespace ISIS


