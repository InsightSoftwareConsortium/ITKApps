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
#include "ImageCoordinateGeometry.h"
#include "ImageIORoutines.h"
#include "IRISApplication.h"
#include "IRISException.h"
#include "IRISImageData.h"
#include "SystemInterface.h"
#include "UserInterfaceLogic.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkNumericTraits.h"


#include <itksys/SystemTools.hxx>

#include <limits>

using namespace std;

// Define the verbose output stream
ostream &verbose = cout;

// MAX grey value - TODO find somewhere to stick this
const GreyType MAXGREYVAL = itk::NumericTraits<GreyType>::max();

#include "Registry.h"

// A templated load image method
template<class TPixel>
bool IRISLoadImage(const char *file, 
                   typename itk::SmartPointer< itk::Image<TPixel,3> > &target)
{
  try
    {
    LoadImageFromFile(file,target);
    return true;
    }
  catch(itk::ExceptionObject &exc)
    {
    cerr << "Error loading file '" << file << "'" << endl;
    cerr << "Reason: " << exc << endl;
    return false;
    }
}

/** Make sure we have a path to the SNAP root directory 
 * so that we can read external files */
bool FindDataDirectoryInteractive(const char *sExePath, SystemInterface &system)
{
  // Try to find the root directory
  while(!system.FindDataDirectory(sExePath))
    {
    // Tell the user the directory is missing
    int choice = 
      fl_choice("The SNAP root directory could not be found!\n"
                "Please click 'Search' to find this directory\n"
                "or click 'Exit' to terminate the program.",
                "Exit Program","Browse...","More Info...");

    if(choice == 0)
      {
      // Exit the application
      return false;
      }      
    else if(choice == 2)
      {
      // More help
      fl_message("  SNAP could not find its help files and other data that it needs\n"
                 "to run properly.  These files should be located in a subdirectory \n"
                 "called ProgramData in the same directory where the SNAP executable \n"
                 "is located.  \n"
                 "  If you are not sure where to look, search your computer's hard drive \n"
                 "for a file called '%s', and then use the Browse... button to tell SNAP \n"
                 "where this file is located.\n"
                 "  For more help, consult your system administrator.",
                 system.GetProgramDataDirectoryTokenFileName());
      }
    else
      {
      // The file we're looking for
      string sMissingFile = system.GetProgramDataDirectoryTokenFileName();
      string sTitle = "Find a directory that contains file " + sMissingFile;

      // Look for the file using a file chooser
      Fl_File_Chooser fc(sMissingFile.c_str(),"Directory Token File (*.txt)",
                         Fl_File_Chooser::SINGLE,sTitle.c_str());
      
      // Show the file chooser
      fc.show();
      while(fc.visible()) Fl::wait();

      // Get the filename from the user
      const char *file = fc.value();
      
      // If user hit cancel, continue
      if(!file) continue;

      // Make sure that the filename matches and get the path
      string sBrowseName = itksys::SystemTools::GetFilenameName(file);
             
      // If not, check if they've selected a valid directory
      if(sBrowseName != sMissingFile)
        {
        // Wrong directory specified!
        fl_alert("The directory must contain file '%s'!",sMissingFile.c_str());
        continue;
        }

      // Make sure that the filename matches and get the path
      string sBrowsePath = itksys::SystemTools::GetFilenamePath(file);

      // Set the path
      system["System.ProgramDataDirectory"] << sBrowsePath;
      }
    }
  return true;
}

bool LoadUserPreferencesInteractive(SystemInterface &system) 
{
  try
    {
    system.LoadUserPreferences();
    }
  catch(std::string &exc)
    {
    if(0 == fl_choice("Error reading preferences file %s\n%s",
                      "Exit Program","Continue",NULL,
                      system.GetUserPreferencesFileName(),exc.c_str()))
      {
      return false;
      }   
    }

  return true;
}


