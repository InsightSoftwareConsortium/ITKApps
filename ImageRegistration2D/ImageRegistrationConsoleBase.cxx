/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageRegistrationConsoleBase.cxx
  Language:  C++

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <ImageRegistrationConsoleBase.h>
#include <FL/fl_ask.H>


/************************************
 *
 *  Constructor
 *
 ***********************************/
ImageRegistrationConsoleBase 
::ImageRegistrationConsoleBase()
{
// here is were the pipelining takes place i.e., here is were we set input and outputs

  m_FixedImageReader                  = FixedImageReaderType::New();

  m_MovingImageReader                 = MovingImageReaderType::New();

  m_FixedImageItkToVtkConnector      = ItkToVtkConnectorType::New();

  m_MovingImageItkToVtkConnector    = ItkToVtkConnectorType::New();

  m_FixedImageVtkToItkConnector      = VtkToItkConnectorType::New();

  m_MovingImageVtkToItkConnector      = VtkToItkConnectorType::New();

  m_MovingImageNormalizeFilter            = NormalizeFilterType::New();
  m_FixedImageNormalizeFilter             = NormalizeFilterType::New();
  m_NormalizedInputMovingImageNormalizeFilter   = NormalizeFilterType::New();
  m_TransformedMovingImageNormalizeFilter = NormalizeFilterType::New();


  m_RegisteredImageWriter                 = ImageWriterType::New();

  m_Caster                  = CastFilterType::New();

  m_InputTransform                        = AffineTransformType::New();

  m_InputInterpolator                   = InterpolatorType::New();

  m_ResampleNormalizedInputMovingImageFilter  = InternalResampleFilterType::New();

  //
  // registration method related members
  //
  m_Interpolator                        = InterpolatorType::New();

  m_Transform              = TransformType::New();

  m_Optimizer              = OptimizerType::New();

  m_ImageRegistrationMethod             = ImageRegistrationMethodType::New();

  //
  // Create the Command observer and register it with the optimizer.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  m_Optimizer->AddObserver( itk::IterationEvent(), observer );


  //
  // output image related members
  //
  m_ResampleMovingImageFilter        = ResampleFilterType::New();


  m_FixedImageIsLoaded  = false;
  m_MovingImageIsLoaded = false;

}




/************************************
 *
 *  Destructor
 *
 ***********************************/
ImageRegistrationConsoleBase 
::~ImageRegistrationConsoleBase()
{

}



 
/************************************
 *
 *  Load Fixed Image
 *
 ***********************************/
void
ImageRegistrationConsoleBase 
::LoadFixedImage( const char * filename )
{
  if( !filename )
  {
    return;
  }

  m_FixedImageReader->SetFileName( filename );

  try
  {
        m_FixedImageReader->Update();
  }
  catch(itk::ExceptionObject & err )
  {
    std::cout<< "ExceptionObject caught !" << std::endl;
    std::cout<< err << std::endl;
    std::cout<< "while reading fixed image"<<std::endl;
  }

 
  m_FixedImageIsLoaded = true;

}


 
/************************************
 *
 *  Load Moving Image
 *
 ***********************************/
void
ImageRegistrationConsoleBase 
::LoadMovingImage( const char * filename )
{
  if( !filename )
  {
    return;
  }

  m_MovingImageReader->SetFileName( filename );

  try
  {
       m_MovingImageReader->Update();
  }
  catch(itk::ExceptionObject & err )
  {
    std::cout<< "ExceptionObject caught !" << std::endl;
    std::cout<< err << std::endl;
    std::cout<< "while reading moving image"<<std::endl;
  }



  m_MovingImageIsLoaded = true;

}



/************************************
 *
 *  Save Registered Image
 *
 ***********************************/
void
ImageRegistrationConsoleBase 
::SaveRegisteredImage( const char * filename )
{
  if( !filename )
  {
    return;
  }

  m_Caster->SetInput( m_ResampleMovingImageFilter->GetOutput() );

  m_RegisteredImageWriter->SetInput( m_Caster->GetOutput() );
  m_RegisteredImageWriter->SetFileName( filename );

    try
  {
      m_RegisteredImageWriter->Update();
  }
  catch(itk::ExceptionObject & err )
  {
    std::cout<< "ExceptionObject caught !" << std::endl;
    std::cout<< err << std::endl;
    std::cout<< "while writing moving image"<<std::endl;
  }


}



 
/************************************
 *
 *  Show Status
 *
 ***********************************/
