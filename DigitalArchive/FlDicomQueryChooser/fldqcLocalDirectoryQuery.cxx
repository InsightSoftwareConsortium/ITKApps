/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    fldqcLocalDirectoryQuery.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "fldqcLocalDirectoryQuery.h"
#include <ofstdinc.h>

/** */
LocalDirectoryQuery::LocalDirectoryQuery() 
{
}

// Compares two strings
int LocalDirectoryQuery::StringMatching  (const char *pmod, const char *pstr)
{
  int uni;
  if (*pmod == '\0')
    return (*pstr == '\0');

  if (  *pmod == *pstr || (*pmod == '?' && *pstr != '\0'))
    return (StringMatching (pmod + 1, pstr + 1));

  if (*pmod == '*') 
  {
    if ( *(pmod + 1) == '\0' )
      return (OFTrue);
    while (  ( (uni = StringMatching (pmod + 1, pstr)) == OFFalse ) && (*pstr != '\0'))
      pstr++;
    return (uni);
  }
  else if (*pmod != *pstr)
    return (OFFalse);
  
  return OFFalse;
}

int LocalDirectoryQuery::MakeDirectoryQuery(const char *_dir, OFList<OFString> *fileNames,Fl_Browser* patientbrowser, OFList<OFString> *patientIDlist )
{
  // file list returned by filename_list
  dirent     **files; 
  // number of files in *files
  int          numFiles; 
  // contains corrected directory string
  char         dir[F_LEN];  
  // contains full path
  char         fullname[F_LEN];   
  char         tmp[F_LEN];
  OFCondition  status; 
  fileNames->clear();

  // bad code follows -- check for . and .. and null filenames
  if (strlen(_dir) == 0)
    return 1;
  // end of bad code

  // Convert relative filename to absolute and clean it up a bit
  fl_filename_expand(tmp, _dir);
  fl_filename_absolute(dir, tmp);

  // Trash the trailing slash
  const int len = strlen(dir);
  if (dir[len - 1] == '/' || dir[len - 1] == '\\')
    dir[len - 1] = '\0';

  // FLTK bug -- under windows, filename_list requires the trailing backslash
  sprintf(tmp, "%s%c", dir, SLASH);

  // Get a list of files in the directory
  numFiles = fl_filename_list(tmp, &files);

  for (int i = 0; i < numFiles; i++) 
  {
    // First we're going to trash the . and .. entries
    if (files[i]->d_name[0] == '.')
      continue;
    // Create a full path name
    sprintf(fullname, "%s%s", tmp, files[i]->d_name);
    fileNames->push_back(fullname);
  }

  OFListIterator(OFString) iter = fileNames->begin();
  OFListIterator(OFString) last = fileNames->end();
  DcmFileFormat *fileformat = NULL;
  OFList<OFString> Pid;
     
  // iterate over all input filenames 
  while ((iter != last) )
  { 
    if (fileformat != NULL) delete fileformat;
      fileformat = new DcmFileFormat;
    if (fileformat == NULL) 
    {
      std::cerr <<"Help, out of memory"<< std::endl;
      return 0;
    }        
    // add datasets
    // add only files. No Directories !!
    if (!( fl_filename_isdir ((*iter).c_str() ))) 
      status = fileformat->loadFile((*iter).c_str());
            
    std::cout << " Filename is " <<  (*iter).c_str() << std::endl;
    if (status.good())
    {
      const char * patientid = NULL ;                                                                         
      if (fileformat->getDataset()->findAndGetString(DCM_PatientID , patientid).good()) 
        Pid.push_back(patientid);                       
    }
    ++iter;
  } 

  if ( (Pid.size()) == 0 ) 
  {
    std::cout << "***************************************************" << std::endl  ; 
    std::cout << "Selected Directory does not contain any Dicom Files" << std::endl  ; 
    // Finally, delete our file list
    delete [] files;
    return 0;
  }
  else
  {
    patientIDlist->clear();
    iter = Pid.begin();
    OFString tmp1 ;
    
    while ( (Pid.size()) > 0)
    {
      tmp1 = (*iter);
      patientIDlist->push_back(tmp1);
      Pid.remove(tmp1);
      iter = Pid.begin();
      //  tmp1 = NULL;
    }
  }
    
  OFListIterator(OFString) itertmp = patientIDlist->begin();
  OFListIterator(OFString) lasttmp = patientIDlist->end();
 
  while ((itertmp != lasttmp) )
  {
    std::cout << " Patient ID is " <<  (*itertmp) << std::endl;
    OFListIterator(OFString) iterfile = fileNames->begin();
    OFListIterator(OFString) lastfile = fileNames->end();

    while ((iterfile != lastfile) )
    {
      if (fileformat != NULL) delete fileformat;
        fileformat = new DcmFileFormat;
      if (fileformat == NULL) 
      {
        std::cerr <<"Help, out of memory"<< std::endl;
        return 0;
      }   
     
      // add datasets 
      status = fileformat->loadFile((*iterfile).c_str());
          
      if (status.good())
      {
        const char * patientid = NULL ;
        const char * patientname = NULL ;
                                    
        if (fileformat->getDataset()->findAndGetString(DCM_PatientID , patientid).good()) 
        { 
          int equal = StringMatching  (patientid,((*itertmp).c_str()) );
          if (equal == 1 )
          if (fileformat->getDataset()->findAndGetString(DCM_PatientsName , patientname).good()) 
          {  
            if ( patientname == NULL ) 
              patientbrowser->add("Anonymized");
            else  
              patientbrowser->add( patientname);
            break;
          }                    
        }                
      } 
      ++iterfile;
    }
    ++itertmp;
  }

  //tmppid->clear();
  Pid.clear();  
    
  // Finally, delete our file list
  delete [] files;
 
  return 1;
}

