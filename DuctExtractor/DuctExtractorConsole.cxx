/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DuctExtractorConsole.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "DuctExtractorConsole.h"
#include <FL/fl_file_chooser.H>
#include "itkExceptionObject.h"
 


/************************************
 *
 *  Constructor
 *
 ***********************************/
DuctExtractorConsole
::DuctExtractorConsole()
{
 

  m_Viewer_Input = new InputImageViewerType;

  m_Viewer_Laplacian = new ImageViewerType;

  m_Viewer_Smoothed  = new ImageViewerType;

  m_Viewer_Region_Growth  = new InputImageViewerType;

  m_Viewer_Input->SetLabel( "Input Image" );

  m_Viewer_Laplacian->SetLabel( "Laplacian" );
  
  m_Viewer_Smoothed->SetLabel( "Smoothed" );

  m_Viewer_Region_Growth->SetLabel( "Region Growth" );

  
  m_Viewer_Input->ClickSelectCallBack( ClickSelectCallback, (void *)this);

  
  progressSlider->Observe( m_Smoother.GetPointer() );
  progressSlider->Observe( m_Laplacian.GetPointer() );
  progressSlider->Observe( m_RegionGrower.GetPointer() );
                              
  loadButton->Observe( m_Reader.GetPointer() );
  inputButton->Observe( m_Reader.GetPointer() );

  laplacianButton->Observe( m_Laplacian.GetPointer() );
  smoothedButton->Observe( m_Smoother.GetPointer() );
  regionGrowthButton->Observe( m_RegionGrower.GetPointer() );

  m_Reader->AddObserver( itk::ModifiedEvent(), laplacianButton->GetRedrawCommand());
  m_Reader->AddObserver( itk::ModifiedEvent(), smoothedButton->GetRedrawCommand());
  m_Reader->AddObserver( itk::ModifiedEvent(), regionGrowthButton->GetRedrawCommand());

  this->ShowStatus("Let's start by loading an image...");

}




/************************************
 *
 *  Destructor
 *
 ***********************************/
DuctExtractorConsole
::~DuctExtractorConsole()
{

  if( m_Viewer_Input ) 
    {
    delete m_Viewer_Input;
    m_Viewer_Input = 0;
    }

  if( m_Viewer_Laplacian ) 
    {
    delete m_Viewer_Laplacian;
    m_Viewer_Laplacian = 0;
    }

  if( m_Viewer_Smoothed ) 
    {
    delete m_Viewer_Smoothed;
    m_Viewer_Smoothed = 0;
    }

  if( m_Viewer_Region_Growth ) 
    {
    delete m_Viewer_Region_Growth;
    m_Viewer_Region_Growth = 0;
    }


}



 
/************************************
 *
 *  Load
 *
 ***********************************/
void
DuctExtractorConsole
::Load( void )
{

  const char * filename = fl_file_chooser("Image filename","*.*","");
  if( !filename )
  {
    return;
  }

  this->ShowStatus("Loading image file...");
  
  try 
    {
    DuctExtractorConsoleBase::Load( filename );
    }
  catch( ... ) 
    {
    this->ShowStatus("Problems reading file format");
    controlsGroup->deactivate();
    return;
    }


  this->ShowStatus("File Loaded");

  controlsGroup->activate();


}

   


/************************************
 *
 *  Show
 *
 ***********************************/
void
DuctExtractorConsole
::Show( void )
{
  consoleWindow->show();
}





/************************************
 *
 *  Hide
 *
 ***********************************/
void
DuctExtractorConsole
::Hide( void )
{
  consoleWindow->hide();
  m_Viewer_Input->Hide();
  m_Viewer_Laplacian->Hide();
  m_Viewer_Smoothed->Hide();
  m_Viewer_Region_Growth->Hide();
}





/************************************
 *
 *  Quit
 *
 ***********************************/
void
DuctExtractorConsole
::Quit( void )
{
  Hide();
}




 
/************************************
 *
 *  Show Status
 *
 ***********************************/
