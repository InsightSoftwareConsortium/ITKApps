/* perform a pixel-wise intensity transformation using a IntensityWindowing function */

#include "vvITKFilterModule.h"

#include "itkIntensityWindowingImageFilter.h"

static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  const unsigned int Dimension = 3;

  const float windowMinimum  = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
  const float windowMaximum  = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );
  const float outputMinimum  = atof( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ) );
  const float outputMaximum  = atof( info->GetGUIProperty(info, 3, VVP_GUI_VALUE ) );

  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_UNSIGNED_CHAR:
      {
      typedef  unsigned char                        PixelType;
      typedef  itk::Image< PixelType, Dimension >   ImageType; 
      typedef  itk::IntensityWindowingImageFilter< ImageType,  ImageType >   FilterType;
      VolView::PlugIn::FilterModule< FilterType > module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Transforming intensities with a IntensityWindowing function...");
      // Set the parameters on it
      module.GetFilter()->SetWindowMinimum( static_cast<PixelType>( windowMinimum  ) );
      module.GetFilter()->SetWindowMaximum( static_cast<PixelType>( windowMaximum  ) );
      module.GetFilter()->SetOutputMinimum( static_cast<PixelType>( outputMinimum  ) );
      module.GetFilter()->SetOutputMaximum( static_cast<PixelType>( outputMaximum  ) );
      // Execute the filter
      module.ProcessData( pds  );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      typedef  unsigned short                       PixelType;
      typedef  itk::Image< PixelType, Dimension >   ImageType; 
      typedef  itk::IntensityWindowingImageFilter< ImageType,  ImageType >   FilterType;
      VolView::PlugIn::FilterModule< FilterType > module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Transforming intensities with a IntensityWindowing function...");
      // Set the parameters on it
      module.GetFilter()->SetWindowMinimum( static_cast<PixelType>( windowMinimum  ) );
      module.GetFilter()->SetWindowMaximum( static_cast<PixelType>( windowMaximum  ) );
      module.GetFilter()->SetOutputMinimum( static_cast<PixelType>( outputMinimum  ) );
      module.GetFilter()->SetOutputMaximum( static_cast<PixelType>( outputMaximum  ) );
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

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Window Minimum");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarMinimum( info ) );
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Desired value for the minimum intensity of the input window.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS, VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRange( info ) );

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Window Maximum");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarMaximum( info ) );
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Desired value for the maximum intensity of the input window.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS, VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRange( info ) );

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL, "Output Minimum");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarTypeMinimum( info ) );
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "Desired value for the minimum intensity of the output image.");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS, VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarTypeRange( info ) );

  info->SetGUIProperty(info, 3, VVP_GUI_LABEL, "Output Maximum");
  info->SetGUIProperty(info, 3, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 3, VVP_GUI_DEFAULT, VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarTypeMaximum( info ) );
  info->SetGUIProperty(info, 3, VVP_GUI_HELP, "Desired value for the maximum intensity of the output image.");
  info->SetGUIProperty(info, 3, VVP_GUI_HINTS, VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarTypeRange( info ) );

  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, "0");
  
  info->OutputVolumeScalarType = 
                       info->InputVolumeScalarType;
  info->OutputVolumeNumberOfComponents = 
                       info->InputVolumeNumberOfComponents;
  memcpy(info->OutputVolumeDimensions,info->InputVolumeDimensions,
         3*sizeof(int));
  memcpy(info->OutputVolumeSpacing,info->InputVolumeSpacing,
         3*sizeof(float));
  memcpy(info->OutputVolumeOrigin,info->InputVolumeOrigin,
         3*sizeof(float));

  return 1;
}


extern "C" {
  
void VV_PLUGIN_EXPORT vvITKIntensityWindowingInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Intensity Windowing (ITK)");
  info->SetProperty(info, VVP_GROUP, "Intensity Transformation");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                "Intensity Windowing Transform");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filters applies a pixel-wise intensity transform by using a IntensityWindowing function");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "1");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "4");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "1"); // actually depends on pixel size

}

}
