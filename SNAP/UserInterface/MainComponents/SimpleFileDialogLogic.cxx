/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SimpleFileDialogLogic.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "SimpleFileDialogLogic.h"
#include "itkCommand.h"
#include "FL/Fl_File_Chooser.h"

#include <algorithm>

using namespace std;

SimpleFileDialogLogic
::SimpleFileDialogLogic()
{
  m_FileChooserLoad = NULL;
  m_FileChooserSave = NULL;
}

SimpleFileDialogLogic
::~SimpleFileDialogLogic()
{
  if(m_FileChooserLoad)
    delete m_FileChooserLoad;
  if(m_FileChooserSave)
    delete m_FileChooserSave;
}

void
SimpleFileDialogLogic
::MakeWindow()
{
  SimpleFileDialog::MakeWindow();
  m_FileChooserLoad = 
    new Fl_File_Chooser(NULL,NULL,Fl_File_Chooser::SINGLE,"Select a File");
  m_FileChooserSave = 
    new Fl_File_Chooser(NULL,NULL,Fl_File_Chooser::CREATE,"Select a File");
}

void
SimpleFileDialogLogic
::DisplayLoadDialog(Registry *history,const char *file)
{
  m_SaveMode = false;
  this->DisplayDialog(history,file);
}
  
void
SimpleFileDialogLogic
::DisplaySaveDialog(Registry *history,const char *file)
{
  m_SaveMode = true;
  this->DisplayDialog(history,file);
}

void 
SimpleFileDialogLogic
::DisplayDialog(Registry *registry, const char *file)
{
  // Store the registry
  m_History = registry;

  // If the filename was supplied, update it in the UI
  if(file)
    {
    m_InFile->value(file);
    }

  // Get the history list from the registry
  vector<string> array = m_History->GetArray(string(""));
  vector<string>::reverse_iterator it;

  m_InHistory->clear();
  if(array.size() > 0)
    {  
    // Add each item to the history menu (history is traversed
    // backwards)
    for(it=array.rbegin();it!=array.rend();it++)
      {
      // FLTK's add() treats slashes as submenu separators, hence this code
      m_InHistory->replace(m_InHistory->add("dummy"),it->c_str());
      }

    // Activate the history menu    
    m_InHistory->activate();
    }
  else
    {
    // Deactivate history
    m_InHistory->deactivate();
    }

  // Deactivate / activate the OK button
  this->OnFileChange();

  // Show the dialog and wait until it closes
  m_Window->show();
  while(m_Window->shown())
    Fl::wait();  
}

void 
SimpleFileDialogLogic
::OnFileChange()
{
  // Disable the OK button if the file box is empty
  if(!m_InFile->value() || strlen(m_InFile->value()) == 0)
    m_BtnOk->deactivate();
  else
    m_BtnOk->activate();
}

void 
SimpleFileDialogLogic
::OnHistoryChange()
{
  // Put the seleted history into the file box
  m_InFile->value(m_InHistory->mvalue()->text);

  // Act as if the user changed the file
  OnFileChange();
}

void 
SimpleFileDialogLogic
::OnBrowseAction()
{
  // Choose which file chooser to use
  Fl_File_Chooser *fc = (m_SaveMode) ? m_FileChooserSave : m_FileChooserLoad;

  // If there is something in the file box, pass it to the chooser
  if(m_InFile->value() && strlen(m_InFile->value()))
    fc->value(m_InFile->value());

  // Set the pattern
  fc->filter(m_Pattern.c_str());

  // Show the dialog
  fc->show();  
  while(fc->shown()) Fl::wait();

  // Once the dialog is done, get the value
  if(fc->value() && strlen(fc->value()))
    {
    m_InFile->value(fc->value());
    m_BtnOk->activate();
    }  
}

void 
SimpleFileDialogLogic
::OnOkAction()
{
  try 
    {
    // Fire the appropriate event
    if(m_SaveMode)
      m_SaveCallback->Execute((itk::Object *) 0,itk::NoEvent());
    else 
      m_LoadCallback->Execute((itk::Object *) 0,itk::NoEvent());
  }
  catch(...)
  {
    return;
  }

  // Merge the file with the history
  string file = m_InFile->value();
  vector<string> array = m_History->GetArray(string(""));
  vector<string>::iterator it;

  // First, search the history for the instance of the file and delete
  // existing occurences
  while((it = find(array.begin(),array.end(),file)) != array.end())
    array.erase(it);

  // Append the file to the end of the array
  array.push_back(file);

  // Trim the array to appropriate size
  if(array.size() > 20)
    array.erase(array.begin(),array.begin() + array.size() - 20);

  // Store the new array to the registry
  m_History->PutArray(array);      

  // Hide the window
  m_Window->hide();
}

void 
SimpleFileDialogLogic
::OnCancelAction()
{
  // Hide the window
  m_Window->hide();
}
