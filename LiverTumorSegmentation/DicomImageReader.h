/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DicomImageReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ISIS_DicomImageReader_h__
#define __ISIS_DicomImageReader_h__

#include "DicomImageReaderGUI.h"

/** 
  \class DicomImageReader
  \brief Class for reading DICOM series files with FLTK interface.

  This class is derived from the DicomImageReaderGUI class. It defines
  the necessary callbacks of the DicomImageReaderGUI class.

*/
namespace ISIS
{

template< class TVolumeType >
class DicomImageReader : public DicomImageReaderGUI
{
public:

  typedef TVolumeType                           VolumeType;
  typedef typename VolumeType::Pointer          VolumePointer;
  typedef itk::ImageSeriesReader< VolumeType >  ReaderType; 
  typedef typename ReaderType::Pointer          ReaderPointer; 

public:
  DicomImageReader( void );

  virtual ~DicomImageReader( void );

  /**  The method GetOutput returns pointer to the read image data.  
  */

  const VolumeType * GetOutput();

  /**  The method CollectSeriesAndSelectOne props up an FLTK window
       with the list of names of all series names whose images are 
     found in the chosen image directory. The user choses the series of 
     choice from the list.
 */

  void CollectSeriesAndSelectOne();

  /**  The method checks if the image data has been read.  
  */
  bool IsVolumeLoaded() const;

  /**  The method adds "command" to be executed
       at the end of the read event.  
  */
  void AddObserver( itk::Command * );


protected:

  void ReadVolume();


public:

  ReaderPointer      m_Reader; 
  
  bool               m_VolumeIsLoaded;

  itk::Object::Pointer    m_Notifier;

};


}  // end namespace ISIS

  
#ifndef ITK_MANUAL_INSTANTIATION
#include "DicomImageReader.txx"
#endif

#endif