// Display tags to infoBrowser window depending upon the Query Level
void getInfoFromDataset(DcmDataset *dset, Fl_Browser* info, int qLevel)
{   
  DIC_LO  patientID;
  DIC_PN  patientsName;
  DIC_UI  studyInstanceUID;
  DIC_CS  studyId;
  //DIC_US  rows;
  //DIC_US  columns;
  DIC_DS  PixelSpacing;
  DIC_UI  seriesInstanceUID;
  DIC_IS seriesNumber; 
  DIC_CS modality; 
  DIC_UI  sopInstanceUID; 
  DIC_IS imageNumber;

  char outStr1[100];
  DU_getStringDOElement(dset, DCM_PatientID ,patientID);
  sprintf(outStr1, "PatientsID: %s",patientID);
  char outStr2[100]; 
  DU_getStringDOElement(dset, DCM_PatientsName, patientsName);
  sprintf(outStr2, "PatientsName: %s",patientsName );
  char outStr3[100]; 
  DU_getStringDOElement(dset, DCM_StudyInstanceUID ,studyInstanceUID  );
  sprintf(outStr3, "StudyInstanceUID : %s",studyInstanceUID );
  char outStr4[100];
  DU_getStringDOElement(dset, DCM_StudyID ,studyId);
  sprintf(outStr4, "StudyID : %s", studyId);
  char outStr5[100];
  DU_getStringDOElement(dset, DCM_PixelSpacing ,PixelSpacing);
  sprintf(outStr5, "PixelSpacing : %s",PixelSpacing );
  // DU_getStringDOElement(dset, DCM_Rows ,rows);
  // sprintf(outStr, "Rows : %s", rows);
  // info->add(outStr);
  // DU_getStringDOElement(dset, DCM_Columns ,columns);
  // sprintf(outStr, "Columns : %s", columns);
  // info->add(outStr);
  char outStr6[100];
  DU_getStringDOElement(dset, DCM_SeriesInstanceUID  ,seriesInstanceUID);
  sprintf(outStr6, "SeriesInstanceUID : %s", seriesInstanceUID);
  char outStr7[100];
  DU_getStringDOElement(dset, DCM_SeriesNumber ,seriesNumber);
  sprintf(outStr7, "SeriesNumber : %s", seriesNumber);
  char outStr8[100];
  DU_getStringDOElement(dset, DCM_Modality ,modality);
  sprintf(outStr8, "Modality : %s", modality);
  char outStr9[100];
  DU_getStringDOElement(dset, DCM_SOPInstanceUID  ,sopInstanceUID );
  sprintf(outStr9, "SOPInstanceUID: %s", sopInstanceUID );
  char outStr10[100];
  DU_getStringDOElement(dset, DCM_InstanceNumber  , imageNumber);
  sprintf(outStr10, "ImageNumber: %s", imageNumber);

  switch (qLevel)
  {
    case 0:
      info->add(outStr1);  
      info->add(outStr2); 
      break;
    case 1:
      info->add(outStr1);  
      info->add(outStr2);     
      info->add(outStr3);
      info->add(outStr4);
      break;
    case 2:
      info->add(outStr1);  
      info->add(outStr2);     
      info->add(outStr3);
      info->add(outStr4);
      info->add(outStr5);
      info->add(outStr6);
      info->add(outStr7);
      info->add(outStr8);
      break;
    case 3:
      info->add(outStr1);  
      info->add(outStr2);     
      info->add(outStr3);
      info->add(outStr4);
      info->add(outStr5);
      info->add(outStr6);
      info->add(outStr7);
      info->add(outStr8);
      info->add(outStr9);
      info->add(outStr10);
      break;
    case 4:
      break;
  } 
}

