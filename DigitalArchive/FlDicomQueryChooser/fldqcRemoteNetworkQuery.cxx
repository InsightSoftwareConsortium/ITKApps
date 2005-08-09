/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    fldqcRemoteNetworkQuery.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "fldqcRemoteNetworkQuery.h"
#include "fldqcNet.h"
#include "fldqcLocalDirectoryQuery.h"
 
#include <ofstdinc.h>

#define PATIENTFORMAT "%-30s %-12s \n"
#define STUDYFORMAT   "%-30s %-s \n"
#define SERIESFORMAT  "%-6s %-8s %-s\n"
#define IMAGEFORMAT   "%-5s %-s\n"

 
OFBool AddPatientEntry(DBEntry *db, DcmDataset *reply);
OFBool AddStudyEntry(PatientData *db, DcmDataset *reply);
OFBool AddSeriesEntry(StudyData *study, DcmDataset *reply);
OFBool AddImageEntry(SeriesData *series, DcmDataset *reply);


OFBool GenericEntryCallback(GenericCallbackStruct *cbs, DcmDataset *reply)
{   
  if (cbs->db) 
  {
    return AddPatientEntry(cbs->db, reply);
  }
  if (cbs->patient) 
  {
    return AddStudyEntry(cbs->patient, reply);
  }
  if (cbs->study) 
  {
    return AddSeriesEntry(cbs->study, reply);
  }
  if (cbs->series) 
  {
    return AddImageEntry(cbs->series, reply);
  }
  return OFFalse;
}


/** Find for remote DBs */
typedef struct 
{
  GenericEntryCallbackFunction cbf;
  GenericCallbackStruct *cbs;
}   TI_LocalFindCallbackData;

/** */
static void findCallback
( 
  void *callbackData,
  T_DIMSE_C_FindRQ *  ,  //original find request 
  int responseCount,
  //  pending response received 
  T_DIMSE_C_FindRSP *response,
  // pending response identifiers 
  DcmDataset *responseIdentifiers 
)
{
  TI_LocalFindCallbackData *cbd;
  cbd = (TI_LocalFindCallbackData*)callbackData;

  // call the callback function 
  cbd->cbf(cbd->cbs, responseIdentifiers);

  // responseIdentifers will be deleted in DIMSE_findUser() 
}

/** */
OFBool RemoteFindQuery(Config *conf, DBEntry *db, DcmDataset *query,GenericEntryCallbackFunction callbackFunction,GenericCallbackStruct *callbackData)
{
  OFBool ok = OFTrue;
  TI_LocalFindCallbackData cbd;
  OFCondition           cond = EC_Normal;
  T_ASC_PresentationContextID presId;
  DIC_US        msgId;
  T_DIMSE_C_FindRQ  req;
  T_DIMSE_C_FindRSP rsp;
  DcmDataset    *statusDetail = NULL;

  conf->m_CurrentPeerTitle = db->m_Title;

  // make sure we have an association 
  ok = ChangeAssociation(conf);
  if (!ok) 
  {
    return OFFalse;
  }

  cbd.cbf = callbackFunction;
  cbd.cbs = callbackData;

  // which presentation context should be used
  presId = ASC_findAcceptedPresentationContextID(conf->assoc,UID_FINDPatientRootQueryRetrieveInformationModel);
    
  if(presId == 0) 
  {
    std::cerr << "No Presentation Context for Find Operation"<< std::endl;
    return OFFalse;
  }

  msgId =  conf->assoc->nextMsgID++;

  req.MessageID = msgId;
  strcpy(req.AffectedSOPClassUID,UID_FINDPatientRootQueryRetrieveInformationModel);
  req.Priority = DIMSE_PRIORITY_LOW;

  cond = DIMSE_findUser(conf->assoc, presId, &req, query,findCallback, &cbd, DIMSE_BLOCKING, 0, &rsp, &statusDetail);

  if(cond.good()) 
  {
    DIMSE_printCFindRSP(stdout, &rsp);
  } 
  else 
  {
    std::cerr << "Find Failed:"<< std::endl;
    DimseCondition::dump(cond);
  }
    
  if(statusDetail != NULL) 
  {
    std::cout <<"  Status Detail:"<< std::endl;
    statusDetail->print(COUT);
    delete statusDetail;
  }
  return cond.good();
}


/** RemoteNetworkQuery  (_prefs)
 *  Creates an instance of the class.  Nothing is allocated.
 *  A pointer to a preferences class is required to instantiate the parameters member. */
RemoteNetworkQuery::RemoteNetworkQuery(const fldqcPreferences *_prefs)
{ 
  // pointer to AP titles, etc
  prefs        = _prefs;  
}

