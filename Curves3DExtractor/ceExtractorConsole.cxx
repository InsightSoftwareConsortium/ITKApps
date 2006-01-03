/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ceExtractorConsole.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "ceExtractorConsole.h"
#include <FL/fl_file_chooser.H>
#include <FL/fl_ask.H>
#include "itkImageFileWriter.h"
#include "itkColorTable.h" 

#ifdef INTERMEDIATE_OUTPUTS
  #include <iostream>
#endif




/************************************
 *
 *  Constructor
 *
 ***********************************/
ceExtractorConsole
::ceExtractorConsole()
{

  m_InputViewer.SetLabel( "Input Image" );

  m_Viewer_Laplacian.SetLabel( "Laplacian" );

  m_Viewer_Gradient_Modulus.SetLabel( "Gradient Modulus" );

  m_Viewer_Lambda1.SetLabel( "Lambda_1 image (1 > 2 > 3)" );
  m_Viewer_Lambda2.SetLabel( "Lambda_2 image (1 > 2 > 3)" );
  m_Viewer_Lambda3.SetLabel( "Lambda_3 image (1 > 2 > 3)" );

  m_Viewer_Extracted_Points.SetLabel("Points of Extracted Curves");

  m_ParametricSpaceSamplesShape = PointSetShapeType::New();

  m_ParametricSpaceSamplesShape->SetPointSet( 
                          m_ParametricSpace->GetOutput() );
  
  // OpenGL display list mode
  m_ParametricSpaceSamplesShape->SetCompileMode( fltk::Shape3D::compileExecute ); 

  m_ParametricSpaceViewer.SetLabel("Parametric Space");



  m_ExtractedParametricSpaceSamplesShape = PointSetShapeType::New();

  m_ExtractedParametricSpaceSamplesShape->SetPointSet( 
                          m_SpatialFunctionFilter->GetOutput() );

  progressSlider->Observe( m_Reader.GetPointer() );
  progressSlider->Observe( m_Hessian.GetPointer() );
  progressSlider->Observe( m_EigenFilter.GetPointer() );
  progressSlider->Observe( m_Laplacian.GetPointer() );
  progressSlider->Observe( m_GradientMagnitude.GetPointer() );
  progressSlider->Observe( m_ParametricSpace.GetPointer() );
  progressSlider->Observe( m_SpatialFunctionFilter.GetPointer() );
  progressSlider->Observe( m_InverseParametricFilter.GetPointer() );


  loadButton->Observe( m_Reader.GetPointer() );
  inputButton->Observe( m_Reader.GetPointer() );
  gradientButton->Observe( m_GradientMagnitude.GetPointer() );
  modulusButton->Observe( m_GradientMagnitude.GetPointer() );
  hessianButton->Observe( m_Hessian.GetPointer() );
  laplacianButton->Observe( m_Laplacian.GetPointer() );
  lambda1Button->Observe( m_EigenCastfilter1.GetPointer() );
  lambda2Button->Observe( m_EigenCastfilter2.GetPointer() );
  lambda3Button->Observe( m_EigenCastfilter3.GetPointer() );
  parametricSpaceButton->Observe( m_ParametricSpace.GetPointer() );
  extractedParametricPointsButton->Observe( m_SpatialFunctionFilter.GetPointer() );
  curve3DPointsButton->Observe( m_InverseParametricFilter.GetPointer() );

  m_Reader->AddObserver( itk::ModifiedEvent(), modulusButton->GetRedrawCommand() );
  m_Reader->AddObserver( itk::ModifiedEvent(), hessianButton->GetRedrawCommand() );
  m_Reader->AddObserver( itk::ModifiedEvent(), laplacianButton->GetRedrawCommand() );
  m_Reader->AddObserver( itk::ModifiedEvent(), parametricSpaceButton->GetRedrawCommand() );
  m_Reader->AddObserver( itk::ModifiedEvent(), extractedParametricPointsButton->GetRedrawCommand() );
  m_Reader->AddObserver( itk::ModifiedEvent(), curve3DPointsButton->GetRedrawCommand() );


  // Register the PointSet as a Drawer in the OpenGL window
  m_ParametricSpaceViewer.GetNotifier()->AddObserver( 
                         fltk::GlDrawEvent(), 
                         m_ParametricSpaceSamplesShape->GetDrawCommand() );

  // Notify the OpenGL window when the set of points changes
  m_ParametricSpace->AddObserver( itk::EndEvent(),
                        m_ParametricSpaceViewer.GetRedrawCommand() );


  fltk::Shape3D::ColorType  parametricSpacePointsColor;
  parametricSpacePointsColor.SetRed(   0.0 );
  parametricSpacePointsColor.SetGreen( 0.0 );
  parametricSpacePointsColor.SetBlue(  0.0 );
  m_ParametricSpaceSamplesShape->SetColor( parametricSpacePointsColor );

  fltk::GlWindowInteractive::ColorType  parametricSpaceBackgroundColor;
  parametricSpaceBackgroundColor.SetRed(   1.0 );
  parametricSpaceBackgroundColor.SetGreen( 1.0 );
  parametricSpaceBackgroundColor.SetBlue(  1.0 );
  m_ParametricSpaceViewer.GetGlWindow()->SetBackground( 
                                           parametricSpaceBackgroundColor );

  m_ImageSpaceSamplesShape = ImageSpacePointSetShapeType::New();

  fltk::Shape3D::ColorType curve3DColor;
  curve3DColor.SetRed(   1.0 );
  curve3DColor.SetGreen( 0.0 );
  curve3DColor.SetBlue(  0.0 );
  m_ImageSpaceSamplesShape->SetColor( curve3DColor );

  m_ImageSpaceSamplesShape->SetPointSet(
                  m_InverseParametricFilter->GetOutput() );

  // Register the SpatialFunctionControl as a Drawer in the OpenGL window
  m_ParametricSpaceViewer.GetNotifier()->AddObserver( 
                  fltk::GlDrawEvent(), 
                  m_SpatialFunctionControl->GetDrawCommand() );

  // Notify the OpenGL window when the spatial function changes
  m_SpatialFunctionControl->AddObserver( 
                  fltk::RedrawEvent(),
                  m_ParametricSpaceViewer.GetRedrawCommand() );


  //m_Viewer_Extracted_Points.AddObserver(
  //                fltk::GlDrawEvent(), 
  //                m_ImageSpaceSamplesShape->GetDrawCommand() );


  m_ExtractedParametricSpaceSamplesShape->SetCompileMode( fltk::Shape3D::compileExecute ); 
  
  m_ExtractedParametricSpaceViewer.SetLabel("Filtered Parametric Space");
  
  
  m_SpatialFunctionControl->AddObserver( 
                  fltk::RedrawEvent(),
                  m_ExtractedParametricSpaceViewer.GetRedrawCommand() );

  m_ExtractedParametricSpaceViewer.GetNotifier()->AddObserver( 
                  fltk::GlDrawEvent(), 
                  m_ExtractedParametricSpaceSamplesShape->GetDrawCommand() );

  
  this->ShowStatus("Let's start by loading an image...");
}




