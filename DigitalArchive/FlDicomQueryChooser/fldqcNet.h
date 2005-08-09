/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    fldqcNet.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef NET_H
#define NET_H

#include "fldqcDatatypes.h"
#include "fldqcRemoteNetworkQuery.h"

OFBool  ChangeAssociation(Config *conf);
OFBool  AttachAssociation(Config *conf);
OFBool  DetatchAssociation(Config *conf, OFBool abortFlag);

#endif