/** Destructor */
RemoteNetworkQuery::~RemoteNetworkQuery()
{  
  #ifdef HAVE_WINSOCK_H
    WSACleanup();
  #endif
}

/** Patient Level */
void RemoteNetworkQuery::DestroyPatientEntries(DBEntry *db)
{
  int i;
  if(db == NULL) 
  {
    return;
  }
  for (i=0; i<db->m_PatientCount; i++) 
  {
    DestroyStudyEntries(db->patients[i]);
    free(db->patients[i]);
    db->patients[i] = NULL;
  }
  db->m_PatientCount = 0;
}

/** AddPatientEntry */
OFBool AddPatientEntry(DBEntry *db, DcmDataset *reply)
{
  PatientData *se;
  OFBool ok = OFTrue;

  if (db-> m_PatientCount >= MAXPATIENTS) 
  {
    std::cerr <<"addPatientEntry: too many patients"<< std::endl;
    return OFFalse;
  }

  se = (PatientData*) malloc(sizeof(PatientData));
  if (se == NULL) 
  {
    return OFFalse;
  }
    
  // make sure its clean 
  bzero((char*)se, sizeof(PatientData));  

  // extract info from reply 
  ok = ok && (ok = DU_getStringDOElement(reply, DCM_PatientsName,se->m_PatientsName));
  ok = ok && (ok = DU_getStringDOElement(reply, DCM_PatientID,se->m_PatientID));
  if (!ok) 
  {
    std::cerr << "addPatientEntry: missing data in DB reply"<< std::endl;
    return OFFalse;
  }

  DU_stripLeadingAndTrailingSpaces(se->m_PatientsName);
  DU_stripLeadingAndTrailingSpaces(se->m_PatientID);

  // add to array 
  db->patients[db->m_PatientCount] = se;
  db->m_PatientCount++;

  return OFTrue;
}

/** */
OFBool RemoteNetworkQuery::BuildRemotePatients(Config *conf, DBEntry *db, const char* name, const char* id)
{
  if (db->m_IsRemoteDB) 
    {
      GenericCallbackStruct cbs;
      DcmDataset *query = NULL;
      OFBool ok = OFTrue;
                                                          
      cbs.db = db;
      cbs.patient = NULL;
      cbs.study = NULL; cbs.series = NULL;

      DestroyPatientEntries(db);

      // get all known patients 
      if (query != NULL)
      {
        delete query;
      }
      query = new DcmDataset;
      
      if (query == NULL) 
      {
        std::cerr << "Help, out of memory" << std::endl;
        return OFFalse;
      }

      DU_putStringDOElement(query, DCM_QueryRetrieveLevel, "PATIENT");
      DU_putStringDOElement(query, DCM_PatientsName, name);
      DU_putStringDOElement(query, DCM_PatientID,id );

      ok = RemoteFindQuery(conf, db, query, GenericEntryCallback, &cbs);
      delete query;

      return ok;
    }
  else 
  {
    std::cerr << "BuildRemotePatients: cannot query database"<< std::endl;
    return OFFalse;
  }
}


/** Study Level */
void RemoteNetworkQuery::DestroyStudyEntries(PatientData *patient)
{
  int i;
  if (patient == NULL) 
  {
    return;
  }

  for (i=0; i<patient->m_StudyCount; i++) 
  {
    DestroySeriesEntries(patient->studies[i]);
    free(patient->studies[i]);
    patient->studies[i] = NULL;
  }
  patient->m_StudyCount = 0;
}

/** */
OFBool AddStudyEntry(PatientData *patient, DcmDataset *reply)
{
  StudyData *se;
  OFBool ok = OFTrue;

  if (patient->m_StudyCount >= MAXSTUDIES) 
  {
    std::cerr << "addStudyEntry: too many studies"<< std::endl;
    return OFFalse;
  }

  se = (StudyData*) malloc(sizeof(StudyData));
  if (se == NULL) 
  {
    return OFFalse;
  }

  // make sure its clean 
  bzero((char*)se, sizeof(StudyData));  

  // extract info from reply
  ok = DU_getStringDOElement(reply, DCM_StudyInstanceUID,se->m_StudyInstanceUID);
  ok = ok && (ok = DU_getStringDOElement(reply, DCM_StudyID,se->m_StudyID));

    if (!ok)
    {
      std::cerr << "addStudyEntry: missing data in DB reply"<< std::endl;
      return OFFalse;
    }

  DU_stripLeadingAndTrailingSpaces(se->m_StudyInstanceUID);
  DU_stripLeadingAndTrailingSpaces(se->m_StudyID);

  // add to array 
  patient->studies[patient->m_StudyCount] = se;
  patient->m_StudyCount++;

  return OFTrue;
}

