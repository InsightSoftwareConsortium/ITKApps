/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ThresholdLevelSetSegmentationModule.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  
  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.
  
    This software is distributed WITHOUT ANY WARRANTY; without even 
    the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
    PURPOSE.  See the above copyright notices for more information.
    
=========================================================================*/


#include "ThresholdLevelSetSegmentationModule.h"

namespace ISIS {
  
  ThresholdLevelSetSegmentationModule::ThresholdLevelSetSegmentationModule()
  {  
    m_RescaleIntensityFilter      =   RescaleIntensityFilterType::New();
    m_ConfidenceConnectedFilter   =   ConnectedFilterType::New();
    m_ThresholdLevelSetFilter     =   ThresholdLevelSetFilterType::New();
    m_RescaleOutputFilter         =   OutputRescaleIntensityFilterType::New();  
    m_CastFeatureFilter           =   CastFeatureFilterType::New();
    
    m_Multiplier = 1.5f;
    m_NumberOfIterations = 5;
    m_InitialNeighborhoodRadius = 2;

    m_ConfidenceConnectedFilter->SetReplaceValue( 255 );
    //m_ConfidenceConnectedFilter->SetMultiplier( m_Multiplier ); 
    //m_ConfidenceConnectedFilter->SetNumberOfIterations( m_NumberOfIterations ); 
    //m_ConfidenceConnectedFilter->SetInitialNeighborhoodRadius( m_InitialNeighborhoodRadius ); 
    
    m_RescaleIntensityFilter->SetOutputMinimum( -4.0 ); 
    m_RescaleIntensityFilter->SetOutputMaximum(  4.0 ); 
    
    m_RescaleOutputFilter->SetOutputMinimum(  0  );
    m_RescaleOutputFilter->SetOutputMaximum( 255 );
    
    m_ThresholdLevelSetFilter->SetFeatureImage( m_CastFeatureFilter->GetOutput() );  
    m_RescaleIntensityFilter->SetInput( m_ConfidenceConnectedFilter->GetOutput() );
    m_ThresholdLevelSetFilter->SetInput( m_RescaleIntensityFilter->GetOutput() );
    m_RescaleOutputFilter->SetInput( m_ThresholdLevelSetFilter->GetOutput() );
    
    // Allow filter to release data as the execution of
    // pipeline progresses from filter to filter.
    // This reduces memory consumption
    m_ConfidenceConnectedFilter->ReleaseDataFlagOn();
    m_RescaleIntensityFilter->ReleaseDataFlagOn();
    m_ThresholdLevelSetFilter->ReleaseDataFlagOn();
    
    
    m_Notifier = NotifierType::New();
    
    // Create the progress accumulator
    m_ProgressAccumulator = itk::ProgressAccumulator::New();
    m_ProgressAccumulator->SetMiniPipelineFilter( m_Notifier );
    
    // Register the filters with the progress accumulator
    m_ProgressAccumulator->RegisterInternalFilter(m_ConfidenceConnectedFilter,0.1f);
    m_ProgressAccumulator->RegisterInternalFilter(m_RescaleIntensityFilter,0.2f);
    m_ProgressAccumulator->RegisterInternalFilter(m_CastFeatureFilter,0.2f);
    m_ProgressAccumulator->RegisterInternalFilter(m_ThresholdLevelSetFilter,  0.5f);
    m_ProgressAccumulator->ResetProgress();

    m_LowerThreshold = 80.0f; 
    m_UpperThreshold = 110.f;
    m_CurvatureScaling = 1.0f;
    m_PropagationScaling = 1.0f;
    m_AdvectionScaling = 1.0f;
    m_MaximumRMSError =  0.001f;
    m_MaximumIterations = 100;
  }
  
  
  
  
  
  ThresholdLevelSetSegmentationModule
    ::~ThresholdLevelSetSegmentationModule()
  {
  }
  
  
  
  
  void
    ThresholdLevelSetSegmentationModule
    ::SetInput( const InputImageType * image )
  {
    m_ConfidenceConnectedFilter->SetInput( image );
    m_CastFeatureFilter->SetInput( image );  
  }
  
  
  
  const ThresholdLevelSetSegmentationModule::OutputImageType *
    ThresholdLevelSetSegmentationModule
    ::GetOutput()
  {
    return m_RescaleOutputFilter->GetOutput();
  }
  
  
  
    const ThresholdLevelSetSegmentationModule::OutputImageType * 
    ThresholdLevelSetSegmentationModule
    ::GetInitialSegmentationOutput()
  {
    return m_ConfidenceConnectedFilter->GetOutput();
  }

  
  void
    ThresholdLevelSetSegmentationModule
    ::SetSeedPoint( int x, int y, int z )
  {
    typedef InternalImageType::IndexType   IndexType;
    IndexType SeedPoint;
    SeedPoint[0] = x;
    SeedPoint[1] = y;
    SeedPoint[2] = z;
    m_ConfidenceConnectedFilter->SetSeed( SeedPoint );
  }
  
  
  itk::Notifier* ThresholdLevelSetSegmentationModule::GetNotifier( void )
  {
    return m_Notifier;
  }
  
  
  void ThresholdLevelSetSegmentationModule::Execute()
  {
    try
    {
      std::cout << "Initiating Threshold Level Set parameters..." << std::endl;
      
      // Set the variable values into itk Objects
      m_ConfidenceConnectedFilter->SetMultiplier( m_Multiplier ); 
      m_ConfidenceConnectedFilter->SetNumberOfIterations( m_NumberOfIterations ); 
      m_ConfidenceConnectedFilter->SetInitialNeighborhoodRadius( m_InitialNeighborhoodRadius ); 

      m_ThresholdLevelSetFilter->SetLowerThreshold( m_LowerThreshold );
      m_ThresholdLevelSetFilter->SetUpperThreshold( m_UpperThreshold );
      m_ThresholdLevelSetFilter->SetCurvatureScaling( m_CurvatureScaling );
      m_ThresholdLevelSetFilter->SetPropagationScaling( m_PropagationScaling );
      m_ThresholdLevelSetFilter->SetAdvectionScaling( m_AdvectionScaling );
      m_ThresholdLevelSetFilter->SetMaximumRMSError( m_MaximumRMSError );
      m_ThresholdLevelSetFilter->SetMaximumIterations( m_MaximumIterations );

      // Initialize the progress counter
      m_ProgressAccumulator->ResetProgress();
      
      m_ConfidenceConnectedFilter->Update();
      
      std::cout << "Region growing initialization for level set" << std::endl;
      
      m_RescaleIntensityFilter->Update();
      
      std::cout << "Input rescaled" << std::endl;
      
      m_CastFeatureFilter->Update();
      
      std::cout << "Input casted for feature image" << std::endl;
      
      m_ThresholdLevelSetFilter->Update();
      
      std::cout << "Threshold Segmentation Level Set done" << std::endl;
      
      m_RescaleOutputFilter->Update();
      
      std::cout << "Output rescaled" << std::endl;
    }
    catch( itk::ExceptionObject & excep )
    {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
    }
  }


  int ThresholdLevelSetSegmentationModule::GetElapsedIterations( void )
  {
    return m_ThresholdLevelSetFilter->GetElapsedIterations();
  }

  
  
}  // end of namespace ISIS