// creates global pointers
// sets up the GUI and lets things run
// -g c:\grant\app\Images\MRIcrop-orig.gipl -s c:\grant\app\Images\MRIcrop-seg.gipl --rai RAI --labels c:\grant\app\Images\MRIcrop-seg.label
int main(int argc, char **argv) 
{
  // Parse command line parameters
  CommandLineArgumentParser parser;
  parser.AddOption("--grey",1);
  parser.AddSynonim("--grey","-g");

  parser.AddOption("--segmentation",1);
  parser.AddSynonim("--segmentation","-s");
  parser.AddSynonim("--segmentation","-seg");

  parser.AddOption("--orientation",1);
  parser.AddSynonim("--orientation","--rai");

  parser.AddOption("--labels",1);
  parser.AddSynonim("--labels","--label");
  parser.AddSynonim("--labels","-l");

  CommandLineArgumentParseResult parseResult;
  if(!parser.TryParseCommandLine(argc,argv,parseResult))
    {
    // Print usage info and exit
    cerr << "ITK-SnAP Command Line Usage:" << endl;
    cerr << "   snap [options]" << endl;
    
    cerr << "Options:" << endl;
    
    cerr << "   --grey, -g FILE              : " <<
      "Load greyscale image FILE" << endl;
    
    cerr << "   --segmentation, -s FILE      : " <<
      "Load segmentation image FILE" << endl;
    
    cerr << "   --labels, -l FILE            : " <<
      "Load label description file FILE" << endl;
    
    cerr << "   --orientation, --rai XYZ     : " << 
      "Use 3 letter orientation code XYZ, def: RAI" << endl;
    return -1;
    }

  // Create a new IRIS application
  IRISApplication *iris = new IRISApplication;

  // Initialize the operating system interface
  SystemInterface system;  

  // Load the user preferences into the system
  if(!LoadUserPreferencesInteractive(system))
    return -1;

  // Work with the user to try to find the root directory
  if(!FindDataDirectoryInteractive(argv[0],system))
    return -1;

  // Create a UI object
  UserInterfaceLogic *ui = new UserInterfaceLogic(iris,&system);

  // Initialize FLTK
  Fl::visual(FL_DOUBLE|FL_INDEX);
  Fl::gl_visual(FL_RGB);  
  Fl::background(236,233,216);
  
  // Show the IRIS Interface
  ui->ShowIRIS();

  // Show the splash screen
  ui->ShowSplashScreen();

  // Check if the user passed in command line arguments
  if(parseResult.IsOptionPresent("--grey"))
    {
    // Get the filename
    const char *fnGrey = parseResult.GetOptionParameter("--grey");

    // Check for RAI
    const char *raiCode = "RAI";
    if(parseResult.IsOptionPresent("--orientation"))
      {
      const char *newRAI = parseResult.GetOptionParameter("--orientation");
      if(ImageCoordinateGeometry::IsRAICodeValid(newRAI))
        raiCode = newRAI;
      else
        cerr << "Invalid orientation code: '" << newRAI 
          << "'. Using default code 'RAI'." << endl;
      }

    // Update the splash screen
    ui->UpdateSplashScreen("Loading grey image...");
    
    // Load the image using itk IO
    IRISApplication::GreyImageType::Pointer img;
    if(IRISLoadImage(fnGrey,img))
      {      
      // Load the image
      iris->UpdateIRISGreyImage(img,raiCode);

      // Save the filename for the UI
      iris->GetGlobalState()->SetGreyFileName(fnGrey);

      // Update the user interface
      ui->OnGreyImageUpdate();
      }
    else
      {
      return -1;
      }

    // Load the segmentation if supplied
    if(parseResult.IsOptionPresent("--segmentation"))
      {
      // Get the filename 
      const char *fname = parseResult.GetOptionParameter("--segmentation");

      // Update the splash screen
      ui->UpdateSplashScreen("Loading segmentation image...");

      // Try to load the image
      IRISApplication::LabelImageType::Pointer img;
      if(IRISLoadImage(fname,img))
        {
        iris->UpdateIRISSegmentationImage(img);
        iris->GetGlobalState()->SetSegmentationFileName(fname);
        ui->OnSegmentationImageUpdate();
        }
      }    
    }

  // Load labels if supplied
  if(parseResult.IsOptionPresent("--labels"))
    {
    // Get the filename 
    const char *fname = parseResult.GetOptionParameter("--labels");
    
    // Update the splash screen
    ui->UpdateSplashScreen("Loading label descriptions...");

    try 
      {
      // Load the labels
      iris->ReadLabelDescriptionsFromTextFile(fname);
    
      // Update the user interfafce
      ui->OnSegmentationLabelsUpdate();
      }
    catch(itk::ExceptionObject &exc)
      {
      cerr << "Error reading label descriptions: " << 
        exc.GetDescription() << endl;
      }
    }

  // Show the welcome message
  ui->UpdateSplashScreen("Welcome to SnAP!");
    
  // Show the splash screen
  ui->HideSplashScreen();

  // Run the FL driver
  Fl::run();

  // Write the user's preferences to disk
  try 
    {
    system.SaveUserPreferences();
    }
  catch(std::string &exc)
    {
    fl_alert("Failed to write preferences to file %s\n%s",
             system.GetUserPreferencesFileName(),exc.c_str());
    }
  
  // Delete the UI object
  delete ui;

  // Terminate the application
  delete iris;
  
  return 0;
}

/*
 *Log: SNAPMain.cxx
 *Revision 1.3  2003/09/15 19:06:58  pauly
 *FIX: Trying to get last changes to compile
 *
 *Revision 1.2  2003/09/13 15:18:01  pauly
 *FIX: Got SNAP to work properly with different image orientations
 *
 *Revision 1.1  2003/09/11 13:51:01  pauly
 *FIX: Enabled loading of images with different orientations
 *ENH: Implemented image save and load operations
 *
 *Revision 1.3  2003/08/27 14:03:23  pauly
 *FIX: Made sure that -Wall option in gcc generates 0 warnings.
 *FIX: Removed 'comment within comment' problem in the cvs log.
 *
 *Revision 1.2  2003/08/27 04:57:47  pauly
 *FIX: A large number of bugs has been fixed for 1.4 release
 *
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