OFBool RemoteNetworkQuery::BuildRemoteStudies(Config *conf, DBEntry *db,PatientData  *patient)
{
    if (db->m_IsRemoteDB) 
    {
      GenericCallbackStruct cbs;
      DcmDataset *query = NULL;
      OFBool ok = OFTrue;

      cbs.db = NULL;
      cbs.patient = patient;
      cbs.study = NULL; cbs.series = NULL;

      DestroyStudyEntries(patient);

      // get all known studies 
      if (query != NULL) 
        delete query;
      query = new DcmDataset;
        
      if (query == NULL) 
      {
        std::cerr <<"Help, out of memory"<< std::endl;
        return OFFalse;
      }

      DU_putStringDOElement(query, DCM_QueryRetrieveLevel, "STUDY");
      DU_putStringDOElement(query, DCM_PatientID, patient->m_PatientID);
      DU_putStringDOElement(query, DCM_StudyInstanceUID, NULL);
      DU_putStringDOElement(query, DCM_StudyID, NULL);

      ok = RemoteFindQuery(conf, db, query, GenericEntryCallback, &cbs);
      delete query;

      return ok;
    }
    else 
    {
      std::cerr << "BuildRemoteStudies: cannot query database"<< std::endl;
      return OFFalse;
    }
}

//
// Series Level
//

void RemoteNetworkQuery::DestroySeriesEntries(StudyData *study)
{
  int i;
  if (study == NULL) 
    return;

  for (i=0; i<study->m_SeriesCount; i++) 
  {
    DestroyImageEntries(study->series[i]);
    free(study->series[i]);
    study->series[i] = NULL;
  }    
  
  study->m_SeriesCount = 0;
}

OFBool AddSeriesEntry(StudyData *study, DcmDataset *reply)
{
  SeriesData *series;
  OFBool ok = OFTrue;

  if (study->m_SeriesCount >= MAXSERIES) 
  {
    std::cerr << "addSeriesEntry: too many series"<< std::endl;
    return OFFalse;
  }

  series = (SeriesData*) malloc(sizeof(SeriesData));
    
  if (series == NULL) 
    return OFFalse;
  // make sure its clean 
  bzero((char*)series, sizeof(SeriesData)); 

  // extract info from reply 
  ok = DU_getStringDOElement(reply, DCM_SeriesInstanceUID,series->m_SeriesInstanceUID);
  ok = ok && (ok = DU_getStringDOElement(reply, DCM_SeriesNumber,series->m_SeriesNumber));
  ok = ok && (ok = DU_getStringDOElement(reply, DCM_Modality,series->m_Modality));
  
  if (!ok) 
  {
    std::cerr << "addSeriesEntry: missing data in DB reply"<< std::endl;
    return OFFalse;
  }

  DU_stripLeadingAndTrailingSpaces(series->m_SeriesInstanceUID);
  DU_stripLeadingAndTrailingSpaces(series->m_SeriesNumber);
  DU_stripLeadingAndTrailingSpaces(series->m_Modality);

  series->m_IntSeriesNumber = atoi(series->m_SeriesNumber);

  // add to array 
  study->series[study->m_SeriesCount] = series;
  study->m_SeriesCount++;

  return OFTrue;
}
/*
int SeriesCompare(const void *a, const void *b)
{
  SeriesData **sa, **sb;
  int cmp = 0;

  // compare function for qsort, a and b are pointers to
  // the images array elements.  The routine must return an
  // integer less than, equal to, or greater than 0 according as
  // the first argument is to be considered less than, equal to,
  // or greater than the second.
  // 
  sa = (SeriesData **)a;
  sb = (SeriesData **)b;
  cmp = (*sa)->m_IntSeriesNumber - (*sb)->m_IntSeriesNumber;

  return cmp;
}
*/

