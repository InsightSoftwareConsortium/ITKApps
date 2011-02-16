/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    CannySegmentationLevelSet.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <CannySegmentationLevelSet.h>
#include <FL/Fl_File_Chooser.H>





/************************************
 *
 *  Constructor
 *
 ***********************************/
CannySegmentationLevelSet
::CannySegmentationLevelSet()
{

  m_InputImageViewer.SetLabel("Input Image");

  m_ThresholdedImageViewer.SetLabel("Thresholded Image");

  m_ZeroSetImageViewer.SetLabel("Fast Marching Output");

  m_FastMarchingImageViewer.SetLabel("Zero Set Image");

  m_GradientMagnitudeImageViewer.SetLabel("Gradient Magnitude Image");

  m_EdgePotentialImageViewer.SetLabel("Edge Potential Image");

  m_InputImageViewer.ClickSelectCallBack( ClickSelectCallback, (void *)this);

  // Initialize ITK filter with GUI values
  m_SigmoidFilter->SetAlpha( sigmoidAlphaValueInput->value() );
  m_SigmoidFilter->SetBeta(  sigmoidBetaValueInput->value()  );

  this->SetZeroSetValue( zeroSetValueInput->value() );
  m_CannyFilter->SetNumberOfIterations( 
        static_cast<unsigned int>( cannySegmentationIterationsValueInput->value() ) );

  m_CannyFilter->SetMaximumRMSError( 
               cannySegmentationRMSErrorValueInput->value() );

  m_CannyFilter->SetCurvatureScaling(
               cannySegmentationCurvatureScalingValueInput->value() );

  m_CannyFilter->SetPropagationScaling(
               cannySegmentationPropagationScalingValueInput->value() );

  m_CannyFilter->SetAdvectionScaling(
               cannySegmentationAdvectionScalingValueInput->value() );

  m_CannyFilter->SetThreshold(
               cannySegmentationStrenghtThresholdValueInput->value() );

  m_CannyFilter->SetVariance( 
               cannySegmentationVarianceValueInput->value() );

  m_DerivativeFilter->SetSigma( sigmaValueInput->value() );

  m_FastMarchingFilter->SetStoppingValue( fastMarchingStoppingValueInput->value() );

  m_OutputLevelSetViewer.SetLabel("Output Level Set");

  m_OutputCannyEdgesViewer.SetLabel("Canny Edges");

  // Connect Observers in the GUI 
  inputImageButton->Observe( m_ImageReader.GetPointer() );
  thresholdedImageButton->Observe( m_ThresholdFilter.GetPointer() );
  outputLevelSetButton->Observe( m_CannyFilter.GetPointer() );
  gradientMagnitudeButton->Observe( m_DerivativeFilter.GetPointer() );
  edgePotentialButton->Observe( m_SigmoidFilter.GetPointer() );
  fastMarchingResultButton->Observe( m_FastMarchingFilter.GetPointer() );
  outputCannyEdgesButton->Observe( m_CannyFilter.GetPointer() );
  contourExportVTKButton->Observe( m_CannyFilter.GetPointer() );

  progressSlider->Observe( m_CastImageFilter.GetPointer() );
  progressSlider->Observe( m_DerivativeFilter.GetPointer() );
  progressSlider->Observe( m_ThresholdFilter.GetPointer() );
  progressSlider->Observe( m_SigmoidFilter.GetPointer() );
  progressSlider->Observe( m_ImageReader.GetPointer() );
  progressSlider->Observe( m_CannyFilter.GetPointer() );
  progressSlider->Observe( m_FastMarchingFilter.GetPointer() );
  progressSlider->Observe( m_InputThresholdFilter.GetPointer() );
  

}



/************************************
 *
 *  Destructor
 *
 ***********************************/
CannySegmentationLevelSet
::~CannySegmentationLevelSet()
{

}




/************************************
 *
 * Show main console
 *
 ***********************************/
void
CannySegmentationLevelSet
::ShowConsole(void)
{
  consoleWindow->show();
}




/********************************************
 *
 * Quit : requires to hide all fltk windows
 *
 *******************************************/
void
CannySegmentationLevelSet
::Quit(void)
{
  m_InputImageViewer.Hide();
  m_ThresholdedImageViewer.Hide();
  m_EdgePotentialImageViewer.Hide();
  m_GradientMagnitudeImageViewer.Hide();
  m_OutputLevelSetViewer.Hide();
  m_ZeroSetImageViewer.Hide();
  m_FastMarchingImageViewer.Hide();
  m_OutputCannyEdgesViewer.Hide();
  
  consoleWindow->hide();
}






 
/************************************
 *
 *  Load Input Image
 *
 ***********************************/
void
CannySegmentationLevelSet
::LoadInputImage( void )
{

  const char * filename = fl_file_chooser("Input Image filename","*.*","");
  if( !filename )
  {
    return;
  }

  this->ShowStatus("Loading input image file...");
  
  try 
  {
    CannySegmentationLevelSetBase::LoadInputImage( filename );
  }
  catch( itk::ExceptionObject & excp ) 
  {
    fl_alert( excp.GetDescription() );
    controlsGroup->deactivate();
    return;
  }
  catch( ... ) 
  {
    this->ShowStatus("Problems reading file format");
    controlsGroup->deactivate();
    return;
  }

  this->ShowStatus("Input Image Loaded");

  controlsGroup->activate();

}


 
/****************************************************************
 *
 *  Export contour representing the Iso-value of the LevelSet
 *
 ***************************************************************/
void
CannySegmentationLevelSet
::ExportContour( void )
{
  const char * filename = fl_file_chooser("Contour filename","*.vtk","");
  if( !filename )
    {
    return;
    }
  m_PolyDataWriter->SetFileName( filename );
  m_PolyDataWriter->Write();
}

 
/************************************
 *
 *  Show Status
 *
 ***********************************/
