/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    vtkITKMutualInformationTransform.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkITKMutualInformationTransform.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkImageExport.h"
#include "vtkTransform.h"

#include "itkVTKImageImport.h"
#include "vtkITKUtility.h"

#include "itkExceptionObject.h"
// turn itk exceptions into vtk errors
#undef itkExceptionMacro  
#define itkExceptionMacro(x) \
  { \
    std::ostringstream message;                \
  message << "itk::ERROR: " << this->GetNameOfClass() \
          << "(" << this << "): " x; \
  std::cerr << message.str().c_str() << std::endl; \
  }

#undef itkGenericExceptionMacro  
#define itkGenericExceptionMacro(x) \
  { \
    std::ostringstream message;                 \
  message << "itk::ERROR: " x; \
  std::cerr << message.str().c_str() << std::endl; \
  }

// itk classes

#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkQuaternionRigidTransform.h"
#include "itkQuaternionRigidTransformGradientDescentOptimizer.h"
#include "itkMutualInformationImageToImageMetric.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageRegistrationMethod.h"
#include "itkNumericTraits.h"
#include "vnl/vnl_math.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

typedef itk::Size<3> SizeType;
typedef itk::Index<3> IndexType;

vtkCxxRevisionMacro(vtkITKMutualInformationTransform, "$Revision: 1.21 $");
vtkStandardNewMacro(vtkITKMutualInformationTransform);

//----------------------------------------------------------------------------
vtkITKMutualInformationTransform::vtkITKMutualInformationTransform()
{
  this->SourceImage=NULL;
  this->TargetImage=NULL;
  this->SourceStandardDeviation = 2.0;
  this->TargetStandardDeviation = 2.0;
  this->LearningRate = .005;
  this->TranslateScale = 64;
  this->NumberOfIterations = 500;  
  this->NumberOfSamples = 50;
  this->Matrix->Identity();

  this->NumberOfHistogramBins = 20;   

  this->MaximumStepLength = 4.00; 
  this->MinimumStepLength = 0.005;   

  this->UseMattes = false;
  this->UseViolaWells = true;

  this->QuaternionRigidMode = true;
  this->AffineMode = false;
}