/************************************
 *
 *  Destructor
 *
 ***********************************/
ceExtractorConsole
::~ceExtractorConsole()
{

}




/************************************
 *
 *  Load
 *
 ***********************************/
void
ceExtractorConsole
::Load( void )
{
  Fl_File_Chooser *fileChooser = new Fl_File_Chooser("",
       "MetaImage (*.{mhd,mha})\tVTK (*.{vtk})\tNrrd (*.{nrrd})\tAnalyze (*.{img,hdr})\tgipl (*.{gipl})", 
       Fl_File_Chooser::SINGLE, "Choose file");
  fileChooser->show();
  
  while( fileChooser->shown()) { Fl::wait(); }

  if( fileChooser->value() )
    {
    //fl_cursor(Fl_Cursor(FL_CURSOR_DEFAULT), 
    //                      (Fl_Color) 56, (Fl_Color) 255);
    this->Load( fileChooser->value() );
    }
  else
    { 
    return; 
    }
}



 
/************************************
 *
 *  Load
 *
 ***********************************/
void
ceExtractorConsole
::Load( const char * filename )
{

  this->ShowStatus("Loading image file...");
  
  try 
  {
    ceExtractorConsoleBase::Load( filename );
  }
  catch(...)
  {
    this->ShowStatus("Problems reading file format");
    controlsGroup->deactivate();
    return;
  }


  this->ShowStatus("File Loaded");

  controlsGroup->activate();

  m_InputViewer.Update();


  m_Reader->InvokeEvent( itk::EndEvent() );

}

   
/************************************
 *
 *  Show
 *
 ***********************************/
void
ceExtractorConsole
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
ceExtractorConsole
::Hide( void )
{

  consoleWindow->hide();
  m_InputViewer.Hide();
  m_Viewer_Laplacian.Hide();
  m_Viewer_Gradient_Modulus.Hide();
  m_Viewer_Extracted_Points.Hide();
  
  m_ParametricSpaceViewer.Hide();
  m_ExtractedParametricSpaceViewer.Hide();

  this->ceExtractorConsoleBase::HideSpatialFunctionControl();

}





/************************************
 *
 *  Quit
 *
 ***********************************/
