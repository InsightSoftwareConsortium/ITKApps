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
  m_Reader = VolumeReaderType::New();

  m_InputCaster = InputCasterType::New();

  m_Curvature = CurvatureDiffusionType::New();

  m_SeedCaster = InputCasterType::New();

  m_SegmentedCaster = SegmentedCasterType::New();

  m_thresholder = ThresholdingFilterType::New();

  m_fastMarching = FastMarchingFilterType::New();
  
  m_thresholdSegmentation = ThresholdSegmentationLevelSetImageFilterType::New();

  m_writerCaster = WriteCasterType::New();
  m_writer = SegmentWriterType::New();
  m_writer2 = BinaryWriterType::New();

  m_thresholdSegmentation->SetPropagationScaling( 1.0 );
  m_thresholdSegmentation->SetCurvatureScaling( 1.0 );
  m_thresholdSegmentation->SetMaximumRMSError( 0.02 );
  m_thresholdSegmentation->SetMaximumIterations( 1200 );
  m_thresholdSegmentation->SetIsoSurfaceValue(0.0);
  m_thresholdSegmentation->SetUseNegativeFeaturesOn();
  m_thresholdSegmentation->SetFeatureImage( m_Curvature->GetOutput() );

  m_thresholder->SetInput( m_thresholdSegmentation->GetOutput() );   

  m_seeds = NodeContainer::New();

  m_BinaryThresh = BinaryThresholdType::New();
  m_BinaryThresh2 = BinaryThresholdType2::New();
  m_SpeedCaster = SegmentedCasterType::New();

  m_image = WriteImageType3D::New();
  m_thresh = WriteImageType3D::New();

  m_guessImage = InputImageType::New();

  m_minMax = CalculatorType::New();
  m_minMaxSeed = CalculatorType::New();
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
 
/************************************
 *
 *  Load
 *
 ***********************************/
void
SegmenterConsoleBase 
::Load( const char * filename )
{
  if( !filename )
  {
    return;
  }

  m_Reader->SetFileName( filename );
  try {
    m_Reader->Update();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }

}