OFBool RemoteNetworkQuery::BuildRemoteSeries(Config *conf, DBEntry *db, PatientData *patient,StudyData *study)
{
  if (db->m_IsRemoteDB) 
  {
    GenericCallbackStruct cbs;
    DcmDataset *query = NULL;
    OFBool ok = OFTrue;

    cbs.db = NULL;
    cbs.patient = NULL;
    cbs.study = study; cbs.series = NULL;
    DestroySeriesEntries(study);

    // get all known studies 
    if (query != NULL) 
      delete query;
    query = new DcmDataset;
      
    if (query == NULL) 
    {
      std::cerr <<"Help, out of memory"<< std::endl;
      return OFFalse;
    }

    DU_putStringDOElement(query, DCM_QueryRetrieveLevel, "SERIES");
    DU_putStringDOElement(query, DCM_PatientID, patient->m_PatientID);
    DU_putStringDOElement(query, DCM_StudyInstanceUID,study->m_StudyInstanceUID);
    DU_putStringDOElement(query, DCM_SeriesInstanceUID, NULL);
    DU_putStringDOElement(query, DCM_Modality, NULL);
    DU_putStringDOElement(query, DCM_SeriesNumber, NULL);

    ok = RemoteFindQuery(conf, db, query, GenericEntryCallback, &cbs);
    delete query;

    return ok;
  }
  else 
  {
    std::cerr << "BuildRemoteSeries: cannot query database"<< std::endl;
    return OFFalse;
  }

}


//
// Image Level
//

void RemoteNetworkQuery::DestroyImageEntries(SeriesData *series)
{
  int i;

  if (series == NULL) 
    return;

  for (i=0; i<series->m_ImageCount; i++) 
  {
    free(series->images[i]);
    series->images[i] = NULL;
  }
  
  series->m_ImageCount = 0;
}

OFBool AddImageEntry(SeriesData *series, DcmDataset *reply)
{
  ImageData *image;
  OFBool ok = OFTrue;
  DIC_CS studyID;

  if (series->m_ImageCount >= MAXIMAGES) 
  {
    std::cerr <<"addImageEntry: too many images"<< std::endl;
    return OFFalse;
  }

  image = (ImageData*) malloc(sizeof(ImageData));
  if (image == NULL) 
    return OFFalse;

  bzero((char*)image, sizeof(ImageData)); // make sure its clean 
  bzero((char*)studyID, sizeof(DIC_CS));

  // extract info from reply 
  ok = DU_getStringDOElement(reply, DCM_SOPInstanceUID,image->m_SopInstanceUID);
  ok = ok && (ok = DU_getStringDOElement(reply, DCM_InstanceNumber,image->m_ImageNumber));
    
  if (!ok) 
  {
    std::cerr << "addImageEntry: missing data in DB reply"<< std::endl;
    return OFFalse;
  }

  DU_stripLeadingAndTrailingSpaces(image->m_SopInstanceUID);
  DU_stripLeadingAndTrailingSpaces(image->m_ImageNumber);

  image->m_IntImageNumber = atoi(image->m_ImageNumber);

  // add to array
  series->images[series->m_ImageCount] = image;
  series->m_ImageCount++;

  return OFTrue;
}

/*
int ImageCompare(const void *a, const void *b)
{
  ImageData **ia, **ib;
  int cmp = 0;

  // ompare function for qsort, a and b are pointers to
  // the images array elements.  The routine must return an
  // integer less than, equal to, or greater than 0 according as
  // the first argument is to be considered less than, equal to,
  // or greater than the second.
  //
  ia = (ImageData **)a;
  ib = (ImageData **)b;

  //compare image numbers 
  cmp = (*ia)->m_IntImageNumber - (*ib)->m_IntImageNumber;

  return cmp;
}
*/

OFBool RemoteNetworkQuery::BuildRemoteImages(Config *conf, DBEntry *db,PatientData *patient, StudyData *study,SeriesData  *series)
{
  if (db->m_IsRemoteDB) 
  {
    GenericCallbackStruct cbs;
    DcmDataset *query = NULL;
    OFBool ok = OFTrue;

    cbs.db = NULL;
    cbs.patient = NULL;
    cbs.study = NULL; cbs.series = series;

    DestroyImageEntries(series);

    /* get all known studies */
    if (query != NULL) 
      delete query;
    query = new DcmDataset;
    if (query == NULL)
    {
      std::cerr << "Help, out of memory!"<< std::endl;
      return OFFalse;
    }

    DU_putStringDOElement(query, DCM_QueryRetrieveLevel, "IMAGE");
    DU_putStringDOElement(query, DCM_PatientID, patient->m_PatientID);
    DU_putStringDOElement(query, DCM_StudyInstanceUID,study->m_StudyInstanceUID);
    DU_putStringDOElement(query, DCM_SeriesInstanceUID,series->m_SeriesInstanceUID);
    DU_putStringDOElement(query, DCM_InstanceNumber, NULL);
    DU_putStringDOElement(query, DCM_SOPInstanceUID, NULL);

    ok = RemoteFindQuery(conf, db, query, GenericEntryCallback, &cbs);
    delete query;

    return ok;
  }
  else 
  {
    std::cerr << "BuildRemoteImages: cannot query database"<< std::endl;
    return OFFalse;
  }
}


