/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPCommonUI.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SNAPCommonIO_h_
#define __SNAPCommonIO_h_

/** 
 * This is an include file that should be inserted into all SNAP UI 
 * header files 
 **/

// Include the common items from the logic part of SNAP
#include "SNAPCommon.h"

// Enable cygwin support
#if defined(_WIN32)
#include <itkWindows.h>
#endif

#endif // __SNAPCommonIO_h_