//
//   Given a PatientData,this function will find all corresponding studies filenames.
//
int LocalDirectoryQuery::PatientFileQuery (const char * key, Fl_Browser *studybrowser, OFList<OFString> *fileNames,OFList<OFString> *patientfileNames,OFList<OFString> *studyUIDlist, Fl_Browser *info , int queryLevel )
{
  OFListIterator(OFString) iter = fileNames->begin();
  OFListIterator(OFString) last = fileNames->end();
  DcmFileFormat *fileformat = NULL;
  OFList<OFString> tmpsUID;
  OFBool ispatient = OFTrue;
  tmpsUID.clear();
  patientfileNames->clear();
  studyUIDlist->clear();
  OFCondition status;

  // iterate over all input filenames 
  while ((iter != last) )
    { 
    if (fileformat != NULL) 
      {
      delete fileformat;
      }
    fileformat = new DcmFileFormat;
    if (fileformat == NULL) 
      {
      std::cerr <<"Help, out of memory"<< std::endl;
      return 0;
      }   
     
    // add datasets 
    status = fileformat->loadFile((*iter).c_str());
       
    if (status.good())
      {
      const char * patientid = NULL ;
      const char * patientname = NULL ;
      const char * studyInstanceUID ;
                 
      if (fileformat->getDataset()->findAndGetString(DCM_PatientID , patientid).good()) 
        { 
        int equal = StringMatching (patientid,key) ;
        if (equal == 1 ) 
          {     
          if (ispatient)
            {
            DcmDataset *obj = fileformat->getDataset();
            getInfoFromDataset(obj, info, queryLevel);
            ispatient = OFFalse;
            }
          if (fileformat->getDataset()->findAndGetString(DCM_StudyInstanceUID ,studyInstanceUID ).good()) 
            { 
            tmpsUID.push_back(studyInstanceUID);
            patientfileNames->push_back(*iter);
            }                    
          }
        }  
      }
    ++iter;
    }

  iter = tmpsUID.begin();
  OFString tmp1 ;
    
  while ( (tmpsUID.size()) > 0)
    {
    tmp1 = (*iter);
    studyUIDlist->push_back(tmp1);
    tmpsUID.remove(tmp1);
    iter = tmpsUID.begin();
    }
  
  OFListIterator(OFString) itertmp = studyUIDlist->begin();
  OFListIterator(OFString) lasttmp = studyUIDlist->end();
 
  while ((itertmp != lasttmp) )
    {
    std::cout << " Study UID is " <<  (*itertmp) << std::endl;
    OFListIterator(OFString) iterfile = fileNames->begin();
    OFListIterator(OFString) lastfile = fileNames->end();

    while ((iterfile != lastfile) )
      {
      if (fileformat != NULL) 
        {
        delete fileformat;
        }
      fileformat = new DcmFileFormat;
      if (fileformat == NULL) 
        {
        std::cout <<"Help, out of memory"<< std::endl;
        return 0;
        }   
      
      // add datasets 
      status = fileformat->loadFile((*iterfile).c_str());
          
      if (status.good())
        {
        const char * studyInstanceUID =NULL ;
        const char * studyID = NULL ;
                 
        if (fileformat->getDataset()->findAndGetString(DCM_StudyInstanceUID, studyInstanceUID).good()) 
          {
          int equal = StringMatching (studyInstanceUID,((*itertmp).c_str()) );
          if (equal == 1 )
            if (fileformat->getDataset()->findAndGetString(DCM_StudyID , studyID).good()) 
            {
              if ( studyID == NULL ) 
                studybrowser->add("Anonymized");
              else   
                studybrowser->add(studyID );
              break;
            }                   
          }  
        }             
        ++iterfile;
      }                        
    ++itertmp;  
  }

  return 1;
}

//
//   Given a StudyData,this function will find all corresponding series filenames.
//