void
DuctExtractorConsole
::ShowStatus( const char * message )
{
  DuctExtractorConsoleBase::ShowStatus( message );
  statusTextOutput->value( message );
  Fl::check();
}




 
/************************************
 *
 *  Show Input Image
 *
 ***********************************/
void
DuctExtractorConsole
::ShowInput( void )
{

  if( ! (m_ImageFileNameAvailable) ) 
    {
    this->ShowStatus("Please provide an image filename first");
    return;
    }

  m_Reader->Update();
  m_Viewer_Input->SetImage( m_Reader->GetOutput() );  
  m_Viewer_Input->Show();

}


 
/************************************
 *
 *  Show Laplacian
 *
 ***********************************/
void
DuctExtractorConsole
::ShowLaplacian( void )
{

  this->ShowStatus("Computing Laplacian...");
  m_Laplacian->UpdateLargestPossibleRegion();
  m_Viewer_Laplacian->SetImage( m_Laplacian->GetOutput() );  
  m_Viewer_Laplacian->Show();
  this->ShowStatus("Laplacian done");

}



 
/************************************
 *
 *  Show Gradient Smoothed
 *
 ***********************************/
void
DuctExtractorConsole
::ShowSmoothed( void )
{

  this->ShowStatus("Computing Smoothing...");
  m_Smoother->UpdateLargestPossibleRegion();
  m_Viewer_Smoothed->SetImage( m_Smoother->GetOutput() );  
  m_Viewer_Smoothed->Show();
  this->ShowStatus("Smoothing done");

}





 
/************************************
 *
 *  Show Gradient Region Growth
 *
 ***********************************/
void
DuctExtractorConsole
::ShowRegionGrowth( void )
{

  this->ShowStatus("Computing Region Growth...");
  m_RegionGrower->UpdateLargestPossibleRegion();
  m_Viewer_Region_Growth->SetImage( m_Reader->GetOutput() );  
  m_Viewer_Region_Growth->SetOverlay( m_RegionGrower->GetOutput() );  
  m_Viewer_Region_Growth->Show();
  this->ShowStatus("Region growth done");
}





 
/************************************
 *
 *  Execute
 *
 ***********************************/
void
DuctExtractorConsole
::Execute( void )
{

  if( ! (m_ImageFileNameAvailable) ) 
  {
    this->ShowStatus("Please provide an image filename first");
    return;
  }


  this->ShowStatus("Starting process...");

  try
    {
    DuctExtractorConsoleBase::Execute();
    }
  catch( itk::ProcessAborted  & )
    {
    this->ShowStatus("Process cancelled by the User !");
    return;
    }

  this->ShowStatus("Filtering done ");
  
}






/************************************
 *
 *  Write the Segmentation
 *
 ***********************************/
void
DuctExtractorConsole
::WriteSegmentation(void)
{

  const char * filename = fl_file_chooser("Segmentation filename","*.*","");
  if( !filename )
  {
    return;
  }

  this->ShowStatus("Writing Segmentation ...");
  DuctExtractorConsoleBase::WriteSegmentation( filename );
  this->ShowStatus("Segmentation written");

}


 
/*****************************************
 *
 *  Callback for Selecting a seed point
 *
 *****************************************/
void
DuctExtractorConsole
::ClickSelectCallback(float x, float y, float z, float value, void * args )
{

  DuctExtractorConsole * self = 
     static_cast<DuctExtractorConsole *>( args );

  self->SelectSeedPoint( x, y, z );

}



 
/*****************************************
 *
 *  Callback for Selecting a seed point
 *
 *****************************************/
void
DuctExtractorConsole
::SelectSeedPoint(float x, float y, float z)
{
  typedef RegionGrowthFilterType::IndexType IndexType;

  IndexType seed;

  seed[0] = static_cast<IndexType::IndexValueType>( x );
  seed[1] = static_cast<IndexType::IndexValueType>( y );
  seed[2] = static_cast<IndexType::IndexValueType>( z );

  m_RegionGrower->AddSeed( seed );

}




