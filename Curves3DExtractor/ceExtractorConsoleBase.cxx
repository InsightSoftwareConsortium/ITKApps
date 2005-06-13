/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ceExtractorConsoleBase.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "ceExtractorConsoleBase.h"

 
/************************************
 *
 *  Constructor
 *
 ***********************************/
ceExtractorConsoleBase 
::ceExtractorConsoleBase()
{

  m_ImageLoaded = false;

  m_Reader     = VolumeReaderType::New();

  //  || Gradient( Image ) ||
  m_GradientMagnitude = GradientMagnitudeFilterType::New();
  m_GradientMagnitude->SetInput( m_Reader->GetOutput() );
  
  m_Hessian = HessianFilterType::New();
  m_Hessian->SetInput( m_Reader->GetOutput() );

  // Show the laplacian image. (Trace of the hessian matrix)
  m_Laplacian = HessianToLaplacianImageFilter::New();
  m_Laplacian->SetInput( m_Hessian->GetOutput() );
  
  // Compute eigen values.. order them in ascending order
  m_EigenFilter = EigenAnalysisFilterType::New();
  m_EigenFilter->SetDimension( HessianPixelType::Dimension );
  m_EigenFilter->SetInput( m_Hessian->GetOutput() );
  m_EigenFilter->OrderEigenValuesBy( 
      EigenAnalysisFilterType::FunctorType::OrderByValue );

  // Create an adaptor and plug the output to the parametric space
  m_EigenAdaptor1 = ImageAdaptorType::New();
  EigenValueAccessor< EigenValueArrayType > accessor1;
  accessor1.SetEigenIdx( 0 );
  m_EigenAdaptor1->SetImage( m_EigenFilter->GetOutput() );
  m_EigenAdaptor1->SetPixelAccessor( accessor1 );
  
  m_EigenAdaptor2 = ImageAdaptorType::New();
  EigenValueAccessor< EigenValueArrayType > accessor2;
  accessor2.SetEigenIdx( 1 );
  m_EigenAdaptor2->SetImage( m_EigenFilter->GetOutput() );
  m_EigenAdaptor2->SetPixelAccessor( accessor2 );

  m_EigenAdaptor3 = ImageAdaptorType::New();
  EigenValueAccessor< EigenValueArrayType > accessor3;
  accessor3.SetEigenIdx( 2 );
  m_EigenAdaptor3->SetImage( m_EigenFilter->GetOutput() );
  m_EigenAdaptor3->SetPixelAccessor( accessor3 );

  // m_EigenCastfilter1 will give the eigen values with the maximum eigen
  // value. m_EigenCastfilter3 will give the eigen values with the 
  // minimum eigen value.
  m_EigenCastfilter1 = CastImageFilterType::New();
  m_EigenCastfilter1->SetInput( m_EigenAdaptor3 );
  m_EigenCastfilter2 = CastImageFilterType::New();
  m_EigenCastfilter2->SetInput( m_EigenAdaptor2 );
  m_EigenCastfilter3 = CastImageFilterType::New();
  m_EigenCastfilter3->SetInput( m_EigenAdaptor1 );

  m_ParametricSpace = ParametricSpaceFilterType::New();
  // maximum eigen value
  m_ParametricSpace->SetInput( 0, m_EigenCastfilter1->GetOutput() );
  m_ParametricSpace->SetInput( 1, m_EigenCastfilter2->GetOutput() );
  // minimum eigen value
  m_ParametricSpace->SetInput( 2, m_EigenCastfilter3->GetOutput() );
  
  m_SpatialFunctionFilter = SpatialFunctionFilterType::New();
  m_SpatialFunctionFilter->SetInput(  
                              m_ParametricSpace->GetOutput() );
  
  m_SpatialFunctionControl = SpatialFunctionControlType::New();
  m_SpatialFunctionControl->SetSpatialFunction( 
                  m_SpatialFunctionFilter->GetSpatialFunction() );

#ifdef FRUSTUM_FUNCTION
  // We must choose samples in the parametric space 
  //  such that lambda_2 =~ lambda_3   and  lambda_1 =~ 0 
  m_SpatialFunctionControl->SetAngleZ( -45.0f );  
  m_SpatialFunctionControl->SetApertureAngleX( 14.0f );
  m_SpatialFunctionControl->SetApertureAngleY(  19.0f );
  m_SpatialFunctionControl->SetTopPlane( 53.0f );
  m_SpatialFunctionControl->SetBottomPlane( 6.0f );
  m_SpatialFunctionControl->SetApex( 0.0f, 0.0f, 0.0f );
  m_SpatialFunctionControl->SetRotationPlane( 
          SpatialFunctionControlType::RotateInYZPlane ); 
#endif


#ifdef SPHERE_FUNCTION
  m_SpatialFunctionControl->SetRadius( 1.0f );
#endif
  
  m_InverseParametricFilter = InverseParametricFilterType::New();
  m_InverseParametricFilter->SetInput( 
      m_SpatialFunctionFilter->GetOutput() );

  // Generate an image from the extracted points for an overlay display
  m_PointSetToImageFilter = PointSetToImageFilterType::New();
  m_PointSetToImageFilter->SetInput( 
          m_InverseParametricFilter->GetOutput() );

  m_OverlayResampleFilter = OverlayImageResampleFilterType::New();
  m_OverlayResampleFilter->SetInput( m_PointSetToImageFilter->GetOutput() );
  
  // Output of the ThresholdImageFilter will be used as the 
  // overlay image of extracted points
  m_ThresholdImageFilter     = ThresholdImageFilterType::New();
  m_ThresholdImageFilter->SetLowerThreshold( 0.0001 ); // Get all non-zero pixels
  m_ThresholdImageFilter->SetUpperThreshold( itk::NumericTraits< 
      ThresholdImageFilterType::OutputPixelType >::max() );
  m_ThresholdImageFilter->SetOutsideValue( 0 );
  m_ThresholdImageFilter->SetInsideValue( 255 );
  m_ThresholdImageFilter->SetInput( m_OverlayResampleFilter->GetOutput() );

#ifdef INTERMEDIATE_OUTPUTS
  m_EigenValueWriter = EigenValueWriterType::New();
#endif
}




