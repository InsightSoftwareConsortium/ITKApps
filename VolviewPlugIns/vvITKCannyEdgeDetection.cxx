/* perform edge-detection using the Canny filter */

#include "vvITKFilterModuleWithCasting.h"

#include "itkCannyEdgeDetectionImageFilter.h"

static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  const unsigned int Dimension = 3;

  typedef   float            InternalPixelType;
  typedef   float            OutputPixelType;

  typedef   itk::Image< InternalPixelType, Dimension > InternalImageType; 
  typedef   itk::Image< OutputPixelType,   Dimension > OutputImageType; 

  typedef   itk::CannyEdgeDetectionImageFilter< 
                                InternalImageType,  
                                OutputImageType >   FilterType;
 
  const float        variance           = atof(  info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
  const float        maximumError       = atof(  info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );
  const float        threshold          = atof(  info->GetGUIProperty(info, 2, VVP_GUI_VALUE ) );


  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_UNSIGNED_CHAR:
      {
      VolView::PlugIn::FilterModuleWithCasting< unsigned char, 
                                                FilterType,
                                                unsigned char  > module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Canny edge detection...");
      // Set the parameters on it
      module.GetFilter()->SetVariance( variance );
      module.GetFilter()->SetMaximumError( maximumError );
      module.GetFilter()->SetThreshold( threshold );
      // Execute the filter
      module.ProcessData( pds  );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      VolView::PlugIn::FilterModuleWithCasting< unsigned short,
                                                FilterType,
                                                unsigned char  > module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Canny edge detection...");
      // Set the parameters on it
      module.GetFilter()->SetVariance( variance );
      module.GetFilter()->SetMaximumError( maximumError );
      module.GetFilter()->SetThreshold( threshold );
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

  info->SetProperty(info,VVP_RESULTING_COMPONENT_1_UNITS,"Canny Edges");
  
  return 0;
}


static int UpdateGUI(void *inf)
{
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Variance");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "2.0");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Variance used by the discrete Gaussian image filter that smooths the image before detecting local maxima.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "1.0 20.0 1.0");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Maximum error");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, "0.05");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Maximum error to be tolerated by the approximation of the Gaussian kernel implemented in the Discrete Gaussian image filter.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , "0.01 2.0 0.01");

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL, "Threshold");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "Threshold used to eliminate small local maxima.");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , "0.1 20.0 0.1");

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
  
void VV_PLUGIN_EXPORT vvITKCannyEdgeDetectionInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Canny Edge Detection (ITK)");
  info->SetProperty(info, VVP_GROUP, "Edge Detection");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                 "Edge detection using the Canny filter");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filter applies an edge-detection filter developed by Canny. First it smooths the image using a discrete Gaussian filter. Then it detects local maxima and marks the position of those local maxima.");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "3");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "8");
  
}

}

