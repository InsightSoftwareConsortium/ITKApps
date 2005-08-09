/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    fldqcRemoteNetworkQuery.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _fldqcFunctions_H
#define _fldqcFunctions_H

#include "fldqcDatatypes.h"
#include "fldqcPreferences.h"
#include "fldqcNet.h"

#include <dcfilefo.h>
#include <dcdebug.h>
#include <ofstd.h>
#include <dcuid.h>
#include <dcdeftag.h>
#include <diutil.h>


typedef struct
{
  DBEntry *db;
  PatientData *patient;
  StudyData *study;
  SeriesData *series;
}GenericCallbackStruct;


typedef OFBool (*GenericEntryCallbackFunction)(GenericCallbackStruct *cbstruct, DcmDataset *reply);

OFBool RemoteFindQuery(Config *conf, DBEntry *db, DcmDataset *query, GenericEntryCallbackFunction callbackFunction, GenericCallbackStruct *callbackData);


class RemoteNetworkQuery 
{

public:
  /** Constructor */    
  RemoteNetworkQuery (const fldqcPreferences*);
  /** The destructor */
  ~RemoteNetworkQuery ();

  /** Send Query at Patient Level */
  PatientData*    Patient (Config *, int );
  OFBool          Patient (Config *, int ,Fl_Browser*, const char*, const char*);
  
  /** Send Query at Study Level */      
  StudyData*      Study   (Config *, int );
  OFBool          Study   (Config *, int,Fl_Browser* );
   
  /** Send Query at Series Level */   
  SeriesData*     Series  (Config *, int );
  OFBool          Series  (Config *, int,Fl_Browser* );
  
  /** Send Query at Image Level */    
  ImageData*      Image   (Config *, int );
  OFBool          Image   (Config *, int ,Fl_Browser*);
   
  /** Patient level */
  /** Interogate the database and build up a patient structure. */
  OFBool BuildRemotePatients(Config *conf, DBEntry *db, const char* name, const char* id);
  /**  Interogate the database and destroy a patient entry  */
  void DestroyPatientEntries(DBEntry *db);
  OFBool ActualizePatients(Config *conf);
       
  /** Study level */ 
  /**  Interogate the database and build up a study structure. */ 
  OFBool BuildRemoteStudies(Config *conf, DBEntry *db,PatientData  *patient);
  /**  Interogate the database and destroy a series entry  */
  void DestroyStudyEntries(PatientData *db);
  OFBool ActualizeStudies(Config *);
       
  /** Series level */
  /**  Interogate the database and build up a series structure. */
  OFBool BuildRemoteSeries(Config *conf, DBEntry *db, PatientData *patient,StudyData *study);
  /**  Interogate the database and destroy a study entry  */
  void DestroySeriesEntries(StudyData *study);
  OFBool ActualizeSeries(Config *);
       
  /** Image level */
  /**  Interogate the database and build up a image structure. */
  OFBool BuildRemoteImages(Config *conf, DBEntry *db,PatientData *patient, StudyData *study,SeriesData  *series);
  /**  Interogate the database and destroy an image entry  */
  void DestroyImageEntries(SeriesData *series);
  OFBool ActualizeImages(Config *);   
    
protected:
       
  /** Data relating AP titles, etc */
  const  fldqcPreferences  *prefs;         
      
};


#endif




