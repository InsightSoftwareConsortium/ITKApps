/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    macros.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/**
 * macros.h defines macros, constants, and other common parameters in
 * the Liver Tumor Segmentation Library. Currently it only implements
 * the Get and Set macros, which are reduced down versions of similar
 * macros in itkMacro.h. 
 */
#ifndef __igstk_Macros_h__
#define __igstk_Macros_h__

/** Set built-in type.  Creates member Set"name"() (e.g., SetTimeStep(time)); */
#define  SetMacro(name,type) \
  virtual void Set##name (const type _arg) \
  { \
  if (this->m_##name != _arg) \
      { \
      this->m_##name = _arg; \
      } \
  } 

/** Get built-in type.  Creates member Get"name"() (e.g., GetTimeStep(time)); */
#define GetMacro(name,type) \
  virtual type Get##name () \
  { \
     return this->m_##name; \
  }



#endif




