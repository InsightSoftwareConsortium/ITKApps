/* This module encapsulates the full preprocessing required for
   applying the DeformableModel image filter for segmenting the 
   volume.  It requires seed points and the original image as inputs. */


#include "vvITKDeformableModelModule.txx"


static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  const float sigma                 = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ));

  const unsigned int numberOfSeeds = info->NumberOfMarkers;
  if( numberOfSeeds < 1 )
    {
    info->SetProperty( info, VVP_ERROR, "Please select points using the 3D Markers in the Annotation menu" ); 
    return -1;
    }

  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_UNSIGNED_CHAR:
      {
      typedef  unsigned char                              PixelType;
      typedef VolView::PlugIn::DeformableModelModule< 
                                            PixelType >   ModuleType;
      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Computing Fast Marching Module...");
      module.SetSigma( sigma );
      // Execute the filter
      module.ProcessData( pds  );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      typedef  unsigned short                             PixelType;
      typedef VolView::PlugIn::DeformableModelModule< 
                                            PixelType >   ModuleType;
      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Computing Fast Marching Module...");
      module.SetSigma( sigma );
      // Execute the filter
      module.ProcessData( pds  );
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
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "The input image is smoothed with a Gaussian during the computation of the Gradient Magnitude. This sigma value should be large enough to attenuate image noise, but not as large as to prevent the level set front from getting close to the edges of objects in the image.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "0.1 10.0 0.1");

  
  return 1;
}


extern "C" {
  
void VV_PLUGIN_EXPORT vvITKDeformableModelModuleInit(vtkVVPluginInfo *info)
{
  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Deformable Model (ITK)");
  info->SetProperty(info, VVP_GROUP, "Segmentation");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Deformable Model Module");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This module implements a Deformable Model. A initial surface is deformed under the effect of internal and external forces. The external forces are excerted by the image attracting the surface to the contour of anatomical structures. The internal forces preserve the smoothness and continuity of the surface during its evolution.");

  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "1");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,   "16");
  info->SetProperty(info, VVP_PRODUCES_MESH_ONLY,           "1");

}

}