#if 0
// currently unused 
static OFBool release(Config *conf, int /*arg*/, const char */*cmdbuf*/)
{
  return detatchAssociation(conf, OFFalse);
}

static OFBool abort(Config *conf, int /*arg*/, const char */*cmdbuf*/)
{
  return detatchAssociation(conf, OFTrue);
}
#endif


OFBool RemoteNetworkQuery::ActualizePatients(Config *conf)
{
  DBEntry *db;
  db = conf->dbEntries[conf->m_Currentdb];

  // get a list of all the available studies in the current database 
  if (!BuildRemotePatients(conf, db,NULL,NULL))
    return OFFalse;

  if (db->m_PatientCount == 0) 
  {
    std::cerr <<"No Studies in Database:" << db->m_Title << std::endl;
    return OFFalse;
  }

  if (db->m_CurrentPatient < 0 || db->m_CurrentPatient >= db->m_PatientCount)
    db->m_CurrentPatient = 0;

  return OFTrue;
}


PatientData* RemoteNetworkQuery::Patient(Config *conf, int arg)
{
  DBEntry *db;
  PatientData  *se;
      
  std::cout <<"patient:arg = " << arg << std::endl;
   
  db = conf->dbEntries[conf->m_Currentdb];
  
  if (db->m_IsRemoteDB) 
  {
    conf->m_CurrentPeerTitle = db->m_Title;
    // make sure we have an association 
    OFBool ok =  ChangeAssociation(conf);
    if (!ok) 
      return OFFalse;
  }
  
  if (!ActualizePatients(conf))
    return OFFalse;


  #ifndef RETAIN_ASSOCIATION
  if (conf->dbEntries[conf->m_Currentdb]->m_IsRemoteDB) 
    DetatchAssociation(conf, OFFalse);
  #endif

  if (arg >= 0) 
  {
    // set current study 
    // while (arg >= db->patientCount)
    //     arg = arg -1;

    if (arg >= db->m_PatientCount) 
    {   
      int i = (db->m_PatientCount) - 1 ;
      std::cerr << "ERROR: Study Choice: 0 - " << i << std::endl;
      //   return OFFalse;
    }
    db->m_CurrentPatient = arg;    
  }
  
  se = db->patients[arg];
 
  return se;
}

OFBool RemoteNetworkQuery::Patient(Config *conf, int arg,Fl_Browser* browser, const char* name, const char * id )
{
  DBEntry *db;
  PatientData *se;
  int i;

  std::cout <<"patient: arg=" << arg << std::endl;
  db = conf->dbEntries[conf->m_Currentdb];

  if (db->m_IsRemoteDB) 
  {
    conf->m_CurrentPeerTitle = db->m_Title;
    // make sure we have an association 
    OFBool ok = ChangeAssociation(conf);
    if (!ok) 
      return OFFalse;
  }

  db = conf->dbEntries[conf->m_Currentdb];

  // get a list of all the available studies in the current database 
  if (!BuildRemotePatients(conf, db, name, id))
    return OFFalse;

  if (db->m_PatientCount == 0) 
  {
    std::cerr <<"No Studies in Database: " << db->m_Title << std::endl;
    return OFFalse;
  }

  if (db->m_CurrentPatient < 0 || db->m_CurrentPatient >= db->m_PatientCount)
    db->m_CurrentPatient = 0;

  #ifndef RETAIN_ASSOCIATION
  if (conf->dbEntries[conf->m_Currentdb]->m_IsRemoteDB) 
  {
    DetatchAssociation(conf, OFFalse);
  }
  #endif

  if (arg >= 0) 
  {
    // set current study 
    if (arg >= db->m_PatientCount) 
    {
      int i =  (db->m_PatientCount) - 1 ;
      std::cerr <<"ERROR: Study Choice: 0 - " << i << std::endl;
      return OFFalse;
    }
    db->m_CurrentPatient = arg;
    //  return OFTrue;
  }

  // list studies to user 
  std::cout << "      "<< std::endl;
  printf(PATIENTFORMAT, "Patient", "PatientID");
  for (i=0; i<db->m_PatientCount; i++) 
  {
    if (db->m_CurrentPatient == i) 
    {
      std::cout <<"*"<< std::endl;
    } 
    else 
    {
      std::cout <<" "<< std::endl;
    }
     
    printf(" %2d) ", i);
    se = db->patients[i];
    printf(PATIENTFORMAT, se->m_PatientsName,se->m_PatientID);
    browser->add(se->m_PatientsName);    
  }

  printf("\n");
  std::cout << db->m_PatientCount << " Patients in Database: " << db->m_Title << std::endl;
  
  return OFTrue;
}