//----------------------------------------------------------------------------
vtkITKMutualInformationTransform::~vtkITKMutualInformationTransform()
{
  if(this->SourceImage)
    { 
    this->SourceImage->Delete();
    }
  if(this->TargetImage)
    { 
    this->TargetImage->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkITKMutualInformationTransform::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << "SourceStandardDeviation: " << this->SourceStandardDeviation  << endl;
  os << "TargetStandardDeviation: " << this->SourceStandardDeviation  << endl;
  os << "LearningRate: " << this->LearningRate  << endl;
  os << "TranslateScale: " << this->TranslateScale  << endl;
  os << "NumberOfSamples: " << this->NumberOfSamples  << endl;
  os << "NumberOfIterations: " << this->NumberOfIterations  << endl;

  os << "NumberOfHistogramBins: " << this->NumberOfHistogramBins << endl;   
  os << "MaximumStepLength: " << this->MaximumStepLength << endl; 
  os << "MinimumStepLength: " << this->MinimumStepLength << endl;   

  os << "UseMattes: " << this->UseMattes << endl;
  os << "UseViolaWells: " << this->UseViolaWells << endl;

  os << "AffineMode: " << this->AffineMode << endl;
  os << "QuaternionRigidMode: " << this->QuaternionRigidMode << endl;

  os << "SourceImage: " << this->SourceImage << "\n";
  if(this->SourceImage) 
    {
    this->SourceImage->PrintSelf(os,indent.GetNextIndent());
    }
  os << "TargetImage: " << this->TargetImage << "\n";
  if(this->TargetImage)
    { 
    this->TargetImage->PrintSelf(os,indent.GetNextIndent());
    }
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data,
// using the Viola-Wells metric and the Quaternion Rigid transform
template <class T>
static void vtkITKMIViolaWellsQuaternionRigidExecute(vtkITKMutualInformationTransform *self,
                               vtkImageData *source,
                               vtkImageData *target,
                               vtkMatrix4x4 *matrix,
                               T vtkNotUsed(dummy2))                               
{
  // Declare the input and output types
  typedef itk::Image<T,3> OutputType;
    
  // Declare the registration types
  typedef itk::QuaternionRigidTransform<double> TransformType;
  typedef itk::LinearInterpolateImageFunction<OutputType, double> InterpolatorType;
  typedef itk::ImageRegistrationMethod<OutputType,OutputType> RegistrationType;
  typedef itk::MutualInformationImageToImageMetric<OutputType, OutputType> MetricType; 
  typedef itk::QuaternionRigidTransformGradientDescentOptimizer OptimizerType;
          
  // Source
  vtkImageExport *movingVtkExporter = vtkImageExport::New();
  movingVtkExporter->SetInput(source);
    
  typedef itk::VTKImageImport<OutputType> ImageImportType;
   
  typename ImageImportType::Pointer movingItkImporter = ImageImportType::New();
  ConnectPipelines(movingVtkExporter, movingItkImporter);
    
  // Target
  vtkImageExport *fixedVtkExporter = vtkImageExport::New();
  fixedVtkExporter->SetInput(target);
  
  typename ImageImportType::Pointer fixedItkImporter = ImageImportType::New();
  ConnectPipelines(fixedVtkExporter, fixedItkImporter);
  
  //-----------------------------------------------------------
  // Set up the registrator
  //-----------------------------------------------------------
  typename MetricType::Pointer metric = MetricType::New();
  typename TransformType::Pointer transform = TransformType::New();
  typename OptimizerType::Pointer optimizer = OptimizerType::New();
  typename InterpolatorType::Pointer interpolator  = InterpolatorType::New();
  typename RegistrationType::Pointer registration  = RegistrationType::New();
  typename RegistrationType::ParametersType guess(transform->GetNumberOfParameters() );
  
  // the guess is derived from the current matrix.
  vnl_matrix<double> matrix3x4(3,4);
  matrix3x4[0][0] = matrix->Element[0][0];
  matrix3x4[0][1] = matrix->Element[1][0];
  matrix3x4[0][2] = matrix->Element[2][0];
  matrix3x4[0][3] = matrix->Element[0][3];
  matrix3x4[1][0] = matrix->Element[0][1];
  matrix3x4[1][1] = matrix->Element[1][1];
  matrix3x4[1][2] = matrix->Element[2][1];
  matrix3x4[1][3] = matrix->Element[1][3];
  matrix3x4[2][0] = matrix->Element[0][2];
  matrix3x4[2][1] = matrix->Element[1][2];
  matrix3x4[2][2] = matrix->Element[2][2];
  matrix3x4[2][3] = matrix->Element[2][3];
  
  vnl_quaternion<double> matrixAsQuaternion(matrix3x4);
  
  guess[0]= matrixAsQuaternion.x();
  guess[1]= matrixAsQuaternion.y();
  guess[2]= matrixAsQuaternion.z();
  guess[3]= matrixAsQuaternion.r();
  guess[4] = matrix->Element[0][3];
  guess[5] = matrix->Element[1][3];
  guess[6] = matrix->Element[2][3];
  
  // The guess is: a quaternion followed by a translation
  registration->SetInitialTransformParameters (guess);
  
  // Set translation scale
  typedef OptimizerType::ScalesType ScaleType;
  
  ScaleType scales(transform->GetNumberOfParameters());
  scales.Fill( 1.0 );
  for( unsigned j = 4; j < 7; j++ )
    {
    scales[j] = 1.0 / vnl_math_sqr(self->GetTranslateScale());
    }
  
  // Set metric related parameters
  metric->SetMovingImageStandardDeviation( self->GetSourceStandardDeviation() );
  metric->SetFixedImageStandardDeviation( self->GetTargetStandardDeviation() );
  metric->SetFixedImageRegion( self->GetFixedImageExtent() );
  metric->SetNumberOfSpatialSamples( self->GetNumberOfSamples() );
  
  fixedItkImporter->Update();
  movingItkImporter->Update();
  
  // Connect up the components
  registration->SetMetric(metric);
  registration->SetOptimizer(optimizer);
  registration->SetTransform(transform);
  registration->SetInterpolator(interpolator);
  registration->SetFixedImage(fixedItkImporter->GetOutput());
  registration->SetMovingImage(movingItkImporter->GetOutput());
  
  // Setup the optimizer
  optimizer->SetScales(scales);
  optimizer->MaximizeOn();   
  optimizer->SetLearningRate( self->GetLearningRate() );      
  optimizer->SetNumberOfIterations( self->GetNumberOfIterations() );
  
  // Start registration
  
  registration->StartRegistration();
  
  // Get the results
  typename RegistrationType::ParametersType solution = 
    registration->GetLastTransformParameters();
  
  vnl_quaternion<double> quat(solution[0],solution[1],solution[2],solution[3]);
  vnl_matrix_fixed<double,3,3> mat = quat.rotation_matrix_transpose();
  
  // Convert the vnl matrix to a vtk mtrix
  matrix->Element[0][0] = mat(0,0);
  matrix->Element[0][1] = mat(1,0);
  matrix->Element[0][2] = mat(2,0);
  matrix->Element[0][3] = solution[4];
  matrix->Element[1][0] = mat(0,1);
  matrix->Element[1][1] = mat(1,1);
  matrix->Element[1][2] = mat(2,1);
  matrix->Element[1][3] = solution[5];
  matrix->Element[2][0] = mat(0,2);
  matrix->Element[2][1] = mat(1,2);
  matrix->Element[2][2] = mat(2,2);
  matrix->Element[2][3] = solution[6];
  matrix->Element[3][0] = 0;
  matrix->Element[3][1] = 0;
  matrix->Element[3][2] = 0;
  matrix->Element[3][3] = 1;
  
  self->Modified(); 
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data,
// using the Mattes metric and the Quaternion Rigid transform
template <class T>
static void vtkITKMIMattesQuaternionRigidExecute(vtkITKMutualInformationTransform *self,
                               vtkImageData *source,
                               vtkImageData *target,
                               vtkMatrix4x4 *matrix,
                               T vtkNotUsed(dummy2))                               
{
  // Declare the input and output types
  typedef itk::Image<T,3> OutputType;
  
  // Declare the registration types
  typedef itk::QuaternionRigidTransform<double> TransformType;
  typedef itk::LinearInterpolateImageFunction<OutputType, double> InterpolatorType;
  typedef itk::ImageRegistrationMethod<OutputType,OutputType> RegistrationType;
  typedef itk::MattesMutualInformationImageToImageMetric<OutputType, OutputType> MetricType; 
  typedef itk::QuaternionRigidTransformGradientDescentOptimizer OptimizerType;    
   // Source
  vtkImageExport *movingVtkExporter = vtkImageExport::New();
  movingVtkExporter->SetInput(source);
  
  typedef itk::VTKImageImport<OutputType> ImageImportType;
  
  typename ImageImportType::Pointer movingItkImporter = ImageImportType::New();
  ConnectPipelines(movingVtkExporter, movingItkImporter);
  
  // Target
  vtkImageExport *fixedVtkExporter = vtkImageExport::New();
  fixedVtkExporter->SetInput(target);
  
  typename ImageImportType::Pointer fixedItkImporter = ImageImportType::New();
  ConnectPipelines(fixedVtkExporter, fixedItkImporter);
  
  //-----------------------------------------------------------
  // Set up the registrator
  //-----------------------------------------------------------
  typename MetricType::Pointer metric = MetricType::New();
  typename TransformType::Pointer transform = TransformType::New();
  typename OptimizerType::Pointer optimizer = OptimizerType::New();
  typename InterpolatorType::Pointer interpolator  = InterpolatorType::New();
  typename RegistrationType::Pointer registration  = RegistrationType::New();
  typename RegistrationType::ParametersType guess(transform->GetNumberOfParameters() );
  
  // the guess is derived from the current matrix.
  vnl_matrix<double> matrix3x4(3,4);
  matrix3x4[0][0] = matrix->Element[0][0];
  matrix3x4[0][1] = matrix->Element[1][0];
  matrix3x4[0][2] = matrix->Element[2][0];
  matrix3x4[0][3] = matrix->Element[0][3];
  matrix3x4[1][0] = matrix->Element[0][1];
  matrix3x4[1][1] = matrix->Element[1][1];
  matrix3x4[1][2] = matrix->Element[2][1];
  matrix3x4[1][3] = matrix->Element[1][3];
  matrix3x4[2][0] = matrix->Element[0][2];
  matrix3x4[2][1] = matrix->Element[1][2];
  matrix3x4[2][2] = matrix->Element[2][2];
  matrix3x4[2][3] = matrix->Element[2][3];
  
  vnl_quaternion<double> matrixAsQuaternion(matrix3x4);
  
  guess[0]= matrixAsQuaternion.x();
  guess[1]= matrixAsQuaternion.y();
  guess[2]= matrixAsQuaternion.z();
  guess[3]= matrixAsQuaternion.r();
  guess[4] = matrix->Element[0][3];
  guess[5] = matrix->Element[1][3];
  guess[6] = matrix->Element[2][3];
  
  // The guess is: a quaternion followed by a translation
  registration->SetInitialTransformParameters (guess);
  
  // Set translation scale
  typedef OptimizerType::ScalesType ScaleType;
  
  ScaleType scales(transform->GetNumberOfParameters());
  scales.Fill( 1.0 );
  for( unsigned j = 4; j < 7; j++ )
    {
    scales[j] = 1.0 / vnl_math_sqr(self->GetTranslateScale());
    }
  
  // Set metric related parameters
  metric->SetNumberOfHistogramBins( self->GetNumberOfHistogramBins() );
  metric->SetNumberOfSpatialSamples( self->GetNumberOfSamples() );
  metric->SetFixedImageRegion( self->GetFixedImageExtent() );  

  fixedItkImporter->Update();
  movingItkImporter->Update();
  
  // Connect up the components
  registration->SetMetric(metric);
  registration->SetOptimizer(optimizer);
  registration->SetTransform(transform);
  registration->SetInterpolator(interpolator);
  registration->SetFixedImage(fixedItkImporter->GetOutput());
  registration->SetMovingImage(movingItkImporter->GetOutput());    
  optimizer->SetScales(scales);
  optimizer->SetMinimize(true);   
  optimizer->SetLearningRate( self->GetLearningRate() );      
  optimizer->SetNumberOfIterations( self->GetNumberOfIterations() );
   // Start registration    
  registration->StartRegistration();
  
  // Get the results
  typename RegistrationType::ParametersType solution = 
    registration->GetLastTransformParameters();
  
  vnl_quaternion<double> quat(solution[0],solution[1],solution[2],solution[3]);
  vnl_matrix_fixed<double,3,3> mat = quat.rotation_matrix_transpose();
  
  // Convert the vnl matrix to a vtk mtrix
  matrix->Element[0][0] = mat(0,0);
  matrix->Element[0][1] = mat(1,0);
  matrix->Element[0][2] = mat(2,0);
  matrix->Element[0][3] = solution[4];
  matrix->Element[1][0] = mat(0,1);
  matrix->Element[1][1] = mat(1,1);
  matrix->Element[1][2] = mat(2,1);
  matrix->Element[1][3] = solution[5];
  matrix->Element[2][0] = mat(0,2);
  matrix->Element[2][1] = mat(1,2);
  matrix->Element[2][2] = mat(2,2);
  matrix->Element[2][3] = solution[6];
  matrix->Element[3][0] = 0;
  matrix->Element[3][1] = 0;
  matrix->Element[3][2] = 0;
  matrix->Element[3][3] = 1;
  
  self->Modified(); 
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data,
// using the Viola-Wells metric and the Affine transform
template <class T>
static void vtkITKMIViolaWellsAffineExecute(vtkITKMutualInformationTransform *self,
                               vtkImageData *source,
                               vtkImageData *target,
                               vtkMatrix4x4 *matrix,
                               T vtkNotUsed(dummy2))                               
{
  // Declare the input and output types
  typedef itk::Image<T,3> OutputType;
  
  // Declare the registration types
  typedef itk::AffineTransform<double> TransformType;
  typedef itk::LinearInterpolateImageFunction<OutputType, double> InterpolatorType;
  typedef itk::ImageRegistrationMethod<OutputType,OutputType> RegistrationType;
  typedef itk::MutualInformationImageToImageMetric<OutputType, OutputType> MetricType; 
  typedef itk::RegularStepGradientDescentOptimizer OptimizerType;    
  // Source
  vtkImageExport *movingVtkExporter = vtkImageExport::New();
  movingVtkExporter->SetInput(source);
  
  typedef itk::VTKImageImport<OutputType> ImageImportType;
  
  typename ImageImportType::Pointer movingItkImporter = ImageImportType::New();
  ConnectPipelines(movingVtkExporter, movingItkImporter);
  
  // Target
  vtkImageExport *fixedVtkExporter = vtkImageExport::New();
  fixedVtkExporter->SetInput(target);
  
  typename ImageImportType::Pointer fixedItkImporter = ImageImportType::New();
  ConnectPipelines(fixedVtkExporter, fixedItkImporter);
  
  //-----------------------------------------------------------
  // Set up the registrator
  //-----------------------------------------------------------
  typename MetricType::Pointer metric = MetricType::New();
  typename TransformType::Pointer transform = TransformType::New();
  typename OptimizerType::Pointer optimizer = OptimizerType::New();
  typename InterpolatorType::Pointer interpolator  = InterpolatorType::New();
  typename RegistrationType::Pointer registration  = RegistrationType::New();
  typename RegistrationType::ParametersType guess(transform->GetNumberOfParameters() );
    
  guess[0] = matrix->Element[0][0];
  guess[1] = matrix->Element[0][1];
  guess[2] = matrix->Element[0][2];
  guess[3] = matrix->Element[1][0];
  guess[4] = matrix->Element[1][1];
  guess[5] = matrix->Element[1][2];
  guess[6] = matrix->Element[2][0];
  guess[7] = matrix->Element[2][1];
  guess[8] = matrix->Element[2][2];
  guess[9] = matrix->Element[0][3];
  guess[10] = matrix->Element[1][3];
  guess[11] = matrix->Element[2][3];
  
  registration->SetInitialTransformParameters (guess);
  
  // Set translation scale
  typedef OptimizerType::ScalesType ScaleType;
  
  ScaleType scales(transform->GetNumberOfParameters());
  scales.Fill( 1.0 );
  for( unsigned j = 9; j < 12; j++ )
    {
    scales[j] = 1.0 / vnl_math_sqr(self->GetTranslateScale());
    }
  
  // Set metric related parameters
  metric->SetMovingImageStandardDeviation( self->GetSourceStandardDeviation() );
  metric->SetFixedImageStandardDeviation( self->GetTargetStandardDeviation() );
  metric->SetFixedImageRegion( self->GetFixedImageExtent() );
  metric->SetNumberOfSpatialSamples( self->GetNumberOfSamples() );

  fixedItkImporter->Update();
  movingItkImporter->Update();
  
  // Connect up the components
  registration->SetMetric(metric);
  registration->SetOptimizer(optimizer);
  registration->SetTransform(transform);
  registration->SetInterpolator(interpolator);
  registration->SetFixedImage(fixedItkImporter->GetOutput());
  registration->SetMovingImage(movingItkImporter->GetOutput());    

  optimizer->SetScales(scales);
  optimizer->SetMaximize(true);   
  optimizer->SetMaximumStepLength( self->GetMaximumStepLength() );      
  optimizer->SetMinimumStepLength( self->GetMinimumStepLength() );      
  optimizer->SetNumberOfIterations( self->GetNumberOfIterations() );
   // Start registration    
  registration->StartRegistration();
  
  // Get the results
  typename RegistrationType::ParametersType solution = 
    registration->GetLastTransformParameters();
    
  // Convert the vnl matrix to a vtk mtrix
  matrix->Element[0][0] = solution[0];
  matrix->Element[0][1] = solution[1];
  matrix->Element[0][2] = solution[2];
  matrix->Element[0][3] = solution[9];
  matrix->Element[1][0] = solution[3];
  matrix->Element[1][1] = solution[4];
  matrix->Element[1][2] = solution[5];
  matrix->Element[1][3] = solution[10];
  matrix->Element[2][0] = solution[6];
  matrix->Element[2][1] = solution[7];
  matrix->Element[2][2] = solution[8];
  matrix->Element[2][3] = solution[11];
  matrix->Element[3][0] = 0;
  matrix->Element[3][1] = 0;
  matrix->Element[3][2] = 0;
  matrix->Element[3][3] = 1;
  
  self->Modified(); 
}

//----------------------------------------------------------------------------
// This templated function executes the filter for any type of data,
// using the Mattes metric and the Affine transform
template <class T>
static void vtkITKMIMattesAffineExecute(vtkITKMutualInformationTransform *self,
                               vtkImageData *source,
                               vtkImageData *target,
                               vtkMatrix4x4 *matrix,
                               T vtkNotUsed(dummy2))                               
{
  // Declare the input and output types
  typedef itk::Image<T,3> OutputType;
  
  // Declare the registration types
  typedef itk::AffineTransform<double> TransformType;
  typedef itk::LinearInterpolateImageFunction<OutputType, double> InterpolatorType;
  typedef itk::ImageRegistrationMethod<OutputType,OutputType> RegistrationType;
  typedef itk::MattesMutualInformationImageToImageMetric<OutputType, OutputType> MetricType; 
  typedef itk::RegularStepGradientDescentOptimizer OptimizerType;    
  // Source
  vtkImageExport *movingVtkExporter = vtkImageExport::New();
  movingVtkExporter->SetInput(source);
  
  typedef itk::VTKImageImport<OutputType> ImageImportType;
  
  typename ImageImportType::Pointer movingItkImporter = ImageImportType::New();
  ConnectPipelines(movingVtkExporter, movingItkImporter);
  
  // Target
  vtkImageExport *fixedVtkExporter = vtkImageExport::New();
  fixedVtkExporter->SetInput(target);
  
  typename ImageImportType::Pointer fixedItkImporter = ImageImportType::New();
  ConnectPipelines(fixedVtkExporter, fixedItkImporter);
  
  //-----------------------------------------------------------
  // Set up the registrator
  //-----------------------------------------------------------
  typename MetricType::Pointer metric = MetricType::New();
  typename TransformType::Pointer transform = TransformType::New();
  typename OptimizerType::Pointer optimizer = OptimizerType::New();
  typename InterpolatorType::Pointer interpolator  = InterpolatorType::New();
  typename RegistrationType::Pointer registration  = RegistrationType::New();
  typename RegistrationType::ParametersType guess(transform->GetNumberOfParameters() );
    
  guess[0] = matrix->Element[0][0];
  guess[1] = matrix->Element[0][1];
  guess[2] = matrix->Element[0][2];
  guess[3] = matrix->Element[1][0];
  guess[4] = matrix->Element[1][1];
  guess[5] = matrix->Element[1][2];
  guess[6] = matrix->Element[2][0];
  guess[7] = matrix->Element[2][1];
  guess[8] = matrix->Element[2][2];
  guess[9] = matrix->Element[0][3];
  guess[10] = matrix->Element[1][3];
  guess[11] = matrix->Element[2][3];
  
  registration->SetInitialTransformParameters (guess);
  
  // Set translation scale
  typedef OptimizerType::ScalesType ScaleType;
  
  ScaleType scales(transform->GetNumberOfParameters());
  scales.Fill( 1.0 );
  for( unsigned j = 9; j < 12; j++ )
    {
    scales[j] = 1.0 / vnl_math_sqr(self->GetTranslateScale());
    }
  
  // Set metric related parameters
  metric->SetNumberOfHistogramBins( self->GetNumberOfHistogramBins() );
  metric->SetNumberOfSpatialSamples( self->GetNumberOfSamples() );
  metric->SetFixedImageRegion( self->GetFixedImageExtent() );  

  fixedItkImporter->Update();
  movingItkImporter->Update();
  
  // Connect up the components
  registration->SetMetric(metric);
  registration->SetOptimizer(optimizer);
  registration->SetTransform(transform);
  registration->SetInterpolator(interpolator);
  registration->SetFixedImage(fixedItkImporter->GetOutput());
  registration->SetMovingImage(movingItkImporter->GetOutput());    

  optimizer->SetScales(scales);
  optimizer->SetMinimize(true);   
  optimizer->SetMaximumStepLength( self->GetMaximumStepLength() );      
  optimizer->SetMinimumStepLength( self->GetMinimumStepLength() );      
  optimizer->SetNumberOfIterations( self->GetNumberOfIterations() );
   // Start registration    
  registration->StartRegistration();
  
  // Get the results
  typename RegistrationType::ParametersType solution = 
    registration->GetLastTransformParameters();
    
  // Convert the vnl matrix to a vtk mtrix
  matrix->Element[0][0] = solution[0];
  matrix->Element[0][1] = solution[1];
  matrix->Element[0][2] = solution[2];
  matrix->Element[0][3] = solution[9];
  matrix->Element[1][0] = solution[3];
  matrix->Element[1][1] = solution[4];
  matrix->Element[1][2] = solution[5];
  matrix->Element[1][3] = solution[10];
  matrix->Element[2][0] = solution[6];
  matrix->Element[2][1] = solution[7];
  matrix->Element[2][2] = solution[8];
  matrix->Element[2][3] = solution[11];
  matrix->Element[3][0] = 0;
  matrix->Element[3][1] = 0;
  matrix->Element[3][2] = 0;
  matrix->Element[3][3] = 1;
  
  self->Modified(); 
}

//----------------------------------------------------------------------------
// Update the 4x4 matrix. Updates are only done as necessary.
 
void vtkITKMutualInformationTransform::InternalUpdate()
{

  if (this->SourceImage == NULL || this->TargetImage == NULL)
    {
    this->Matrix->Identity();
    return;
    }

  // Make sure inputs are up to date
  this->SourceImage->UpdateInformation();
  this->TargetImage->UpdateInformation();

  // Handle float and double for now
  switch (this->SourceImage->GetScalarType())
    {
    case VTK_FLOAT:
      {
      if (this->TargetImage->GetScalarType() != VTK_FLOAT)
        { 
        vtkErrorMacro (<< "Target type " << this->TargetImage->GetScalarType()
                       << " must be match Source type "
                       << this->SourceImage->GetScalarType());
        this->Matrix->Identity();
        return;
        }
      
      float dummy = 0.0;
      if ((UseMattes==true)&&(QuaternionRigidMode==true))
        vtkITKMIMattesQuaternionRigidExecute(this,
                                       this->SourceImage,
                                       this->TargetImage,
                                       this->Matrix,
                                       dummy);
      else if ((UseViolaWells==true)&&(QuaternionRigidMode==true))
        vtkITKMIViolaWellsQuaternionRigidExecute(this,
                                                 this->SourceImage,
                                                 this->TargetImage,
                                                 this->Matrix,
                                                 dummy);
      else if ((UseMattes==true)&&(AffineMode==true))
        vtkITKMIMattesAffineExecute(this,
                                    this->SourceImage,
                                    this->TargetImage,
                                    this->Matrix,
                                    dummy);
      else if ((UseViolaWells==true)&&(AffineMode==true))
        vtkITKMIViolaWellsAffineExecute(this,
                                        this->SourceImage,
                                        this->TargetImage,
                                        this->Matrix,
                                        dummy);
      break;
      }
    case VTK_DOUBLE:
      {
      if (this->TargetImage->GetScalarType() != VTK_SHORT)
        { 
        vtkErrorMacro (<< "Target type " << this->TargetImage->GetScalarType()
                       << " must be match Source type "
                       << this->SourceImage->GetScalarType());
        this->Matrix->Identity();
        return;
        }
      
      double dummy = 0.0;
      if ((UseMattes==true)&&(QuaternionRigidMode==true))
        vtkITKMIMattesQuaternionRigidExecute(this,
                                       this->SourceImage,
                                       this->TargetImage,
                                       this->Matrix,
                                       dummy);
      else if ((UseViolaWells==true)&&(QuaternionRigidMode==true))
        vtkITKMIViolaWellsQuaternionRigidExecute(this,
                                                 this->SourceImage,
                                                 this->TargetImage,
                                                 this->Matrix,
                                                 dummy);
      else if ((UseMattes==true)&&(AffineMode==true))
        vtkITKMIMattesAffineExecute(this,
                                    this->SourceImage,
                                    this->TargetImage,
                                    this->Matrix,
                                    dummy);
      else if ((UseViolaWells==true)&&(AffineMode==true))
        vtkITKMIViolaWellsAffineExecute(this,
                                        this->SourceImage,
                                        this->TargetImage,
                                        this->Matrix,
                                        dummy);
      break;
      }
    default:
      {
      if (this->SourceImage->GetScalarType() != VTK_FLOAT)
        {
        vtkErrorMacro (<< "Source type " << this->SourceImage->GetScalarType()
                       << " must be float OR double");
        this->Matrix->Identity();
        return;
        }
      }
    }
}

//------------------------------------------------------------------------
void vtkITKMutualInformationTransform::Identity()
{
    this->Matrix->Identity();
}

//------------------------------------------------------------------------
void vtkITKMutualInformationTransform::Initialize(vtkLinearTransform *initial)
{
  this->Matrix->DeepCopy( initial->GetMatrix());
}

//------------------------------------------------------------------------
unsigned long vtkITKMutualInformationTransform::GetMTime()
{
  unsigned long result = this->vtkLinearTransform::GetMTime();
  unsigned long mtime;

  if (this->SourceImage)
    {
    mtime = this->SourceImage->GetMTime(); 
    if (mtime > result)
      {
      result = mtime;
      }
    }
  if (this->TargetImage)
    {
    mtime = this->TargetImage->GetMTime(); 
    if (mtime > result)
      {
      result = mtime;
      }
    }
  return result;
}
//------------------------------------------------------------------------
void vtkITKMutualInformationTransform::SetSourceImage(vtkImageData *source)
{
  if (this->SourceImage == source)
    {
    return;
    }

  if (this->SourceImage)
    {
    this->SourceImage->Delete();
    }

  source->Register(this);
  this->SourceImage = source;

  this->Modified();
}

//------------------------------------------------------------------------
void vtkITKMutualInformationTransform::SetTargetImage(vtkImageData *target)
{
  if (this->TargetImage == target)
    {
    return;
    }

  if (this->TargetImage)
    {
    this->TargetImage->Delete();
    }

  target->Register(this);
  this->TargetImage = target;
  this->Modified();
}

void vtkITKMutualInformationTransform::SetFixedImageExtent(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax)
{
  int sizeX = xMax-xMin+1;
  int sizeY = yMax-yMin+1;
  int sizeZ = zMax-zMin+1;

  SizeType size = {{sizeX, sizeY, sizeZ}};
  IndexType index = {{xMin, yMin, zMin}};
  FixedImageExtent.SetIndex(index);
  FixedImageExtent.SetSize(size);
}


//----------------------------------------------------------------------------
void vtkITKMutualInformationTransform::Inverse()
{
  vtkImageData *tmp1 = this->SourceImage;
  vtkImageData *tmp2 = this->TargetImage;
  this->TargetImage = tmp1;
  this->SourceImage = tmp2;
  this->Modified();
}

//----------------------------------------------------------------------------
vtkAbstractTransform *vtkITKMutualInformationTransform::MakeTransform()
{
  return vtkITKMutualInformationTransform::New(); 
}

//----------------------------------------------------------------------------
void vtkITKMutualInformationTransform::InternalDeepCopy(vtkAbstractTransform *transform)
{
  vtkITKMutualInformationTransform *t = (vtkITKMutualInformationTransform *)transform;

  this->SetSourceStandardDeviation(t->SourceStandardDeviation);
  this->SetTargetStandardDeviation(t->TargetStandardDeviation);
  this->SetLearningRate(t->LearningRate);
  this->SetTranslateScale(t->TranslateScale);
  this->SetNumberOfSamples(t->NumberOfSamples);    

  this->Modified();
}


