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
#include <itkFastMarchingImageFilter.h>
#include <itkCurvatureAnisotropicDiffusionImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>

/**
 * \class SegmenterConsoleBase
 *
 * This class contains declarations of pipeline objects and any methods that
 * need to be exposed to the GUI.  The fluid generated SegmentationConsoleGUI
 * class will inherit from this class so that its methods can reference these
 * objects.  The final SegmenterConsole class inherits from SegmenterConsoleGUI
 * so that it can access both the pipeline objects and the GUI widgets.
 *
 */
class SegmenterConsoleBase 
{  
public:
  typedef   float                              InputPixelType;
  typedef   float                              OutputPixelType;
  typedef   unsigned char                      WritePixelType;

  typedef   float                              ComputationType;

  typedef   itk::Image< InputPixelType, 2 >    InputImageType;
  typedef   itk::Image< OutputPixelType, 2 >   OutputImageType;
  typedef   itk::Image< WritePixelType, 2 >    WriteImageType;

  typedef   itk::Image< InputPixelType, 3 >    InputImageType3D;
  typedef   itk::Image< OutputPixelType, 3 >   OutputImageType3D;
  typedef   itk::Image< WritePixelType, 3 >    WriteImageType3D;

  typedef   itk::ImageFileReader< 
                            InputImageType >   ReaderType;

  typedef   itk::CastImageFilter<
                            InputImageType, 
                            InputImageType3D > InputCasterType;

  typedef   itk::CastImageFilter<
                            WriteImageType, 
                            WriteImageType3D > SegmentedCasterType;

  typedef   itk::CurvatureAnisotropicDiffusionImageFilter<
                            InputImageType,
                            InputImageType  >  CurvatureDiffusionType;

  typedef   itk::FastMarchingImageFilter<
                            InputImageType,
                            InputImageType >   FastMarchingFilterType;

  typedef   itk::BinaryThresholdImageFilter<
                            InputImageType,
                            WriteImageType >   BinaryThresholdType;
  
  typedef   itk::ThresholdSegmentationLevelSetImageFilter<
                            InputImageType, 
                            InputImageType > 
                                      ThresholdSegmentationLevelSetImageFilterType;

  typedef   itk::CastImageFilter<InputImageType, WriteImageType> WriteCasterType;
  typedef   itk::ImageFileWriter<InputImageType>  SegmentWriterType;
  typedef   itk::ImageFileWriter<WriteImageType>  BinaryWriterType;

  typedef   FastMarchingFilterType::NodeContainer  NodeContainer;
  typedef   FastMarchingFilterType::NodeType       NodeType;

  typedef   itk::MinimumMaximumImageCalculator<InputImageType> CalculatorType;

  SegmenterConsoleBase();
  virtual ~SegmenterConsoleBase();
  
protected:
  ReaderType::Pointer                                            m_Reader;
  CurvatureDiffusionType::Pointer                                m_Curvature;
  InputCasterType::Pointer                                       m_InputCaster;

  BinaryThresholdType::Pointer                                   m_thresholder;
  FastMarchingFilterType::Pointer                                m_fastMarching;

  ThresholdSegmentationLevelSetImageFilterType::Pointer          m_thresholdSegmentation;

  WriteCasterType::Pointer                                       m_writerCaster;
  SegmentWriterType::Pointer                                     m_segmentWriter;
  BinaryWriterType::Pointer                                      m_maskWriter;

  ReaderType::Pointer                                            m_SeedReader;
  InputCasterType::Pointer                                       m_SeedCaster;
  NodeContainer::Pointer                                         m_seeds;
  InputImageType::IndexType                                      m_seedPosition;
  NodeType                                                       m_node;

  SegmentedCasterType::Pointer                                   m_SegmentedCaster;

  BinaryThresholdType::Pointer                                   m_speedThresh;
  SegmentedCasterType::Pointer                                   m_SpeedCaster;

  BinaryThresholdType::Pointer                                   m_maskThresh;

  WriteImageType3D::Pointer                                      m_image;
  WriteImageType3D::Pointer                                      m_thresh;

  InputImageType3D::IndexType                                    m_start1;
  InputImageType3D::SizeType                                     m_size1;
  InputImageType3D::RegionType                                   m_region1;

  InputImageType::Pointer                                        m_guessImage;
  InputImageType::IndexType                                      m_start2;
  InputImageType::SizeType                                       m_size2;
  InputImageType::RegionType                                     m_region2;

  // this is used in multiple places to calculate min and max
  // so each time the input is set
  CalculatorType::Pointer                                        m_minMax;
};

#endif
