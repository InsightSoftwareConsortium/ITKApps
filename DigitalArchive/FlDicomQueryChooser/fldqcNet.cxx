/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    fldqcNet.cxx
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
#include "fldqcNet.h"

#include <ofstdinc.h>

BEGIN_EXTERN_C
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
END_EXTERN_C

// abstract syntaxes for storage SOP classes are taken from dcmdata 
static const char *abstractSyntaxes[] = 
{
  UID_VerificationSOPClass,
  UID_FINDPatientRootQueryRetrieveInformationModel 
};

/** */
OFBool DetatchAssociation(Config *conf, OFBool abortFlag)
{
  OFCondition cond = EC_Normal;
  DIC_NODENAME presentationAddress;
  DIC_AE peerTitle;

  if (conf->assoc == NULL) 
  {
    return OFTrue;  /* nothing to do */
  }

  ASC_getPresentationAddresses(conf->assoc->params, NULL,presentationAddress);
  ASC_getAPTitles(conf->assoc->params, NULL, peerTitle, NULL);

  if (abortFlag) 
  {
    // abort association 
    std::cout <<" Aborting Association "<< peerTitle << std::endl;
    cond = ASC_abortAssociation(conf->assoc);
    if (cond.bad()) 
    {
      std::cerr <<"Association Abort Failed:"<< std::endl;
      DimseCondition::dump(cond);
    }
  } 
  else 
  {
    // release association
    //  if (verbose)
    std::cout <<"Releasing Association "<< peerTitle << std::endl;
    cond = ASC_releaseAssociation(conf->assoc);
    if (cond.bad()) 
    {
      std::cerr << "Association Release Failed:"<< std::endl;
      DimseCondition::dump(cond);
    }
  }
  
  ASC_dropAssociation(conf->assoc);
  ASC_destroyAssociation(&conf->assoc);

  if (abortFlag) 
  {
    std::cout <<"Aborted Association (" << presentationAddress << "," << peerTitle << ")" << std::endl;
  } 
  else 
  {
    std::cout <<"Released Association (" << presentationAddress << "," << peerTitle << ")" << std::endl;
  }

  return OFTrue;
}

/** */
static OFCondition AddPresentationContexts(T_ASC_Parameters *params)
{
  OFCondition cond = EC_Normal;
  int i;
  int pid = 1;

  //
  // We prefer to accept Explicitly encoded transfer syntaxes.
  // If we are running on a Little Endian machine we prefer
  // LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
  // Some SCP implementations will just select the first transfer
  // syntax they support (this is not part of the standard) so
  // organise the proposed transfer syntaxes to take advantage
  // of such behaviour.
  //

  E_TransferSyntax networkTransferSyntax = EXS_Unknown;
  unsigned int numTransferSyntaxes = 0;
  const char* transferSyntaxes[] = { NULL, NULL, NULL };

  if (networkTransferSyntax == EXS_LittleEndianImplicit)
  {
    transferSyntaxes[0] = UID_LittleEndianImplicitTransferSyntax;
    numTransferSyntaxes = 1;
  }
  else
  {
    // gLocalByteOrder is defined in dcxfer.h
    if (gLocalByteOrder == EBO_LittleEndian)
    {
      // we are on a little endian machine
      transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
    } 
    else 
    {
      // we are on a big endian machine 
      transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
      transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
      transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
      numTransferSyntaxes = 3;
    }
  }

  // first add presentation contexts for find and verification
  for (i=0; i<(int)DIM_OF(abstractSyntaxes) && cond.good(); i++)
  {
    cond = ASC_addPresentationContext( params, pid, abstractSyntaxes[i], transferSyntaxes, numTransferSyntaxes);
    pid += 2; /* only odd presentation context id's */
  }

  // and then for all storage SOP classes 
  for (i=0; i<numberOfDcmStorageSOPClassUIDs && cond.good(); i++)
  {
    cond = ASC_addPresentationContext( params, pid, dcmStorageSOPClassUIDs[i], transferSyntaxes, numTransferSyntaxes);
    // only odd presentation context id's 
    pid += 2;
  }

  return cond;
}

