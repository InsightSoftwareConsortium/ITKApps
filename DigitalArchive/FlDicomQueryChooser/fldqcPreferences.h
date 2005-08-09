/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    fldqcPreferences.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

   This software is distributed WITHOUT ANY WARRANTY; without even 
   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
   PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _fldqcPreferences_h
#define _fldqcPreferences_h

#include "fldqcDatatypes.h"
#include <stdlib.h>
#include <string.h>
#include <fldqcPreferencesGUI.h>

#define S_LEN 255

class fldqcPreferences : public fldqcPreferencesGUI
{
//  friend class FlDicomQueryChooser;
//  friend class FlDicomQueryChooserGUI;
//  friend class fldqcPreferencesGUI; 

public:              
  /** Constructor */
  fldqcPreferences();
  /** create and load from file */

  fldqcPreferences(char*);       
  
  /** the copy constructor */
  fldqcPreferences(fldqcPreferences*); 
  
  /** the destructor */
  ~fldqcPreferences();          

  /** set to defaults */
  void SetDefaults();     
  
  /** read prefs from file */
  int ReadFromFile(char*);  
  
  /** write back to file */
  int WriteToFile(char*);  

  /** Show */
  void Show();

  /** Quit */
  void Quit(bool save);

  /** Save the settings */
  void SaveSettings();

  /** Init the settings */
  void InitSettings();

  /** Set the autosave method */
  void SetAutoSave(int val) 
    {
    if(val==0)
      {
      m_AutoSave = false;
      }
    else
      {
      m_AutoSave = true;
      }
    }

  bool GetAutoSave() {return m_AutoSave;}

  char* GetServer() {return m_server;}
  char* GetCalledAP() {return m_calledAP;} 
  char* GetCallingAP() {return m_callingAP;} 
  char* GetServerPort() {return m_serverPort;} 
  char* GetClientPort() {return m_clientPort;} 

protected:

  /** server's hostname */
  char m_server[S_LEN]; 
  /** server's AP Title */
  char m_calledAP[S_LEN];  
  /** our AP Title */
  char m_callingAP[S_LEN]; 
  /** server's port */
  char m_serverPort[S_LEN];    
  /** client's port */
  char m_clientPort[S_LEN];    

  /** save prefs on exit */
  bool m_AutoSave;
};

#endif