void
ImageRegistrationConsoleBase 
::ShowStatus( const char * )
{

}

 


/************************************
 *
 *  Stop Registration
 *
 ***********************************/
void
ImageRegistrationConsoleBase 
::Stop( void )
{
  // TODO: add a Stop() method to Optimizers 
  //m_ImageRegistrationMethod->GetOptimizer()->Stop();

}




/************************************
 *
 *  Execute
 *
 ***********************************/
void
ImageRegistrationConsoleBase 
::Execute( void )
{

  
  m_ImageRegistrationMethod->SetFixedImage(  m_FixedImageNormalizeFilter->GetOutput() );
  
  m_ImageRegistrationMethod->SetMovingImage( m_MovingImageNormalizeFilter->GetOutput() );

  m_ImageRegistrationMethod->SetOptimizer( m_Optimizer );
  
  m_ImageRegistrationMethod->SetInterpolator( m_Interpolator );

  m_ImageRegistrationMethod->SetTransform( m_Transform );

  // next two statements were introduced for compatibility with itk 1.6 and up
  // see code of Examples/Filtering/ResampleImageFilter4.cxx
  // However, they don't work on itk 1.4
  const InternalImageType::SpacingType&
    fixedSpacing = m_FixedImageNormalizeFilter->GetOutput()->GetSpacing();
  const InternalImageType::PointType&
    fixedOrigin  = m_FixedImageNormalizeFilter->GetOutput()->GetOrigin();

  // working code for itk 1.4
//  const double * fixedSpacing = m_FixedImageNormalizeFilter->GetOutput()->GetSpacing();
//  const double * fixedOrigin  = m_FixedImageNormalizeFilter->GetOutput()->GetOrigin();
  
  FixedImageType::SizeType fixedSize  = 
          m_FixedImageNormalizeFilter->GetOutput()->GetLargestPossibleRegion().GetSize();
  
  TransformType::InputPointType centerFixed;
  
  centerFixed[0] = fixedOrigin[0] + fixedSpacing[0] * fixedSize[0] / 2.0;
  centerFixed[1] = fixedOrigin[1] + fixedSpacing[1] * fixedSize[1] / 2.0;

  // working code for itk 1.4
//  const double * movingSpacing = m_MovingImageNormalizeFilter->GetOutput()->GetSpacing();
//  const double * movingOrigin  = m_MovingImageNormalizeFilter->GetOutput()->GetOrigin();

  // working code for itk 1.6 and up
  const InternalImageType::SpacingType&
    movingSpacing = m_MovingImageNormalizeFilter->GetOutput()->GetSpacing();
  const InternalImageType::PointType&
    movingOrigin  = m_MovingImageNormalizeFilter->GetOutput()->GetOrigin();
  
  MovingImageType::SizeType movingSize = 
            m_MovingImageNormalizeFilter->GetOutput()->GetLargestPossibleRegion().GetSize();
  
  TransformType::InputPointType centerMoving;
  
  centerMoving[0] = movingOrigin[0] + movingSpacing[0] * movingSize[0] / 2.0;
  centerMoving[1] = movingOrigin[1] + movingSpacing[1] * movingSize[1] / 2.0;

  m_Transform->SetCenter( centerFixed );
  m_Transform->SetTranslation( centerMoving - centerFixed );
  m_Transform->SetAngle( 0.0 );
  
  m_ImageRegistrationMethod->SetInitialTransformParameters( m_Transform->GetParameters() );

  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( m_Transform->GetNumberOfParameters() );

  optimizerScales[0] = m_RotationScale; //for rigid2d only
  optimizerScales[1] = m_TranslationScale;
  optimizerScales[2] = m_TranslationScale;
  optimizerScales[3] = m_TranslationScale;
  optimizerScales[4] = m_TranslationScale;
  


  m_Optimizer->SetScales( optimizerScales );
  m_Optimizer->SetMinimumStepLength( m_MinimumStepLength ); 
  m_Optimizer->SetMaximumStepLength( m_MaximumStepLength );
  m_Optimizer->SetNumberOfIterations( m_NumberOfIterations );

    
  try
  {
    m_ImageRegistrationMethod->StartRegistration();
  }
  catch(itk::ExceptionObject & err )
  {
    cout<< "ExceptionObject caught !" << endl;
    cout<< err << endl;
  }
}




 
/************************************
 *
 *  Generate Registered Moving Image
 *
 ***********************************/
