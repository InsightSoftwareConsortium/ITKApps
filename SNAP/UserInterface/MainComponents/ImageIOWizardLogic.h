/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageIOWizardLogic.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ImageIOWizardLogic_h_
#define __ImageIOWizardLogic_h_

#include <cassert>

#include "ImageWrapper.h"
#include "ImageIOWizard.h"
#include "FL/fl_draw.h"

/**
 * \class ImageIOWizardLogic
 * The implementation of the Image IO Wizard UI class.  This class defines the
 * callbacks in the user interface for the class.  It is templated over the type
 * of the input that is to be procured.
 */
template <class TPixel>
class ImageIOWizardLogic : public ImageIOWizard 
{
public:
  typedef ImageWrapper<TPixel> WrapperType;

  /** A constructor */
  ImageIOWizardLogic();

  /** A descructor */
  virtual ~ImageIOWizardLogic();

  // Callback methods overridden from the Base class
  void OnCancel();
  void OnFilePageBrowse();
  void OnFilePageNext();
  void OnFilePageRAWCheck();
  void OnFilePageFileInputChange();
  void OnHeaderPageNext();
  void OnHeaderPageBack();
  void OnHeaderPageInputChange();        
  void OnOrientationPageNext();
  void OnOrientationPageBack();
  void OnOrientationPageSelectPreset();
  void OnOrientationPageSelect();
  void OnOrientationPageRAIChange();
  void OnSummaryPageFinish();
  void OnSummaryPageBack();

  /**
   * Get the image that has been loaded by this object
   */
  const ImageWrapper<TPixel> &GetLoadedImage() {
    assert(IsImageLoaded());
    return m_Image;
  }

  /**
   * Has the image been loaded successfully?
   */
  bool IsImageLoaded() {
    return m_ImageLoaded;
  }

  /**
   * A method to initialize the pages in the wizard.  Child classes should override
   * this and enable/disable irrelevant pages and fields.  The method takes as a 
   * parameter an ImageWrapper, to which it makes changes.
   *
   * The method returns true if an image was loaded with success, false in case of cancellation
   */
  virtual bool DisplayInputWizard(WrapperType *target);    

  /**
   * Get the filename that was loaded
   */
  const char *GetFileName() {
    return m_InFilePageBrowser->value();
  }

protected:

  /**
   * An instance of an image of type TPixel
   */
  ImageWrapper<TPixel> *m_Image;

  /**
   * Has the image been loaded?
   */
  bool m_ImageLoaded;

  /**
   * This method should return a string containing the patterns of files
   * that this dialog is allowed to read
   */
  virtual const char *GetFilePatterns();

  /**
   * A method that controls how we propagate forward and back in the dialog.
   * Fires an assertion if called from where you can't go back or forward.  Inactive
   * pages are simply skipped.
   */
  void GoForward(Fl_Group *current = NULL);
  void GoBack(Fl_Group *current = NULL);

  // This is called after an image gets loaded
  void DoImageLoadCommon();

  virtual void ApplyRAI();

  // Check if the image is valid
  virtual bool CheckFinalValidity() {
    return true;
  }

  // Apply a new RAI structure
  void SetRAI(const char *);

  // Apply a new RAI structure
  void GetRAI(char *);

  // Apply a new RAI structure
  bool IsRAIValid(const char *);

  /**
   * Display the information about the image, and allow expert users to 
   * update that information
   */
  void UpdateSummaryPage();

};

/*
class SNAPGreyImageIOWizard : public ImageIOWizardLogic<GreyType> {
    public:

    protected:
        void Initialize();
};

class SNAPSegmentationImageIOWizard : public ImageIOWizardLogic<LabelType> {
    public:

    protected:
        void Initialize();
};
*/

// Include the TXX file
// TODO: Synch with ITK txx mechanism
#include "ImageIOWizardLogic.txx"

#endif // __ImageIOWizardLogic_h_