void
ceExtractorConsole
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
ceExtractorConsole
::ShowStatus( const char * message )
{
  ceExtractorConsoleBase::ShowStatus( message );
  statusTextOutput->value( message );
  Fl::check();
}




 
/************************************
 *
 *  Show Input Image
 *
 ***********************************/
void
ceExtractorConsole
::ShowInput( void )
{

  if( ! (m_ImageLoaded) ) 
  {
    this->ShowStatus("Please load an image first");
    return;
  }

  m_InputViewer.SetImage( m_Reader->GetOutput() );  
  m_InputViewer.Show();

}



 
/************************************
 *
 *  Compute Hessian
 *
 ***********************************/
void
ceExtractorConsole
::ComputeHessian( void )
{

  m_Hessian->Update();

  typedef itk::ImageFileWriter< HessianImageType > HessianWriterType;
  HessianWriterType::Pointer writer = HessianWriterType::New();
  writer->SetInput( m_Hessian->GetOutput() );
  writer->SetFileName("Hessian.mhd");

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error writing hessian image" << std::endl;
    std::cerr << excp << std::endl;
    return;
    }

}



 
/************************************
 *
 *  Compute Gradient
 *
 ***********************************/
void
ceExtractorConsole
::ComputeGradient( void )
{
  m_GradientMagnitude->Update();
}



 
/************************************
 *
 *  Compute Hessian Eigen Analysis
 *
 ***********************************/
void
ceExtractorConsole
::ComputeHessianEigenAnalysis( void )
{
  m_EigenFilter->Update();
}



 
/************************************
 *
 *  Show Laplacian
 *
 ***********************************/
void
ceExtractorConsole
::ShowLaplacian( void )
{
  m_Laplacian->UpdateLargestPossibleRegion();
  m_Viewer_Laplacian.SetImage( m_Laplacian->GetOutput() );
  m_Viewer_Laplacian.Show();
}


void
ceExtractorConsole 
::ShowLambda1( void )
{
  m_EigenCastfilter1->Update();
  m_Viewer_Lambda1.SetImage( m_EigenCastfilter1->GetOutput() );
  m_Viewer_Lambda1.Show();
}

void
ceExtractorConsole 
::ShowLambda2( void )
{
  m_EigenCastfilter2->Update();
  m_Viewer_Lambda2.SetImage( m_EigenCastfilter2->GetOutput() );
  m_Viewer_Lambda2.Show();
}


void
ceExtractorConsole 
::ShowLambda3( void )
{
  m_EigenCastfilter3->Update();
  m_Viewer_Lambda3.SetImage( m_EigenCastfilter3->GetOutput() );
  m_Viewer_Lambda3.Show();
}



 
/************************************
 *
 *  Show Gradient Modulus
 *
 ***********************************/
void
ceExtractorConsole
::ShowGradientModulus( void )
{

  m_GradientMagnitude->Update();
  m_Viewer_Gradient_Modulus.SetImage( m_GradientMagnitude->GetOutput() );  
  m_Viewer_Gradient_Modulus.Show();

}



 
 
/************************************************
 *
 *  Show Gradient projected on max EigenVector
 *
 ***********************************************/
/*
void
ceExtractorConsole
::ShowGradientOnEigenVector( void )
{

  m_ScalarProduct->Update(); 
  m_Viewer_Gradient_On_EigenVector.SetImage( m_ScalarProduct->GetOutput() );  
  m_Viewer_Gradient_On_EigenVector.Show();

}
*/


 
/*******************************************
 *
 *  Extracted Parametric Points
 *
 ******************************************/
void
ceExtractorConsole
::ShowExtractedParametricPoints( void )
{

  m_SpatialFunctionFilter->Update(); 
  m_ExtractedParametricSpaceViewer.Show();
  this->ResetViewOfExtractedParametricSpace();

}

 
/*******************************************
 *
 *  Show Input Image and Extracted Points
 *
 ******************************************/
