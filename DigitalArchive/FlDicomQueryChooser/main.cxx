
/*=========================================================================


  Program:   Insight Segmentation & Registration Toolkit
  Module:    main.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/** Creates an instance of FlDicomQueryChooserGUI and starts her up..*/
#include <osconfig.h>
#include <dcdict.h>
#include "FlDicomQueryChooser.h"

int main (int argc, char** argv)
{
  FlDicomQueryChooser gui;
  
  Fl::args(argc, argv);

  // make sure data dictionary is loaded 
  if( !dcmDataDict.isDictionaryLoaded() )
    {
    std::cerr << "Warning: no data dictionary loaded, check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE << std::endl;
    }
  //gui.Show();
 
  FlDicomQueryChooser::ImageType::Pointer myImage = gui.LoadImage();


  Fl::run();


  return  1;
  
}
