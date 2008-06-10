/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkRegisterITKMonitorVTK.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkOrientedImage.h"

#include "itkVersorRigid3DTransform.h"
#include "itkCenteredTransformInitializer.h"
#include "itkVersorRigid3DTransformOptimizer.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkTransformFileWriter.h"

#include "vtkRegistrationMonitor.h"
#include "vtkKWImage.h"
#include "vtkContourFilter.h"
#include "vtkImageData.h"

#include "metaCommand.h"
#include "metaOutput.h"

//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.
//
#include "itkCommand.h"
class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  typedef itk::VersorRigid3DTransformOptimizer     OptimizerType;
  typedef   const OptimizerType   *    OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    OptimizerPointer optimizer = 
      dynamic_cast< OptimizerPointer >( object );
    if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
      return;
      }
    std::cout << optimizer->GetCurrentIteration() << "   ";
    std::cout << optimizer->GetValue() << "   ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};

int usage_examples()
{
  std::cout << std::endl << "Usage Examples:" << std::endl;
  std::cout << "  -h" << std::endl;
  std::cout << "  Print usage statement and examples" << std::endl;
  std::cout << std::endl;
  std::cout << "  FixedImageFilename" << std::endl;
  std::cout << "  MovingImageFilename" << std::endl;
  std::cout << "  OutputImageFilename" << std::endl;
  std::cout << "  TransformFilename" << std::endl;
  return 0;
}