void
CannySegmentationLevelSet
::ShowStatus( const char * message )
{
  statusTextOutput->value( message );
  Fl::check();
}




 
/************************************
 *
 *  Show Input Image
 *
 ***********************************/
void
CannySegmentationLevelSet
::ShowInputImage( void )
{

  if( !m_InputImageIsLoaded )
    {
    return;
    }

  m_CastImageFilter->Update();
  m_InputImageViewer.SetImage( m_CastImageFilter->GetOutput() );  
  m_InputImageViewer.SetOverlay( m_SeedImage );
  m_InputImageViewer.Show();

}




 
/************************************
 *
 *  Show LevelSet Output
 *
 ***********************************/
void
CannySegmentationLevelSet
::ShowOutputLevelSet( void )
{

  if( !m_InputImageIsLoaded )
    {
    return;
    }
  this->RunCanny();
  m_OutputLevelSetViewer.SetImage( m_CannyFilter->GetOutput() );  
  m_OutputLevelSetViewer.Show();

}


  

 
/*******************************************************
 *
 *  Show CannyEdges Output overlaid to input image 
 *
 ******************************************************/
void
CannySegmentationLevelSet
::ShowOutputCannyEdges( void )
{

  if( !m_InputImageIsLoaded )
    {
    return;
    }
  this->RunCanny();
  m_OutputCannyEdgesViewer.SetImage( m_CastImageFilter->GetOutput() );
  m_OutputCannyEdgesViewer.SetOverlay( m_CannyEdgesThresholdFilter->GetOutput() );  
  m_OutputCannyEdgesViewer.Show();
  m_OutputCannyEdgesViewer.SetOverlayOpacity( 1.0 );

}




/************************************
 *
 *  Show Gradient Magnitude
 *
 ***********************************/
void
CannySegmentationLevelSet
::ShowGradientMagnitudeImage( void )
{

  if( !m_InputImageIsLoaded )
    {
    return;
    }
  this->ComputeGradientMagnitude();
  m_GradientMagnitudeImageViewer.SetImage( m_DerivativeFilter->GetOutput() );  
  m_GradientMagnitudeImageViewer.Show();

}



 
/************************************
 *
 *  Show The Edge Potential Map
 *
 ***********************************/
void
CannySegmentationLevelSet
::ShowEdgePotentialImage( void )
{

  if( !m_InputImageIsLoaded )
    {
    return;
    }
  this->ComputeEdgePotential();
  m_EdgePotentialImageViewer.SetImage( m_SigmoidFilter->GetOutput() );  
  m_EdgePotentialImageViewer.Show();

}





 
/************************************
 *
 *  Show Thresholded Image
 *
 ***********************************/
void
CannySegmentationLevelSet
::ShowThresholdedImage( void )
{
  m_CastImageFilter->Update();
  m_ThresholdFilter->Update();
  m_ThresholdedImageViewer.SetImage( m_CastImageFilter->GetOutput() );
  m_ThresholdedImageViewer.SetOverlay( m_ThresholdFilter->GetOutput() );  
  m_ThresholdedImageViewer.Show();
  m_ThresholdedImageViewer.SetOverlayOpacity( 0.5 );

}




 
/************************************
 *
 *  Show Fast Marching Result Image
 *
 ***********************************/
void
CannySegmentationLevelSet
::ShowFastMarchingResultImage( void )
{
  m_CastImageFilter->Update();
  m_InputThresholdFilter->Update();
  m_FastMarchingImageViewer.SetImage( m_CastImageFilter->GetOutput() );
  m_FastMarchingImageViewer.SetOverlay( m_InputThresholdFilter->GetOutput() );
  m_FastMarchingImageViewer.Show();
  m_FastMarchingImageViewer.SetOverlayOpacity( 0.5 );
}





 
/*****************************************
 *
 *  Callback for Selecting a seed point
 *
 *****************************************/
void
CannySegmentationLevelSet
::ClickSelectCallback(float x, float y, float z, float value, void * args )
{

  CannySegmentationLevelSet * self = 
     static_cast<CannySegmentationLevelSet *>( args );

  self->SelectSeedPoint( x, y, z );

}



 
/*****************************************
 *
 *  Callback for Selecting a seed point
 *
 *****************************************/
void
CannySegmentationLevelSet
::SelectSeedPoint(float x, float y, float z)
{

  typedef SeedImageType::IndexType IndexType;
  IndexType seed;
  seed[0] = static_cast<IndexType::IndexValueType>( x );
  seed[1] = static_cast<IndexType::IndexValueType>( y );
  seed[2] = static_cast<IndexType::IndexValueType>( z );

  CannySegmentationLevelSetBase::AddSeed( seed );

  m_InputImageViewer.Update();

}


  
  

  
/*****************************************
 *
 *  Clear the seed used to generate the 
 *  initial level set front
 *
 *****************************************/
void
CannySegmentationLevelSet
::ClearSeeds()
{
  this->CannySegmentationLevelSetBase::ClearSeeds();
  m_ZeroSetImageViewer.Update();
  m_InputImageViewer.Update();
  Fl::check();
}
  
 


/*  Finaly the main() that will instantiate the application  */
int main()
{

  try 
    {
    CannySegmentationLevelSet * console = new CannySegmentationLevelSet();
    console->ShowConsole();
    Fl::run();
    delete console;
    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << "ITK exception caught in main" << std::endl;
    std::cerr << e << std::endl;
    }
  catch( std::exception & e )
    {
    std::cerr << "STD exception caught in main" << std::endl;
    std::cerr << e.what() << std::endl;
    }
  catch( ... )
    {
    std::cerr << "unknown exception caught in main" << std::endl;
    }


  return 0;

}



