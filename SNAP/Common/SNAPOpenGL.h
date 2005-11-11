/*=========================================================================
                                                                                
  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPOpenGL.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.
                                                                                
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SNAPOpenGL_h_
#define __SNAPOpenGL_h_


#include "IRISVectorTypes.h"

// Include OpenGL headers according to the platform
#ifdef __APPLE__
  #include <glu.h>
  #include <FL/gl.h>
#else
  #include <FL/gl.h>
  #include <GL/glu.h>
#endif

#ifndef _WIN32
#ifndef GLU_VERSION_1_2
#define GLU_VERSION_1_2
#endif
#endif

// Inline functions for use with vector classes
inline void glVertex( const Vector3f &x ) { glVertex3fv(x.data_block()); }
inline void glVertex( const Vector3d &x ) { glVertex3dv(x.data_block()); }
inline void glVertex( const Vector3i &x ) { glVertex3iv(x.data_block()); }
inline void glVertex( const Vector2f &x ) { glVertex2fv(x.data_block()); }
inline void glVertex( const Vector2d &x ) { glVertex2dv(x.data_block()); }
inline void glVertex( const Vector2i &x ) { glVertex2iv(x.data_block()); }

inline void glTranslate( const Vector3f &x ) { glTranslatef(x[0],x[1],x[2]); }
inline void glTranslate( const Vector3d &x ) { glTranslated(x[0],x[1],x[2]); }

inline void glScale( const Vector3f &x ) { glScalef(x[0],x[1],x[2]); }
inline void glScale( const Vector3d &x ) { glScaled(x[0],x[1],x[2]); }


#endif // __SNAPOpenGL_h_