int LocalDirectoryQuery::StudyFileQuery (const char * key, Fl_Browser *seriesbrowser, OFList<OFString> *patientfileNames,OFList<OFString> *studyfileNames,OFList<OFString> *seriesUIDlist, Fl_Browser *info , int queryLevel )
{    
  OFListIterator(OFString) iter = patientfileNames->begin();
  OFListIterator(OFString) last = patientfileNames->end();
  OFBool isstudy = OFTrue;  
  DcmFileFormat *fileformat = NULL;
  OFList<OFString> tmpsUID;
     
  tmpsUID.clear();
  studyfileNames->clear();
  seriesUIDlist->clear();
  OFCondition status;
  // iterate over all input filenames 
  while ((iter != last) )
  { 
    if (fileformat != NULL) delete fileformat;
      fileformat = new DcmFileFormat;
    if (fileformat == NULL) 
    {
      std::cerr <<"Help, out of memory"<< std::endl;
      return 0;
    }   
     
    // add datasets 
    status = fileformat->loadFile((*iter).c_str());
    if (status.good())
    {
      const char * seriesInstanceUID = NULL ;
      const char * studyInstanceUID ;
      const char * studyID = NULL ;  
      if (fileformat->getDataset()->findAndGetString(DCM_StudyInstanceUID ,studyInstanceUID).good()) 
      { 
        int equal = StringMatching (studyInstanceUID,key) ;
        if (equal == 1 ) 
        {
          if (isstudy)
          {    
            DcmDataset *obj = fileformat->getDataset();
            getInfoFromDataset(obj, info, queryLevel);
            isstudy = OFFalse;
          }

          if (fileformat->getDataset()->findAndGetString(DCM_SeriesInstanceUID ,seriesInstanceUID ).good()) 
          { 
            tmpsUID.push_back(seriesInstanceUID);
            studyfileNames->push_back(*iter);
          }                    
        }
      }            
    }
    ++iter;
  }

  // tmppid->clear();
  iter = tmpsUID.begin();
  OFString tmp1 ;
    
  while ( (tmpsUID.size()) > 0)
  {
    tmp1 = (*iter);
    seriesUIDlist->push_back(tmp1);
    tmpsUID.remove(tmp1);
    iter = tmpsUID.begin();
  }
  
  OFListIterator(OFString) itertmp = seriesUIDlist->begin();
  OFListIterator(OFString) lasttmp = seriesUIDlist->end();
 
  while ((itertmp != lasttmp) )
  {
    std::cout << " Series UID is " <<  (*itertmp) << std::endl;           
    OFListIterator(OFString) iterfile = patientfileNames->begin();
    OFListIterator(OFString) lastfile = patientfileNames->end();

    while ((iterfile != lastfile) )
    {
      if (fileformat != NULL) delete fileformat;
        fileformat = new DcmFileFormat;
      if (fileformat == NULL) 
      {
        std::cout <<"Help, out of memory"<< std::endl;
        return 0;
      }        
      // add datasets 
      status = fileformat->loadFile((*iterfile).c_str());
          
      if (status.good())
      {
        const char * seriesInstanceUID=NULL ;
        const char *  modality= NULL ;
                  
        if (fileformat->getDataset()->findAndGetString(DCM_SeriesInstanceUID, seriesInstanceUID).good()) 
        { 
          int equal =  StringMatching (seriesInstanceUID,((*itertmp).c_str()) );
          if (equal == 1 )
            if (fileformat->getDataset()->findAndGetString(DCM_Modality,modality).good()) 
            { 
              if ( modality == NULL ) 
                seriesbrowser->add("Anonymized");
              else    
                seriesbrowser->add(modality );
              break;
            }                   
        }                 
      }                  
      ++iterfile;
    }          
    ++itertmp;
  } 
    
  return 1;
}

