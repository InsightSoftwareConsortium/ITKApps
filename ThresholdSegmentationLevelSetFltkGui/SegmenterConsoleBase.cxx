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
  m_Reader = ReaderType::New();
  m_SeedReader = ReaderType::New();
  m_Curvature = CurvatureDiffusionType::New();

  m_InputCaster = InputCasterType::New();
  m_SeedCaster = InputCasterType::New();
  m_SegmentedCaster = SegmentedCasterType::New();
  m_SpeedCaster = SegmentedCasterType::New();
  m_writerCaster = WriteCasterType::New();

  m_thresholder = BinaryThresholdType::New();
  m_fastMarching = FastMarchingFilterType::New();
  m_thresholdSegmentation = ThresholdSegmentationLevelSetImageFilterType::New();
  // these should match the gui initializations in SegmenterConsole.cxx
  m_thresholdSegmentation->SetPropagationScaling( 1.0 );
  m_thresholdSegmentation->SetCurvatureScaling( 1.0 );
  m_thresholdSegmentation->SetEdgeWeight(1.0);
  m_thresholdSegmentation->SetMaximumRMSError( 0.02 );
  m_thresholdSegmentation->SetMaximumIterations( 1200 );
  m_thresholdSegmentation->SetIsoSurfaceValue(0.0);
  m_thresholdSegmentation->ReverseExpansionDirectionOff();
  m_thresholdSegmentation->SetSmoothingIterations(0);
  m_thresholdSegmentation->SetSmoothingConductance(0.5);
  m_thresholdSegmentation->SetSmoothingTimeStep(0.100);


  m_segmentWriter = SegmentWriterType::New();
  m_maskWriter = BinaryWriterType::New();

  m_seeds = NodeContainer::New();

  m_speedThresh = BinaryThresholdType::New();
  m_maskThresh = BinaryThresholdType::New();

  m_image = WriteImageType3D::New();
  m_thresh = WriteImageType3D::New();
  m_guessImage = InputImageType::New();

  m_minMax = CalculatorType::New();

  // set up pipeline
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

