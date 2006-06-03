/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageCalculatorUtils.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if ! defined(__ImageCalculator_h__)
#define __ImageCalculator_h__
#include <iostream>

//This function prints the valid pixel types.
extern void PrintDataTypeStrings(void);

//This function compares strings.
extern int CompareNoCase( const std::string &s, const std::string& s2 );



#endif // __ImageCalculator_h__
