/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IRISTypes.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __IrisTypes_h_
#define __IrisTypes_h_

#include <FL/Fl.H>
#include <cassert>

// Include the VectorNx defintions
#include "IRISVectorTypes.h"

// short name (release tag, e.g. $Name:  $)
static const char IRISver[] = "$Name:  $";
// Full RCS info
static const char IRISversion[] = "@(#) Id";

#define PIPE_UNCOMPRESS 1

#ifdef WIN32
  #define strncasecmp strnicmp
  #undef PIPE_UNCOMPRESS
#endif

#undef PIPE_UNCOMPRESS

#if defined(WIN32) || defined(__i386__)
  #undef WORDS_BIGENDIAN
#else
  #define WORDS_BIGENDIAN 1
#endif

// -----------------------------------------------------------
// Make standard streams available to all
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

// Specify a standard stream for verbose messages
extern std::ostream &verbose;

// Specify a standard stream for error messages
extern std::ostream &snaperr;

/**
 * Bubble structure
 * an object of this class stores information about an individual bubble 
 * initialized in the Snake window
 * author Konstantin Bobkov
 */
struct Bubble {
  Vector3i center; // center of the bubble
  int radius;     // radius of the bubble
};

// Voxel Types
typedef unsigned char LabelType;
typedef unsigned short GreyType;
extern const GreyType MAXGREYVAL;



/**
 TODO: Remove this class!
 */

// Trickier: convert between orientation vectors (1,2,3) (relative to RAI)
// and GIPL orientation definitions (GIPL flag1)
static int GIPLToRAIvec[] = {
  1,2,3, // UNDEFINED           (default for raw files)
  1,2,3, // UNDEFINED_PROJECTION
  1,3,2, // AP_PROJECTION
  0,0,3, // LATERAL_PROJECTION
  0,0,4, // OBLIQUE_PROJECTION
  0,0,5, // none
  0,0,6, // none
  0,0,7, // none
  1,2,3, // UNDEFINED_TOMO
  1,2,-3, // AXIAL
  1,-3,-2, // CORONAL
  2,-3,1, // SAGITTAL
  0,0,12  // OBLIQUE_TOMO
};

class RAIConvert {
public:
  static Vector3i GIPLToRAI(char g) { 
    Vector3i rai;
    for (int i=0; i<3; i++) rai[i] = GIPLToRAIvec[3*g+i];
    return rai;
  }
  static char RAIToGIPL(Vector3i rai) { return RAIToGIPL(rai[0], rai[1], rai[2]);}
  static char RAIToGIPL(int *rai) { return RAIToGIPL(rai[0], rai[1], rai[2]);}

  static char RAIToGIPL(int rai1, int rai2, int rai3) {
    for (int i=0; i<13; i++)
      if (rai1 == GIPLToRAIvec[3*i+0] &&
          rai2 == GIPLToRAIvec[3*i+1] &&
          rai3 == GIPLToRAIvec[3*i+2]) return i;
    return 0;
  }

  static char RAIToChar(int r) {
    if (r<-3 || r>3 || r==0) {
      cerr << "illegal RAI code " << r << endl;
      return 0;
    }
    char rtbl[] = { 'S', 'P', 'L', 0, 'R', 'A', 'I'};
    return rtbl[r+3];
  }

  static int CharToRAI(char c) {
    switch (c) {
    case 'R':
    case 'r': return 1;
    case 'L':
    case 'l': return -1;
    case 'A':
    case 'a': return 2;
    case 'P':
    case 'p': return -2;
    case 'I':
    case 'i': return 3;
    case 'S':
    case 's': return -3;
    default: cerr << "illegal RAI char " << c << endl;
    }
    return 0;
  }
};


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
 * A get macro for boolean variables, IsXXX instead of GetXXX
 */
#define irisIsMacro(name) \
    virtual bool Is##name () const { \
    return this->m_##name; \
} 

// Number of color labels
#define MAX_COLOR_LABELS 256

#endif // __IrisTypes_h_
/*
 *Log: IRISTypes.h
 *Revision 1.2  2003/08/27 04:57:45  pauly
 *FIX: A large number of bugs has been fixed for 1.4 release
 *
 *Revision 1.1  2003/07/12 04:53:17  pauly
 *Initial checkin of SNAP application to the InsightApplications tree
 *
 *Revision 1.10  2003/07/12 01:34:17  pauly
 *More final changes before ITK checkin
 *
 *Revision 1.9  2003/07/11 23:31:04  pauly
 **** empty log message ***
 *
 *Revision 1.7  2003/06/08 16:11:42  pauly
 *User interface changes
 *Automatic mesh updating in SNAP mode
 *
 *Revision 1.6  2003/05/12 02:51:05  pauly
 *Got code to compile on UNIX
 *
 *Revision 1.5  2003/05/05 12:30:18  pauly
 **** empty log message ***
 *
 *Revision 1.4  2003/04/23 20:36:23  pauly
 **** empty log message ***
 *
 *Revision 1.3  2003/04/23 06:05:17  pauly
 **** empty log message ***
 *
 *Revision 1.2  2003/04/18 00:25:37  pauly
 **** empty log message ***
 *
 *Revision 1.1  2003/03/07 19:29:47  pauly
 *Initial checkin
 *
 *Revision 1.1.1.1  2002/12/10 01:35:36  pauly
 *Started the project repository
 *
 *
 *Revision 1.6  2002/04/27 17:47:44  bobkov
 *Commented Bubble class
 *
 *Revision 1.5  2002/04/11 23:04:17  bobkov
 *commented the Bubble class
 *cvs: ----------------------------------------------------------------------
 *
 *Revision 1.4  2002/03/21 15:20:49  moon
 **** empty log message ***
 *
 *Revision 1.3  2002/03/08 14:06:20  moon
 *Added Header and Log tags to all files
 **/
