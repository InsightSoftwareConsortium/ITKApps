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
#include "GUITestPadLogic.h"

// Define the verbose output stream
std::ostream &verbose = std::cout;

int main(void) 
{
  GUITestPadLogic tpad;
  tpad.MakeWindow();
  tpad.m_WinTestPad->show();

  Fl::run();
  return 0;
}
