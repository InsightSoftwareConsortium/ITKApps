/* This plugin adapts the GeodesicActveContour filter.
   It does not perform any preprocessing. The user should provide
   the speed image and the initial level set as inputs. */

#include "vvITKFilterModuleTwoInputs.h"


static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  const float zeroSetValue          = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ));
  const float gaussianSigma         = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ));
  const float curvatureScaling      = atof( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ));
  const float propagationScaling    = atof( info->GetGUIProperty(info, 3, VVP_GUI_VALUE ));
  const float advectionScaling      = atof( info->GetGUIProperty(info, 4, VVP_GUI_VALUE ));
  const float maximumRMSError       = atof( info->GetGUIProperty(info, 5, VVP_GUI_VALUE ));

  const unsigned int maximumNumberOfIterations = atoi( info->GetGUIProperty(info, 6, VVP_GUI_VALUE ));

  const unsigned int numberOfSeeds = info->NumberOfMarkers;
  if( numberOfSeeds < 1 )
    {
    info->SetProperty( info, VVP_ERROR, "Please select points using the 3D Markers in the Annotation menu" ); 
    return -1;
    }

  char tmp[1024];

  itk::Index<3> seedPosition;

  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_UNSIGNED_CHAR:
      {
      typedef  unsigned char                              PixelType;
      typedef VolView::PlugIn::GeodesicActiveContourModule< 
                                            PixelType >   ModuleType;
      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Computing Geodesic Active Contour...");
      module.SetDistanceFromSeeds( distanceFromSeeds );
      module.GetFilter()->SetSigma( gaussianSigma );
      module.GetFilter()->SetCurvatureScaling( curvatureScaling );
      module.GetFilter()->SetPropagationScaling( propagationScaling );
      module.GetFilter()->SetAdvectionScaling( advectionScaling );
      module.GetFilter()->SetMaximumRMSError( maximumRMSError );
      module.GetFilter()->SetMaximumIterations( maximumNumberOfIterations );
      // Execute the filter
      module.ProcessData( pds  );
      sprintf(tmp,"Total number of iterations = %d \n Final RMS error = %f",
                         module.GetElapsedIterations(),
                         module.GetRMSChange());
      info->SetProperty( info, VVP_REPORT_TEXT, tmp );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      typedef  unsigned short                             PixelType;
      typedef VolView::PlugIn::GeodesicActiveContourModule< 
                                            PixelType >   ModuleType;
      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetDistanceFromSeeds( distanceFromSeeds );
      module.GetFilter()->SetSigma( gaussianSigma );
      module.GetFilter()->SetUpdateMessage("Computing Geodesic Active Contour...");
      module.GetFilter()->SetCurvatureScaling( curvatureScaling );
      module.GetFilter()->SetPropagationScaling( propagationScaling );
      module.GetFilter()->SetAdvectionScaling( advectionScaling );
      module.GetFilter()->SetMaximumRMSError( maximumRMSError );
      module.GetFilter()->SetMaximumIterations( maximumNumberOfIterations );
      // Execute the filter
      module.ProcessData( pds  );
      sprintf(tmp,"Total number of iterations = %d \n Final RMS error = %f",
                         module.GetElapsedIterations(),
                         module.GetRMSChange());
      info->SetProperty( info, VVP_REPORT_TEXT, tmp );
      break; 
      } 
    }
  }
  catch( itk::ExceptionObject & except )
  {
    info->SetProperty( info, VVP_ERROR, except.what() ); 
    return -1;
  }

  info->UpdateProgress( info, 1.0, "Geodesic Active Contour LevelSet Done !");

  return 0;
}


static int UpdateGUI(void *inf)
{
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Zero Set value.");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "5.0");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "An initial level will be created using the seed points. The zero set will be placed at a certain distance from the seed points. The value set in this scale is the distance to be used");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "1.0 100.0 1.0");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Sigma for gradient magnitude.");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Sigma used by the smoothing previous to computing the gradient magnitude. Large values of sigma will reduce noise in the image but will also degrade contours. Sigma is measured in millimeters, not pixels");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , "0.1 10.0 0.1");

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL, "Curvature scaling.");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "Scaling factor for the curvature contribution. Larger values will result in smoother contours");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , "0.1 10.0 0.1");

  info->SetGUIProperty(info, 3, VVP_GUI_LABEL, "Propagation scaling.");
  info->SetGUIProperty(info, 3, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 3, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 3, VVP_GUI_HELP, "Scaling factor for the inflation factor. Larger factors will result in rapid expansion with irregular borders");
  info->SetGUIProperty(info, 3, VVP_GUI_HINTS , "0.1 10.0 0.1");

  info->SetGUIProperty(info, 4, VVP_GUI_LABEL, "Advection scaling.");
  info->SetGUIProperty(info, 4, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 4, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 4, VVP_GUI_HELP, "Scaling factor for the advection factor. This factor generates a force that is proportional to the slant of the level set at a point.");
  info->SetGUIProperty(info, 4, VVP_GUI_HINTS , "0.1 10.0 0.1");

  info->SetGUIProperty(info, 5, VVP_GUI_LABEL, "Maximum RMS Error.");
  info->SetGUIProperty(info, 5, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 5, VVP_GUI_DEFAULT, "0.06");
  info->SetGUIProperty(info, 5, VVP_GUI_HELP, "Threshold of the RMS change between one iteration and the previous one. This is a convergence criteria, the process will stop when the RMS change is lower than the value set here");
  info->SetGUIProperty(info, 5, VVP_GUI_HINTS , "0.01 0.5 0.01");

  info->SetGUIProperty(info, 6, VVP_GUI_LABEL, "Maximum iterations.");
  info->SetGUIProperty(info, 6, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 6, VVP_GUI_DEFAULT, "100.0");
  info->SetGUIProperty(info, 6, VVP_GUI_HELP, "The maximum number of iteration to apply the time step in the partial differental equation.");
  info->SetGUIProperty(info, 6, VVP_GUI_HINTS , "1.0 500.0 1.0");

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
  
void VV_PLUGIN_EXPORT vvITKGeodesicActiveContourInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Geodesic Active Contour (ITK)");
  info->SetProperty(info, VVP_GROUP, "Segmentation - Level Sets");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Geodesic Active Contour");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This module applies the Geodesic Active Contour method for segmenting a volume. No preprocessing is performed here. The user must provide as inputs an initial level set and the feature image which will be used to compute speeds. The current image being visualized will be taken as the initial level set. The speed image required  as a second input can be specified by providinng a file name in the GUI.");

  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "7");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,   "16");

}

}
