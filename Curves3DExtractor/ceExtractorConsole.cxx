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

  m_Viewer_Smoothed.SetLabel( "Smoothed" );
  
  m_Viewer_Gradient_Modulus.SetLabel( "Gradient Modulus" );

  m_Viewer_Extracted_Points.SetLabel("Points of Extracted Curves");

  m_Viewer_Gradient_On_EigenVector.SetLabel( "Gradient Projected on EigenVector" );

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
  progressSlider->Observe( m_H1x.GetPointer() );
  progressSlider->Observe( m_H1y.GetPointer() );
  progressSlider->Observe( m_H1z.GetPointer() );
  progressSlider->Observe( m_H2x.GetPointer() );
  progressSlider->Observe( m_H2y.GetPointer() );
  progressSlider->Observe( m_H2z.GetPointer() );
  progressSlider->Observe( m_Hx.GetPointer() );
  progressSlider->Observe( m_Hy.GetPointer() );
  progressSlider->Observe( m_Hz.GetPointer() );
  progressSlider->Observe( m_Smooth.GetPointer() );
  progressSlider->Observe( m_Hessian.GetPointer() );
  progressSlider->Observe( m_Add.GetPointer() );
  progressSlider->Observe( m_Modulus.GetPointer() );
  progressSlider->Observe( m_Gradient.GetPointer() );
  progressSlider->Observe( m_ScalarProduct.GetPointer() );
  progressSlider->Observe( m_ParametricSpace.GetPointer() );
  progressSlider->Observe( m_SpatialFunctionFilter.GetPointer() );
  progressSlider->Observe( m_InverseParametricFilter.GetPointer() );

  loadButton->Observe( m_Reader.GetPointer() );
  inputButton->Observe( m_Reader.GetPointer() );
  smoothButton->Observe( m_Smooth.GetPointer() );
  gradientButton->Observe( m_Gradient.GetPointer() );
  hessianButton->Observe( m_Hessian.GetPointer() );
  modulusButton->Observe( m_Modulus.GetPointer() );
  gradientOnEigenVectorButton->Observe( m_ScalarProduct.GetPointer() );
  parametricSpaceButton->Observe( m_ParametricSpace.GetPointer() );
  extractedParametricPointsButton->Observe( m_SpatialFunctionFilter.GetPointer() );
  curve3DPointsButton->Observe( m_InverseParametricFilter.GetPointer() );

  m_Reader->AddObserver( itk::ModifiedEvent(), smoothButton->GetRedrawCommand() );
  m_Reader->AddObserver( itk::ModifiedEvent(), hessianButton->GetRedrawCommand() );
  m_Reader->AddObserver( itk::ModifiedEvent(), modulusButton->GetRedrawCommand() );
  m_Reader->AddObserver( itk::ModifiedEvent(), tubenessButton->GetRedrawCommand() );
  m_Reader->AddObserver( itk::ModifiedEvent(), gradientOnEigenVectorButton->GetRedrawCommand() );
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

  const char * filename = fl_file_chooser("Image filename","*.*","latmag0Acor.mha");
  if( !filename )
  {
    return;
  }

  this->Load( filename );

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
  m_Viewer_Smoothed.Hide();
  m_Viewer_Laplacian.Hide();
  m_Viewer_Gradient_Modulus.Hide();
  m_Viewer_Gradient_On_EigenVector.Hide();
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
 *  Show Smoothed XY
 *
 ***********************************/
void
ceExtractorConsole
::ShowSmoothed( void )
{

  m_Smooth->Update();
  m_Viewer_Smoothed.SetImage( m_Smooth->GetOutput() );  
  m_Viewer_Smoothed.Show();

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

  m_Gradient->Update();

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

  std::cout << "ComputeHessianEigenAnalysis" << std::endl;
  m_Hessian->Update();

  HessianImageType::ConstPointer hessian = m_Hessian->GetOutput();

  typedef itk::ImageRegionConstIterator< HessianImageType > HessianIterator;
  
  HessianIterator itr( hessian, hessian->GetBufferedRegion() );
  itr.GoToBegin();

  typedef HessianPixelType HessianMatrixType;
  HessianMatrixType matrix = itr.Get();

  typedef HessianMatrixType::EigenValuesArrayType     EigenValuesArrayType;
  typedef HessianMatrixType::EigenVectorsMatrixType   EigenVectorsMatrixType;
  
  EigenValuesArrayType    eigenValues;
  EigenVectorsMatrixType  eigenVectors;
  
  matrix.ComputeEigenAnalysis( eigenValues, eigenVectors );

  std::cout << "Hessian Matrix = " << std::endl;
  std::cout << matrix << std::endl;

  std::cout << "Eigen Values = " << std::endl;
  std::cout << eigenValues << std::endl;

  std::cout << "Eigen Vectors = " << std::endl;
  std::cout << eigenVectors << std::endl;

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

  m_Add->Update();
  m_Viewer_Laplacian.SetImage( m_Add->GetOutput() );  
  m_Viewer_Laplacian.Show();

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

  m_Modulus->Update();
  m_Viewer_Gradient_Modulus.SetImage( m_Modulus->GetOutput() );  
  m_Viewer_Gradient_Modulus.Show();

}



 
 
/************************************************
 *
 *  Show Gradient projected on max EigenVector
 *
 ***********************************************/
void
ceExtractorConsole
::ShowGradientOnEigenVector( void )
{

  m_ScalarProduct->Update(); 
  m_Viewer_Gradient_On_EigenVector.SetImage( m_ScalarProduct->GetOutput() );  
  m_Viewer_Gradient_On_EigenVector.Show();

}



 
/*******************************************
 *
 *  Extracted Parametric Points
 *
 ******************************************/
void
ceExtractorConsole
::ShowExtractedParametricPoints( void )
{

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

  m_Viewer_Extracted_Points.SetImage( m_Reader->GetOutput() );  
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

  m_SpatialFunctionFilter->Update(); 

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


  this->ShowStatus("Filtering Image with a Gaussian...");

  try 
    {
    ceExtractorConsoleBase::Execute();
    }
  catch ( itk::ExceptionObject & expt )
    {
    fl_alert( expt.GetDescription() );
    }


  this->ShowStatus("Filtering done ");
  
}











