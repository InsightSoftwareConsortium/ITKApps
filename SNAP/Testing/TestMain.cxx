/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    TestMain.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
// Define the verbose output stream
#include "SNAPTestDriver.h"

std::ostream &verbose = std::cout;

int main(int argc, char *argv[]) 
{
  SNAPTestDriver driver;
  driver.Run(argc,argv);
  return 0;
}
