/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SystemInterface.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#ifndef __SystemInterface_h_
#define __SystemInterface_h_

#include "Registry.h"
#include <string>

/**
 * \class SystemInterface 
 * \brief An interface between SNAP and the operating system.
 * This class is responsible for finding the system directory, reading and 
 * writing user preferences to disk, etc.
 */
class SystemInterface : public Registry
{
public:
  /** 
   * A method that checks whether the SNAP system directory can be found and 
   * if it can't, prompts the user for the directory.  If the user refuses to 
   * supply the directory, it throws an exception 
   */
  bool FindDataDirectory(const char *pathToExe);

  /** 
   * Get a file relative to the root directory (returns absolute filename), or
   * throws exception if the file does not exist
   */
  std::string GetFileInRootDirectory(const char *fnRelative);

  /** Loads the registry containing user preferences */
  void LoadUserPreferences();

  /** Save the preferences */
  void SaveUserPreferences();

  /** Get the filename for the user preferences */
  const char *GetUserPreferencesFileName() 
  { 
    return m_UserPreferenceFile.c_str(); 
  }

  /** The name of the token file that sits at the root of the program data
   * directory */
  static const char *GetProgramDataDirectoryTokenFileName()
  {
    return "SNAPProgramDataDirectory.txt";
  }

private:
  std::string m_UserPreferenceFile;
  std::string m_DataDirectory;
  std::string m_DocumentationDirectory;
};





#endif //__SystemInterface_h_
