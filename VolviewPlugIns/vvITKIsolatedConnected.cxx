/* perform segmentation using the confidence connected image filter */

#include "vvITKFilterModule.h"

#include "itkIsolatedConnectedImageFilter.h"

static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  const unsigned int Dimension = 3;

  const float lower           = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
  const float upperLimit      = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );
  const float upperTolerance  = atof( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ) );
  const int   replaceValue    = atoi( info->GetGUIProperty(info, 3, VVP_GUI_VALUE ) );

   if( info->NumberOfMarkers < 1 )
    {
    info->SetProperty( info, VVP_ERROR, "Please select a seed point using the 3D Markers in the Annotation menu" ); 
    return -1;
    }

  itk::Index<Dimension> seed1;
  itk::Index<Dimension> seed2;
  VolView::PlugIn::FilterModuleBase::Convert3DMarkerToIndex( info, 0, seed1 );
  VolView::PlugIn::FilterModuleBase::Convert3DMarkerToIndex( info, 1, seed2 );

  char tmp[1024];

  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_UNSIGNED_CHAR:
      {
      typedef  unsigned char                             InputPixelType;
      typedef  itk::Image< InputPixelType, Dimension >   InputImageType; 
      typedef  unsigned char                             OutputPixelType;
      typedef  itk::Image< OutputPixelType, Dimension >  OutputImageType; 
      typedef  InputImageType::IndexType                 IndexType;
      typedef  itk::IsolatedConnectedImageFilter< InputImageType,  
                                                    OutputImageType >   FilterType;
      VolView::PlugIn::FilterModule< FilterType > module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Isolated Connected Region Growing...");
      // Set the parameters on it
      module.GetFilter()->SetIsolatedValueTolerance( static_cast<InputPixelType>( upperTolerance  ) );
      module.GetFilter()->SetUpperValueLimit( static_cast<InputPixelType>( upperLimit  ) );
      module.GetFilter()->SetLower( static_cast<InputPixelType>( lower ) );
      module.GetFilter()->SetReplaceValue( replaceValue );
      module.GetFilter()->SetSeed1( seed1 );
      module.GetFilter()->SetSeed2( seed2 );
      // Execute the filter
      module.ProcessData( pds  );
      const float isolatedValue = static_cast<float>( module.GetFilter()->GetIsolatedValue() );
      sprintf( tmp, "Upper threshold found = %f\n This is the intensity value that disconnects the two seeds",
               isolatedValue ); 
      info->SetProperty( info, VVP_REPORT_TEXT, tmp );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      typedef  unsigned short                            InputPixelType;
      typedef  itk::Image< InputPixelType, Dimension >   InputImageType; 
      typedef  unsigned char                             OutputPixelType;
      typedef  itk::Image< OutputPixelType, Dimension >  OutputImageType; 
      typedef  InputImageType::IndexType                 IndexType;
      typedef  itk::IsolatedConnectedImageFilter<  InputImageType,  
                                                    OutputImageType >   FilterType;
      VolView::PlugIn::FilterModule< FilterType > module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Isolated Connected Region Growing...");
      // Set the parameters on it
      module.GetFilter()->SetIsolatedValueTolerance( static_cast<InputPixelType>( upperTolerance  ) );
      module.GetFilter()->SetUpperValueLimit( static_cast<InputPixelType>( upperLimit  ) );
      module.GetFilter()->SetLower( static_cast<InputPixelType>( lower ) );
      module.GetFilter()->SetReplaceValue( replaceValue );
      module.GetFilter()->SetSeed1( seed1 );
      module.GetFilter()->SetSeed2( seed2 );
      // Execute the filter
      module.ProcessData( pds );
      const float isolatedValue = static_cast<float>( module.GetFilter()->GetIsolatedValue() );
      sprintf( tmp, "Upper threshold found = %f\n This is the intensity value that disconnects the two seeds",
               isolatedValue ); 
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
  return 0;
}


static int UpdateGUI(void *inf)
{
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL,"Lower Threshold");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT,  VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarMinimum( info ) );
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Lower threshold. Only pixels with intensities higher than this threshold will be considered to be added to the region.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRange( info ) );

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL,"Upper Limit");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT,  VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarMaximum( info ) );
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "This is the maximum value that will be attempted for the upper threshold that will separate the two seeds.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRange( info ) );

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL,"Upper Tolerance");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "Precision required for the upper threshold that will separate the two seeds. The upper threshold is found using a bipartition algorithm, this tolerance defines the stopping criterion for the bipartition.");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRange( info ) );

  info->SetGUIProperty(info, 3, VVP_GUI_LABEL, "Replace Value");
  info->SetGUIProperty(info, 3, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 3, VVP_GUI_DEFAULT, "255");
  info->SetGUIProperty(info, 3, VVP_GUI_HELP, "Value to assign to the binary mask of the segmented region. The rest of the image will be set to zero.");
  info->SetGUIProperty(info, 3, VVP_GUI_HINTS , "1 255.0 1.0");

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
  
void VV_PLUGIN_EXPORT vvITKIsolatedConnectedInit(vtkVVPluginInfo *info)
{
  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Isolated Connected (ITK) ");
  info->SetProperty(info, VVP_GROUP, "Segmentation - Region Growing");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Isolated Connedted Segmentation");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filter applies a region growing algorithm for segmentation. You must provide two seed points, the filter will extract a region connected to the first seed point and not connected to the second one. This is quite useful for separating adjacent structures. The method will grow a region around the first seed point in such a way that the pixels in the region have intensities higher than the lower threshold. The upper threshold is computed by the filter as the value that prevents the seed2 point to be included in the region of seed1.");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "4");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "4");// It is actually dependent of the complexity of the shape to segment

}

}
