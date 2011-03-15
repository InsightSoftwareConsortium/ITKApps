/* perform a pixel-wise intensity transformation using a Normalized Correlation
 * function */

#include "vtkVVPluginAPI.h"

#include "itkCenteredTransformInitializer.h"
#include "itkImage.h"
#include "itkImageRegistrationMethod.h"
#include "itkImportImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkVersorRigid3DTransform.h"
#include "itkVersorRigid3DTransformOptimizer.h"
#include "itkResampleImageFilter.h"
#include "itkShrinkImageFilter.h"

#include <stdio.h>

// use itkRigid3DTransform instead?
// use VerserRigid3DTransform (not centered ?)
// use versor transform optimizer?

// =======================================================================
// The main class definition
// =======================================================================
template <class PixelType> class ImageRegistrationRunner
{
public:
  // define our typedefs
  typedef ImageRegistrationRunner                      Self;
  typedef itk::Image< PixelType, 3 >                   ImageType; 
  typedef itk::ImportImageFilter< PixelType, 3>        ImportFilterType;
  typedef itk::ShrinkImageFilter<ImageType, ImageType> ShrinkFilterType;
  typedef itk::VersorRigid3DTransform< double >        TransformType;
  typedef itk::VersorRigid3DTransformOptimizer         OptimizerType;
  typedef itk::NormalizedCorrelationImageToImageMetric<
    ImageType, ImageType> MetricType;
  typedef itk::LinearInterpolateImageFunction<ImageType, double> 
  InterpolatorType;
  typedef itk::ImageRegistrationMethod< ImageType, ImageType> RegistrationType;
  typedef itk::ResampleImageFilter< ImageType, ImageType > ResampleFilterType;

  typedef itk::ImageRegion<3>     RegionType;
  typedef itk::Index<3>           IndexType;
  typedef itk::Size<3>            SizeType;

  // Command/Observer intended to update the progress
  typedef itk::MemberCommand< Self >  CommandType;

  typedef itk::CenteredTransformInitializer<TransformType,
                                            ImageType, ImageType > 
  TransformInitializerType;

  // Description:
  // The funciton to call for progress of the optimizer
  void ProgressUpdate( itk::Object * caller, const itk::EventObject & event );

  // Description:
  // The constructor
  ImageRegistrationRunner();

  // Description:
  // The destructor
  virtual ~ImageRegistrationRunner() {};

  // Description:
  // Imports the two input images from Volview into ITK
  virtual void ImportPixelBuffer( vtkVVPluginInfo *info, 
                                  const vtkVVProcessDataStruct * pds );

  // Description:
  // Copies the resulting data into the output image
  virtual void CopyOutputData( vtkVVPluginInfo *info, 
                               const vtkVVProcessDataStruct * pds );

  // Description:
  // Sets up the pipeline and invokes the registration process
  int Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds );

private:
  // delare out instance variables
  typename MetricType::Pointer          m_Metric;
  typename TransformType::Pointer       m_Transform;
  typename OptimizerType::Pointer       m_Optimizer;
  typename InterpolatorType::Pointer    m_Interpolator;
  typename RegistrationType::Pointer    m_Registration;
  typename ImportFilterType::Pointer    m_ImportFilter;
  typename ImportFilterType::Pointer    m_ImportFilter2;
  typename ResampleFilterType::Pointer  m_Resample;
  typename CommandType::Pointer         m_CommandObserver;
  vtkVVPluginInfo *m_Info;
};

  
  
