/* Warps an image using a KernelSpline for interpolating landmark-based displacements */

#include "vtkVVPluginAPI.h"

#include "itkImage.h"
#include "itkImageRegistrationMethod.h"
#include "itkImportImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkElasticBodySplineKernelTransform.h"


// =======================================================================
// The main class definition
// =======================================================================
template <class PixelType> class LandmarkWarpingRunner
{
public:
  // define our typedefs
  typedef itk::Image< PixelType, 3 >                              ImageType; 
  typedef itk::ImportImageFilter< PixelType, 3>                   ImportFilterType;
  typedef itk::LinearInterpolateImageFunction<ImageType, double>  InterpolatorType;
  typedef itk::ResampleImageFilter< ImageType, ImageType >        ResampleFilterType;
  typedef itk::ElasticBodySplineKernelTransform< double, 3 >      TransformType;

  typedef TransformType::PointSetType         PointSetType;
  typedef PointSetType::PointsContainer       LandmarkContainer;
  typedef LandmarkContainer::Pointer          LandmarkContainerPointer;
  typedef LandmarkContainer::Iterator         LandmarkIterator;
  typedef PointSetType::PointType             LandmarkType;

  typedef float                   MarkersCoordinatesType;

  typedef itk::ImageRegion<3>     RegionType;
  typedef itk::Index<3>           IndexType;
  typedef itk::Size<3>            SizeType;

  // Description:
  // The funciton to call for progress of the optimizer
  void ProgressUpdate( itk::Object * caller, const itk::EventObject & event );

  // Description:
  // The constructor
  LandmarkWarpingRunner();

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
  typename InterpolatorType::Pointer    m_Interpolator;
  typename ImportFilterType::Pointer    m_ImportFilter;
  typename ImportFilterType::Pointer    m_ImportFilter2;
  typename ResampleFilterType::Pointer  m_Resample;
  typename TransformType::Pointer       m_Transform;
  typename PointSetType::Pointer        m_SourceLandmarks;
  typename PointSetType::Pointer        m_TargetLandmarks;
  vtkVVPluginInfo *m_Info;
};

  
  
// =======================================================================
// progress Callback
template <class PixelType> 
void LandmarkWarpingRunner<PixelType>::
ProgressUpdate( itk::Object * caller, const itk::EventObject & event )
{
  if( typeid( itk::ProgressEvent ) == typeid( event ) )
    {
    m_Info->UpdateProgress(m_Info, m_Resample->GetProgress(), "Resampling..."); 
    }
}

// =======================================================================
// Constructor
template <class PixelType> 
LandmarkWarpingRunner<PixelType>::LandmarkWarpingRunner() 
{
  m_ImportFilter    = ImportFilterType::New();
  m_ImportFilter2   = ImportFilterType::New();
  m_Interpolator    = InterpolatorType::New();
  m_Resample        = ResampleFilterType::New();
  m_Transform       = TransformType::New(); 
  m_SourceLandmarks = PointSetType::New();
  m_TargetLandmarks = PointSetType::New();
}

