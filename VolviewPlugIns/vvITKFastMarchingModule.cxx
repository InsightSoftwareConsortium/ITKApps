/* This module encapsulates the full preprocessing required for
   applying the FastMarching image filter for segmenting the 
   volume.  It requires seed points and the original image as inputs. */


#include "vvITKFastMarchingModule.txx"


static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  const float stoppingValue         = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ));
  const float sigma                 = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ));
  const float lowestBasinValue      = atof( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ));
  const float lowestBorderValue     = atof( info->GetGUIProperty(info, 3, VVP_GUI_VALUE ));

  const unsigned int numberOfSeeds = info->NumberOfMarkers;
  if( numberOfSeeds < 1 )
    {
    info->SetProperty( info, VVP_ERROR, "Please select points using the 3D Markers in the Annotation menu" ); 
    return -1;
    }

  // Take the first marker as the seed point
  const float * seedCoordinates = info->Markers;

  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_UNSIGNED_CHAR:
      {
      typedef  unsigned char                              PixelType;
      typedef VolView::PlugIn::FastMarchingModule< 
                                            PixelType >   ModuleType;
      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetStoppingValue( stoppingValue );
      module.SetSigma( sigma );
      module.SetLowestBasinValue( lowestBasinValue ); 
      module.SetLowestBorderValue( lowestBorderValue );
      for(unsigned int i=0; i< numberOfSeeds; i++)
        {
        const float seedx =  static_cast< int >( seedCoordinates[0] );
        const float seedy =  static_cast< int >( seedCoordinates[1] );
        const float seedz =  static_cast< int >( seedCoordinates[2] );
        module.AddSeed( seedx, seedy, seedz );
        seedCoordinates += 3; // pass to next point
        }
      // Execute the filter
      module.ProcessData( pds  );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      // ... when ready, copy/paste from U-Char implementation.
      module.SetPluginInfo( info );
      module.SetStoppingValue( stoppingValue );
      module.SetSigma( sigma );
      module.SetLowestBasinValue( lowestBasinValue ); 
      module.SetLowestBorderValue( lowestBorderValue );
      for(unsigned int i=0; i< numberOfSeeds; i++)
        {
        const float seedx =  static_cast< int >( seedCoordinates[0] );
        const float seedy =  static_cast< int >( seedCoordinates[1] );
        const float seedz =  static_cast< int >( seedCoordinates[2] );
        module.AddSeed( seedx, seedy, seedz );
        seedCoordinates += 3; // pass to next point
        }

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

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Stopping Value");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "50.0");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Defines a stopping value for the time up to which the front propagation will be computed.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "1 200 1");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Sigma for gradient magnitude.");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "The input image is smoothed with a Gaussian during the computation of the Gradient Magnitude. This sigma value should be large enough to attenuate image noise, but not as large as to prevent the level set front from getting close to the edges of objects in the image.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , "0.1 10.0 0.1");

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL, "Bottom of basin.");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "0.0");
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "The lowest value of the gradient magnitude in the inside of the region to be segmented. This value will be mapped by the Sigmoid into the fastest propagation in the speed image.");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , "0.1 10.0 0.1");

  info->SetGUIProperty(info, 3, VVP_GUI_LABEL, "Lowest of basin border.");
  info->SetGUIProperty(info, 3, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 3, VVP_GUI_DEFAULT, "6.0");
  info->SetGUIProperty(info, 3, VVP_GUI_HELP, "The lowest value of the gradient magnitude in the border of the region to be segmented. This value will be mapped by the Sigmoid into the slowest propagation in the speed image.");
  info->SetGUIProperty(info, 3, VVP_GUI_HINTS , "0.1 50.0 0.1");

  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, "0");
  
  info->OutputVolumeScalarType = VTK_UNSIGNED_SHORT;
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
  
void VV_PLUGIN_EXPORT vvITKFastMarchingModuleInit(vtkVVPluginInfo *info)
{
  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Fast Marching Module (ITK)");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Fast Marching Module");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This module applies a Fast Marching level set method for segmenting a volume. All the necessary  preprocessing is packaged in this module. This makes it a good choice when you are already familiar with the parameters settings requires for you particular data set. When you are applying FastMarching to a new data set, you may want to rather go step by step using each one the individual filters.");

  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "4");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,   "16");

}

}
