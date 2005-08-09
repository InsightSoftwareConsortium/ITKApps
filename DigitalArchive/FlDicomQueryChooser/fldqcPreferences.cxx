/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    fldqcPreferences.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "fldqcPreferences.h"

/** Constructor */
fldqcPreferences::fldqcPreferences() 
{
  this->SetDefaults();
}

/** Constructor */
fldqcPreferences::fldqcPreferences (char *_file)
{
  if (!ReadFromFile(_file))
  {
    this->SetDefaults();
  }
}

/** Copy constructor */
fldqcPreferences::fldqcPreferences(fldqcPreferences* _prefs)
{
  strcpy(m_server,     _prefs->m_server);
  strcpy(m_calledAP,   _prefs->m_calledAP);
  strcpy(m_callingAP,  _prefs->m_callingAP);
  strcpy(m_serverPort, _prefs->m_serverPort);
  strcpy(m_clientPort, _prefs->m_clientPort); 
  m_AutoSave    = _prefs->m_AutoSave;
}

/** Destructor */
fldqcPreferences::~fldqcPreferences() 
{
}

/** Sets to default values */
void fldqcPreferences::SetDefaults()
{
  m_server[0]     = '\0';
  m_calledAP[0]   = '\0';
  m_callingAP[0]  = '\0';
  m_serverPort[0] = '\0';
  m_clientPort[0] = '\0'; 
  m_AutoSave = 0;
}

/** Reads data in from a file, returns 1 if everything went ok */
int fldqcPreferences::ReadFromFile(char *_filename)
{
  std::ifstream file;
  char buf[S_LEN];

  file.open(_filename, std::ios::in);
  if(!file) 
    {
    std::cout << "fldqcPreferences::read : Error reading fldqcPreferences from " 
            << _filename << std::endl;
    return 0;
    }
  
  file.getline(buf,       S_LEN);     // the first line is bullshit

  // network settings
  file.getline(m_server,     S_LEN);    // server's nodename
  file.getline(m_serverPort, S_LEN);    // server's port
  file.getline(m_calledAP,   S_LEN);    // server's AP title
  file.getline(m_callingAP,  S_LEN);    // our AP title
  file.getline(m_clientPort, S_LEN); 

  //    file.getline(storageAP,  S_LEN);    // storage server's AP title
  // non-string values, options, etc
  file.getline(buf, S_LEN);

  if(atoi(buf))
    {
    m_AutoSave  = true;
    }
  else
    {
    m_AutoSave  = false;
    }
  file.close();
  return 1;
}

/** Writes prefs back to disk, returns 1 if everything went ok */
int fldqcPreferences::WriteToFile(char *_filename)
{
  std::ofstream file(_filename, std::ios::out);

  if (!file) 
  {
    std::cout << "fldqcPreferences: Error opening " << _filename
              << ", to write fldqcPreferences to disk. " << std::endl;
    return 0;
  }

  file << "FLDicomQueryChooser Prefs Configuration File - do not edit!" << std::endl;
  file << m_server                   << std::endl;
  file << m_serverPort               << std::endl;
  file << m_calledAP                 << std::endl;
  file << m_callingAP                << std::endl;
  file << m_clientPort               << std::endl;
  file << m_AutoSave                 << std::endl;

  file.close();
  return 1;
}
  

/** Show */
void fldqcPreferences::Show()
{
  //create our working copy
  //current = new fldqcPreferences(prefs);

  // set up the data for editing
  this->InitSettings();

  // show the window
  prefsWindow->show();
}

/** Quit */
void fldqcPreferences::Quit(bool save)
{
//save indicates if current should replace prefs
/*if (current != NULL)
{

  if (_save == 1) 
  {
     memcpy(prefs,current, sizeof(fldqcPreferences));
     delete current;
     current = NULL;
  }
  else 
  { 
     delete current;
     current = NULL; 
  }

} */     

  // close the gui
  prefsWindow->hide();

}

/** Save the settings */
void fldqcPreferences::SaveSettings()
{
// save fields to current

// items in the Network group
/*strcpy(current->m_server, serverInput->value());
strcpy(current->m_calledAP, calledAPInput->value());
strcpy(current->m_callingAP, callingAPInput->value());
strcpy(current->m_clientPort, clientPortInput->value());
strcpy(current->m_serverPort, portInput->value());
*/
}

/** Init the settings */
void fldqcPreferences::InitSettings()
{
  // initialize current settings
  /*if (current) 
  {
   // items in the Network group
   serverInput->value(current->m_server);
   calledAPInput->value(current->m_calledAP);
   callingAPInput->value(current->m_callingAP);
   clientPortInput->value(current->m_clientPort);
   portInput->value(current->m_serverPort);
  }*/
}
