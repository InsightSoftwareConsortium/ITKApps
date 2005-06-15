/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DuctExtractorConsole.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __DUCT_EXTRACTOR_CONSOLE__
#define __DUCT_EXTRACTOR_CONSOLE__

#include <fltkImageViewer.h>
#include <DuctExtractorConsoleGUI.h>

class DuctExtractorConsole : public DuctExtractorConsoleGUI {

public:
  
  typedef DuctExtractorConsoleBase::MaskPixelType OverlayPixelType;
  
  typedef fltk::ImageViewer< PixelType,
                       OverlayPixelType > ImageViewerType;

  typedef fltk::ImageViewer< InputPixelType,     
                       OverlayPixelType > InputImageViewerType;

public:
  DuctExtractorConsole();
  virtual ~DuctExtractorConsole();
  virtual void Show(void);
  virtual void Hide(void);
  virtual void Load(void);
  virtual void Quit(void);
  virtual void ShowStatus(const char * text);
  virtual void ShowInput(void);
  virtual void ShowSmoothed(void);
  virtual void ShowLaplacian(void);
  virtual void ShowRegionGrowth(void);
  virtual void Execute(void);
  virtual void WriteSegmentation(void);
  virtual void SelectSeedPoint(float x, float y, float z);
  static void ClickSelectCallback(float x, float y, float z, float value, void * args );

private:

  InputImageViewerType          * m_Viewer_Input;
  ImageViewerType               * m_Viewer_Laplacian;
  ImageViewerType               * m_Viewer_Smoothed;
  InputImageViewerType          * m_Viewer_Region_Growth;

};



#endif
