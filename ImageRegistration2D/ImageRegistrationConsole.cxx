/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageRegistrationConsole.cxx
  Language:  C++

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

//
// 2D image registration
//

#include <ImageRegistrationConsole.h>
#include <FL/Fl_File_Chooser.H>

#include "time.h"
#include <iostream>

/************************************
 *
 *  Constructor
 *
 ***********************************/
ImageRegistrationConsole
::ImageRegistrationConsole()
{
 
  m_FixedImageViewer        = ImageViewerType::New();
  m_MovingImageViewer        = ImageViewerType::New();
  m_InputMovingImageViewer      = InputImageViewerType::New();
  m_NormalizedInputMovingImageViewer = ImageViewerType::New();
  m_TransformedMovingImageViewer  = ImageViewerType::New();
  m_InputFixedImageViewer      = InputImageViewerType::New();
  m_RegisteredMovingImageViewer     = InputImageViewerType::New();
  m_MixedChannelViewer              = MixedChannelViewerType::New() ;

  m_MovingImageViewer->SetLabel( "Moving Image" );
  m_FixedImageViewer->SetLabel( "Fixed Image" );

  m_InputMovingImageViewer->SetLabel( "Input Moving Image" );
  m_InputFixedImageViewer->SetLabel( "Input Fixed Image" );
  m_NormalizedInputMovingImageViewer->SetLabel( "Normalized Input Moving Image" );
  m_TransformedMovingImageViewer->SetLabel( "Transformed Moving Image" );

  m_RegisteredMovingImageViewer->SetLabel( "Registered Moving Image" );
  m_MixedChannelViewer->SetLabel( "Mixed Channel View" );

  
  progressSlider->Observe( m_ResampleMovingImageFilter.GetPointer() );

  inputFixedImageButton->Observe(   m_FixedImageReader.GetPointer()  );
  loadFixedImageButton->Observe(    m_FixedImageReader.GetPointer()  );
  inputMovingImageButton->Observe(  m_MovingImageReader.GetPointer() );
  loadMovingImageButton->Observe(   m_MovingImageReader.GetPointer() );

  this->ShowStatus("Let's start by loading an image...");

}




/************************************
 *
 *  Destructor
 *
 ***********************************/
ImageRegistrationConsole
::~ImageRegistrationConsole()
{

}



 
/************************************
 *
 *  Load Moving Image
 *
 ***********************************/
void
ImageRegistrationConsole
::LoadMovingImage( void )
{

  const char * filename = fl_file_chooser("Moving Image Filename","*.*","");
  if( !filename )
  {
    return;
  }

  this->ShowStatus("Loading moving image file...");
  
  try 
  {
    ImageRegistrationConsoleBase::LoadMovingImage( filename );
  }
  catch( ... ) 
  {
    this->ShowStatus("Problems reading file format");
    controlsGroup->deactivate();
    return;
  }


  this->ShowStatus("Moving Image Loaded");

  controlsGroup->activate();

}


 
/************************************
 *
 *  Load Fixed Image
 *
 ***********************************/
void
ImageRegistrationConsole
::LoadFixedImage( void )
{

  const char * filename = fl_file_chooser("Fixed Image Filename","*.*","");
  if( !filename )
  {
    return;
  }

  this->ShowStatus("Loading fixed image file...");
  
  try 
  {
    ImageRegistrationConsoleBase::LoadFixedImage( filename );
  }
  catch( ... ) 
  {
    this->ShowStatus("Problems reading file format");
    controlsGroup->deactivate();
    return;
  }


  this->ShowStatus("Fixed Image Loaded");

  controlsGroup->activate();

}



/************************************
 *
 *  Save Registered Image
 *
 ***********************************/
void
ImageRegistrationConsole
::SaveRegisteredImage( void )
{

  const char * filename = fl_file_chooser("Registered Image Filename","*.*","");
  if( !filename )
  {
    return;
  }

  this->ShowStatus("Saving registered image file...");
  
  try 
  {
    ImageRegistrationConsoleBase::SaveRegisteredImage( filename );
  }
  catch( ... ) 
  {
    this->ShowStatus("Problems saving file format");
    return;
  }

  this->ShowStatus("Registered Image Saved");

}


/************************************
 *
 *  Show
 *
 ***********************************/
