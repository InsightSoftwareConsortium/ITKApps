/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageIOWizardLogic.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ImageIOWizardLogic_txx_
#define __ImageIOWizardLogic_txx_

#include "FL/fl_file_chooser.h"
#include <limits>
#include <cmath>
#include <map>

using std::numeric_limits;
using std::map;

template <class TPixel>
void ImageIOWizardLogic<TPixel>
::GoBack(Fl_Group *current) 
{
  // The link to the last page
  Fl_Group *last = NULL;

  // In the input is NULL, get the current page from the wizard widget
  current = (current == NULL) ? (Fl_Group *)m_WizInput->value() : current;

  if (current == m_PageHeader)
    last = m_PageFile;
  else if (current == m_PageOrientation)
    last = m_PageHeader;
  else if (current == m_PageSummary)
    last = m_PageOrientation;
  else
    assert(0 == "Next page not valid at this position in the wizard");

  if (!last->active())
    {
    GoBack(last);
    } 
  else
    {
    m_WizInput->value(last);
    }
}

template <class TPixel>
void ImageIOWizardLogic<TPixel>
::GoForward(Fl_Group *current) 
{
  // The link to the next page
  Fl_Group *next = NULL;

  // In the input is NULL, get the current page from the wizard widget
  current = (current == NULL) ? (Fl_Group *)m_WizInput->value() : current;

  // Follow the sequence of the pages
  if (current == m_PageFile)
    next = m_PageHeader;
  else if (current == m_PageHeader)
    next = m_PageOrientation;
  else if (current == m_PageOrientation)
    next = m_PageSummary;
  else
    assert(0 == "Next page not valid at this position in the wizard");

  // Check if the page is active 
  if (!next->active())
    {
    GoForward(next);
    } else
    {
    m_WizInput->value(next);
    }           
}

template <class TPixel>
const char *ImageIOWizardLogic<TPixel>
::GetFilePatterns() 
{
  static const char *pattern = 
      "All Image Files (*.{gipl,mha,mhd,img,raw*})\t"
      "GIPL Files (*.{gipl,gipl.gz})\t"
      "MetaImage Files (*.{mha,mhd})\t"
      "Analyze Files (*.{img,img.gz})\t"
      "RAW Files (*.{raw*})";
  return pattern;
}

template <class TPixel>
void ImageIOWizardLogic<TPixel>
::OnFilePageBrowse() 
{
  // Get the path and pattern for reading in the file 
  const char *pattern = GetFilePatterns();
  const char *path = m_InFilePageBrowser->value();
  path = strlen(path) ? path : NULL;

  // Bring up th choice dialog
  const char *fname = fl_file_chooser("Please select an image file",pattern,path);
  if (fname)
    {
    m_InFilePageBrowser->value(fname);
    OnFilePageFileInputChange();
    }
}

template <class TPixel>
void ImageIOWizardLogic<TPixel>
::OnFilePageFileInputChange() 
{
  // Check the length of the input
  const char *text = m_InFilePageBrowser->value();
  if (text != NULL && strlen(text) > 0)
    {
    m_BtnFilePageNext->activate();

    // Check the file extension and update the RAW checkbox
    // if the extension contains .raw
    m_ChkFilePageRAW->activate();
    
    if (strstr(text,".raw") || strstr(text,".RAW") || strstr(text,".Raw"))
      {
      m_ChkFilePageRAW->value(1);
      } 
    else
      {
      m_ChkFilePageRAW->value(0);
      }
    OnFilePageRAWCheck();
    } 
  else
    {
    m_BtnFilePageNext->deactivate();
    m_ChkFilePageRAW->value(0);
    m_ChkFilePageRAW->deactivate();
    OnFilePageRAWCheck();
    }          
}

template <class TPixel>
void ImageIOWizardLogic<TPixel>::OnFilePageRAWCheck()
{
  // When the check box changes value, the status of 
  // the header page is tied to it
  if (m_ChkFilePageRAW->value())
    m_PageHeader->activate();
  else
    m_PageHeader->deactivate();
}

