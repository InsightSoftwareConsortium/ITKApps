/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageRegistrationConsole.h
  Language:  C++

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

//
// 2D image registration
//

#ifndef IMAGEREGISTRATIONCONSOLE
#define IMAGEREGISTRATIONCONSOLE

#include <fltkImage2DViewer.h>
#include <fltkRegularStepGradientDescent.h>
#include <ImageRegistrationConsoleGUI.h>
#include <fltkRGBImage2DViewer.h>

class ImageRegistrationConsole : public ImageRegistrationConsoleGUI {

public:

  typedef ImageRegistrationConsoleBase::InternalPixelType PixelType;

  typedef fltk::Image2DViewer< PixelType >    ImageViewerType;
  typedef ImageViewerType::Pointer            ImageViewerPointer;
  typedef fltk::RegularStepGradientDescent    OptimizerGUIType;

  typedef ImageRegistrationConsoleBase::InputPixelType InputPixelType;

  typedef fltk::Image2DViewer< InputPixelType >    InputImageViewerType;
  typedef InputImageViewerType::Pointer            InputImageViewerPointer;
  
  typedef fltk::RGBImage2DViewer< InputPixelType > MixedChannelViewerType;  
public:

  ImageRegistrationConsole();
  virtual ~ImageRegistrationConsole();

  virtual void Show(void);
  virtual void Hide(void);

  virtual void LoadFixedImage(void);
  virtual void LoadMovingImage(void);
  
  virtual void SaveRegisteredImage(void);
  
  virtual void Quit(void);
  virtual void ShowStatus(const char * text);

  virtual void ShowInputFixedImage(void);
  virtual void ShowInputMovingImage(void);
  virtual void ShowFixedImage(void);
  virtual void ShowNormalizedInputMovingImage(void);
  virtual void ShowTransformedMovingImage(void);
  virtual void ShowMovingImage(void);
  virtual void ShowRegisteredMovingImage(void);
  virtual void ShowMixedChannelImage(void) ;

  virtual void Execute(void);

  virtual void GenerateRegisteredMovingImage(void);
  
  virtual void SelectMutualInformationMetric(void);
  virtual void SelectMeanSquaresMetric(void);

  virtual void GenerateNormalizedInputMovingImage(void);
  virtual void GenerateFixedImage(void);
  virtual void GenerateTransformedMovingImage(void);
  virtual void GenerateMovingImage(void);


private:
  InputImageViewerPointer    m_InputMovingImageViewer;
  ImageViewerPointer         m_NormalizedInputMovingImageViewer;
  ImageViewerPointer         m_TransformedMovingImageViewer;
  ImageViewerPointer         m_MovingImageViewer;
  
  InputImageViewerPointer    m_InputFixedImageViewer;
  ImageViewerPointer         m_FixedImageViewer;

  InputImageViewerPointer    m_RegisteredMovingImageViewer;

  MixedChannelViewerType::Pointer m_MixedChannelViewer;

  OptimizerGUIType           m_OptimizerInterface;

};



#endif