// =======================================================================
// Import data
template <class PixelType> 
void LandmarkWarpingRunner<PixelType>::
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
void LandmarkWarpingRunner<PixelType>::
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
int LandmarkWarpingRunner<PixelType>::
Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
{
  m_Info = info;

  const unsigned int numberOfLandmarks = info->NumberOfMarkers;

  if( numberOfLandmarks == 0 )
    {
    info->SetProperty( info, VVP_ERROR, "This plugins requires the user to provide a set of 3D markers pairs"); 
    return -1;
    }

  if( ( numberOfLandmarks & 1 ) == 0 )
    {
    info->SetProperty( info, VVP_ERROR, "The number of 3D markers must be even. They should form pairs"); 
    return -1;
    }


  LandmarkContainerPointer sourceLandmarks   = m_SourceLandmarks->GetPoints();
  LandmarkContainerPointer targetLandmarks   = m_TargetLandmarks->GetPoints();

  sourceLandmarks->Reserve( numberOfLandmarks / 2 );
  targetLandmarks->Reserve( numberOfLandmarks / 2 );

  LandmarkIterator         sourceLandmarkItr = sourceLandmarks->Begin();
  LandmarkIterator         targetLandmarkItr = targetLandmarks->Begin();


  LandmarkType landmark;

  unsigned int landmarkId = 0;

  const MarkersCoordinatesType * markersCoordinates = info->Markers;

  for( unsigned int i=0; i < numberOfLandmarks; i++ )
    {
    landmark[0] = *markersCoordinates++; 
    landmark[1] = *markersCoordinates++;
    landmark[2] = *markersCoordinates++;
    sourceLandmarks->InsertElement( landmarkId, landmark );
    landmark[0] = *markersCoordinates++; 
    landmark[1] = *markersCoordinates++;
    landmark[2] = *markersCoordinates++;
    targetLandmarks->InsertElement( landmarkId, landmark );
    landmarkId++;
    }

  m_Transform->SetTargetLandmarks( m_TargetLandmarks );
  m_Transform->SetSourceLandmarks( m_SourceLandmarks );

  m_Transform->ComputeWMatrix();


  this->ImportPixelBuffer( info, pds );  
  
  // Execute the filter
  m_ImportFilter->Update();
  m_ImportFilter2->Update();

  m_Resample->SetTransform( m_Transform );
  m_Resample->SetInput( m_ImportFilter2->GetOutput() );
  m_Resample->SetSize( 
    m_ImportFilter->GetOutput()->GetLargestPossibleRegion().GetSize());
  m_Resample->SetOutputOrigin( m_ImportFilter->GetOutput()->GetOrigin() );
  m_Resample->SetOutputSpacing( m_ImportFilter->GetOutput()->GetSpacing());
  m_Resample->SetDefaultPixelValue(0);
  
  info->UpdateProgress(info,0,"Starting Resample ...");      
  m_Resample->Update();
  
  this->CopyOutputData( info, pds );
  

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
      LandmarkWarpingRunner<signed char> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      LandmarkWarpingRunner<unsigned char> runner;
      result = runner.Execute( info, pds );
      break; 
      }
/*   ONLY DO A COUPLE OF TYPES WHILE WE DEBUG THIS
    case VTK_SHORT:
      {
      LandmarkWarpingRunner<signed short> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      LandmarkWarpingRunner<unsigned short> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    case VTK_INT:
      {
      LandmarkWarpingRunner<signed int> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_INT:
      {
      LandmarkWarpingRunner<unsigned int> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    case VTK_LONG:
      {
      LandmarkWarpingRunner<signed long> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_LONG:
      {
      LandmarkWarpingRunner<unsigned long> runner;
      result = runner.Execute( info, pds );
      break; 
      }
    case VTK_FLOAT:
      {
      LandmarkWarpingRunner<float> runner;
      result = runner.Execute( info, pds );
      break; 
      }
*/
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

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Output Format");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_CHOICE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT , "Append The Volumes");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP,
                       "How do you want the output stored? There are two choices here. Appending creates a single output volume that has two components, the first component from the input volume and the second component is from the registered second input. The second choice is to Relace the current volume. In this case the Registered second input replaces the original volume.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS, "2\nAppend The Volumes\nReplace The Current Volume");

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
  
void VV_PLUGIN_EXPORT vvITKLandmarkWarpingInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Image Warping (ITK)");
  info->SetProperty(info, VVP_GROUP, "Registration");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                            "Warps one image into the space of the other using landmarks");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filter takes two volumes and a set of landmark pairs. The landmarks are used for computing a deformation field interpolated with KernelSplines. One of the images is mapped through the deformation field into the space of the other image.");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "2");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "0"); 
  info->SetProperty(info, VVP_REQUIRES_SECOND_INPUT,        "1");
}

}
