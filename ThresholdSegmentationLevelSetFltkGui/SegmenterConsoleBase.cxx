/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SegmenterConsoleBase.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <SegmenterConsoleBase.h>

/************************************
 *
 *  Constructor
 *
 ***********************************/
SegmenterConsoleBase 
::SegmenterConsoleBase()
{
  // Allocate all of the pipeline objects.
  m_Reader     = ReaderType::New();
  m_SeedReader = ReaderType::New();
  m_Curvature  = CurvatureDiffusionType::New();

  m_InputCaster     = InputCasterType::New();
  m_SeedCaster      = InputCasterType::New();
  m_SegmentedCaster = SegmentedCasterType::New();
  m_SpeedCaster     = SegmentedCasterType::New();
  m_writerCaster    = WriteCasterType::New();

  m_thresholder  = BinaryThresholdType::New();
  m_fastMarching = FastMarchingFilterType::New();
  m_thresholdSegmentation = ThresholdSegmentationLevelSetImageFilterType::New();
  
  m_segmentWriter = SegmentWriterType::New();
  m_maskWriter = BinaryWriterType::New();

  m_seeds = NodeContainer::New();

  m_speedThresh = BinaryThresholdType::New();
  m_maskThresh = BinaryThresholdType::New();

  m_image = WriteImageType3D::New();
  m_thresh = WriteImageType3D::New();
  m_guessImage = InputImageType::New();

  m_minMax = CalculatorType::New();

  // All the pipelines are as follows:
  // m_Reader->m_Curvature->m_thresholdSegmentation->m_segmentWriter
  // m_Reader->m_Curvature->m_thresholdSegmentation->m_thresholder->m_SegmentedCaster
  // m_Reader->m_Curvature->m_thresholdSegmentation->m_speedThresh->m_SpeedCaster
  // m_Reader->m_Curvature->m_thresholdSegmentation->m_maskThresh->m_maskWriter
  // m_SeedReader->m_SeedCaster
  // m_Reader->m_Curvature->m_InputCaster
  //
  m_Curvature->SetInput(m_Reader->GetOutput());
  m_thresholdSegmentation->SetFeatureImage( m_Curvature->GetOutput() );
  m_InputCaster->SetInput(m_Curvature->GetOutput());
  m_thresholder->SetInput( m_thresholdSegmentation->GetOutput() );
  m_speedThresh->SetInput(const_cast<InputImageType *>(m_thresholdSegmentation->GetSpeedImage()));
  m_maskThresh->SetInput(m_thresholdSegmentation->GetOutput());
  m_SpeedCaster->SetInput(m_speedThresh->GetOutput());
  m_SegmentedCaster->SetInput(m_thresholder->GetOutput());
  m_SeedCaster->SetInput(m_SeedReader->GetOutput());
  m_segmentWriter->SetInput(m_thresholdSegmentation->GetOutput());
  m_maskWriter->SetInput(m_maskThresh->GetOutput());
}

/************************************
 *
 *  Destructor
 *
 ***********************************/
SegmenterConsoleBase 
::~SegmenterConsoleBase()
{
}
 
