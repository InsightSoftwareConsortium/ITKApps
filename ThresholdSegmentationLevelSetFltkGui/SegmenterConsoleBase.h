/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SegmenterConsoleBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef SEGMENTERCONSOLEBASE
#define SEGMENTERCONSOLEBASE

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <itkThresholdSegmentationLevelSetImageFilter.h>
#include "itkFastMarchingImageFilter.h"
#include "itkZeroCrossingImageFilter.h"

#include "itkCurvatureAnisotropicDiffusionImageFilter.h"

#include "itkCastImageFilter.h"

#include "itkBinaryThresholdImageFilter.h"

#include "itkMinimumMaximumImageCalculator.h"


class SegmenterConsoleBase 
{

public:
  typedef   float                             InputPixelType;
  typedef   float                             OutputPixelType;
  typedef   unsigned char                     WritePixelType;

  typedef   float                             ComputationType;

  typedef   itk::Image< InputPixelType, 2 >   InputImageType;
  typedef   itk::Image< OutputPixelType, 2 >  OutputImageType;
  typedef   itk::Image< WritePixelType, 2 >   WriteImageType;

  typedef   itk::Image< InputPixelType, 3 >   InputImageType3D;
  typedef   itk::Image< OutputPixelType, 3 >  OutputImageType3D;
  typedef   itk::Image< WritePixelType, 3 >   WriteImageType3D;

  typedef   itk::ImageFileReader< 
                            InputImageType >  VolumeReaderType;

  typedef   itk::CastImageFilter<
                            InputImageType, 
                            InputImageType3D > InputCasterType;

  typedef   itk::CastImageFilter<
                            WriteImageType, 
                            WriteImageType3D > SegmentedCasterType;

  typedef   itk::CurvatureAnisotropicDiffusionImageFilter<
                            InputImageType,
                            InputImageType  > CurvatureDiffusionType;

  typedef   itk::ZeroCrossingImageFilter<
                            InputImageType,
                            WriteImageType > ThresholdingFilterType;

  typedef   itk::FastMarchingImageFilter<
                            InputImageType,
                            InputImageType > FastMarchingFilterType;
  
  typedef   itk::ThresholdSegmentationLevelSetImageFilter<
                            InputImageType, 
                            InputImageType > 
                                      ThresholdSegmentationLevelSetImageFilterType;

  typedef   itk::CastImageFilter<InputImageType, WriteImageType> WriteCasterType;
  typedef   itk::ImageFileWriter<InputImageType>  SegmentWriterType;
  typedef   itk::ImageFileWriter<WriteImageType>  BinaryWriterType;
  
  
  typedef   itk::BinaryThresholdImageFilter<
                            InputImageType,
                            WriteImageType >  BinaryThresholdType;

  typedef   itk::BinaryThresholdImageFilter<
                            InputImageType,
                            WriteImageType >  BinaryThresholdType2;
  
  typedef   FastMarchingFilterType::NodeContainer  NodeContainer;
  typedef   FastMarchingFilterType::NodeType       NodeType;

  typedef   itk::MinimumMaximumImageCalculator<InputImageType> CalculatorType;

public:

  SegmenterConsoleBase();
  virtual ~SegmenterConsoleBase();
  virtual void Load(void) = 0;
  virtual void Load(const char * filename);

protected:

  VolumeReaderType::Pointer                                      m_Reader;
  InputCasterType::Pointer                                       m_InputCaster;

  CurvatureDiffusionType::Pointer                                m_Curvature;

  ThresholdingFilterType::Pointer                                m_thresholder;
  FastMarchingFilterType::Pointer                                m_fastMarching;
  ThresholdSegmentationLevelSetImageFilterType::Pointer          m_thresholdSegmentation;
  WriteCasterType::Pointer                                       m_writerCaster;
  SegmentWriterType::Pointer                                     m_writer;
  BinaryWriterType::Pointer                                      m_writer2;
  NodeContainer::Pointer                                         m_seeds;
  InputImageType::IndexType                                      m_seedPosition;
  NodeType                                                       m_node;

  VolumeReaderType::Pointer                                      m_SeedReader;

  InputCasterType::Pointer                                       m_SeedCaster;

  SegmentedCasterType::Pointer                                   m_SegmentedCaster;

  BinaryThresholdType::Pointer                                   m_BinaryThresh;
  BinaryThresholdType2::Pointer                                   m_BinaryThresh2;
  SegmentedCasterType::Pointer                                   m_SpeedCaster;

  WriteImageType3D::Pointer                                      m_image;
  WriteImageType3D::Pointer                                      m_thresh;

  InputImageType3D::IndexType                                    m_start1;
  InputImageType3D::SizeType                                     m_size1;
  InputImageType3D::RegionType                                   m_region1;

  InputImageType::Pointer                                        m_guessImage;
  InputImageType::IndexType                                      m_start2;
  InputImageType::SizeType                                       m_size2;
  InputImageType::RegionType                                     m_region2;

  CalculatorType::Pointer                                        m_minMax;
  CalculatorType::Pointer                                        m_minMaxSeed;
};



#endif