/************************************
 *
 *  Destructor
 *
 ***********************************/
ceExtractorConsoleBase 
::~ceExtractorConsoleBase()
{

}



 
/************************************
 *
 *  Load
 *
 ***********************************/
void
ceExtractorConsoleBase 
::Load( const char * filename )
{

  if( !filename )
  {
    return;
  }

  m_Reader->SetFileName( filename );
  m_Reader->Update();

  InputImageType::Pointer inputImage = m_Reader->GetOutput();

  inputImage->SetRequestedRegion( 
      inputImage->GetLargestPossibleRegion() );

  m_ImageLoaded = true;

}


 
/************************************
 *
 *  Show Progress
 *
 ***********************************/
void
ceExtractorConsoleBase 
::ShowProgress( float )
{

}


 
/************************************
 *
 *  Show Status
 *
 ***********************************/
void
ceExtractorConsoleBase 
::ShowStatus( const char * )
{

}




 
/************************************
 *
 *  Set Sigma
 *
 ***********************************/
void
ceExtractorConsoleBase 
::SetSigma( RealType value )
{
  m_GradientMagnitude->SetSigma( value );
  m_Hessian->SetSigma( value );
}




 
/************************************
 *
 *  Execute
 *
 ***********************************/
void
ceExtractorConsoleBase 
::Execute( void )
{

  if( ! (m_ImageLoaded) ) 
  {
    ShowStatus("Please load an image first");
    return;
  }
  
  m_EigenFilter->UpdateLargestPossibleRegion();

#ifdef INTERMEDIATE_OUTPUTS
  m_EigenValueWriter->SetInput( m_EigenCastfilter1->GetOutput() );
  m_EigenValueWriter->SetFileName( "EigenValueImage1.mhd");
  m_EigenValueWriter->Update();
  m_EigenValueWriter->SetInput( m_EigenCastfilter2->GetOutput() );
  m_EigenValueWriter->SetFileName( "EigenValueImage2.mhd");
  m_EigenValueWriter->Update();
  m_EigenValueWriter->SetInput( m_EigenCastfilter3->GetOutput() );
  m_EigenValueWriter->SetFileName( "EigenValueImage3.mhd");
  m_EigenValueWriter->Update();
#endif
  
  m_ParametricSpace->Update();

}



 
/************************************
 *
 *  Show Spatial Function Control
 *
 ***********************************/
void
ceExtractorConsoleBase 
::ShowSpatialFunctionControl( void )
{
  m_SpatialFunctionControl->Show();
}


 
/************************************
 *
 *  Hide Spatial Function Control
 *
 ***********************************/
void
ceExtractorConsoleBase 
::HideSpatialFunctionControl( void )
{
  m_SpatialFunctionControl->Hide();
}


