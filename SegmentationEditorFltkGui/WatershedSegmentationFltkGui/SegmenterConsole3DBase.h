/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SegmenterConsole3DBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef SEGMENTERCONSOLE3DBASE
#define SEGMENTERCONSOLE3DBASE

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <itkRescaleIntensityImageFilter.h>

#include <itkGradientMagnitudeImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>

#include <itkWatershedFilterAndWriter.h>
#include <itkFlipImageFilter.h>



class SegmenterConsole3DBase 
{

public:
  typedef   float                             InputPixelType;
  typedef   float                             PixelType;
  typedef   float                             OutputPixelType;
  typedef   unsigned char                     WritePixelType;

  typedef   float                             ComputationType;

  typedef   itk::Image< InputPixelType, 3 >   InputImageType;
  typedef   itk::Image< PixelType, 3 >        ImageType;
  typedef   itk::Image< OutputPixelType, 3 >  OutputImageType;
  typedef   itk::Image< WritePixelType, 3 >   WriteImageType;

  typedef   itk::ImageFileReader< 
                            InputImageType >  VolumeReaderType;

  typedef   itk::FlipImageFilter<InputImageType> FlipType;

  typedef   itk::GradientMagnitudeImageFilter<
                            InputImageType,
                            ImageType >       GradientMagnitudeFilterType;

  typedef   itk::DiscreteGaussianImageFilter<
                            ImageType,
                            ImageType >       DiscreteGaussianImageFilterType;

  typedef   itk::WatershedFilterAndWriter<ImageType> WatershedFilterType;
  
  typedef   itk::RescaleIntensityImageFilter<
                      OutputImageType,
                      WriteImageType >        RescaleIntensityImageFilterType; 
                                 
public:

  SegmenterConsole3DBase();
  virtual ~SegmenterConsole3DBase();
  virtual void Load(void) = 0;
  virtual void Load(const char * filename);
  virtual void ShowProgress(float);
  virtual void ShowStatus(const char * text);
  virtual void ShowGradientImage(void);
  virtual void ShowSegmentedImage(void);
  virtual void SetVariance( double );
  virtual void SetMaxFlood( double );
  virtual void SetThreshold( double );
  virtual void SetTreeFileName( const char * );
  virtual void SetSegmentedFileName( const char * );
  virtual void SaveSegmentedData( void );

protected:

  VolumeReaderType::Pointer                                      m_Reader;

  FlipType::Pointer                                              m_Flip;

  GradientMagnitudeFilterType::Pointer                           m_FilterGrad;

  DiscreteGaussianImageFilterType::Pointer                       m_FilterGauss;
  
  WatershedFilterType::Pointer                                   m_FilterW;

  bool                                        m_ImageFileNameAvailable;

  RescaleIntensityImageFilterType::Pointer    m_Rescaler;  

};



#endif
