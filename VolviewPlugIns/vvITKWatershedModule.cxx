/* This module encapsulates the full preprocessing required for
   applying the Watershed image filter for segmenting the 
   volume.  It requires seed points and the original image as inputs. */


#include "vvITKWatershedModule.txx"


template <class InputPixelType>
class WatershedModuleRunner
  {
  public:
      typedef VolView::PlugIn::WatershedModule< InputPixelType >   ModuleType;

  public:
    WatershedModuleRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      const float sigma          = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ));
      const float threshold      = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ));
      const float waterLevel     = atof( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ));

      const unsigned int numberOfSeeds = info->NumberOfMarkers;

      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Computing Watershed Module...");
      module.SetSigma( sigma );
      module.SetThreshold( threshold );
      module.SetWaterLevel( waterLevel ); 
      itk::Index<3> seedPosition;
      for(unsigned int i=0; i< numberOfSeeds; i++)
        {
        VolView::PlugIn::FilterModuleBase::Convert3DMarkerToIndex( info, i, seedPosition );
        module.AddSeed( seedPosition );
        }
      // Execute the filter
      module.ProcessData( pds  );
    }
  };



static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  if( info->InputVolumeNumberOfComponents != 1 )
    {
    info->SetProperty( info, VVP_ERROR, "This filter requires a single-component data set as input" ); 
    return -1;
    }
  
  if( info->NumberOfMarkers < 1 )
    {
    info->SetProperty( info, VVP_ERROR, "Please select points using the 3D Markers in the Annotation menu" ); 
    return -1;
    }

  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_CHAR:
      {
      WatershedModuleRunner<signed char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      WatershedModuleRunner<unsigned char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_SHORT:
      {
      WatershedModuleRunner<signed short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      WatershedModuleRunner<unsigned short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_INT:
      {
      WatershedModuleRunner<signed int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_INT:
      {
      WatershedModuleRunner<unsigned int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_LONG:
      {
      WatershedModuleRunner<signed long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_LONG:
      {
      WatershedModuleRunner<unsigned long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_FLOAT:
      {
      WatershedModuleRunner<float> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_DOUBLE:
      {
      WatershedModuleRunner<double> runner;
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

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Sigma for gradient magnitude.");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "The input image is smoothed with a Gaussian during the computation of the Gradient Magnitude. This sigma value should be large enough to attenuate image noise, but not as large as to prevent the basins from being separated.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "0.1 10.0 0.1");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Threshold for minimum basin.");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, "0.01");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "The lowest value of water level for which basins will be computed. This prevents to spend time in computing micro basins at the lowest levels.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , "0.001 0.1 0.001");

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL, "Water Level.");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "0.1");
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "The level of water at which the basins will be identified. It is expressed as a fraction of the maximum possible level.");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , "0.01 0.5 0.01");

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
  
void VV_PLUGIN_EXPORT vvITKWatershedModuleInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Watershed Module (ITK)");
  info->SetProperty(info, VVP_GROUP, "Segmentation - Level Sets");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Watershed Module");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This module applies a Watershed method for segmenting a volume. Before running this filter you must set one 3D marker on the region that you want to segment. This marker will be used to select the water basin to be binarized in order to produce the output binary mask at the end of the processing. All the necessary  preprocessing is packaged in this module. This makes it a good choice when you are already familiar with the parameters settings requires for you particular data set. When you are applying Watershed to a new data set, you may want to rather go step by step using each one the individual filters.");

  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "3");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,   "10");

}

}
