/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    Segmenter3D.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <Segmenter3D.h>

int main()
{

  
  SegmenterConsole3D * console = new SegmenterConsole3D();

  console->Show();

  Fl::run();

  delete console;
  
  return 0;
}