void
ImageRegistrationConsole
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
ImageRegistrationConsole
::Hide( void )
{
  consoleWindow->hide();
  m_FixedImageViewer->Hide();
  m_MovingImageViewer->Hide();
  m_RegisteredMovingImageViewer->Hide();
  m_MixedChannelViewer->Hide();

  m_InputMovingImageViewer->Hide();
  m_NormalizedInputMovingImageViewer->Hide();
  m_TransformedMovingImageViewer->Hide();
  m_InputFixedImageViewer->Hide();

}





/************************************
 *
 *  Quit
 *
 ***********************************/
void
ImageRegistrationConsole
::Quit( void )
{
  this->Hide();
}






 
/************************************
 *
 *  Show Status
 *
 ***********************************/
void
ImageRegistrationConsole
::ShowStatus( const char * message )
{
  ImageRegistrationConsoleBase::ShowStatus( message );
  statusTextOutput->value( message );
  Fl::check();
}


/************************************
 *
 *  Show Input Fixed Image
 *
 ***********************************/
void
ImageRegistrationConsole
::ShowInputFixedImage( void )
{

  if( !m_FixedImageIsLoaded )
    {
    return;
    }

  m_InputFixedImageViewer->SetImage( m_FixedImageReader->GetOutput() );  
  m_InputFixedImageViewer->Show();
}


 
/************************************
 *
 *  Show Fixed Image
 *
 ***********************************/
void
ImageRegistrationConsole
::ShowFixedImage( void )
{

  if( !m_FixedImageIsLoaded )
    {
    return;
    }

  m_FixedImageViewer->SetImage( m_FixedImageNormalizeFilter->GetOutput() );  
  m_FixedImageViewer->Show();
}



/************************************
 *
 *  Show Input Moving Image
 *
 ***********************************/
void
ImageRegistrationConsole
::ShowInputMovingImage( void )
{

  if( !m_MovingImageIsLoaded )
    {
    return;
    }

  m_InputMovingImageViewer->SetImage( m_MovingImageReader->GetOutput() );  
  m_InputMovingImageViewer->Show();

}

/************************************
 *
 *  Show Moving Image
 *
 ***********************************/
void
ImageRegistrationConsole
::ShowMovingImage( void )
{

  if( !m_MovingImageIsLoaded )
    {
    return;
    }

  m_MovingImageViewer->SetImage( m_MovingImageNormalizeFilter->GetOutput() );
  m_MovingImageViewer->Show();

}

/************************************
 *
 *  Show Normalized Input Moving Image
 *
 ***********************************/
void
ImageRegistrationConsole
::ShowNormalizedInputMovingImage( void )
{

  if( !m_MovingImageIsLoaded )
    {
    return;
    }

  m_NormalizedInputMovingImageViewer->SetImage( m_NormalizedInputMovingImageNormalizeFilter->GetOutput() );
  m_NormalizedInputMovingImageViewer->Show();

}
 
/************************************
 *
 *  Show Transformed Moving Image
 *
 ***********************************/
void
ImageRegistrationConsole
::ShowTransformedMovingImage( void )
{

  if( !m_MovingImageIsLoaded )
    {
    return;
    }

  m_TransformedMovingImageViewer->SetImage( m_TransformedMovingImageNormalizeFilter->GetOutput() );  
  m_TransformedMovingImageViewer->Show();

}



 
/************************************
 *
 *  Show Registered Moving Image
 *
 ***********************************/
void
ImageRegistrationConsole
::ShowRegisteredMovingImage( void )
{

  if( !m_MovingImageIsLoaded )
    {
    return;
    }

  m_RegisteredMovingImageViewer->SetImage( m_ResampleMovingImageFilter->GetOutput() );
  m_RegisteredMovingImageViewer->Show();
}


/************************************
 *
 *  Show Mixed Channel Image
 *
 * Reed Channel : Fixed Image
 * Green Channel: Registered Image
 * Blue Channel:  None
 *
 ***********************************/
