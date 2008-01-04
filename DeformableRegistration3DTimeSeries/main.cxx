/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    main.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "itkImage.h"

#include "DeformableRegistration3DTimeSeries.h"
#include <FL/Fl.H>
//#include <windows.h>

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//    LPSTR lpCmdLine, int nCmdShow)
int main(int argc, char** argv )
{
  fltk::DeformableRegistration3DTimeSeries ImageViewer;           
 
  try
    {
    ImageViewer.Show();
    Fl::run();
    }
  catch( std::exception & ex )
    {
    std::cerr << ex.what() << std::endl;
    }
  return 0;
}
