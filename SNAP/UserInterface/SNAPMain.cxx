/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SNAPMain.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "CommandLineArgumentParser.h"
#include "GreyImageWrapperImplementation.h"
#include "IRISApplication.h"
#include "UserInterfaceLogic.h"

#include <limits>

using namespace std;

// Define the verbose output stream
ostream &verbose = cout;

// MAX grey value - TODO find somewhere to stick this
const GreyType MAXGREYVAL = numeric_limits<GreyType>::max();
   
// creates global pointers
// sets up the GUI and lets things run
int main(int argc, char **argv) 
{
  // Parse command line parameters
  CommandLineArgumentParser parser;
  parser.AddOption("--grey",1);
  parser.AddSynonim("--grey","-g");

  CommandLineArgumentParseResult parseResult;
  if(!parser.TryParseCommandLine(argc,argv,parseResult))
    {
    // Print usage info and exit
    cerr << "ITK-SnAP Command Line Usage:" << endl;
    cerr << "   snap [options]" << endl;
    cerr << "Options:" << endl;
    cerr << "   --grey, -g FILE              : " <<
      "Load greyscale image FILE on startup" << endl;
    return -1;
    }

  Fl::visual(FL_DOUBLE|FL_INDEX);
  Fl::gl_visual(FL_RGB);

  // Create a new IRIS application
  IRISApplication *iris = new IRISApplication;

  // Show the IRIS Interface
  iris->GetUserInterface()->ShowIRIS();

  // Check if the user passed in command line arguments
  if(parseResult.IsOptionPresent("--grey"))
    {
    // Get the filename
    const char *fnGrey = parseResult.GetOptionParameter("--grey",0);

    // Create a grey wrapper and load an image
    GreyImageWrapper *wrapper = new GreyImageWrapperImplementation();
    if(wrapper->LoadFromFile(fnGrey))
      {
      iris->GetUserInterface()->DoLoadImage(wrapper);
      }
    else
      {
      cerr << "Error loading file '" << fnGrey << "'" << endl;
      return -1;
      }
    }

  // Run the FL driver
  Fl::run();
  
  // Terminate the application
  delete iris;
  
  return 0;
}

/*Log: SNAPMain.cxx
 *Revision 1.1  2003/07/12 04:46:50  pauly
 *Initial checkin of the SNAP application into the InsightApplications tree.
 *
 *Revision 1.1  2003/07/11 23:33:57  pauly
 **** empty log message ***
 *
 *Revision 1.8  2003/07/11 21:41:38  pauly
 *Preparation for ITK checkin
 *
 *Revision 1.7  2003/07/01 16:53:59  pauly
 **** empty log message ***
 *
 *Revision 1.6  2003/06/23 23:59:32  pauly
 *Command line argument parsing
 *
 *Revision 1.5  2003/06/14 22:42:06  pauly
 *Several changes.  Started working on implementing the level set function
 *in ITK.
 *
 *Revision 1.4  2003/05/05 12:30:18  pauly
 **** empty log message ***
 *
 *Revision 1.3  2003/04/18 17:32:18  pauly
 **** empty log message ***
 *
 *Revision 1.2  2003/04/16 05:04:17  pauly
 *Incorporated intensity modification into the snap pipeline
 *New IRISApplication
 *Random goodies
 *
 *Revision 1.1  2003/03/07 19:29:47  pauly
 *Initial checkin
 *
 *Revision 1.1.1.1  2002/12/10 01:35:36  pauly
 *Started the project repository
 *
 *
 *Revision 1.8  2002/04/01 22:27:57  moon
 *Took out global snake3D.  It's now part of SnakeVoxDataClass
 *
 *Revision 1.7  2002/03/26 19:20:13  moon
 *Changed full_data back to VoxDataClass, from SnakeVoxDataClass.  roi_data
 *is a SnakeVoxDataClass now.
 *
 *Revision 1.6  2002/03/23 02:16:37  scheuerm
 *Added subclass of VoxData called SnakeVoxData which includes
 *a preprocessed image. Doesn't do much yet but it's a start.
 *
 *Revision 1.5  2002/03/19 19:35:06  moon
 *added snakewrapper to makefile so it gets compiled. started putting in callback,
 *etc. for snake vcr buttons.  added snake object to IrisGlobals, instantiated in Main
 *
 *Revision 1.4  2002/03/08 13:54:47  moon
 *trying to add log tags
 **/