void
ImageRegistrationConsole
::ShowMixedChannelImage( void )
{
  if( !m_MovingImageIsLoaded )
    {
      return;
    }
  int sizeX = m_FixedImageReader->GetOutput()->GetLargestPossibleRegion().GetSize()[0] ;
  int sizeY = m_FixedImageReader->GetOutput()->GetLargestPossibleRegion().GetSize()[1] ;

  m_MixedChannelViewer->SetRedChannel( m_FixedImageReader->GetOutput()) ;
  m_MixedChannelViewer->SetGreenChannel( m_ResampleMovingImageFilter->GetOutput() ) ;


  m_MixedChannelViewer->FillChannel(2, 0, sizeX, sizeY) ;

  m_MixedChannelViewer->Show();
}



 
/************************************
 *
 *  Execute
 *
 ***********************************/
void
ImageRegistrationConsole
::Execute( void )
{

  this->ShowStatus("Registering Moving Image against Fixed Image ...");

  m_NumberOfIterations   = static_cast<unsigned int>( numberOfIterations->value() );
  m_MinimumStepLength    = minimumStepLength->value();
  m_MaximumStepLength    = maximumStepLength->value();
  m_RotationScale        = rotationScale->value();
  m_TranslationScale     = translationScale->value();
  m_ScalingScale         = scalingScale->value();


  clock_t time_begin ;
  clock_t time_end ;

  time_begin = clock() ;
  ImageRegistrationConsoleBase::Execute();
  time_end = clock() ;

  OptimizerType::ParametersType finalParameters = 
                    m_ImageRegistrationMethod->GetLastTransformParameters();

  const double finalAngle           = finalParameters[0]; //rigid2d
  const double finalRotationCenterX = finalParameters[1];
  const double finalRotationCenterY = finalParameters[2];
  const double finalTranslationX    = finalParameters[3];
  const double finalTranslationY    = finalParameters[4];
  
  
  const unsigned int numberOfIterations = m_Optimizer->GetCurrentIteration();

  const double bestValue = m_Optimizer->GetValue();

  //
  // Print out results
  //
  const double finalAngleInDegrees = finalAngle * 45.0 / atan(1.0);

  std::cout << "Result = " << std::endl;
  std::cout << " Angle (radians)   = " << finalAngle  << std::endl;
  std::cout << " Angle (degrees)   = " << finalAngleInDegrees  << std::endl;
  std::cout << " Rotation Center X      = " << finalRotationCenterX  << std::endl;
  std::cout << " Rotation Center Y      = " << finalRotationCenterY  << std::endl;
  std::cout << " Translation X = " << finalTranslationX  << std::endl;
  std::cout << " Translation Y = " << finalTranslationY  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;

  std::cout << " Registration done in " <<
      double(time_end - time_begin) / CLOCKS_PER_SEC << "seconds." 
      << std::endl ;


  this->ShowStatus("Registration done ");
  meanSquaresGroup->activate();
  mutualInformationGroup->activate();
}


/************************************
 *
 *  Select Mutual Information Metric
 *
 ***********************************/
void
ImageRegistrationConsole
::SelectMutualInformationMetric( void )
{
  
  if( !m_MovingImageIsLoaded )
    {
    return;
    }
  meanSquaresGroup->deactivate();

  m_MutualInformationNumberOfBins = 
      static_cast<unsigned int>( mutualInformationNumberOfBins->value() );

  registrationMethodButton->selection_color( FL_RED );
  registrationMethodButton->value( 1 );
  registrationMethodButton->redraw();
  
  ImageRegistrationConsoleBase::SelectMutualInformationMetric();

  registrationMethodButton->selection_color( FL_GREEN );
  registrationMethodButton->value( 1 );
  registrationMethodButton->redraw();
  

}

/************************************
 *
 *  Select Mean Squares Metric
 *
 ***********************************/
void
ImageRegistrationConsole
::SelectMeanSquaresMetric( void )
{
  
  if( !m_MovingImageIsLoaded )
    {
    return;
    }
  mutualInformationGroup->deactivate();

  registrationMethodButton->selection_color( FL_RED );
  registrationMethodButton->value( 1 );
  registrationMethodButton->redraw();
  
  ImageRegistrationConsoleBase::SelectMeanSquaresMetric();
  
  registrationMethodButton->selection_color( FL_GREEN );
  registrationMethodButton->value( 1 );
  registrationMethodButton->redraw();
  
}

/************************************
 *
 *  Generate Registered Moving Image
 *  Modify button colors and then
 *  delegate to base class
 *
 ***********************************/