/** Attach Association */
OFBool AttachAssociation(Config *conf)
{
  OFCondition cond = EC_Normal;
  DIC_NODENAME presentationAddress;
  T_ASC_Parameters *params;
  DIC_NODENAME localHost;
  DIC_AE myAETitle;

  if (conf->assoc != NULL) 
  {
    DetatchAssociation(conf, OFFalse);
  }

  if (conf->dbEntries[conf->m_Currentdb]->m_IsRemoteDB)
  {
    strcpy(myAETitle, conf->m_MyAETitle);
  } 
  else 
  {
    strcpy(myAETitle, conf->dbEntries[conf->m_Currentdb]->m_Title);
  }

  cond = ASC_createAssociationParameters(&params, conf->m_MaxReceivePDULength);
  
  if (cond.bad()) 
  {
    std::cerr << "Help, cannot create association parameters:"<< std::endl;
    DimseCondition::dump(cond);
    return OFFalse;
  }
  
  ASC_setAPTitles(params, myAETitle, conf->m_CurrentPeerTitle, NULL);
 
  gethostname(localHost, sizeof(localHost) - 1);
  sprintf(presentationAddress, "%s:%d", conf->m_PeerHostName,conf->m_Peerport);
  ASC_setPresentationAddresses(params, localHost, presentationAddress);

  cond = AddPresentationContexts(params);
  
  if (cond.bad()) 
  {
    std::cerr << "Help, cannot add presentation contexts:" << std::endl;
    DimseCondition::dump(cond);
    ASC_destroyAssociationParameters(&params);
    return OFFalse;
  }

  // create association 
  std::cout <<"Requesting Association"<< std::endl;
  cond = ASC_requestAssociation(conf->net, params, &conf->assoc);
  if (cond.bad()) 
  {
    if (cond == DUL_ASSOCIATIONREJECTED)
    {
      T_ASC_RejectParameters rej;
      ASC_getRejectParameters(params, &rej);
      std::cerr << "Association Rejected:"<< std::endl;
      ASC_printRejectParameters(stderr, &rej);
      fprintf(stderr, "\n");
      ASC_dropAssociation(conf->assoc);
      ASC_destroyAssociation(&conf->assoc);  
      return OFFalse;
    } 
    else 
    {
      std::cerr <<"Association Request Failed: Peer " << presentationAddress << "," << conf->m_CurrentPeerTitle << std::endl;
      DimseCondition::dump(cond);
      ASC_dropAssociation(conf->assoc);
      ASC_destroyAssociation(&conf->assoc);
      return OFFalse;
    }
  }
 
  if (ASC_countAcceptedPresentationContexts(params) == 0)
  {
    std::cout << "All Presentation Contexts Refused: Peer " << presentationAddress << "," << conf->m_CurrentPeerTitle << std::endl;
    ASC_abortAssociation(conf->assoc);
    ASC_dropAssociation(conf->assoc);
    ASC_destroyAssociation(&conf->assoc);
    return OFFalse;
  }

  std::cout <<"New Association Started (" << presentationAddress << "," << conf->m_CurrentPeerTitle << ")" << std::endl;

  return OFTrue;
}


/** Change Association */
OFBool ChangeAssociation(Config *conf)
{
  DIC_AE actualPeerAETitle;
  OFBool ok = OFTrue;

  if (conf->assoc != NULL) 
  {
    // do we really need to change the association 
    ASC_getAPTitles(conf->assoc->params, NULL, actualPeerAETitle, NULL);
    if (strcmp(actualPeerAETitle, conf->m_CurrentPeerTitle) == 0)
    {
      // no need to change 
     return OFTrue;
    }
  }

  ok = DetatchAssociation(conf, OFFalse);
  if(!ok) 
  {
    return ok;
  }

  ok = AttachAssociation(conf);
  return ok;
}