template <class TPixel>
void ImageIOWizardLogic<TPixel>
::OnFilePageNext() 
{
  // Check if a file has been specified
  const char *fname = m_InFilePageBrowser->value();
  assert(fname && strlen(fname) > 0);

  // See if the user wants to treat the file as RAW
  if (m_ChkFilePageRAW->value() > 0)
    {
    GoForward();
    }

  else
    {

    // Try to read the file without generating an error
    fl_cursor(FL_CURSOR_WAIT,FL_FOREGROUND_COLOR, FL_BACKGROUND_COLOR);
    bool success= m_Image->LoadFromFile(fname);
    fl_cursor(FL_CURSOR_DEFAULT,FL_FOREGROUND_COLOR, FL_BACKGROUND_COLOR);

    if (success)
      {
      // Update the UI to the image
      DoImageLoadCommon();

      // Go forward
      GoForward();
      } 
    else
      {
      fl_alert("Error. Image could not be read!");
      }
    }
}

template <class TPixel>
void 
ImageIOWizardLogic<TPixel>
::OnCancel() 
{
  // Clear the data stored in the image
  m_Image->Reset();
  m_Image = NULL;

  // Set the status to negative: nothing was loaded
  m_ImageLoaded = false;

  // Hide this window
  m_WinInput->hide();
}

template <class TPixel>
void 
ImageIOWizardLogic<TPixel>
::OnHeaderPageNext() 
{
  // Make sure we're not here by mistake
  assert(m_ChkFilePageRAW->value() == 1);

  // Get the byte type
  typedef typename ImageWrapper<TPixel>::RAWImagePixelType RAWPixelType;  
  RAWPixelType ptype = 
    static_cast<RAWPixelType>(ImageWrapper<TPixel>::IW_UBYTE + 
                              (int)m_InHeaderPageVoxelType->value());

  // Create the vector for the image size
  Vector3ui size((unsigned int) m_InHeaderPageDimX->value(),
                 (unsigned int) m_InHeaderPageDimY->value(),
                 (unsigned int) m_InHeaderPageDimZ->value());

  // Create the vector for the image spacing
  Vector3d spacing(m_InHeaderPageSpacingX->value(),
                   m_InHeaderPageSpacingY->value(),
                   m_InHeaderPageSpacingZ->value());

  // Load the image with supplied information
  fl_cursor(FL_CURSOR_WAIT,FL_FOREGROUND_COLOR, FL_BACKGROUND_COLOR);
  
  bool success = 
    m_Image->LoadFromRAWFile(m_InFilePageBrowser->value(),
                             size,spacing,
                             (unsigned int) m_InHeaderPageHeaderSize->value(),
                             ptype,
                             m_InHeaderPageByteAlign->value() == 0);

  fl_cursor(FL_CURSOR_DEFAULT,FL_FOREGROUND_COLOR, FL_BACKGROUND_COLOR);

  if(success) 
    {
    // Update the UI to the image
    DoImageLoadCommon();

    // Go forward
    GoForward();
    }
  else
    {
    fl_alert("Error. Image could not be read!");
    }
}

template <class TPixel>
void 
ImageIOWizardLogic<TPixel>
::DoImageLoadCommon() 
{  
  // Better have an image that's initialized
  assert(m_Image->IsInitialized());
}

template <class TPixel>
void 
ImageIOWizardLogic<TPixel>
::OnHeaderPageInputChange() 
{
  // Header input has changed
  if (m_InHeaderPageDimX->value() >= 1 &&
      m_InHeaderPageDimY->value() >= 1 &&
      m_InHeaderPageDimZ->value() >= 1 &&
      m_InHeaderPageSpacingX->value() > 0 &&
      m_InHeaderPageSpacingY->value() > 0 &&
      m_InHeaderPageSpacingZ->value() > 0)
    {
    m_BtnHeaderPageNext->activate();
    }
  else
    {
    m_BtnHeaderPageNext->deactivate();
    }
}

template <class TPixel>
void 
ImageIOWizardLogic<TPixel>
::OnHeaderPageBack() 
{
  GoBack();
}

