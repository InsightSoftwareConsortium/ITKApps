/* perform segmentation using the confidence connected image filter */

#include "vvITKFilterModuleDoubleOutput.h"

#include "itkConnectedThresholdImageFilter.h"

template <class InputPixelType>
class ConnectedThresholdRunner
  {
  public:
    itkStaticConstMacro( Dimension, unsigned int, 3);
    typedef  itk::Image< InputPixelType, Dimension >  InputImageType; 
    typedef  unsigned char                            OutputPixelType;
    typedef  itk::Image< OutputPixelType, Dimension > OutputImageType; 
    typedef  typename InputImageType::IndexType       IndexType;
    typedef  itk::ConnectedThresholdImageFilter< InputImageType,  
                                                 OutputImageType >   FilterType;
  public:
    ConnectedThresholdRunner( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      const float upper            = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
      const float lower            = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );
      const int   replaceValue     = atoi( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ) );
      const bool  compositeOutput  = atoi( info->GetGUIProperty(info, 3, VVP_GUI_VALUE ) );

      if( info->NumberOfMarkers < 1 )
        {
        info->SetProperty( info, VVP_ERROR, "Please select seed points using the 3D Markers in the Annotation menu" ); 
        return;
        }

      VolView::PlugIn::FilterModuleDoubleOutput< FilterType > module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Threshold Connected Region Growing...");
      // Set the parameters on it
      module.GetFilter()->SetUpper( static_cast<InputPixelType>( upper  ) );
      module.GetFilter()->SetLower( static_cast<InputPixelType>( lower ) );
      module.GetFilter()->SetReplaceValue( replaceValue );

      itk::Index<Dimension> seed;
      const unsigned int numberOfSeeds = info->NumberOfMarkers;
      for( unsigned int i=0; i<numberOfSeeds; i++)
        {
        VolView::PlugIn::FilterModuleBase::Convert3DMarkerToIndex( info, i, seed );
        module.GetFilter()->AddSeed( seed );
        }

      module.SetProduceDoubleOutput( compositeOutput          );
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
        ConnectedThresholdRunner<signed char> runner( info, pds );
        break; 
        }
      case VTK_UNSIGNED_CHAR:
        {
        ConnectedThresholdRunner<unsigned char> runner( info, pds );
        break; 
        }
      case VTK_SHORT:
        {
        ConnectedThresholdRunner<signed short> runner( info, pds );
        break; 
        }
      case VTK_UNSIGNED_SHORT:
        {
        ConnectedThresholdRunner<unsigned short> runner( info, pds );
        break; 
        }
      case VTK_INT:
        {
        ConnectedThresholdRunner<signed int> runner( info, pds );
        break; 
        }
      case VTK_UNSIGNED_INT:
        {
        ConnectedThresholdRunner<unsigned int> runner( info, pds );
        break; 
        }
      case VTK_LONG:
        {
        ConnectedThresholdRunner<signed long> runner( info, pds );
        break; 
        }
      case VTK_UNSIGNED_LONG:
        {
        ConnectedThresholdRunner<unsigned long> runner( info, pds );
        break; 
        }
      case VTK_FLOAT:
        {
        ConnectedThresholdRunner<float> runner( info, pds );
        break; 
        }
      case VTK_DOUBLE:
        {
        ConnectedThresholdRunner<double> runner( info, pds );
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

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL,"Upper Threshold");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT,  VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarMaximum( info ) );
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Upper threshold. Only pixels with intensities lower than this threshold will be considered to be added to the region.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRange( info ) );

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL,"Lower Threshold");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT,  VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarMinimum( info ) );
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Lower threshold. Only pixels with intensities higher than this threshold will be considered to be added to the region.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRange( info ) );

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL, "Replace Value");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "255");
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "Value to assign to the binary mask of the segmented region. The rest of the image will be set to zero.");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , "1 255.0 1.0");

  info->SetGUIProperty(info, 3, VVP_GUI_LABEL, "Produce composite output");
  info->SetGUIProperty(info, 3, VVP_GUI_TYPE, VVP_GUI_CHECKBOX);
  info->SetGUIProperty(info, 3, VVP_GUI_DEFAULT, "0");
  info->SetGUIProperty(info, 3, VVP_GUI_HELP, "This filter produce by default a binary image as output. Enabling this option will instead generate a composite output combining the input image and the binary mask as an image of two components. This is convenient for evaluating the quality of a segmentation.");


  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, "0");

  info->OutputVolumeScalarType = VTK_UNSIGNED_CHAR;
  info->OutputVolumeNumberOfComponents = 1;
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "4");

  const char * compositeOutputProperty = info->GetGUIProperty(info, 3, VVP_GUI_VALUE ); 

  // During the startup of the application this string is not yet defined.
  // We should then check for it before trying to use it.
  if( compositeOutputProperty )
    {
    const bool compositeOutput  = atoi( compositeOutputProperty );
    if( compositeOutput )
      {
      info->OutputVolumeScalarType = info->InputVolumeScalarType;
      info->OutputVolumeNumberOfComponents = 2;
      info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "5");
      }
    }

 
  memcpy(info->OutputVolumeDimensions,info->InputVolumeDimensions,
         3*sizeof(int));
  memcpy(info->OutputVolumeSpacing,info->InputVolumeSpacing,
         3*sizeof(float));
  memcpy(info->OutputVolumeOrigin,info->InputVolumeOrigin,
         3*sizeof(float));

  return 1;
}



extern "C" {
  
void VV_PLUGIN_EXPORT vvITKConnectedThresholdInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Connected Threshold (ITK) ");
  info->SetProperty(info, VVP_GROUP, "Segmentation - Region Growing");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Connected Threshold Segmentation");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filter applies an region growing algorithm for segmentation. The criterion for including new pixels in the region is defined by an intensity range whose bound are provided by the user. These bounds are described as the lower and upper thresholds. The region is grown starting from a set of seed points that the user should provide in the form of 3D markers.");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "4");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "4");// It is actually dependent of the complexity of the shape to segment

}

}