int main( int argc, char *argv[] )
{
  MetaCommand command;

  command.AddField("fixedImage","FixedImageFilename",MetaCommand::STRING,true);
  command.AddField("movingImage","MovingImageFilename",MetaCommand::STRING,true);
  command.AddField("outputImage","OutputImageFilename",MetaCommand::STRING,true);
  command.SetOption("outputTransform","t",false,"TransformFilename",
                                                       MetaCommand::STRING);

  // This should be put before the parsing
  MetaOutput output;
  output.SetMetaCommand(&command);
  if ( !command.Parse(argc,argv) )
    {
    return EXIT_FAILURE;
    }
 
  std::string fixedImageFileName  = command.GetValueAsString("fixedImage");
  std::string movingImageFileName = command.GetValueAsString("movingImage");
  std::string outputImageFileName  = command.GetValueAsString("outputImage");
  
  const    unsigned int    Dimension = 3;
  typedef  signed short    FixedPixelType;
  typedef  unsigned short  MovingPixelType;
  typedef  float           InternalPixelType;

  typedef itk::OrientedImage< FixedPixelType, Dimension >   InputFixedImageType;
  typedef itk::OrientedImage< MovingPixelType, Dimension >  InputMovingImageType;

  typedef itk::OrientedImage< InternalPixelType, Dimension > InternalImageType;


  typedef itk::VersorRigid3DTransform< double > TransformType;



  typedef itk::VersorRigid3DTransformOptimizer           OptimizerType;

  typedef itk::MattesMutualInformationImageToImageMetric< 
                                    InternalImageType, 
                                    InternalImageType >    MetricType;

  typedef itk:: LinearInterpolateImageFunction< 
                                    InternalImageType,
                                    double          >    InterpolatorType;

  typedef itk::ImageRegistrationMethod< 
                                    InternalImageType, 
                                    InternalImageType >    RegistrationType;

  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();
  

  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator  );


  TransformType::Pointer  transform = TransformType::New();
  registration->SetTransform( transform );


  typedef itk::ImageFileReader< InputFixedImageType  > FixedImageReaderType;
  typedef itk::ImageFileReader< InputMovingImageType > MovingImageReaderType;

  FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
  MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

  fixedImageReader->SetFileName(  fixedImageFileName );
  movingImageReader->SetFileName( movingImageFileName );

  try 
    { 
    movingImageReader->Update();
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return -1;
    } 
 
  try 
    { 
    fixedImageReader->Update();
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return -1;
    } 
 
  typedef itk::CastImageFilter< 
    InputFixedImageType, InternalImageType > FixedCastFilterType;

  typedef itk::CastImageFilter< 
    InputMovingImageType, InternalImageType > MovingCastFilterType;

  FixedCastFilterType::Pointer fixedCaster = FixedCastFilterType::New();
  MovingCastFilterType::Pointer movingCaster = MovingCastFilterType::New();

  fixedCaster->SetInput( fixedImageReader->GetOutput() );
  movingCaster->SetInput( movingImageReader->GetOutput() );

  fixedCaster->Update();
  movingCaster->Update();

  registration->SetFixedImage(    fixedCaster->GetOutput()    );
  registration->SetMovingImage(   movingCaster->GetOutput()   );

  registration->SetFixedImageRegion( 
     fixedCaster->GetOutput()->GetBufferedRegion() );

  typedef itk::CenteredTransformInitializer< TransformType, 
                                             InternalImageType, 
                                             InternalImageType 
                                                 >  TransformInitializerType;

  TransformInitializerType::Pointer initializer = 
                                          TransformInitializerType::New();

  initializer->SetTransform(   transform );
  initializer->SetFixedImage(  fixedCaster->GetOutput() );
  initializer->SetMovingImage( movingCaster->GetOutput() );
  initializer->MomentsOn();

  initializer->InitializeTransform();

  typedef TransformType::VersorType  VersorType;
  typedef VersorType::VectorType     VectorType;

  VersorType     rotation;
  VectorType     axis;
  
  axis[0] = 0.0;
  axis[1] = 0.0;
  axis[2] = 1.0;

  const double angle = 0;

  rotation.Set(  axis, angle  );

  transform->SetRotation( rotation );

  std::cout << "Initial Transform parameters " << std::endl;
  std::cout << transform->GetParameters() << std::endl;

  registration->SetInitialTransformParameters( transform->GetParameters() );


  typedef OptimizerType::ScalesType       OptimizerScalesType;
  OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );


  InputFixedImageType::ConstPointer fixedImage = fixedImageReader->GetOutput();

  InputFixedImageType::SpacingType spacing = fixedImage->GetSpacing();
  InputFixedImageType::SizeType    size    = fixedImage->GetLargestPossibleRegion().GetSize();

  const double translationScaleFactor = 200.0;

  optimizerScales[0] = 1.0;
  optimizerScales[1] = 1.0;
  optimizerScales[2] = 1.0;
  optimizerScales[3] = 1.0 / ( translationScaleFactor * spacing[0] * size[0] );
  optimizerScales[4] = 1.0 / ( translationScaleFactor * spacing[1] * size[1] );
  optimizerScales[5] = 1.0 / ( translationScaleFactor * spacing[2] * size[2] );

  optimizer->SetScales( optimizerScales );

  optimizer->SetMaximumStepLength( 10.000 ); 
  optimizer->SetMinimumStepLength(  0.001 );

  optimizer->SetNumberOfIterations( 100 );
  optimizer->MaximizeOff();

  const unsigned long numberOfImagePixels = 
    fixedImage->GetLargestPossibleRegion().GetNumberOfPixels();

  const unsigned long numberOfSpatialSamples =
    static_cast<unsigned long>( numberOfImagePixels * 0.100 );
    
  metric->SetNumberOfSpatialSamples( numberOfSpatialSamples );


  metric->SetNumberOfHistogramBins( 32 );


  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  //
  // VTK Visualization to monitor the registration process
  //

  vtkRegistrationMonitor monitor;

  vtkKWImage * fixedKWImage  = vtkKWImage::New();
  vtkKWImage * movingKWImage = vtkKWImage::New();

  fixedKWImage->SetITKImageBase( fixedImageReader->GetOutput() );
  movingKWImage->SetITKImageBase( movingImageReader->GetOutput() );

  vtkContourFilter * fixedContour  = vtkContourFilter::New();
  vtkContourFilter * movingContour = vtkContourFilter::New();

  fixedContour->SetInput( fixedKWImage->GetVTKImage() );
  movingContour->SetInput( movingKWImage->GetVTKImage() );

  fixedContour->SetValue(  0, 50.0 ); // level for iso-contour
  movingContour->SetValue( 0, 50.0 ); // level for iso-contour

  monitor.SetFixedSurface( fixedContour->GetOutput() );
  monitor.SetMovingSurface( movingContour->GetOutput() );

  monitor.SetNumberOfIterationPerUpdate( 10 );

  monitor.Observe( optimizer, transform );

  //
  // End of VTK Visualization to monitor the registration process
  //



  try 
    { 
    registration->StartRegistration(); 
    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    return -1;
    } 
  
  OptimizerType::ParametersType finalParameters = 
                    registration->GetLastTransformParameters();

  const double versorX              = finalParameters[0];
  const double versorY              = finalParameters[1];
  const double versorZ              = finalParameters[2];
  const double finalTranslationX    = finalParameters[3];
  const double finalTranslationY    = finalParameters[4];
  const double finalTranslationZ    = finalParameters[5];

  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();

  const double bestValue = optimizer->GetValue();


  // Print out results
  //
  std::cout << std::endl << std::endl;
  std::cout << "Result = " << std::endl;
  std::cout << " versor X      = " << versorX  << std::endl;
  std::cout << " versor Y      = " << versorY  << std::endl;
  std::cout << " versor Z      = " << versorZ  << std::endl;
  std::cout << " Translation X = " << finalTranslationX  << std::endl;
  std::cout << " Translation Y = " << finalTranslationY  << std::endl;
  std::cout << " Translation Z = " << finalTranslationZ  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;

  output.AddFloatField("MetricValue","Final Value of the Metric",bestValue); 
  output.AddIntField("Iterations","Number of iterations",numberOfIterations); 
  output.Write();

  transform->SetParameters( finalParameters );

  TransformType::MatrixType matrix = transform->GetRotationMatrix();
  TransformType::OffsetType offset = transform->GetOffset();

  std::cout << "Matrix = " << std::endl << matrix << std::endl;
  std::cout << "Offset = " << std::endl << offset << std::endl;

  if(command.GetOptionWasSet("outputTransform"))
    {
    std::string outputTransform  = command.GetValueAsString("outputTransform");
    itk::TransformFileWriter::Pointer transformWriter = itk::TransformFileWriter::New();
    transformWriter->SetFileName(outputTransform.c_str());
    transformWriter->SetInput(transform);
    transformWriter->Update();
    }


  typedef itk::ResampleImageFilter< 
                            InputMovingImageType, 
                            InputMovingImageType >    ResampleFilterType;

  TransformType::Pointer finalTransform = TransformType::New();

  finalTransform->SetCenter( transform->GetCenter() );

  finalTransform->SetParameters( finalParameters );

  ResampleFilterType::Pointer resampler = ResampleFilterType::New();

  resampler->SetTransform( finalTransform );
  resampler->SetInput( movingImageReader->GetOutput() );


  resampler->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resampler->SetOutputOrigin(  fixedImage->GetOrigin() );
  resampler->SetOutputSpacing( fixedImage->GetSpacing() );
  resampler->SetOutputDirection( fixedImage->GetDirection() );
  resampler->SetDefaultPixelValue( 0 );

  typedef itk::ImageFileWriter< InputMovingImageType > WriterType;
  WriterType::Pointer      writer =  WriterType::New();

  writer->SetFileName( outputImageFileName );
  writer->UseCompressionOn();
  writer->SetInput( resampler->GetOutput()   );
  writer->Update();


  return 0;
}

