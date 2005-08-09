/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    fldqcDatatypes.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
//
// This document defines the basic units of data holding for each of the four levels of the DICOM query structure
//
#ifndef DATATYPES_H
#define DATATYPES_H

/** make sure OS specific configuration is included first  */
#include "osconfig.h"   

#include <ofcmdln.h>
#include <dicom.h>
#include <cond.h>
#include <assoc.h>
#include <dimse.h>
#include <imagedb.h>
#include <FL/Fl.H>
#include <FL/Fl_Browser.H>
#include <FL/filename.H>

#ifdef WIN32
#define SLASH '/'
#else
#define SLASH '/'  //check this for unix
#endif

/** Constants */
#define MAXDATABASES    100
#define MAXPATIENTS    1000
#define MAXSTUDIES     1000
#define MAXSERIES       500
#define MAXIMAGES      1000
#define E_LEN           255
#define F_LEN          1024

class SeriesData;
class StudyData;
class PatientData;
class DBEntry;
class Config;

/** ImageData handles image data. */
class ImageData      
{
  friend class  RemoteNetworkQuery ;
  friend class  FlDicomQueryChooserGUI;
  friend OFBool AddPatientEntry(DBEntry *, DcmDataset *);
  friend OFBool AddStudyEntry(PatientData *, DcmDataset *);
  friend OFBool AddSeriesEntry(DBEntry *, DcmDataset *);
  friend OFBool AddImageEntry(SeriesData *, DcmDataset *);

public: 
  /** Default Constructor. */
  ImageData(){};
  
public:  
  DIC_UI  m_SopInstanceUID; 
  DIC_IS  m_ImageNumber;
  int     m_IntImageNumber;
};

/** SeriesData handles series data. */
class SeriesData    
{
  friend class  RemoteNetworkQuery ;
  friend class  FlDicomQueryChooserGUI;
  friend OFBool AddPatientEntry(DBEntry *, DcmDataset *);
  friend OFBool AddStudyEntry(PatientData *, DcmDataset *);
  friend OFBool AddSeriesEntry(StudyData *, DcmDataset *);
  friend OFBool AddImageEntry(SeriesData *, DcmDataset *);

public:
  /** Default Constructor. */
  SeriesData(){};

public:  
  DIC_UI  m_SeriesInstanceUID;
  DIC_IS  m_SeriesNumber;
  int     m_IntSeriesNumber;
  DIC_CS  m_Modality;
  /** Array of image pointers */ 
  ImageData *images[MAXIMAGES]; 
  int     m_ImageCount;

};

/** StudyData handles study data. */
class StudyData   
{
  friend class  RemoteNetworkQuery ;
  friend class  FlDicomQueryChooserGUI;
  friend OFBool AddPatientEntry(DBEntry *, DcmDataset *);
  friend OFBool AddStudyEntry(PatientData *, DcmDataset *);
  friend OFBool AddSeriesEntry(StudyData *, DcmDataset *);
  friend OFBool AddImageEntry(SeriesData *, DcmDataset *);

public:
  /** Default Constructor. */
  StudyData(){}; 

public:  
  DIC_UI  m_StudyInstanceUID;
  DIC_CS  m_StudyID;
  /** Array of series pointers */ 
  SeriesData  *series[MAXSERIES]; 
  int     m_SeriesCount;
};

/** PatientData  handles patient data. */
class PatientData
{
  friend class  RemoteNetworkQuery ;
  friend class  FlDicomQueryChooserGUI;
  friend OFBool AddPatientEntry(DBEntry *, DcmDataset *);
  friend OFBool AddStudyEntry(PatientData *, DcmDataset *);
  friend OFBool AddSeriesEntry(StudyData *, DcmDataset *);
  friend OFBool AddImageEntry(SeriesData *, DcmDataset *);

public:
  /** Default constructor */
  PatientData(){};
  
public:  
  DIC_PN  m_PatientsName;
  DIC_LO  m_PatientID;
  /** Array of studies pointers */ 
  StudyData  *studies[MAXSTUDIES];  
  int     m_StudyCount;
};


class DBEntry 
{
  friend class RemoteNetworkQuery;
  friend class  FlDicomQueryChooserGUI;
  friend OFBool AddPatientEntry(DBEntry *, DcmDataset *);
  friend OFBool AddStudyEntry(PatientData *, DcmDataset *);
  friend OFBool AddSeriesEntry(StudyData *, DcmDataset *);
  friend OFBool AddImageEntry(SeriesData *, DcmDataset *);
  friend class FlDicomQueryChooser;

public:
  DBEntry(){};
  /** The CTN AE Title associated with this DB */
  const char *m_Title; 
  /** True if DB is remote */
  OFBool m_IsRemoteDB; 

protected:
  /** Peer titles which can read this database */
  const char **peerTitles;   
  /** Number of peer titles */ 
  int m_PeerTitleCount;             
  PatientData *patients[MAXPATIENTS];
  int m_PatientCount;
  /** Index of current patient */
  int m_CurrentPatient;
  /** Index of current study */
  int m_CurrentStudy; 
  /** Index of current series in current study */
  int m_CurrentSeries; 
  /** Index of current image in current study */
  int m_CurrentImage;                       
};

class Config
{
  friend class RemoteNetworkQuery;
  friend class  FlDicomQueryChooserGUI;
  friend OFBool AddPatientEntry(DBEntry *, DcmDataset *);
  friend OFBool AddStudyEntry(PatientData *, DcmDataset *);
  friend OFBool AddSeriesEntry(StudyData *, DcmDataset *);
  friend OFBool AddImageEntry(SeriesData *, DcmDataset *);
  friend class FlDicomQueryChooser;

public:
  Config(){};
  /** The CTN database we know */ 
  DBEntry **dbEntries;    
  /** Peer to talk to */ 
  const char *m_PeerHostName;  
        
  //int peerNamesCount;
  int m_Peerport;
  /** My application entity title */ 
  const char *m_MyAETitle; 
  /** Active network */ 
  T_ASC_Network *net; 
  /** Number of bytes we can receive */ 
  OFCmdUnsignedInt m_MaxReceivePDULength;
  
  /** Current database index */
  int m_Currentdb; 
  /** Currently active association */
  T_ASC_Association *assoc; 
  /** Current peer title */
  const char *m_CurrentPeerTitle;    
};

#endif