// =======================================================================
// progress Callback
template <class PixelType> 
void ImageRegistrationRunner<PixelType>::
ProgressUpdate( itk::Object * itkNotUsed(caller), const itk::EventObject & event )
{
  char tstr[1024];
  if( typeid( itk::IterationEvent ) == typeid( event ) )
    {
    if (m_Registration->GetFixedImage()->
        GetLargestPossibleRegion().GetNumberOfPixels() > 0.03 *  
        m_ImportFilter->GetOutput()->
        GetLargestPossibleRegion().GetNumberOfPixels())
      {
      sprintf(tstr,"Half Resolution Iteration : %i Value: %g", 
              m_Optimizer->GetCurrentIteration(),
              m_Optimizer->GetValue());
      }
    else
      {
      sprintf(tstr,"Quarter Resolution Iteration : %i Value: %g", 
              m_Optimizer->GetCurrentIteration(),
              m_Optimizer->GetValue());
      }
    m_Info->UpdateProgress(m_Info, 
                           0.8*m_Optimizer->GetCurrentIteration()/
                           m_Optimizer->GetNumberOfIterations() , tstr); 
    }
  if( typeid( itk::ProgressEvent ) == typeid( event ) )
    {
    m_Info->UpdateProgress(m_Info,0.8 + 0.2*m_Resample->GetProgress(), 
                           "Resampling..."); 
    }
}

// =======================================================================
// Constructor
template <class PixelType> 
ImageRegistrationRunner<PixelType>::ImageRegistrationRunner() 
{
  m_CommandObserver    = CommandType::New();
  m_CommandObserver->SetCallbackFunction( 
    this, &ImageRegistrationRunner::ProgressUpdate );
  m_ImportFilter  = ImportFilterType::New();
  m_ImportFilter2 = ImportFilterType::New();
  m_Metric = MetricType::New();
  m_Transform = TransformType::New();
  m_Optimizer = OptimizerType::New();
  m_Optimizer->MinimizeOn();
  m_Interpolator = InterpolatorType::New();
  m_Registration = RegistrationType::New();
  m_Resample = ResampleFilterType::New();
  m_Resample->AddObserver( itk::ProgressEvent(), m_CommandObserver );
  
  m_Registration->SetMetric( m_Metric );
  m_Registration->SetOptimizer( m_Optimizer );
  m_Registration->SetTransform( m_Transform );
  m_Registration->SetInterpolator( m_Interpolator );
  m_Optimizer->AddObserver( itk::IterationEvent(), m_CommandObserver );
}

// =======================================================================
// Import data
template <class PixelType> 
void ImageRegistrationRunner<PixelType>::
ImportPixelBuffer( vtkVVPluginInfo *info, const vtkVVProcessDataStruct * pds )
{
  SizeType   size;
  IndexType  start;
  
  double     origin[3];
  double     spacing[3];
  
  size[0]     =  info->InputVolumeDimensions[0];
  size[1]     =  info->InputVolumeDimensions[1];
  size[2]     =  info->InputVolumeDimensions[2];
  
  for(unsigned int i=0; i<3; i++)
    {
    origin[i]   =  info->InputVolumeOrigin[i];
    spacing[i]  =  info->InputVolumeSpacing[i];
    start[i]    =  0;
    }
  
  RegionType region;
  
  region.SetIndex( start );
  region.SetSize(  size  );
  
  m_ImportFilter->SetSpacing( spacing );
  m_ImportFilter->SetOrigin(  origin  );
  m_ImportFilter->SetRegion(  region  );
  
  unsigned int totalNumberOfPixels = region.GetNumberOfPixels();
  unsigned int numberOfComponents = info->InputVolumeNumberOfComponents;
  unsigned int numberOfPixelsPerSlice = size[0] * size[1];
  
  PixelType *dataBlockStart = static_cast< PixelType * >( pds->inData );
  
  m_ImportFilter->SetImportPointer( dataBlockStart, 
                                    totalNumberOfPixels, false);
  
  size[0]     =  info->InputVolume2Dimensions[0];
  size[1]     =  info->InputVolume2Dimensions[1];
  size[2]     =  info->InputVolume2Dimensions[2];
  
  for(unsigned int i=0; i<3; i++)
    {
    origin[i]   =  info->InputVolume2Origin[i];
    spacing[i]  =  info->InputVolume2Spacing[i];
    start[i]    =  0;
    }
  
  region.SetIndex( start );
  region.SetSize(  size  );
  
  m_ImportFilter2->SetSpacing( spacing );
  m_ImportFilter2->SetOrigin(  origin  );
  m_ImportFilter2->SetRegion(  region  );
  
  totalNumberOfPixels = region.GetNumberOfPixels();
  numberOfComponents = info->InputVolume2NumberOfComponents;
  numberOfPixelsPerSlice = size[0] * size[1];
  
  dataBlockStart = static_cast< PixelType * >( pds->inData2 );
  
  m_ImportFilter2->SetImportPointer( dataBlockStart, 
                                     totalNumberOfPixels, false);
} 

  
// =======================================================================
//  Copy the output data into the volview data structure 
template <class PixelType> 
void ImageRegistrationRunner<PixelType>::
CopyOutputData( vtkVVPluginInfo *info, const vtkVVProcessDataStruct * pds )
{
  // get some useful info
  unsigned int numberOfComponents = info->OutputVolumeNumberOfComponents;
  typedef itk::ImageRegionConstIterator< ImageType > OutputIteratorType;
  PixelType * outData = static_cast< PixelType * >( pds->outData );
    
  // do we append or replace
  const char *result = info->GetGUIProperty(info, 1, VVP_GUI_VALUE);
  if (result && !strcmp(result,"Append The Volumes"))
    {
    // Copy the data (with casting) to the output buffer
    typename ImageType::ConstPointer fixedImage = m_ImportFilter->GetOutput();
    OutputIteratorType ot( fixedImage, fixedImage->GetBufferedRegion() );
    
    // copy the input image
    ot.GoToBegin(); 
    while( !ot.IsAtEnd() )
      {
      *outData = ot.Get();
      ++ot;
      outData += numberOfComponents;
      }
    outData = static_cast< PixelType * >( pds->outData ) + 1;
    }
  
  // Copy the data (with casting) to the output buffer 
  typename ImageType::ConstPointer sampledImage = m_Resample->GetOutput();
  OutputIteratorType ot2( sampledImage, 
                          sampledImage->GetBufferedRegion() );
  
  // copy the registered image
  ot2.GoToBegin(); 
  while( !ot2.IsAtEnd() )
    {
    *outData = ot2.Get();
    ++ot2;
    outData += numberOfComponents;
    }
} 