//
//   Given a SeriesData,this function will find all corresponding image filenames.
//
int LocalDirectoryQuery::SeriesFileQuery (const char * key, Fl_Browser *imagebrowser, OFList<OFString> *studyfileNames,OFList<OFString> *seriesfileNames,OFList<OFString> *imageUIDlist, Fl_Browser *info , int queryLevel )
{
  OFListIterator(OFString) iter = studyfileNames->begin();
  OFListIterator(OFString) last = studyfileNames->end();
  DcmFileFormat *fileformat = NULL;
  OFList<OFString> tmpsUID;
  OFBool isseries = OFTrue;
  tmpsUID.clear();
  seriesfileNames->clear();
  imageUIDlist->clear();
  OFCondition status;
  
  // iterate over all input filenames    
  while ((iter != last) )
    { 
    if (fileformat != NULL) 
      {
      delete fileformat;
      }
    fileformat = new DcmFileFormat;
    if (fileformat == NULL) 
      {
      std::cerr <<"Help, out of memory"<< std::endl;
      return 0;
      }   
     
    // add datasets 
    status = fileformat->loadFile((*iter).c_str());
      
    if (status.good())
      {
      const char * seriesInstanceUID = NULL ;
      const char * sopInstanceUID ;
      const char * modality= NULL ;
                 
      if (fileformat->getDataset()->findAndGetString(DCM_SeriesInstanceUID , seriesInstanceUID ).good()) 
        { 
        int equal = StringMatching (seriesInstanceUID ,key) ;
        if (equal == 1 ) 
          {
          if (isseries)
            {
            DcmDataset *obj = fileformat->getDataset();
            getInfoFromDataset(obj, info, queryLevel);
            isseries = OFFalse;
            }

          if (fileformat->getDataset()->findAndGetString(DCM_SOPInstanceUID ,sopInstanceUID ).good()) 
            { 
            tmpsUID.push_back(sopInstanceUID);
            seriesfileNames->push_back(*iter);
           }                   
         }
       }                 
      }
    ++iter;
    }

  // tmppid->clear();
  iter = tmpsUID.begin();
  OFString tmp1 ;
    
  while ( (tmpsUID.size()) > 0)
    {
    tmp1 = (*iter);
    imageUIDlist->push_back(tmp1);
    tmpsUID.remove(tmp1);
    iter = tmpsUID.begin();
    }
  
  OFListIterator(OFString) itertmp = imageUIDlist->begin();
  OFListIterator(OFString) lasttmp = imageUIDlist->end();
 
  while ((itertmp != lasttmp) )
    {
    std::cout << " Study UID is " <<  (*itertmp) << std::endl;
    OFListIterator(OFString) iterfile = studyfileNames->begin();
    OFListIterator(OFString) lastfile = studyfileNames->end();

    while ((iterfile != lastfile) )
      {
      if (fileformat != NULL)
        {
        delete fileformat;
        }
      fileformat = new DcmFileFormat;
      if (fileformat == NULL) 
        {
        std::cerr <<"Help, out of memory"<< std::endl;
        return 0;     
        }         
      
      // add datasets 
      status = fileformat->loadFile((*iterfile).c_str());
      if (status.good())
        {
        const char * sopInstanceUID =NULL ;
        const char * imageNumber= NULL ;
        if (fileformat->getDataset()->findAndGetString(DCM_SOPInstanceUID, sopInstanceUID).good()) 
          { 
          int equal = StringMatching (sopInstanceUID,((*itertmp).c_str()) );
          if (equal == 1 )
            {
            if (fileformat->getDataset()->findAndGetString(DCM_InstanceNumber , imageNumber).good()) 
              { 
              if ( imageNumber == NULL ) 
                {
                imagebrowser->add("Anonymized");
                }
              else
                {
                imagebrowser->add(imageNumber );
                }
              break;
              }
            }
          }  
        }
      ++iterfile;
      }
    ++itertmp;
    }
  return 1;
}

//
//   Given a SeriesData,this function will find all corresponding image filenames.
//

int LocalDirectoryQuery::ImageFileQuery (const char * pid, OFList<OFString> *seriesfileNames,OFList<OFString> *imagefileNames,  Fl_Browser *info , int queryLevel)
{
  OFListIterator(OFString) iter = seriesfileNames->begin();
  OFListIterator(OFString) last = seriesfileNames->end();
  OFBool isimage = OFTrue;
  DcmFileFormat *fileformat = NULL;
  OFList<OFString> tmpsUID;
  tmpsUID.clear();
  imagefileNames->clear();
 
  // iterate over all input filenames 
  while ((iter != last) )
  { 
    if (fileformat != NULL) delete fileformat;
      fileformat = new DcmFileFormat;
    if (fileformat == NULL) 
    {
       std::cerr <<"Help, out of memory"<< std::endl;
       return 0;
    }   
     
    // add datasets 
    OFCondition status = fileformat->loadFile((*iter).c_str());
    if (status.good())
    {
       const char * sopInstanceUID = NULL ;
       const char * imageNumber= NULL ;
                 
       if (fileformat->getDataset()->findAndGetString(DCM_SOPInstanceUID, sopInstanceUID).good()) 
       { 
         int equal = StringMatching (sopInstanceUID,pid) ;
         if (equal == 1 )
         { 
           if (isimage)
           {  
             DcmDataset *obj = fileformat->getDataset();
             getInfoFromDataset(obj, info, queryLevel);
             isimage = OFFalse;
           }
                  
           //break;
           imagefileNames->push_back(*iter);
         }        
       }                 
    }
    ++iter;
  }

  return 1;
}
