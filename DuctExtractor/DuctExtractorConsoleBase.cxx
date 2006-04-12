/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DuctExtractorConsoleBase.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include <DuctExtractorConsoleBase.h>


/************************************
 *
 *  Constructor
 *
 ***********************************/
DuctExtractorConsoleBase 
::DuctExtractorConsoleBase()
{

  m_ImageFileNameAvailable = false;

  m_Reader        = VolumeReaderType::New();
  m_Laplacian     = LaplacianFilterType::New();
  m_Smoother      = SmoothingFilterType::New();
  m_RegionGrower  = RegionGrowthFilterType::New();
  m_Statistics    = StatisticsFilterType::New();

  m_Laplacian->SetSigma( 2.5 );

  m_Smoother->SetNumberOfIterations( 5 );
  m_Smoother->SetTimeStep( 0.05 );
  
  m_RegionGrower->SetLower( 0.0 );
  m_RegionGrower->SetUpper( 1.0 );
  m_RegionGrower->SetReplaceValue( 
       itk::NumericTraits< MaskPixelType >::One );

  m_Smoother->SetInput(  m_Reader->GetOutput()   );
  m_Laplacian->SetInput( m_Smoother->GetOutput() );
  m_RegionGrower->SetInput( m_Laplacian->GetOutput() );
  m_Statistics->SetInput( m_Laplacian->GetOutput() );

  m_RegionGrower->SetLowerInput( m_Statistics->GetMinimumOutput() );

  m_Writer_Segmentation        = VolumeWriterType::New();

  m_Writer_Segmentation->SetInput( m_RegionGrower->GetOutput() );
  
}




/************************************
 *
 *  Destructor
 *
 ***********************************/
DuctExtractorConsoleBase 
::~DuctExtractorConsoleBase()
{

}



 
/************************************
 *
 *  Load
 *
 ***********************************/
void
DuctExtractorConsoleBase 
::Load( const char * filename )
{

  if( !filename )
  {
    return;
  }

  m_Reader->SetFileName( filename );

  m_ImageFileNameAvailable = true;

}


 
/************************************
 *
 *  Show Progress
 *
 ***********************************/
void
DuctExtractorConsoleBase 
::ShowProgress( float )
{

}


 
/************************************
 *
 *  Show Status
 *
 ***********************************/
void
DuctExtractorConsoleBase 
::ShowStatus( const char * )
{

}




 
/************************************
 *
 *  Set Sigma
 *
 ***********************************/
void
DuctExtractorConsoleBase 
::SetSigma( RealType value )
{
  
  m_Laplacian->SetSigma( value );

}




 
/**********************************************
 *
 *  Set Region Growing Lower Threshold
 *
 **********************************************/
void
DuctExtractorConsoleBase 
::SetRegionGrowingLowerThreshold( RealType value )
{
  
  m_RegionGrower->SetLower( value );

}



 
/**********************************************
 *
 *  Set Region Growing Upper Threshold
 *
 **********************************************/
void
DuctExtractorConsoleBase 
::SetRegionGrowingUpperThreshold( RealType value )
{
  
  m_RegionGrower->SetUpper( value );

}



 
 
/*****************************************************
 *
 *  Set Number of Iterations in the Smoothing Filter
 *
 ****************************************************/
void
DuctExtractorConsoleBase 
::SetSmoothingNumberOfIterations( unsigned int numberOfIterations )
{
  
  m_Smoother->SetNumberOfIterations( numberOfIterations );

}




/************************************
 *
 *  Execute
 *
 ***********************************/
void
DuctExtractorConsoleBase 
::Execute( void )
{

  if( ! (m_ImageFileNameAvailable) ) 
    {
    this->ShowStatus("Please load an image first");
    return;
    }
  
  
  this->ShowStatus("Smoothing the input image...");
  m_Smoother->UpdateLargestPossibleRegion();
  
  this->ShowStatus("Computing Laplacian...");
  m_Laplacian->UpdateLargestPossibleRegion();

  this->ShowStatus("Computing Region Growing...");
  m_RegionGrower->UpdateLargestPossibleRegion();

  this->ShowStatus("Processing Completed");

}





/************************************
 *
 *  Write Laplacian
 *
 ***********************************/
void
DuctExtractorConsoleBase 
::WriteSegmentation( const char * filename )
{
  
  m_Writer_Segmentation->SetFileName( filename );
  m_Writer_Segmentation->Write();

}



 
/************************************
 *
 *  Execute
 *
 ***********************************/
void
DuctExtractorConsoleBase 
::Cancel( void )
{
  m_Smoother->AbortGenerateDataOn();
  m_Laplacian->AbortGenerateDataOn();
  m_RegionGrower->AbortGenerateDataOn();
}

