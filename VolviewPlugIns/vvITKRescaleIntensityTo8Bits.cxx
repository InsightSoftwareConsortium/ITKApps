/* perform a pixel-wise intensity transformation using a RescaleIntensityTo8Bits function */

#include "vvITKFilterModule.h"

#include "itkIntensityWindowingImageFilter.h"



template <class InputPixelType>
class RescaleIntensityTo8BitsRunner
  {
  public:
      typedef  InputPixelType                       PixelType;
      typedef  itk::Image< PixelType, 3 >           ImageType; 
      typedef  unsigned char                        OutputPixelType;
      typedef  itk::Image< OutputPixelType, 3 >     OutputImageType; 
      typedef  itk::IntensityWindowingImageFilter< 
                       ImageType,  OutputImageType >   FilterType;
      typedef  VolView::PlugIn::FilterModule< FilterType >        ModuleType;

  public:
    RescaleIntensityTo8BitsRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      const float windowMinimum  = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
      const float windowMaximum  = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );

      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Transforming intensities with a RescaleIntensityTo8Bits function...");
      // Set the parameters on it
      module.GetFilter()->SetWindowMinimum( static_cast<PixelType>( windowMinimum  ) );
      module.GetFilter()->SetWindowMaximum( static_cast<PixelType>( windowMaximum  ) );
      module.GetFilter()->SetOutputMinimum(   0 );
      module.GetFilter()->SetOutputMaximum( 255 );
      // Execute the filter
      module.ProcessData( pds  );
    }
  };




static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  try 
  {
  switch( info->InputVolumeScalarType )
  {
    case VTK_CHAR:
      {
      RescaleIntensityTo8BitsRunner<signed char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      RescaleIntensityTo8BitsRunner<unsigned char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_SHORT:
      {
      RescaleIntensityTo8BitsRunner<signed short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      RescaleIntensityTo8BitsRunner<unsigned short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_INT:
      {
      RescaleIntensityTo8BitsRunner<signed int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_INT:
      {
      RescaleIntensityTo8BitsRunner<unsigned int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_LONG:
      {
      RescaleIntensityTo8BitsRunner<signed long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_LONG:
      {
      RescaleIntensityTo8BitsRunner<unsigned long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_FLOAT:
      {
      RescaleIntensityTo8BitsRunner<float> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_DOUBLE:
      {
      RescaleIntensityTo8BitsRunner<double> runner;
      runner.Execute( info, pds );
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

  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, "0");
  
  info->OutputVolumeScalarType = VTK_UNSIGNED_CHAR;
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
  
void VV_PLUGIN_EXPORT vvITKRescaleIntensityTo8BitsInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Rescale Intensity to 8 bits (ITK)");
  info->SetProperty(info, VVP_GROUP, "Intensity Transformation");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                "Rescale intensities to an 8 bits range ");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filters applies a pixel-wise intensity transform by using a RescaleIntensity function. The goal is to produce an image of 8bits/pixel in which the intensity range selected by the user gets rescaled to the 8 bits range of [0:255].");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "1");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "2");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "0"); 
}

}