template <class TPixel>
void 
ImageIOWizardLogic<TPixel>
::UpdateSummaryPage()
{
  const char *boTypes[] = 
    {"Big Endian", "Little Endian","Order Not Applicable"};

  // Get the IO base object
  itk::ImageIOBase *ioBase = m_Image->GetImageIO();

  // The object better not be NULL!
  if(ioBase)
    {
    // Print file name
    m_OutSummaryFileName->value(ioBase->GetFileName());

    // Print file dimensions
    m_OutSummaryDimensionX->value(ioBase->GetDimensions(0));
    m_OutSummaryDimensionY->value(ioBase->GetDimensions(1));
    m_OutSummaryDimensionZ->value(ioBase->GetDimensions(2));

    // Print file size in bytes
    m_OutSummarySize->value((int)(ioBase->GetImageSizeInBytes() / (1024.0)));
    
    // Print pixel spacing 
    m_OutSummarySpacingX->value(ioBase->GetSpacing(0));
    m_OutSummarySpacingY->value(ioBase->GetSpacing(1));
    m_OutSummarySpacingZ->value(ioBase->GetSpacing(2));

    // Dump out the file type info
    
    // TODO: This is a workaround on an itk bug with RawImageIO
    if(ioBase->GetComponentType() != itk::ImageIOBase::UNKNOWN)
      {
      // There actually is a type in the IO object
      m_OutSummaryPixelType->value(
        ioBase->ReturnTypeAsString(ioBase->GetComponentType()).c_str());
      }
    else
      {
      m_OutSummaryPixelType->value(m_InHeaderPageVoxelType->text());
      }
    
    m_OutSummaryByteOrder->value(
       boTypes[(unsigned int)(ioBase->GetByteOrder() - ioBase->BigEndian)]);

    // Create a string stream for the dictionary
    std::stringstream sout;    

    // Dump out the image data to the browser
    m_Image->GetImage()->GetMetaDataDictionary().Print(sout);

    sout.flush();
    // m_OutSummaryMetaData->buffer()->text(sout.str().c_str());
    }
  else 
    {
    m_OutSummaryFileName->value("Error loading image.");
    m_OutSummaryDimensionX->value(0);
    m_OutSummaryDimensionY->value(0);
    m_OutSummaryDimensionZ->value(0);
    m_OutSummarySize->value(0);
    m_OutSummaryDimensionX->value(0);
    m_OutSummaryDimensionY->value(0);
    m_OutSummaryDimensionZ->value(0);
    m_OutSummaryPixelType->value("n/a");
    m_OutSummaryByteOrder->value("n/a");
    // m_OutSummaryMetaData->value("n/a");
    }
}

template <class TPixel>
void 
ImageIOWizardLogic<TPixel>
::OnOrientationPageNext() 
{
  
  // Fill out the summary information
  UpdateSummaryPage();
  
  // Go on to the summary page
  GoForward();
}

template <class TPixel>
void 
ImageIOWizardLogic<TPixel>
::OnOrientationPageBack() 
{
  GoBack();
}

template <class TPixel>
void 
ImageIOWizardLogic<TPixel>
::OnOrientationPageRAIChange()
{
  // The user has manually edited the RAI code

  // Check that the code is valid

  // If it is, update the currently selected RAI and update the display

  // If not, give feedback to the user about RAI codes
}

template <class TPixel>
void ImageIOWizardLogic<TPixel>::SetRAI(const char *rai) {
  // Create a static map from RAI characters to menu items
  static map<char,int> raiMap;
  if (raiMap.empty()) {
    raiMap['R'] = 0; raiMap['r'] = 0;
    raiMap['L'] = 1; raiMap['l'] = 1;
    raiMap['A'] = 2; raiMap['a'] = 2;
    raiMap['P'] = 3; raiMap['p'] = 3;
    raiMap['I'] = 4; raiMap['i'] = 4;
    raiMap['S'] = 5; raiMap['s'] = 5;
  }

  // Make sure we have a valid RAI
  assert(rai);
  for (int i=0;i<3;i++)
    assert(raiMap.find(rai[i])!=raiMap.end());

  // Apply the RAI
  m_InOrientationPageX->value(raiMap[rai[0]]);
  m_InOrientationPageY->value(raiMap[rai[1]]);
  m_InOrientationPageZ->value(raiMap[rai[2]]);
};

