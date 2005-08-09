/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    fldqcLocalDirectoryQuery.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef QUERY_H
#define QUERY_H

#include "fldqcDatatypes.h"
#include "fldqcNet.h"

class LocalDirectoryQuery 
{
public:
  /** Constructor. */
  LocalDirectoryQuery ();
  
  int StringMatching(const char *, const char *);
  
  int MakeDirectoryQuery(const char*, OFList<OFString> *, Fl_Browser*, OFList<OFString> *); // Query a directory
  int PatientFileQuery  (const char *, Fl_Browser *, OFList<OFString> *, OFList<OFString> *,  OFList<OFString> * , Fl_Browser *, int );
  int StudyFileQuery    (const char *, Fl_Browser *, OFList<OFString> *, OFList<OFString> *,  OFList<OFString> * , Fl_Browser *, int );
  int SeriesFileQuery   (const char *, Fl_Browser*, OFList<OFString> *, OFList<OFString> *,  OFList<OFString> * , Fl_Browser * , int );
  int ImageFileQuery    (const char *, OFList<OFString> *, OFList<OFString> * , Fl_Browser *, int );
};

#endif







