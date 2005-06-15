/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DuctExtractorConsoleBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __DUCT_EXTRACTOR_CONSOLE_BASE__
#define __DUCT_EXTRACTOR_CONSOLE_BASE__

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkLaplacianRecursiveGaussianImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"

class DuctExtractorConsoleBase 
{

public:
  typedef   unsigned short                    InputPixelType;
  typedef   float                             PixelType;
  typedef   unsigned char                     MaskPixelType;

  typedef   itk::Image< InputPixelType, 3 >   InputImageType;
  typedef   itk::Image< PixelType, 3 >        ImageType;
  typedef   itk::Image< MaskPixelType, 3 >    MaskImageType;

  typedef   itk::ImageFileReader< 
                            InputImageType >       VolumeReaderType;

  typedef   itk::ImageFileWriter< 
                            MaskImageType >        VolumeWriterType;

  typedef   itk::CurvatureFlowImageFilter<
                                    InputImageType,
                                    ImageType > SmoothingFilterType;
  
  typedef   itk::LaplacianRecursiveGaussianImageFilter<
                                    ImageType,
                                    ImageType > LaplacianFilterType;

  typedef   LaplacianFilterType::RealType    RealType;


  typedef   itk::ConnectedThresholdImageFilter< 
                                          ImageType, 
                                          MaskImageType 
                                                >  RegionGrowthFilterType;
            
public:

  DuctExtractorConsoleBase();
  virtual ~DuctExtractorConsoleBase();
  virtual void Load(void) = 0;
  virtual void Load(const char * filename);
  virtual void ShowProgress(float);
  virtual void ShowStatus(const char * text);
  virtual void Execute(void);
  virtual void Cancel(void);
  virtual void SetSigma( RealType );
  virtual void SetRegionGrowingLowerThreshold( RealType );
  virtual void SetRegionGrowingUpperThreshold( RealType );
  virtual void SetSmoothingNumberOfIterations( unsigned int );

  virtual void WriteSegmentation(void) = 0;

  virtual void WriteSegmentation(const char * filename);

protected:

  VolumeReaderType::Pointer          m_Reader;

  LaplacianFilterType::Pointer       m_Laplacian;
  SmoothingFilterType::Pointer       m_Smoother;
  RegionGrowthFilterType::Pointer    m_RegionGrower;

  bool                               m_ImageFileNameAvailable;

  VolumeWriterType::Pointer          m_Writer_Segmentation;

};



#endif