template <class TPixel>
void ImageIOWizardLogic<TPixel>::GetRAI(char rai[3]) {
  // Return the currently set RAI
  static char *raiList = "RLAPIS";

  // Place the rai characters into the array
  rai[0] = raiList[m_InOrientationPageX->value()];
  rai[1] = raiList[m_InOrientationPageY->value()];
  rai[2] = raiList[m_InOrientationPageZ->value()];
}

template <class TPixel>
bool ImageIOWizardLogic<TPixel>::IsRAIValid(const char *rai) {
  assert(rai);

  // Check the validity of the RAI code - no repetition
  // is allowed
  bool rl = false,ap = false,is = false;
  for (int i=0;i<3;i++) {
    switch (rai[i]) {
    case 'r' :
    case 'R' :
    case 'l' :
    case 'L' :
      rl = true;break;
    case 'a' :
    case 'A' :
    case 'p' :
    case 'P' :
      ap = true;break;
    case 'i' :
    case 'I' :
    case 's' :
    case 'S' :
      is = true;break;
    default:
      assert(0 == "Bad RAI");
    };
  };

  // All three should have been encountered!
  return rl && ap && is;
}

template <class TPixel>
void ImageIOWizardLogic<TPixel>::OnOrientationPageSelectPreset() {

  // Whether or not custom is allowed
  int useCustom = 0;

  switch (m_InOrientationPagePreset->value()) {
  case 0 : 
    SetRAI("RAI");break;
  case 1 :
    SetRAI("RAS");break;
  case 2 : 
    SetRAI("ASR");break;
  case 3:
    SetRAI("RSP");break;
  case 4:
    useCustom = 1;
    break;
  default:
    assert(0 == "Unknown Preset");
  }

  // Enable or disable the pane
  if (useCustom)
    m_GrpOrientationPageCustom->activate();
  else
    m_GrpOrientationPageCustom->deactivate();
}

template <class TPixel>
void ImageIOWizardLogic<TPixel>::OnOrientationPageSelect() {
  // Get the RAI code for current configuration
  char rai[3];
  GetRAI(rai);

  // Check if valid
  if (IsRAIValid(rai))
    m_BtnOrientationPageNext->activate();
  else
    m_BtnOrientationPageNext->deactivate();
}

template <class TPixel>
void ImageIOWizardLogic<TPixel>::OnSummaryPageFinish() {

  // Apply the RAI stuff
  ApplyRAI();

  // Perform a final validity / sanity check
  if (CheckFinalValidity()) {
    // Set the status to positive, the image has been loaded!
    m_ImageLoaded = true;

    // Hide this window
    m_WinInput->hide();
  }
}

template <class TPixel>
ImageIOWizardLogic<TPixel>::ImageIOWizardLogic() {
}

template <class TPixel>
ImageIOWizardLogic<TPixel>::~ImageIOWizardLogic() {
}

template <class TPixel>
bool 
ImageIOWizardLogic<TPixel>
::DisplayInputWizard(WrapperType *target) 
{
  m_Image = target;
  m_ImageLoaded = false;

  // Point the wizard to the first page
  m_WizInput->value(m_PageFile);

  // Clear the file name box
  // TODO: Implement a history list
  m_InFilePageBrowser->value("");
  OnFilePageFileInputChange();

  // Show the input window
  m_WinInput->show();

  // Loop until the window has been closed
  while (m_WinInput->visible())
    Fl::wait();

  // Destroy the internal image if it has been loaded
  if(m_ImageLoaded)
  {
    // Release the IO related resources of the image
    m_Image->ReleaseImageIO();
  
    // The image pointer is set to NULL
    m_Image = NULL;
  }

  // Whether or not the load has been succesfull
  return m_ImageLoaded;
}

template <class TPixel>
void 
ImageIOWizardLogic<TPixel>
::OnSummaryPageBack() 
{
  GoBack();
}

template <class TPixel>
void ImageIOWizardLogic<TPixel>
::ApplyRAI() 
{
  // Get the RAI code
  char rai[3];
  GetRAI(rai);

  // Create the image input transform
  // ImageCoordinateTransform ict(rai);

  // Update the wrapper with it
  // m_Image->GetSlicer()->SetImageToDisplayTransform(
}

#endif // __ImageIOWizardLogic_txx_
