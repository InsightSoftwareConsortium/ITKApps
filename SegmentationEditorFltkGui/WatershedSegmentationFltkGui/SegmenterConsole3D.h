/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SegmenterConsole3D.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef SEGMENTERCONSOLE
#define SEGMENTERCONSOLE

#include <fltkImageViewer.h>
#include <fltkRGBImageViewer.h>
#include <SegmenterConsole3DGUI.h>

#include "itkUnaryFunctorImageFilter.h"
#include "itkScalarToRGBPixelFunctor.h"


class SegmenterConsole3D : public SegmenterConsole3DGUI {

public:

  typedef unsigned short OverlayPixelType;
  typedef itk::RGBPixel<unsigned char>   RGBPixelType;
  typedef itk::Image<RGBPixelType, 3>    RGBImageType;
  typedef itk::Image<unsigned long, 3>   LabeledImageType;
  typedef itk::Image<float, 3>           ScalarImageType;

  typedef fltk::ImageViewer< PixelType, OverlayPixelType > ImageViewerType;
  typedef fltk::ImageViewer< InputPixelType, OverlayPixelType > InputImageViewerType;
  typedef fltk::RGBImageViewer< unsigned char, OverlayPixelType > RGBImageViewerType;

  typedef itk::Functor::ScalarToRGBPixelFunctor<unsigned long>
    ColorMapFunctorType;
  typedef itk::UnaryFunctorImageFilter<LabeledImageType,
    RGBImageType, ColorMapFunctorType> ColorMapFilterType;


public:
  SegmenterConsole3D();
  virtual ~SegmenterConsole3D();
  virtual void Show(void);
  virtual void Hide(void);
  virtual void Load(void);
  virtual void Quit(void);
  virtual void ShowStatus(const char * text);
  virtual void ShowInputImage(void);
  virtual void ShowGradientImage(void);
  virtual void ShowSegmentedImage(void);
  virtual void SaveSegmentedData(void);
  virtual void Flip(int);

private:
  InputImageViewerType*      m_InputViewer;

  ImageViewerType*           m_ViewerGrad;

  RGBImageViewerType*        m_SegmentViewer;

  ColorMapFilterType::Pointer m_Colormapper;

  bool axes[3];

};



#endif
