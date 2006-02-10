/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkcore.inl
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#ifndef ITKCORE_H
#define ITKCORE_H
#include "matitk.h"
#include "ParameterContainer.h"
#include "seedcontainer.h"
#define CONTACT "Vincent Chu <vwchu@sfu.ca>, Ghassan Hamarneh <hamarneh@cs.sfu.ca>"
#define OPCOMMAND "matitk(operationName,[parameters],[inputArray1],[inputArray2],[seed(s)Array],[Image(s)Spacing])\n"
//#define CONTACT "HIDDEN TO PRESERVE ANONYMITY"

#include "itkcore.inl"
#endif

#include "itkcore.h"

#define DEBUG_FILTERING
#define DEBUG_SEGMENTATION
#define DEBUG_REGISTRATION

#ifdef DEBUG_FILTERING
#include "itkfiltercore.h"
#endif
#ifdef DEBUG_SEGMENTATION
#include "itksegmentationcore.h"
#endif
#ifdef DEBUG_REGISTRATION
#include "itkregistrationcore.h"
#endif
#include "MATITKTemplatedVariables.h"


template <class ITKPIXELTYPE>
class ITKCore{
public:
  static void ITKEntry(MATITKTemplatedVariables<ITKPIXELTYPE>& GTV)
  {
    if (pstrzOp[0]=='F' || pstrzOp[0]=='f') {
#ifdef DEBUG_FILTERING
      ITKFilterClass<ITKPIXELTYPE>::ITKFilterClassEntry(GTV);
#endif
    }
    else if (pstrzOp[0]=='S' || pstrzOp[0]=='s') {
#ifdef DEBUG_SEGMENTATION
      ITKSegClass<ITKPIXELTYPE>::ITKSegmentationEntry(GTV);
#endif
    }
    else if (pstrzOp[0]=='R' || pstrzOp[0]=='r') {
#ifdef DEBUG_REGISTRATION
      ITKRegClass<ITKPIXELTYPE>::ITKRegistrationEntry(GTV);
#endif
    }
    else
    {
#ifdef DEBUG_FILTERING
      ITKFilterClass<ITKPIXELTYPE>::ITKFilterClassEntry(GTV);
#endif
#ifdef DEBUG_SEGMENTATION
      ITKSegClass<ITKPIXELTYPE>::ITKSegmentationEntry(GTV);
#endif
#ifdef DEBUG_REGISTRATION
      ITKRegClass<ITKPIXELTYPE>::ITKRegistrationEntry(GTV);
#endif
      aboutMATITK();
      if (pstrzOp[0]!='?')
        mexErrMsgTxt("Unknown Opcode");
    }
    return;
  }
};

