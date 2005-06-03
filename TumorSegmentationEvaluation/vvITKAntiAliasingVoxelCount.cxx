/* ISIS Center Georgetown University
This plugin adapts the GeodesicActveContour filter.
It does not perform any preprocessing. The user should provide
the speed image and the initial level set as inputs. */

#include "vvITKAntiAliasingVoxelCount.h"

template <class InputPixelType>
void vvVolumeTemplate(vtkVVPluginInfo *info,
                             vtkVVProcessDataStruct *pds, 
                             InputPixelType *)
{

  typedef itk::Image< InputPixelType, 3 > InputImageType;

  typedef VolView::PlugIn::AntiAliasedVoxelCount< InputImageType>   ModuleType;

  const unsigned int maxNumberOfIterations = atoi( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
  const float        maximumRMSError       = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );
  char tmp[1024];
  ModuleType  module;
  module.SetPluginInfo( info );
  module.SetUpdateMessage("Reducing aliasing effects...");
  module.ProcessData( pds  );
  
  float Volume= module.GetFilter()->GetSum();
  float max= module.GetFilter()->GetMaximum();
  float min= module.GetFilter()->GetMinimum();
  float mean= module.GetFilter()->GetMean();
  InputImageType::SpacingType spacing =module.GetInput()->GetSpacing();
  double factor = 1.0;
  for(unsigned int d=0; d<3; d++)
    {
    factor *= spacing[d];
    }
  sprintf(tmp,"%f mm^3",((Volume/mean*max+Volume)*factor)/(max-min));
}

static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  // make sure there is only one component of input data
  if (info->InputVolumeNumberOfComponents != 1)
    {
    info->SetProperty( info, VVP_ERROR, 
                       "The AntiAlias filter only works with single component data" ); 
    return -1;
    }
  try
    {
    switch (info->InputVolumeScalarType)
      {
      // invoke the appropriate templated function
      vtkTemplateMacro3(vvVolumeTemplate, info, pds, 
                      static_cast<VTK_TT *>(0));
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

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Number of Iterations ");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "5");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Number of times that the diffusion approximation will be computed. The more iterations, the stronger the smoothing");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "1 100 1");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Maximum RMS Error");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, "0.05");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Maximum RMS error allows. This value defines the convergence criterion for the smoothing.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , "0.001 0.1 0.001");

  const char * stringValue = info->GetGUIProperty(info, 0, VVP_GUI_VALUE );
  if( !stringValue )
    {
    info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, "0");
    }
  else
    {
    info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, stringValue);
    }
  
  info->OutputVolumeScalarType = VTK_UNSIGNED_CHAR;
  info->OutputVolumeNumberOfComponents = 1;

  info->OutputVolumeDimensions[0] = info->InputVolumeDimensions[0]; 
  info->OutputVolumeDimensions[1] = info->InputVolumeDimensions[1];
  info->OutputVolumeDimensions[2] = info->InputVolumeDimensions[2];

  memcpy(info->OutputVolumeSpacing,info->InputVolumeSpacing,
         3*sizeof(float));
  memcpy(info->OutputVolumeOrigin,info->InputVolumeOrigin,
         3*sizeof(float));

  return 1;
}

extern "C" {
  
void VV_PLUGIN_EXPORT vvITKAntiAliasingVoxelCountInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  //setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "AntiAliasedVoxelCounting");
  info->SetProperty(info, VVP_GROUP, "ISIS - Volume comparison");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                  "Computing antialiased volume");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filter combines antialiased filter with voxel counting");

  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "2");
  //info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "8"); 

}

}
