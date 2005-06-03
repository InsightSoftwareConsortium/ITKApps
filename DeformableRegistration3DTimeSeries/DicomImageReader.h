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
  typedef TVolumeType                             VolumeType4D;
  typedef typename VolumeType4D::Pointer          VolumePointer;
public:
  DicomImageReader( void );

  virtual ~DicomImageReader( void );

  /**  The method GetOutput returns pointer to the read image data.  
  */

   VolumeType4D* GetOutput()
  {
    return image4D;
  }

  /**  The method CollectSeriesAndSelectOne props up an FLTK window
       with the list of names of all series names whose images are 
     found in the chosen image directory. The user choses the series of 
     choice from the list.
 */

  void CollectSeriesAndSelect();
  void AddSelectOne();
  void RemoveSelectOne();
  

  /**  The method checks if the image data has been read.  
  */
  bool IsVolumeLoaded() const;

  /**  The method adds "command" to be executed
       at the end of the read event.  
  */
  void AddObserver( itk::Command * );
  void ReadVolume();

public:
  bool                    m_VolumeIsLoaded;
  VolumePointer           image4D;
  itk::Object::Pointer    m_Notifier;
};


}  // end namespace ISIS
#ifndef ITK_MANUAL_INSTANTIATION
#include "DicomImageReader.txx"
#endif
  
#endif


