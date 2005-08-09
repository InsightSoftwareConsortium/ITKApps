/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    FlDicomQueryChooser.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef  FlDicomQueryChooser_H
#define  FlDicomQueryChooser_H

#include  "fldqcDatatypes.h"
#include  "fldqcRemoteNetworkQuery.h"
#include  "fldqcLocalDirectoryQuery.h"
#include  "ImageViewerGUI.h"
#include  "FlDicomQueryChooserGUI.h" 

#include <ofstdinc.h>
#include <itkImageSeriesReader.h>
#include <itkImageFileReader.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkImageFileWriter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkImage.h>
#include <FL/Fl_Window.H>
#include <FL/fl_file_chooser.H>
#include <GLSliceView.h>
#include <FL/fl_ask.H>

#ifdef WITH_ZLIB
#include <zlib.h>          /* for zlibVersion() */
#endif

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

/** Location of the preference file */
#ifdef WIN32
#define DCMTKPREFSCFG "networkprefs.cfg"
#else
#define DCMTKPREFSCFG "$HOME/.networkprefsrc"
#endif 

/*
typedef enum {
    PATIENT,
    STUDY ,
    SERIES ,
    IMAGE ,
    UNDEFINED
}   QueryLevels;
*/

class FlDicomQueryChooser : public FlDicomQueryChooserGUI   
{

public:  
  /** Default constructor */
  FlDicomQueryChooser();  
  /** Destructor */
  ~FlDicomQueryChooser(); 
  
  typedef itk::Image<short,3> ImageType;
  typedef ImageType::Pointer  ImagePointer;

  void LoadConfigurationInfo(  char *, const char *, const char*, char*, const char*, const char*, Fl_Browser*);  
  int  DataBaseRegister( char *);
  void AddOverrideKey( const char* );
  void CreateConfigEntries( Config *, int , const char* , char *, const char * );
  /** Set output browser */
  void SetOutput (Fl_Browser*);              
    
  /** Write message to output */
  void    WriteOutput (const char*);            
  
  /** Dicom Functions */
  /** Loads Dicom Objects to ITK */
  void    LoadToITK (OFList<OFString> *, char *);
  /** Saves Dicom Objects at user-specifed location and Image format */
  void    SaveObject (OFList<OFString> *, char *, char *);
  /** Generates a .raw file and dumps the header contents to std::out  */
  int     Dump (char *);
  /** Displays the  Dicom Objects using ImageViewer */
  int     Display(OFList<OFString> *); 
  /** Moves the Dicom Object from the Remote Server to the local machine */
  int     MoveObjectFromServer   (char *, char *, char *, char *, char *, const char *, const char*, const char *, const char *, char * );
  /** Show the GUI */
  void Show();
  void NetworkQueryGo();
  void DirectoryQueryGo();
  void Patient();
  void Study();
  void Series();
  void Image();
  void Move();
  void Load();
  void Save();
  void DumpFile();

  ImagePointer LoadImage();

protected: 
  
  Config                  m_Configuration;  
  /** Preferences */                    
  RemoteNetworkQuery      *qMod; 
  LocalDirectoryQuery     *m_LocalQuery;

};

#endif