void
ceExtractorConsole
::ShowCurve3DPoints( void )
{

  if( ! (m_ImageLoaded) ) 
  {
    this->ShowStatus("Please load an image first");
    return;
  }

  m_InverseParametricFilter->Update();

#ifdef INTERMEDIATE_OUTPUTS  
  // Write the extracted curve points and their corresponding eigen values to a
  // file..
  std::ofstream ofs( "ImageSpace-EigenSpaceMap.txt", std::ios::trunc );
  MeshType::PointDataContainerPointer  pointData   = 
    m_InverseParametricFilter->GetOutput()->GetPointData();
  MeshType::PointsContainerPointer     points      = 
    m_InverseParametricFilter->GetOutput()->GetPoints();
  MeshType::PointsContainerIterator    pointIt     = points->Begin();
  MeshType::PointDataContainerIterator pointDataIt = pointData->Begin();
  while (pointIt != points->End() )
  {
    ofs << "Image space point: " << pointIt.Value()
        << " EigenValue: " << pointDataIt.Value() << std::endl;
    ++pointIt;
    ++pointDataIt;
  }
  ofs.close();
#endif
  
  m_PointSetToImageFilter->Update();

  // Resample the pointset image to have the same spacing/origin as the input image
  m_OverlayResampleFilter->SetOutputSpacing( m_Reader->GetOutput()->GetSpacing() );
  m_OverlayResampleFilter->SetOutputOrigin( m_Reader->GetOutput()->GetOrigin() );
  m_OverlayResampleFilter->SetSize( 
      m_Reader->GetOutput()->GetLargestPossibleRegion().GetSize() );
  m_OverlayResampleFilter->UpdateLargestPossibleRegion();
  
#ifdef INTERMEDIATE_OUTPUTS
  // Write the extracted curves to a file... 
  typedef itk::ImageFileWriter< PointSetImageType > OverlayWriterType;
  OverlayWriterType::Pointer overlayWriter = OverlayWriterType::New();
  overlayWriter->SetInput( m_OverlayResampleFilter->GetOutput() );
  overlayWriter->SetFileName("Overlay.mhd");
  overlayWriter->Update();
#endif

  m_ThresholdImageFilter->Update();

  m_Viewer_Extracted_Points.SetOverlayColorIndex( 0 ); // Overlay with red points

  m_Viewer_Extracted_Points.SetImage( m_Reader->GetOutput() );  
  m_Viewer_Extracted_Points.SetOverlay( m_ThresholdImageFilter->GetOutput() );  
  m_Viewer_Extracted_Points.Update();
  m_Viewer_Extracted_Points.Show();

}

 
/************************************
 *
 *  Show the Parametric Space
 *
 ***********************************/
void
ceExtractorConsole
::ShowParametricSpace( void )
{

  m_ParametricSpaceViewer.Show();
  this->ShowStatus("BLUE");
  this->ResetViewOfParametricSpace();

}



 
/****************************************
 *
 *  Reset View of the Parametric Space
 *
 ****************************************/
void
ceExtractorConsole
::ResetViewOfParametricSpace( void )
{

  m_ParametricSpace->Update(); 

  fltk::GlWindowInteractive::Point3DType center;
  center[0] =   0;
  center[1] =   0;
  center[2] =   0;

  m_ParametricSpaceViewer.GetGlWindow()->ResetViewingParameters();

  m_ParametricSpaceViewer.GetGlWindow()->SetCenter( center );
  m_ParametricSpaceViewer.GetGlWindow()->SetZoom( 50.0 );
  m_ParametricSpaceViewer.GetGlWindow()->SetAltitude( -90.0 );
  m_ParametricSpaceViewer.GetGlWindow()->SetAzimuth( 45.0 );
  
  m_ParametricSpaceViewer.GetGlWindow()->redraw();


}

 
/*************************************************
 *
 *  Reset View of the Extracted Parametric Space
 *
 *************************************************/
void
ceExtractorConsole
::ResetViewOfExtractedParametricSpace( void )
{


  fltk::GlWindowInteractive::Point3DType center;
  center[0] =   0;
  center[1] =   0;
  center[2] =   0;

  m_ExtractedParametricSpaceViewer.GetGlWindow()->ResetViewingParameters();
  
  m_ExtractedParametricSpaceViewer.GetGlWindow()->SetZoom( 50.0 );
  m_ExtractedParametricSpaceViewer.GetGlWindow()->SetAltitude( -90.0 );
  m_ExtractedParametricSpaceViewer.GetGlWindow()->SetAzimuth( 45.0 );
  m_ExtractedParametricSpaceViewer.GetGlWindow()->SetCenter( center );

  m_ExtractedParametricSpaceViewer.GetGlWindow()->redraw();

}
 
 
/************************************
 *
 *  Set Sigma
 *
 ***********************************/
void
ceExtractorConsole
::SetSigma( RealType value )
{
  sigmaCounter->value( value );
  this->ceExtractorConsoleBase::SetSigma( value );
}



  
/************************************
 *
 *  Execute
 *
 ***********************************/
void
ceExtractorConsole
::Execute( void )
{

  if( ! (m_ImageLoaded) ) 
  {
    this->ShowStatus("Please load an image first");
    return;
  }


  this->ShowStatus("Executing.....");

  try 
    {
    ceExtractorConsoleBase::Execute();
    }
  catch ( itk::ExceptionObject & expt )
    {
    fl_alert( expt.GetDescription() );
    }


  this->ShowStatus("Done ");
  
}











