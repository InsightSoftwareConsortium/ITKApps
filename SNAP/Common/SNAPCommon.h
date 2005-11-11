/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPCommon.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SNAPCommon_h_
#define __SNAPCommon_h_

/** This is an include file that should be inserted into all SNAP files */

// Some standard library things are used so commonly that there is no need
// to have them included every time
#include <assert.h>
#include <iostream>


// Specify a standard stream for verbose messages
extern std::ostream &verbose;

// Specify a standard stream for error messages
extern std::ostream &snaperr;

// From ITK we use SmartPointer so many times that it is necessary
#include <itkSmartPointer.h>

// Include the VectorNx defintions
#include "IRISVectorTypes.h"

/** Definitions for the string streams, for compatibility */
typedef itk::OStringStream IRISOStringStream;
typedef std::istringstream IRISIStringStream;

// Short name (release tag, e.g. $Name:  $)
static const char SNAPVersionShort[] = "$Name:  $";

// Full RCS info
static const char SNAPVersion[] = "@(#) Id";

// Non-cvs version
static const char SNAPSoftVersion[] = "ITK-SNAP 1.4b2 [2005.11.10]";

// Release date of the current version
static const char SNAPCurrentVersionReleaseDate[] = "20051110";

// Release date of the latest version whose user preference files are
// incompatible with the current version and will be erased
static const char SNAPLastIncompatibleReleaseDate[] = "20031109";

/**
 * Bubble structure
 * an object of this class stores information about an individual bubble 
 * initialized in the Snake window
 * author Konstantin Bobkov
 */
struct Bubble {
  // center of the bubble
  Vector3i center;
  
  // radius of the bubble
  int radius;     
};

// Voxel types 
// CAREFUL: do not redefine this to INT without disabling the usage of
// UnaryFunctorCache in the GreyImageWrapper type.  Greyscale instensities
// 0 to MAXGREYVAL are used in a cache table, which would be too big with int
typedef unsigned char LabelType;
typedef short GreyType;
extern const GreyType MAXGREYVAL;
extern const GreyType MINGREYVAL;
#define MAX_COLOR_LABELS 256

/************************************************************************/
/* PY: Some macros because I am tired of typing get/set                 */
/************************************************************************/

/**
 * Set macro borrowed from VTK and modified.  Assumes m_ for private vars
 */
#define irisSetMacro(name,type) \
    virtual void Set##name (type _arg) \
{ \
    this->m_##name = _arg; \
} 

/**
 * Get macro borrowed from VTK and modified.  Assumes m_ for private vars
 */
#define irisGetMacro(name,type) \
    virtual type Get##name () const { \
    return this->m_##name; \
} 

/**
 * Set macro for strings
 */
#define irisSetStringMacro(name) \
    virtual void Set##name (const char *_arg) \
{ \
    this->m_##name = _arg; \
} 

/**
 * Get macro borrowed from VTK and modified.  Assumes m_ for private vars
 */
#define irisGetStringMacro(name) \
    virtual const char *Get##name () const { \
    return this->m_##name.c_str(); \
} 

/**
 * A get macro for boolean variables, IsXXX instead of GetXXX
 */
#define irisIsMacro(name) \
    virtual bool Is##name () const { \
    return this->m_##name; \
} 

/** 
 * A rip off from the ITK not used macro to eliminate 'parameter not used'
 * warnings 
 */
#define irisNotUsed(x)

#endif // __SNAPCommonIO_h_