void
ImageRegistrationConsoleBase 
::GenerateRegisteredMovingImage( void )
{

  if( !m_MovingImageIsLoaded )
    {
    return;
    }

  this->ShowStatus("Transforming the reference image...");

  OptimizerType::ParametersType finalParameters = 
                    m_ImageRegistrationMethod->GetLastTransformParameters();
  //
  // to account for transformation introduced by the user so we can use the
  // input moving image for casting purposes
  //
  finalParameters[0] -= (float) m_Angle;
  finalParameters[3] += (float) m_Offset[0];
  finalParameters[4] += (float) m_Offset[1];

  //
  // final parameters are now in a form that allows us to use the input moving image
  //
  
  TransformType::Pointer finalTransform = TransformType::New();

  finalTransform->SetParameters( finalParameters );
  
  m_ResampleMovingImageFilter->SetTransform( finalTransform );

  m_ResampleMovingImageFilter->SetInput( m_MovingImageReader->GetOutput() );

  m_ResampleMovingImageFilter->SetOutputSpacing( 
    m_FixedImageNormalizeFilter->GetOutput()->GetSpacing() );

  m_ResampleMovingImageFilter->SetOutputOrigin( 
    m_FixedImageNormalizeFilter->GetOutput()->GetOrigin() );

  m_ResampleMovingImageFilter->SetSize( 
    m_FixedImageNormalizeFilter->GetOutput()->GetLargestPossibleRegion().GetSize() );

  m_ResampleMovingImageFilter->SetDefaultPixelValue( 0 );


  try
  {
      m_ResampleMovingImageFilter->Update();
  }
  catch(itk::ExceptionObject & err )
  {
    std::cout<< "ExceptionObject caught !" << std::endl;
    std::cout<< err << std::endl;
    std::cout<< "while creating registered moving image"<<std::endl;
  }




  this->ShowStatus("MovingImage Image Transformation done");

}

/************************************
 *
 *  Generate Fixed image
 *
 ***********************************/
void
ImageRegistrationConsoleBase 
::GenerateFixedImage( void )
{


  if( !m_FixedImageIsLoaded )
    {
    return;
    }

  this->ShowStatus("Normalizing the fixed image...");


  //
  // code for vtk processing
  //

  //
  // from itk image data to vtk image data
  //
    
  //
  // creates the connection between itk and vtk
  //
  m_FixedImageItkToVtkConnector->SetInput(m_FixedImageReader->GetOutput());

  vtkImageCast *cast = vtkImageCast::New();
  cast->SetInput( m_FixedImageItkToVtkConnector->GetOutput() );
  cast->SetOutputScalarTypeToFloat();

  //
  // Additional vtk processing
  //

  //
  // to do
  //
 
  //
  // from vtk image data back to itk image data
  //
  m_FixedImageVtkToItkConnector  =  VtkToItkConnectorType::New();

  m_FixedImageVtkToItkConnector->SetInput(cast->GetOutput());
  
  
  try
  {
      m_FixedImageVtkToItkConnector->Update();
  }
  catch(itk::ExceptionObject & err )
  {
    std::cout<< "ExceptionObject caught !" << std::endl;
    std::cout<< err << std::endl;
    std::cout<< "while converting vtk image to itk fixed image"<<std::endl;
  }



  //
  // end of code for vtk processing
  //

  

  m_FixedImageNormalizeFilter->SetInput( m_FixedImageVtkToItkConnector->GetOutput() );
  try
  {
      m_FixedImageNormalizeFilter->Update();
  }
  catch(itk::ExceptionObject & err )
  {
    std::cout<< "ExceptionObject caught !" << std::endl;
    std::cout<< err << std::endl;
    std::cout<< "while normalizing fixed image"<<std::endl;
  }

  


  this->ShowStatus("Fixed Image Normalization done");

}


/************************************
 *
 *  Generate Normalized Input Moving image
 *
 ***********************************/
