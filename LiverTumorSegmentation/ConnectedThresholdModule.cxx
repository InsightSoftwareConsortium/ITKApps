/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ConnectedThresholdModule.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "ConnectedThresholdModule.h"

namespace ISIS {

ConnectedThresholdModule
::ConnectedThresholdModule()
{

  m_SmoothingFilter             =   SmoothingFilterType::New();
  m_ConnectedThresholdFilter    =   ConnectedFilterType::New();

  m_ConnectedThresholdFilter->SetInput( m_SmoothingFilter->GetOutput() );

  /*  Define reasonable defaults for the parameters */
   m_SmoothingIterations = 20;
   m_SmoothingTimeStep = 0.0625;
   m_LowerThreshold = 0;
   m_UpperThreshold = 255;
  
   m_ConnectedThresholdFilter->SetReplaceValue( 255 );

  // Allow filter to release data as the execution of
  // pipeline progresses from filter to filter.
  // This allows to reduce memory consumption
  m_SmoothingFilter->ReleaseDataFlagOn();
  m_ConnectedThresholdFilter->ReleaseDataFlagOn();

}





ConnectedThresholdModule
::~ConnectedThresholdModule()
{
}




void
ConnectedThresholdModule
::SetInput( const InputImageType * image )
{
  m_SmoothingFilter->SetInput( image );
}



const ConnectedThresholdModule::OutputImageType *
ConnectedThresholdModule
::GetOutput()
{
   return m_ConnectedThresholdFilter->GetOutput();
}



void
ConnectedThresholdModule
::SetSeedPoint( int x, int y, int z )
{
   typedef InternalImageType::IndexType   IndexType;
   IndexType SeedPoint;
   SeedPoint[0] = x;
   SeedPoint[1] = y;
   SeedPoint[2] = z;
   m_ConnectedThresholdFilter->SetSeed( SeedPoint );
}



void
ConnectedThresholdModule
::Execute()
{
  try
    {
    std::cout << "Initiating ConnectedThresholdModule Parameters ..." << std::endl;

    m_SmoothingFilter->SetNumberOfIterations( m_SmoothingIterations );
    m_SmoothingFilter->SetTimeStep( m_SmoothingTimeStep );
    m_ConnectedThresholdFilter->SetLower( m_LowerThreshold );
    m_ConnectedThresholdFilter->SetUpper( m_UpperThreshold );

    std::cout << "Initiating segmentation..." << std::endl;
    m_SmoothingFilter->Update();
    std::cout << "Image smoothed" << std::endl;
    m_ConnectedThresholdFilter->Update();
    std::cout << "Image Segmentation Completed." << std::endl;
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught in ConnectedThresholdModule!" << std::endl;
    std::cerr << excep << std::endl;
    }
}


}  // end of namespace ISIS

