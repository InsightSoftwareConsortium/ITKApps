/* This module encapsulates the full preprocessing required for
   applying the DeformableModel image filter for segmenting the 
   volume.  It requires seed points and the original image as inputs. */


#include "vvITKDeformableModelModule.txx"



template <class InputPixelType>
class DeformableModelModuleRunner
  {
  public:
      typedef VolView::PlugIn::DeformableModelModule< 
                                            InputPixelType >   ModuleType;
  public:
    DeformableModelModuleRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Computing Deformable Model...");
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

  const unsigned int numberOfSeeds = info->NumberOfMarkers;
  if( numberOfSeeds < 1 )
    {
    info->SetProperty( info, VVP_ERROR, "Please select the center of the initial spherical model using the 3D Markers in the Annotation menu" ); 
    return -1;
    }

  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_CHAR:
      {
      DeformableModelModuleRunner<signed char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      DeformableModelModuleRunner<unsigned char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_SHORT:
      {
      DeformableModelModuleRunner<signed short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      DeformableModelModuleRunner<unsigned short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_INT:
      {
      DeformableModelModuleRunner<signed int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_INT:
      {
      DeformableModelModuleRunner<unsigned int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_LONG:
      {
      DeformableModelModuleRunner<signed long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_LONG:
      {
      DeformableModelModuleRunner<unsigned long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_FLOAT:
      {
      DeformableModelModuleRunner<float> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_DOUBLE:
      {
      DeformableModelModuleRunner<double> runner;
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

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Radius X");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "10.0");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Radius along X of the ellipsoide surface used to initialize the deformable model");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "0.5 50.0 0.5");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Radius Y");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, "10.0");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Radius along Y of the ellipsoide surface used to initialize the deformable model");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , "0.5 50.0 0.5");

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL, "Radius Z");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "10.0");
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "Radius along Z of the ellipsoide surface used to initialize the deformable model");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , "0.5 50.0 0.5");

  info->SetGUIProperty(info, 3, VVP_GUI_LABEL, "Sigma for gradient magnitude");
  info->SetGUIProperty(info, 3, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 3, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 3, VVP_GUI_HELP, "External forces applied on the deformable model are computed as the gradient of the gradient magnitude from the input image. This sigma value is used in both gradient computations.");
  info->SetGUIProperty(info, 3, VVP_GUI_HINTS , "0.1 10.0 0.1");
  
  info->SetGUIProperty(info, 4, VVP_GUI_LABEL, "Stiffness");
  info->SetGUIProperty(info, 4, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 4, VVP_GUI_DEFAULT, "0.005");
  info->SetGUIProperty(info, 4, VVP_GUI_HELP, "Stiffness of the deformable surface. Larger values of this parameter will reduce the ability of the surface to adapt to details in the image. Too low values will make the surface prone to leaking through small openings.");
  info->SetGUIProperty(info, 4, VVP_GUI_HINTS , "0.0001 0.10 0.0001");
  
  info->SetGUIProperty(info, 5, VVP_GUI_LABEL, "External Forces");
  info->SetGUIProperty(info, 5, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 5, VVP_GUI_DEFAULT, "50.0");
  info->SetGUIProperty(info, 5, VVP_GUI_HELP, "Factor used to weight the contribution of external forces computed from image gradients. Large values of this weight will increase the tendency of the surface to adhere to image contours. Too large values will result in irregular surfaces and leaking. Too low values will result in smoothed surfaces with poor fitting to image structures.");
  info->SetGUIProperty(info, 5, VVP_GUI_HINTS , "1.0 100.0 1.0");
  
  info->SetGUIProperty(info, 6, VVP_GUI_LABEL, "Time Step");
  info->SetGUIProperty(info, 6, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 6, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 6, VVP_GUI_HELP, "Time interval used during the simulation of the surface evolution. Too large values will result in numerical instability. Too low values will result in long computational times.");
  info->SetGUIProperty(info, 6, VVP_GUI_HINTS , "0.1   10.0  0.1");
  
  info->SetGUIProperty(info, 7, VVP_GUI_LABEL, "Number of Iterations");
  info->SetGUIProperty(info, 7, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 7, VVP_GUI_DEFAULT, "200");
  info->SetGUIProperty(info, 7, VVP_GUI_HELP, "Number of times the evolution of the surface will be recomputed through one time step. Too few iterations will result in smoothed surfaces with poor image fitting. Too many iterations will was computational time and make the surface prone to leaking.");
  info->SetGUIProperty(info, 7, VVP_GUI_HINTS , "1 1000 1");
  
  info->SetGUIProperty(info, 8, VVP_GUI_LABEL, "Resolution X");
  info->SetGUIProperty(info, 8, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 8, VVP_GUI_DEFAULT, "50");
  info->SetGUIProperty(info, 8, VVP_GUI_HELP, "Number of longitude sectors in the initial spherical surface. Increasing this number improves the resolution of the surface but penalizes computing time.");
  info->SetGUIProperty(info, 8, VVP_GUI_HINTS , "4 200 1");
  
  info->SetGUIProperty(info, 9, VVP_GUI_LABEL, "Resolution Y");
  info->SetGUIProperty(info, 9, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 9, VVP_GUI_DEFAULT, "50");
  info->SetGUIProperty(info, 9, VVP_GUI_HELP, "Number of latitude sectors in the initial spherical surface. Increasing this number improves the resolution of the surface but penalizes computing time.");
  info->SetGUIProperty(info, 9, VVP_GUI_HINTS , "4 200 1");
  
  return 1;
}


extern "C" {
  
void VV_PLUGIN_EXPORT vvITKDeformableModelModuleInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Deformable Model (ITK)");
  info->SetProperty(info, VVP_GROUP, "Segmentation - Models");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Deformable Model Module");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This module implements a Deformable Model. An initial surface is deformed under the effect of internal and external forces. The external forces are excerted by the image and attract the surface to the contour of anatomical structures. The internal forces preserve the smoothness and continuity of the surface during its evolution.");

  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,         "10");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,   "16");
  info->SetProperty(info, VVP_PRODUCES_MESH_ONLY,           "1");

}

}