void
ImageRegistrationConsoleBase 
::GenerateNormalizedInputMovingImage( void )
{

  if( !m_MovingImageIsLoaded )
    {
    return;
    }

  this->ShowStatus("Normalizing the input moving image...");

  
  //
  // code for vtk processing
  //

  //
  // from itk image data to vtk image data
  //
    
  //
  // creates the connection between itk and vtk
  //
  m_MovingImageItkToVtkConnector->SetInput(m_MovingImageReader->GetOutput());

  vtkImageCast *cast = vtkImageCast::New();
  cast->SetInput( m_MovingImageItkToVtkConnector->GetOutput() );
  cast->SetOutputScalarTypeToFloat();

  //
  // additional vtk processing
  //

  //
  // to do
  //

  //
  // from vtk image data back to itk image data
  //
  m_MovingImageVtkToItkConnector  =  VtkToItkConnectorType::New();

  m_MovingImageVtkToItkConnector->SetInput(cast->GetOutput());

  try
  {
        m_MovingImageVtkToItkConnector->Update();
  }
  catch(itk::ExceptionObject & err )
  {
    std::cout<< "ExceptionObject caught !" << std::endl;
    std::cout<< err << std::endl;
    std::cout<< "while converting from vtk to itk moving image"<<std::endl;
  }




  //
  // end of code for vtk processing
  //

  // intensity normalization

  m_NormalizedInputMovingImageNormalizeFilter->SetInput( m_MovingImageVtkToItkConnector->GetOutput() );
 
  try
  {
      m_NormalizedInputMovingImageNormalizeFilter->Update();
  }
  catch(itk::ExceptionObject & err )
  {
    std::cout<< "ExceptionObject caught !" << std::endl;
    std::cout<< err << std::endl;
    std::cout<< "while normalizing input moving image"<<std::endl;
  }

  

  this->ShowStatus("Moving Image Normalization done");

}

/************************************
 *
 *  Generate Transformed Moving Image
 *
 ***********************************/
void
ImageRegistrationConsoleBase
::GenerateTransformedMovingImage( void )
{
  
  if( !m_MovingImageIsLoaded )
    {
    return;
    }

  this->ShowStatus("Transforming the corrected moving image...");

  m_InputTransform->SetIdentity();

  //
  // to rotate around the center of the image as indicated in the ITK Software Guide
  // Geometrical Transformations rotating an image p. 132 (v. June 7 2003)
  //
  
  // next two statements worked ok in itk 1.4 but gave an error in itk 1.6
//  const double * spacing = m_NormalizedInputMovingImageNormalizeFilter->GetOutput()->GetSpacing();
//  const double * origin  = m_NormalizedInputMovingImageNormalizeFilter->GetOutput()->GetOrigin();

  // next two statements were introduced for compatibility with itk 1.6 and up
  // see code of Examples/Filtering/ResampleImageFilter4.cxx
  // However, they don't work on itk 1.4
  const InternalImageType::SpacingType&
    spacing = m_NormalizedInputMovingImageNormalizeFilter->GetOutput()->GetSpacing();
  const InternalImageType::PointType&
    origin  = m_NormalizedInputMovingImageNormalizeFilter->GetOutput()->GetOrigin();
  
  // this statement works ok for itk 1.4 and itk 1.6
  InternalImageType::SizeType size = 
    m_NormalizedInputMovingImageNormalizeFilter->GetOutput()->GetLargestPossibleRegion().GetSize();
  
  // moving image centre to the origin of the coordinate system
  AffineTransformType::OutputVectorType translation1;
  const double imageCentreX = origin[0] + spacing[0] * size[0] / 2.0;
  const double imageCentreY = origin[1] + spacing[1] * size[1] / 2.0;
  translation1[0] = -imageCentreX;
  translation1[1] = -imageCentreY;
  m_InputTransform->Translate( translation1, false );

  // rotation around new centre
  m_InputTransform->Rotate2D( m_Angle, false );

  // translate origin back to its previous location
  AffineTransformType::OutputVectorType translation2;
  translation2[0] = imageCentreX;
  translation2[1] = imageCentreY;
  m_InputTransform->Translate( translation2, false );

  // apply user translation if any
  m_InputTransform->Translate( m_Offset, false );

  m_ResampleNormalizedInputMovingImageFilter->SetInput( 
    m_NormalizedInputMovingImageNormalizeFilter->GetOutput() );

  m_ResampleNormalizedInputMovingImageFilter->SetInterpolator( m_InputInterpolator );

  m_ResampleNormalizedInputMovingImageFilter->SetDefaultPixelValue( 0 );

  m_ResampleNormalizedInputMovingImageFilter->SetOutputSpacing( 
    m_NormalizedInputMovingImageNormalizeFilter->GetOutput()->GetSpacing() );

  m_ResampleNormalizedInputMovingImageFilter->SetOutputOrigin( 
    m_NormalizedInputMovingImageNormalizeFilter->GetOutput()->GetOrigin() );

  m_ResampleNormalizedInputMovingImageFilter->SetSize( 
    m_NormalizedInputMovingImageNormalizeFilter->GetOutput()->GetLargestPossibleRegion().GetSize() );

  m_ResampleNormalizedInputMovingImageFilter->SetTransform( m_InputTransform );
  
  m_TransformedMovingImageNormalizeFilter->SetInput( 
    m_ResampleNormalizedInputMovingImageFilter->GetOutput() );
  
  
  try
  {
      m_TransformedMovingImageNormalizeFilter->Update();
  }
  catch(itk::ExceptionObject & err )
  {
    std::cout<< "ExceptionObject caught !" << std::endl;
    std::cout<< err << std::endl;
    std::cout<< "while normalizing transformed moving image"<<std::endl;
  }

  this->ShowStatus("Normalized input moving image transformation done");

}