OFBool RemoteNetworkQuery::ActualizeStudies(Config *conf)
{
  DBEntry *db;
  PatientData  *patient;

  db = conf->dbEntries[conf->m_Currentdb];
  if (db->m_PatientCount == 0)
    if (!ActualizePatients(conf))
      return OFFalse;

  patient = db->patients[db->m_CurrentPatient];

  // get a list of all the available studies in the current patient 
  if (!BuildRemoteStudies(conf, db,patient))
    return OFFalse;

  if (patient->m_StudyCount == 0) 
  {
    std::cerr <<"No Studies in patient "<< patient->m_PatientID << " Database: "<< db->m_Title << std::endl;
    return OFFalse;
  }
  if (db->m_CurrentStudy < 0 || db->m_CurrentStudy >= patient->m_StudyCount)
    db->m_CurrentStudy = 0;

  return OFTrue;
}

StudyData* RemoteNetworkQuery::Study(Config *conf, int arg  )
{
  DBEntry *db;
  PatientData  *patient;                     
  StudyData *se;   
    
  std::cout << "study: arg="<< arg << std::endl;
  db = conf->dbEntries[conf->m_Currentdb];

  if (db->m_IsRemoteDB) 
  {
    conf->m_CurrentPeerTitle = db->m_Title;
    // make sure we have an association 
    OFBool ok = ChangeAssociation(conf);
    if (!ok) 
      return OFFalse;
  }

  if (!ActualizeStudies(conf))
    return OFFalse;

  #ifndef RETAIN_ASSOCIATION
  if (conf->dbEntries[conf->m_Currentdb]->m_IsRemoteDB)
    DetatchAssociation(conf, OFFalse);
  #endif
  
  patient = db->patients[db->m_CurrentPatient];

  if (arg >= 0) 
  {
    // set current study 
    while (arg >= patient->m_StudyCount)
      arg = arg -1;
    db->m_CurrentStudy = arg;
  }

  se = patient->studies[arg];
  return se;
}


OFBool RemoteNetworkQuery::Study(Config *conf, int arg ,Fl_Browser* browser )
{
  DBEntry *db;
  PatientData *patient;
  StudyData   *se;
  int i;
   
  std::cout <<"study: arg="<< arg << std::endl;
  db = conf->dbEntries[conf->m_Currentdb];

  if (db->m_IsRemoteDB) 
  {
    conf->m_CurrentPeerTitle = db->m_Title;
    // make sure we have an association 
    OFBool ok = ChangeAssociation(conf);
    if (!ok)
      return OFFalse;
  }

  if (!ActualizeStudies(conf))
    return OFFalse;

  #ifndef RETAIN_ASSOCIATION
  if (conf->dbEntries[conf->m_Currentdb]->m_IsRemoteDB)
    DetatchAssociation(conf, OFFalse);
  #endif
  
  patient = db->patients[db->m_CurrentPatient];

  if (arg >= 0) 
  {
    // set current study 
    if (arg >= patient->m_StudyCount) 
    {    
      int i = (patient->m_StudyCount) - 1 ;
      std::cerr << "ERROR: Study Choice: 0 -  " << std::endl;
      //     return OFFalse;
    }
    db->m_CurrentStudy = arg;
    //  return OFTrue;
  }

  // list studies to user 
  std::cout <<"      "<< std::endl;
  printf(STUDYFORMAT, "StudyID","StudyInstanceUID");
    
  for (i=0; i<patient->m_StudyCount; i++) 
  {
    if (db->m_CurrentStudy == i) 
    {
      std::cout <<"*"<< std::endl;
    } 
    else 
    {
      std::cout <<" "<< std::endl;
    }
    printf(" %2d) ", i);
    se = patient->studies[i];
    printf(STUDYFORMAT, se->m_StudyID,se->m_StudyInstanceUID);
    if (arg == -1)
      browser->add(se->m_StudyID);
  }

  printf("\n");
  std::cout << patient->m_StudyCount << " Studies in PatientID " << patient->m_PatientID << std::endl;
  std::cout << "  Patient: "<<  patient->m_PatientsName << " Database: " << db->m_Title << std::endl;
  return OFTrue;
}


