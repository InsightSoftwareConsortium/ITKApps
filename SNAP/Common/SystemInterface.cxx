/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SystemInterface.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "SystemInterface.h"
#include "FL/Fl_Preferences.h"
#include <itksys/SystemTools.hxx>

using namespace std;

void
SystemInterface
::SaveUserPreferences()
{
  WriteToFile(m_UserPreferenceFile.c_str());
}


void
SystemInterface
::LoadUserPreferences()
{
  // Create a preferences object
  Fl_Preferences test(Fl_Preferences::USER,"itk.org","SNAP");
  
  // Use it to get a path for user data
  char userDataPath[1024]; 
  test.getUserdataPath(userDataPath,1024);
  
  // Construct a valid path
  m_UserPreferenceFile = string(userDataPath) + "/" + "UserPreferences.txt";

  // Check if the file exists, may throw an exception here
  if(itksys::SystemTools::FileExists(m_UserPreferenceFile.c_str()))
  {
    ReadFromFile(m_UserPreferenceFile.c_str());
  }
}

/** 
 * A method that checks whether the SNAP system directory can be found and 
 * if it can't, prompts the user for the directory.  The path parameter is the
 * location of the executable, i.e., argv[0] 
 */
bool 
SystemInterface
::FindDataDirectory(const char *pathToExe)
{
  // Get the directory where the SNAP executable was launched
  using namespace itksys;
  typedef std::string StringType;

  // This is the directory we're trying to set
  StringType sRootDir = "";

  // The first possibility is that the user has specified the data path
  // manually.  Get the path from the registry
  StringType sUserPath = (*this)["System.ProgramDataDirectory"][""];
  StringType sSearchName = sUserPath  + "/" + GetProgramDataDirectoryTokenFileName();
  if(sUserPath.length() && 
    SystemTools::FileIsDirectory(sUserPath.c_str()) &&
    SystemTools::FileExists(sSearchName.c_str()))
    {
    // We've found the path
    sRootDir = sUserPath;
    }
  else
    {
    // First of all, find the executable file.  Since the program may have been
    // in the $PATH variable, we don't know for sure where the data is
    // Create a vector of paths that will be searched for 
    // the file SNAPProgramDataDirectory.txt
    vector<StringType> vPathList;

    // Nevertheless, we'll search for the SNAP executable
    StringType sExeFullPath = SystemTools::FindProgram(pathToExe);

    // If the exe could not be found, we can't use it to construct a list of
    // potential search program data search paths
    if(sExeFullPath.length())
      {
      // Look in the path right off the EXE
      StringType sExePath = SystemTools::GetFilenamePath(sExeFullPath);
      if(sExePath.length())
        {
        vPathList.push_back(sExePath + "/" + "ProgramData");
        }

      // Look in the path just above the EXE (windows)
      StringType sExePathUp = SystemTools::GetFilenamePath(sExePath);
      if(sExePathUp.length()) 
        {
        vPathList.push_back(sExePathUp + "/" + "ProgramData");
        }
      }

    // Now that we have the path list, search for the token file
    StringType sFoundFile = 
      SystemTools::FindFile(GetProgramDataDirectoryTokenFileName(),vPathList);
    if(sFoundFile.length())
      sRootDir = SystemTools::GetFilenamePath(sFoundFile);
    }

  // If a directory was not found, prompt the user
  if(!sRootDir.length())
    return false;

  // Store the property, so the next time we don't have to search at all
  (*this)["System.ProgramDataDirectory"] << sRootDir;
  
  // Set the root directory and relative paths
  m_DataDirectory = sRootDir;

  // Append the paths to get the other directories
  m_DocumentationDirectory = m_DataDirectory + "/HTMLHelp";

  // Done, success
  return true;
}

std::string
SystemInterface
::GetFileInRootDirectory(const char *fnRelative)
{
  // Construct the file name
  string path = m_DataDirectory + "/" + fnRelative;

  // Make sure the file exists ?

  // Return the file
  return path;
}