void
ImageRegistrationConsole
::GenerateRegisteredMovingImage( void )
{
  
  if( !m_MovingImageIsLoaded )
    {
    return;
    }

  registeredMovingImageButton->selection_color( FL_RED );
  registeredMovingImageButton->value( 1 );
  registeredMovingImageButton->redraw();
  
  mixedChannelImageButton->selection_color( FL_RED );
  mixedChannelImageButton->value( 1 );
  mixedChannelImageButton->redraw();
  
  saveRegisteredImageButton->selection_color( FL_RED );
  saveRegisteredImageButton->value( 1 );
  saveRegisteredImageButton->redraw();

  ImageRegistrationConsoleBase::GenerateRegisteredMovingImage();
  
  registeredMovingImageButton->selection_color( FL_GREEN );
  registeredMovingImageButton->value( 1 );
  registeredMovingImageButton->redraw();

  mixedChannelImageButton->selection_color( FL_GREEN );
  mixedChannelImageButton->value( 1 );
  mixedChannelImageButton->redraw();

  saveRegisteredImageButton->selection_color( FL_GREEN );
  saveRegisteredImageButton->value( 1 );
  saveRegisteredImageButton->redraw();

}



/************************************
 *
 *  Generate Fixed Image
 *  Modify button colors and then
 *  delegate to base class
 *
 ***********************************/
void
ImageRegistrationConsole
::GenerateFixedImage( void )
{
  
  if( !m_FixedImageIsLoaded )
    {
    return;
    }

  fixedImageButton->selection_color( FL_RED );
  fixedImageButton->value( 1 );
  fixedImageButton->redraw();
  
  ImageRegistrationConsoleBase::GenerateFixedImage();
  
  fixedImageButton->selection_color( FL_GREEN );
  fixedImageButton->value( 1 );
  fixedImageButton->redraw();

}


/************************************
 *
 *  Generate Normalized Input Moving Image
 *  Modify button colors and then
 *  delegate to base class
 *
 ***********************************/
void
ImageRegistrationConsole
::GenerateNormalizedInputMovingImage( void )
{
  
  if( !m_MovingImageIsLoaded )
    {
    return;
    }

  normalizedInputMovingImageButton->selection_color( FL_RED );
  normalizedInputMovingImageButton->value( 1 );
  normalizedInputMovingImageButton->redraw();
  
    
  ImageRegistrationConsoleBase::GenerateNormalizedInputMovingImage();
  
  normalizedInputMovingImageButton->selection_color( FL_GREEN );
  normalizedInputMovingImageButton->value( 1 );
  normalizedInputMovingImageButton->redraw();

}

/************************************
 *
 *  Generate Transformed Moving Image
 *  Modify button colors and then
 *  delegate to base class
 *
 ***********************************/
void
ImageRegistrationConsole
::GenerateTransformedMovingImage( void )
{
  
  if( !m_MovingImageIsLoaded )
    {
    return;
    }

  transformedMovingImageButton->selection_color( FL_RED );
  transformedMovingImageButton->value( 1 );
  transformedMovingImageButton->redraw();
  
  m_Offset[0] = -movingImageTranslationX->value();
  m_Offset[1] = -movingImageTranslationY->value();
  
  m_Angle   = movingImageRotationAngle->value() * atan( 1.0 ) / 45.0 ;
  
  ImageRegistrationConsoleBase::GenerateTransformedMovingImage();
  
  transformedMovingImageButton->selection_color( FL_GREEN );
  transformedMovingImageButton->value( 1 );
  transformedMovingImageButton->redraw();

}

/************************************
 *
 *  Generate Moving Image
 *  Modify button colors and then
 *  delegate to base class
 *
 ***********************************/
void
ImageRegistrationConsole
::GenerateMovingImage( void )
{
  
  if( !m_MovingImageIsLoaded )
    {
    return;
    }

  movingImageButton->selection_color( FL_RED );
  movingImageButton->value( 1 );
  movingImageButton->redraw();
  
  m_MovingImageNoiseMean = movingImageNoiseMean->value();
  m_MovingImageNoiseStandardDeviation = movingImageNoiseStandardDeviation->value();

  ImageRegistrationConsoleBase::GenerateMovingImage();
  
  movingImageButton->selection_color( FL_GREEN );
  movingImageButton->value( 1 );
  movingImageButton->redraw();

}
