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

// Include OpenGL headers according to the platform
#ifdef __APPLE__
  #include <glu.h>
  #include <FL/gl.h>
#else
  #include <FL/gl.h>
  #include <GL/glu.h>
#endif

#endif // __SNAPOpenGL_h_
