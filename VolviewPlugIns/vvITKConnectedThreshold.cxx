/* perform segmentation using the confidence connected image filter */

#include "vvITKFilterModule.h"

#include "itkConnectedThresholdImageFilter.h"

static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  const unsigned int Dimension = 3;

  const float lower         = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
  const float upper         = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );
  const int   replaceValue  = atoi( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ) );
  const unsigned int initialRadius      = atoi( info->GetGUIProperty(info, 3, VVP_GUI_VALUE ) );

   if( info->NumberOfMarkers < 1 )
    {
    info->SetProperty( info, VVP_ERROR, "Please select a seed point using the 3D Markers in the Annotation menu" ); 
    return -1;
    }

  // Take the first marker as the seed point
  const float * seedCoordinates = info->Markers;

  itk::Index<Dimension> seed;
  seed[0] =  static_cast< int >( seedCoordinates[0] );
  seed[1] =  static_cast< int >( seedCoordinates[1] );
  seed[2] =  static_cast< int >( seedCoordinates[2] );


  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_UNSIGNED_CHAR:
      {
      typedef  unsigned char                        PixelType;
      typedef  itk::Image< PixelType, Dimension >   ImageType; 
      typedef  ImageType::IndexType                 IndexType;
      typedef  itk::ConnectedThresholdImageFilter<  ImageType,  ImageType >   FilterType;
      VolView::PlugIn::FilterModule< FilterType > module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Threshold Connected Region Growing...");
      // Set the parameters on it
      module.GetFilter()->SetUpper( static_cast<PixelType>( upper  ) );
      module.GetFilter()->SetLower( static_cast<PixelType>( lower ) );
      module.GetFilter()->SetReplaceValue( replaceValue );
      module.GetFilter()->SetSeed( seed );
      // Execute the filter
      module.ProcessData( pds  );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      typedef  unsigned short                       PixelType;
      typedef  itk::Image< PixelType, Dimension >   ImageType; 
      typedef  ImageType::IndexType                 IndexType;
      typedef  itk::ConnectedThresholdImageFilter< ImageType,  ImageType >   FilterType;
      VolView::PlugIn::FilterModule< FilterType > module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Threshold Connected Region Growing...");
      // Set the parameters on it
      module.GetFilter()->SetUpper( static_cast<PixelType>( upper  ) );
      module.GetFilter()->SetLower( static_cast<PixelType>( lower ) );
      module.GetFilter()->SetReplaceValue( replaceValue );
      module.GetFilter()->SetSeed( seed );
      // Execute the filter
      module.ProcessData( pds );
      break; 
      }
    }
  }
  catch( itk::ExceptionObject & except )
  {
    info->SetProperty( info, VVP_ERROR, except.what() ); 
    return -1;
  }
  return 0;
}


static int UpdateGUI(void *inf)
{
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL,"Upper Threshold");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT,  VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarMaximum( info ) );
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Upper threshold. Only pixels with intensities lower than this threshold will be considered to be added to the region.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRange( info ) );

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL,"Lower Threshold");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT,  VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarMinimum( info ) );
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Lower threshold. Only pixels with intensities higher than this threshold will be considered to be added to the region.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRange( info ) );

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL, "Replace Value");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "255");
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "Value to assign to the binary mask of the segmented region. The rest of the image will be set to zero.");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , "1 255.0 1.0");

  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, "0");
  
 
  info->OutputVolumeScalarType = info->InputVolumeScalarType;
  info->OutputVolumeNumberOfComponents = 1;
  memcpy(info->OutputVolumeDimensions,info->InputVolumeDimensions,
         3*sizeof(int));
  memcpy(info->OutputVolumeSpacing,info->InputVolumeSpacing,
         3*sizeof(float));
  memcpy(info->OutputVolumeOrigin,info->InputVolumeOrigin,
         3*sizeof(float));

  return 1;
}

extern "C" {
  
void VV_PLUGIN_EXPORT vvITKConnectedThresholdInit(vtkVVPluginInfo *info)
{
  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Connected Threshold (ITK) ");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Connected Threshold Segmentation");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filter applies an region growing algorithm for segmentation. The criterion for including new pixels in the region is defined by an intensity range whose bound are provided by the user. These bounds are described as the lower and upper thresholds.");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "3");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "4");// It is actually dependent of the complexity of the shape to segment

}

}