// =======================================================================
// Main execute method
template <class PixelType> 
int ImageRegistrationRunner<PixelType>::
Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
{
  m_Info = info;
  m_Optimizer->SetNumberOfIterations(
    atoi( info->GetGUIProperty(info, 0, VVP_GUI_VALUE )));
  
  this->ImportPixelBuffer( info, pds );  
  
  // Execute the filter
  m_ImportFilter->Update();
  m_ImportFilter2->Update();

  
  // first do it on reduced resolution images
  typename ShrinkFilterType::Pointer shrink1 = ShrinkFilterType::New();
  typename ShrinkFilterType::Pointer shrink2 = ShrinkFilterType::New();
  
  shrink1->SetInput( m_ImportFilter->GetOutput() );
  shrink1->SetShrinkFactors(4);
  shrink1->Update();
  shrink2->SetInput( m_ImportFilter2->GetOutput() );
  shrink2->SetShrinkFactors(4);
  shrink2->Update();
  
  m_Registration->SetFixedImage( shrink1->GetOutput() );
  m_Registration->SetMovingImage( shrink2->GetOutput() );

  // setup the initializer
  typename TransformInitializerType::Pointer initializer = 
    TransformInitializerType::New();  
  
  m_Transform->SetIdentity();
  initializer->SetTransform( m_Transform );
  initializer->SetFixedImage( shrink1->GetOutput() );
  initializer->SetMovingImage( shrink2->GetOutput() );
  initializer->MomentsOn();
  initializer->InitializeTransform();
  
  typename RegistrationType::ParametersType initParams = 
                                    m_Transform->GetParameters();      

  m_Registration->SetInitialTransformParameters( initParams );

  typename OptimizerType::ScalesType optimizerScales(
                            m_Transform->GetNumberOfParameters());

  optimizerScales[0] = 1.0;
  optimizerScales[1] = 1.0;
  optimizerScales[2] = 1.0;
  optimizerScales[3] = 1.0/
    (10.0*info->InputVolumeSpacing[0]*info->InputVolumeDimensions[0]);
  optimizerScales[4] = 1.0/
    (10.0*info->InputVolumeSpacing[1]*info->InputVolumeDimensions[1]);
  optimizerScales[5] = 1.0/
    (10.0*info->InputVolumeSpacing[2]*info->InputVolumeDimensions[2]);
  m_Optimizer->SetScales(optimizerScales);
  
  m_Optimizer->SetMaximumStepLength(1.0);
  m_Optimizer->SetMinimumStepLength(0.01);
  
  info->UpdateProgress(info,0.0,"Starting Registration ...");      
  try
    {
    m_Registration->StartRegistration();
    }
  catch( itk::ExceptionObject )
    {
    return 1;
    }

  // if we converged without using all of the iterations then increase he
  // resolution to a half resolution volume and continue
  int totalIterations = m_Optimizer->GetCurrentIteration();
  if (m_Optimizer->GetCurrentIteration() <
      m_Optimizer->GetNumberOfIterations())
    {
    info->UpdateProgress(info,0.8*m_Optimizer->GetCurrentIteration()/
                         m_Optimizer->GetNumberOfIterations(),
                         "Starting Half Resolution Registration ...");      
    shrink1->SetShrinkFactors(2);
    shrink1->Update();
    shrink2->SetShrinkFactors(2);
    shrink2->Update();
    m_Registration->SetInitialTransformParameters( 
      m_Registration->GetLastTransformParameters() );
    
    m_Optimizer->SetMaximumStepLength(0.2);
    m_Optimizer->SetMinimumStepLength(0.002);
    m_Optimizer->SetNumberOfIterations( 
            atoi( info->GetGUIProperty(info, 0, VVP_GUI_VALUE )) - 
            m_Optimizer->GetCurrentIteration());
    try
      {
      m_Registration->StartRegistration();
      }
    catch( itk::ExceptionObject )
      {
      return 1;
      }
    totalIterations += m_Optimizer->GetCurrentIteration();
    }
  
  // now get the resulting parameters
  typename RegistrationType::ParametersType finalParameters = 
    m_Registration->GetLastTransformParameters();
  
  typename TransformType::Pointer finalTransform = TransformType::New();
  finalTransform->SetParameters( finalParameters );
  finalTransform->SetCenter(m_Transform->GetCenter());
  
  m_Resample->SetTransform( finalTransform );
  m_Resample->SetInput( m_ImportFilter2->GetOutput() );
  m_Resample->SetSize( 
    m_ImportFilter->GetOutput()->GetLargestPossibleRegion().GetSize());
  m_Resample->SetOutputOrigin( m_ImportFilter->GetOutput()->GetOrigin() );
  m_Resample->SetOutputSpacing( m_ImportFilter->GetOutput()->GetSpacing());
  m_Resample->SetDefaultPixelValue(0);
  
  info->UpdateProgress(info,0.8,"Starting Resample ...");      
  m_Resample->Update();
  
  this->CopyOutputData( info, pds );
  
  // set some output information,
  char results[1024];

  typedef TransformType::VersorType VersorType;
  VersorType versor = finalTransform->GetVersor();
  TransformType::OffsetType offset = finalTransform->GetOffset();
  typedef VersorType::VectorType   AxisType;
  AxisType axis = versor.GetAxis();

  sprintf(results,"Number of Iterations Used: %d\nTranslation: %g %g %g\nRotation Axis %f %f %f %f\nOffset: %g %g %g", 
          m_Optimizer->GetCurrentIteration(),
          finalParameters[3],
          finalParameters[4],
          finalParameters[5],
          axis[0],
          axis[1],
          axis[2],
          versor.GetAngle(),
          offset[0],
          offset[1],
          offset[2]
          );
  info->SetProperty(info, VVP_REPORT_TEXT, results);
  
  return 0;
}