OFBool RemoteNetworkQuery::ActualizeSeries(Config *conf)
{
  DBEntry *db;
  PatientData *patient;
  StudyData  *study;

  db = conf->dbEntries[conf->m_Currentdb];
  if (db->m_PatientCount == 0) 
  {
    if (!ActualizePatients(conf))
      return OFFalse;
  }

  patient = db->patients[db->m_CurrentPatient];
  if (patient->m_StudyCount == 0) 
  {
    if (!ActualizeStudies(conf))
      return OFFalse;
  }

  study =  patient->studies[db->m_CurrentStudy];

  // get a list of all the available series in the current study 
  if (!BuildRemoteSeries(conf, db,patient, study))
    return OFFalse;

  if (study->m_SeriesCount == 0) 
  {
    std::cerr << "No Series in Study " << study->m_StudyID <<" with PatientID "<< patient->m_PatientID << " (Database): "<< db->m_Title<< std::endl;
    return OFFalse;
  }
  if (db->m_CurrentSeries < 0 || db->m_CurrentSeries >= study->m_SeriesCount)
    db->m_CurrentSeries = 0;

  return OFTrue;
}


SeriesData* RemoteNetworkQuery::Series(Config *conf, int arg )
{
  DBEntry *db;
  PatientData *patient;
  StudyData *study;
  SeriesData *series;

  std::cout <<"series: arg=" << arg << std::endl;
  db = conf->dbEntries[conf->m_Currentdb];

  if (db->m_IsRemoteDB) 
  {
    conf->m_CurrentPeerTitle = db->m_Title;
    // make sure we have an association 
    OFBool ok = ChangeAssociation(conf);
    if (!ok) 
      return OFFalse;
  }

  if (!ActualizeSeries(conf))
    return OFFalse;

  #ifndef RETAIN_ASSOCIATION
  if (conf->dbEntries[conf->m_Currentdb]->m_IsRemoteDB) 
  {
    DetatchAssociation(conf, OFFalse);
  }
  #endif
    
  patient = db->patients[db->m_CurrentPatient];
  study =  patient->studies[db->m_CurrentStudy];

  if (arg >= 0) 
  {
    // set current series 
    while (arg >= study->m_SeriesCount)
      arg = arg -1;
    db->m_CurrentSeries = arg;
  }
  series = study->series[arg];

  return series;
}


OFBool RemoteNetworkQuery::Series(Config *conf, int arg,Fl_Browser* browser  )
{
  DBEntry *db;
  PatientData *patient;
  StudyData *study;
  SeriesData *series;
  int i;

  std::cout <<"series: arg="<< arg << std::endl;
  db = conf->dbEntries[conf->m_Currentdb];

  if (db->m_IsRemoteDB) 
  {
    conf->m_CurrentPeerTitle = db->m_Title;
    // make sure we have an association 
    OFBool ok = ChangeAssociation(conf);
    if (!ok)
      return OFFalse;
  }

  if (!ActualizeSeries(conf))
    return OFFalse;

  #ifndef RETAIN_ASSOCIATION
  if (conf->dbEntries[conf->m_Currentdb]->m_IsRemoteDB) 
    DetatchAssociation(conf, OFFalse);
  #endif
    
  patient = db->patients[db->m_CurrentPatient];
  study =  patient->studies[db->m_CurrentStudy];

  if (arg >= 0) 
  {
    // set current series 
    if (arg >= study->m_SeriesCount) 
    {
      int i =  (study->m_SeriesCount) - 1 ; 
      std::cerr <<"ERROR: Series Choice: 0 - " << i << std::endl;
      //   return OFFalse;
    }
    db->m_CurrentSeries = arg;
  }

  // list series to user 
  std::cout << "      " << std::endl;
  printf(SERIESFORMAT, "Series", "Modality", "SeriesInstanceUID");
    
  for (i=0; i<study->m_SeriesCount; i++) 
  {
    if (db->m_CurrentSeries == i) 
    {
      std::cout <<"*"<< std::endl;
    } 
    else 
    {
      std::cout <<" "<< std::endl;
    }
    printf(" %2d) ", i);
    series = study->series[i];
    printf(SERIESFORMAT, series->m_SeriesNumber,series->m_Modality, series->m_SeriesInstanceUID);
    if (arg == -1)
      browser->add(series->m_Modality);
  }

  printf("\n");
  std::cout <<study->m_SeriesCount << " Series in StudyID " << study->m_StudyID << std::endl;
  std::cout <<" Patient:" << patient->m_PatientsName << "(Database): " << db->m_Title << std::endl;
  return OFTrue;
}