/************************************
 *
 *  Generate Moving Image
 *
 ***********************************/
void
ImageRegistrationConsoleBase
::GenerateMovingImage( void )
{
  
  if( !m_MovingImageIsLoaded )
    {
    return;
    }

  this->ShowStatus("Adding Noise to the moving image...");

  //
  // code for adding noise
  //

  GeneratorType::Pointer generator=GeneratorType::New();
  generator->Initialize( (int) 2003 );
  
  MovingImageType::Pointer  movingImage = MovingImageType::New();
  
  // allocating movingImage
  movingImage->SetRegions( 
    m_TransformedMovingImageNormalizeFilter->GetOutput()->GetRequestedRegion() );
  movingImage->Allocate();
    
  ConstIteratorType in( m_TransformedMovingImageNormalizeFilter->GetOutput(), 
    m_TransformedMovingImageNormalizeFilter->GetOutput()->GetRequestedRegion() );

  IteratorType out( movingImage,
    m_TransformedMovingImageNormalizeFilter->GetOutput()->GetRequestedRegion() );

  for (in.GoToBegin(), out.GoToBegin(); !in.IsAtEnd(); ++in, ++out )
  {
    out.Set( in.Get() + m_MovingImageNoiseStandardDeviation * 
      generator->GetVariate() + m_MovingImageNoiseMean );
  }

  //
  // intensity normalization
  //
  m_MovingImageNormalizeFilter->SetInput( movingImage );

  try
  {
      m_MovingImageNormalizeFilter->Update();
  }
  catch(itk::ExceptionObject & err )
  {
    std::cout<< "ExceptionObject caught !" << std::endl;
    std::cout<< err << std::endl;
    std::cout<< "while normalizing moving image"<<std::endl;
  }


  
  this->ShowStatus("Moving image created");

}


 
/************************************
 *
 *  Select the metric (mi with equidistant binning ) to be used to 
 *  compare the images during the 
 *  registration process 
 *
 ***********************************/
void
ImageRegistrationConsoleBase 
::SelectMutualInformationMetric( void )
{

  MutualInformationMetricType::Pointer metric = MutualInformationMetricType::New();
  m_ImageRegistrationMethod->SetMetric( metric );
  MutualInformationMetricType::HistogramType::SizeType histSize;
  histSize[0] = m_MutualInformationNumberOfBins;
  histSize[1] = m_MutualInformationNumberOfBins;
  metric->SetHistogramSize(histSize);

  // Set up transform parameters.
  const unsigned int numberOfParameters = m_Transform->GetNumberOfParameters();

    
  // Set scales for derivative calculation.
  ScalesType scales( numberOfParameters );

  for (unsigned int k = 0; k < numberOfParameters; k++)
    {
    scales[k] = 1;
    }

  metric->SetDerivativeStepLengthScales(scales);

  m_Optimizer->MaximizeOn();
}


/************************************
 *
 *  Select the metric ( mean squares )to be used to 
 *  compare the images during the 
 *  registration process 
 *
 ***********************************/
void
ImageRegistrationConsoleBase 
::SelectMeanSquaresMetric( void )
{

  MeanSquaresMetricType::Pointer 
    metric = MeanSquaresMetricType::New();

  m_ImageRegistrationMethod->SetMetric( metric );

  m_Optimizer->MinimizeOn();
}