static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;
  
  // do some error checking
  if (info->InputVolumeScalarType != info->InputVolume2ScalarType)
    {
    info->SetProperty(
      info, VVP_ERROR,
      "The two inputs do not appear to be of the same data type.");
    return 1;
    }
  
  if (info->InputVolumeNumberOfComponents != 1 ||
      info->InputVolume2NumberOfComponents != 1)
    {
    info->SetProperty(
      info, VVP_ERROR, "The two input volumes must be single component.");
    return 1;
    }

  int result = 0;
  
  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_CHAR:
      {
      ImageRegistrationRunner<signed char> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      ImageRegistrationRunner<unsigned char> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    case VTK_SHORT:
      {
      ImageRegistrationRunner<signed short> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      ImageRegistrationRunner<unsigned short> runner;
      result = runner.Execute( info, pds );
      break; 
      }
#if !( defined(_MSC_VER) && (_MSC_VER <= 1300 ) ) 
      // Too many instantiations causing compiler limit on 
      // VS70 and VS6 and below So remove a few datatypes for 
      // these compilers. If you feel like to need these datatypes,
      // uncomment them and remove some others..
    case VTK_INT:
      {
      ImageRegistrationRunner<signed int> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_INT:
      {
      ImageRegistrationRunner<unsigned int> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    case VTK_LONG:
      {
      ImageRegistrationRunner<signed long> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_LONG:
      {
      ImageRegistrationRunner<unsigned long> runner;
      result = runner.Execute( info, pds );
      break; 
      }
#endif
    case VTK_FLOAT:
      {
      ImageRegistrationRunner<float> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    }
  }
  catch( itk::ExceptionObject & except )
    {
    info->SetProperty( info, VVP_ERROR, except.what() ); 
    return -1;
    }
  return result;
}


static int UpdateGUI(void *inf)
{
  char tmp[1024];
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Maximum Total Iterations");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "30");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "How many iterations to run for the ");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "20 300 1");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Output Format");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_CHOICE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT , "Append The Volumes");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP,
                       "How do you want the output stored? There are two choices here. Appending creates a single output volume that has two components, the first component from the input volume and the second component is from the registered second input. The second choice is to Relace the current volume. In this case the Registered second input replaces the original volume.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS, "2\nAppend The Volumes\nReplace The Current Volume");

  info->OutputVolumeScalarType = info->InputVolumeScalarType;
  memcpy(info->OutputVolumeDimensions,info->InputVolumeDimensions,
         3*sizeof(int));
  memcpy(info->OutputVolumeSpacing,info->InputVolumeSpacing,
         3*sizeof(float));
  memcpy(info->OutputVolumeOrigin,info->InputVolumeOrigin,
         3*sizeof(float));

  // really the memory consumption is one copy of the resampled output for
  // the resample filter plus the gradient for the 1/8th res volume plus the
  // two 1/8th res resampled inputs
  sprintf(tmp,"%f",
          info->InputVolumeScalarSize + 3.0*sizeof(float)/8.0 + 0.5);
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED, tmp); 

  // what output format is selected
  const char *result = info->GetGUIProperty(info, 1, VVP_GUI_VALUE);
  if (result && !strcmp(result,"Append The Volumes"))
    {
    info->OutputVolumeNumberOfComponents = 
      info->InputVolumeNumberOfComponents + 
      info->InputVolume2NumberOfComponents;
    }
  else
    {
    info->OutputVolumeNumberOfComponents =
      info->InputVolume2NumberOfComponents;
    }
  
  return 1;
}


extern "C" {
  
void VV_PLUGIN_EXPORT vvITKImageRegistrationInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Image Registration (ITK)");
  info->SetProperty(info, VVP_GROUP, "Registration");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                            "Register two images using Normalized Correlation");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filter takes two volumes and registers them. There are two choices for the output format. Appending creates a single output volume that has two components, the first component is from the input volume and the second component is from the registered and resampled second input volume. The second choice is to Replace the current volume. In this case the registered and resampled second input replaces the original volume. The two input volumes must have one component and be of the same data type. The registration is done on quarter resolution volumes first (one quarter on each axis) and then if that converges the registration continues with one half resolution volumes. The optimization is done using a regular gradient descent optimizer with a centered quaternion and rigid translation based transform. The error metric is a normalized correlation metric.");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "2");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "0"); 
  info->SetProperty(info, VVP_REQUIRES_SECOND_INPUT,        "1");
}

}
