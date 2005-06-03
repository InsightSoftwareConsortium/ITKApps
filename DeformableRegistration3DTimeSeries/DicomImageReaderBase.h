#ifndef __ISIS_DicomImageReaderBase_h__
#define __ISIS_DicomImageReaderBase_h__

#include "itkImageSeriesReader.h"
#include "itkDICOMSeriesFileNames.h"

/** 
  \class DicomImageReaderBase
  \brief Class for reading DICOM series files.

  This class is the base class for reading DICOM series files. It uses
  the SeriesFileNamesType class of the itk.

*/

namespace ISIS
{

class DicomImageReaderBase
{

  typedef itk::DICOMSeriesFileNames       SeriesFileNamesType;
  typedef SeriesFileNamesType::Pointer    SeriesFileNamesPointer;


public:
  DicomImageReaderBase( void );

  virtual ~DicomImageReaderBase( void );

  /**  The method SetDirectory sets the directory of the DICOM series data.  
  */
  void SetDirectory( const char * directory );

  /**  The method gets the Seried UIDs of the image files in the directory of 
  the DICOM series data.  
  */
  const std::vector<std::string> &GetSeriesUIDs();


protected:

  SeriesFileNamesPointer    m_FilenamesGenerator;

};


}  // end namespace ISIS

  

#endif