OFBool RemoteNetworkQuery::ActualizeImages(Config *conf)
{
  DBEntry *db;
  PatientData *patient;
  StudyData *study;
  SeriesData  *series;

  db = conf->dbEntries[conf->m_Currentdb];
  /// check this
  if (db->m_PatientCount == 0) 
  {
    if (!ActualizePatients(conf))
      return OFFalse;
  }

  patient = db->patients[db->m_CurrentPatient];
  if (patient->m_StudyCount == 0) 
  {
    if (!ActualizeStudies(conf))
      return OFFalse;
  }

  study =  patient->studies[db->m_CurrentStudy];
  if (study->m_SeriesCount == 0) 
  {
    if (!ActualizeSeries(conf))
      return OFFalse;
  }

  series = study->series[db->m_CurrentSeries];

  // get a list of all the available images in the current series 
  if (!BuildRemoteImages(conf, db,patient, study, series))
    return OFFalse;

  if (series->m_ImageCount == 0) 
  {
    std::cerr << "No Images in Series "<< series->m_SeriesNumber << ",Study " << study->m_StudyID <<",Patient "<< patient->m_PatientID << "(Database): "<< db->m_Title << std::endl;
    return OFFalse;
  }
  return OFTrue;
}


ImageData* RemoteNetworkQuery::Image(Config *conf, int arg  )
{
  DBEntry *db;
  PatientData *patient;
  StudyData *study;
  SeriesData *series;
  ImageData *image;
 
  std::cout << "image: arg="<< arg << std::endl;
  db = conf->dbEntries[conf->m_Currentdb];

  if (db->m_IsRemoteDB) 
  {
    conf->m_CurrentPeerTitle = db->m_Title;
    // make sure we have an association 
    OFBool ok = ChangeAssociation(conf);
    if (!ok)
      return OFFalse;
  }

  if (!ActualizeImages(conf))
    return OFFalse;

  #ifndef RETAIN_ASSOCIATION
  if (conf->dbEntries[conf->m_Currentdb]->m_IsRemoteDB)
     DetatchAssociation(conf, OFFalse);
  #endif
    
  patient = db->patients[db->m_CurrentPatient];
  study = patient->studies[db->m_CurrentStudy];
  series = study->series[db->m_CurrentSeries];

  if (arg >= 0) \
  {
    // set current image 
    while (arg >= series->m_ImageCount)
      arg = arg -1;
    db->m_CurrentImage = arg;
  }
  image = series->images[arg];

  return image;
}

OFBool RemoteNetworkQuery::Image(Config *conf, int arg,Fl_Browser* browser  )
{
  DBEntry *db;
  PatientData *patient;
  StudyData  *study;
  SeriesData *series;
  ImageData *image;
  int i;

  std::cout <<"image: arg= " << arg << std::endl;
  db = conf->dbEntries[conf->m_Currentdb];

  if (db->m_IsRemoteDB) 
  {
    conf->m_CurrentPeerTitle = db->m_Title;
    // make sure we have an association 
    OFBool ok = ChangeAssociation(conf);
    if (!ok) 
      return OFFalse;
  }

  if (!ActualizeImages(conf))
    return OFFalse;

  #ifndef RETAIN_ASSOCIATION
  if (conf->dbEntries[conf->m_Currentdb]->m_IsRemoteDB) 
    DetatchAssociation(conf, OFFalse);
  #endif
    
  patient = db->patients[db->m_CurrentPatient];
  study = patient->studies[db->m_CurrentStudy];
  series = study->series[db->m_CurrentSeries];

  if (arg >= 0) 
  {
    // set current image 
    if (arg >= series->m_ImageCount) 
    {
      int i = (series->m_ImageCount) - 1 ;
      std::cerr << "ERROR: Image Choice: 0 - " << i << std::endl;
      //  return OFFalse;
    }
    db->m_CurrentImage = arg;
    //  return OFTrue;
  }

  // list images to user 
  std::cout <<"      "<< std::endl;
  printf(IMAGEFORMAT, "Image", "ImageInstanceUID");
  for (i=0; i<series->m_ImageCount; i++) 
  {
    if (db->m_CurrentImage == i)
    {
      std::cout <<"*"<< std::endl;
    } 
    else 
    {
      std::cout <<" "<< std::endl;
    }
     
    printf(" %2d) ", i);
    image = series->images[i];
    printf(IMAGEFORMAT, image->m_ImageNumber, image->m_SopInstanceUID);
    if (arg == -1)
      browser->add(image->m_ImageNumber);
  }

  printf("\n");
  std::cout << series->m_ImageCount << "Images in " <<series->m_ImageCount << " ,Series " << series->m_Modality << ",StudyID " << study->m_StudyID << std::endl;
  std::cout <<" Patient: "<< patient->m_PatientsName << " (Database): " << db->m_Title << std::endl;
  return OFTrue;
}



