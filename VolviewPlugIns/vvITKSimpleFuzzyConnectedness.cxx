/* perform segmentation using the Isolated Connected image filter */

#include "vvITKFilterModuleDoubleOutput.h"

#include "itkSimpleFuzzyConnectednessScalarImageFilter.h"



template <class InputPixelType>
class SimpleFuzzyConnectednessRunner
  {
  public:
    itkStaticConstMacro( Dimension, unsigned int, 3);
    typedef  itk::Image< InputPixelType, Dimension >  InputImageType; 
    typedef  unsigned char                            OutputPixelType;
    typedef  itk::Image< OutputPixelType, Dimension > OutputImageType; 
    typedef  typename InputImageType::IndexType       IndexType;
    typedef  itk::SimpleFuzzyConnectednessScalarImageFilter< 
                              InputImageType, OutputImageType >   FilterType;
  public:
    SimpleFuzzyConnectednessRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      const float mean              = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
      const float standardDeviation = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );
      const float threshold         = atof( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ) );
      const int   replaceValue      = atoi( info->GetGUIProperty(info, 3, VVP_GUI_VALUE ) );
      const bool  compositeOutput   = atoi( info->GetGUIProperty(info, 4, VVP_GUI_VALUE ) );

      itk::Index<Dimension> seed;
      VolView::PlugIn::FilterModuleBase::Convert3DMarkerToIndex( info, 0, seed );

      VolView::PlugIn::FilterModuleDoubleOutput< FilterType > module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Fuzzy Connectedness Region Growing...");
      // Set the parameters on it
      module.GetFilter()->SetMean( mean );
      module.GetFilter()->SetVariance( standardDeviation * standardDeviation );
      module.GetFilter()->SetThreshold( threshold );
      module.GetFilter()->SetObjectSeed( seed );
      module.SetProduceDoubleOutput( compositeOutput          );
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
    info->SetProperty( info, VVP_ERROR, "Please select one seed point using the 3D Markers in the Annotation menu. This filter will compute affinities between the pixels in the image and the pixel at the seed point. This affinity is based on the properties of path joinning every pixel to the seed point." ); 
    return -1;
    }

  try 
    {
    switch( info->InputVolumeScalarType )
      {
      case VTK_CHAR:
        {
        SimpleFuzzyConnectednessRunner<char> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_UNSIGNED_CHAR:
        {
        SimpleFuzzyConnectednessRunner<unsigned char> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_SHORT:
        {
        SimpleFuzzyConnectednessRunner<signed short> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_UNSIGNED_SHORT:
        {
        SimpleFuzzyConnectednessRunner<unsigned short> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_INT:
        {
        SimpleFuzzyConnectednessRunner<signed int> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_UNSIGNED_INT:
        {
        SimpleFuzzyConnectednessRunner<unsigned int> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_LONG:
        {
        SimpleFuzzyConnectednessRunner<signed long> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_UNSIGNED_LONG:
        {
        SimpleFuzzyConnectednessRunner<unsigned long> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_FLOAT:
        {
        SimpleFuzzyConnectednessRunner<float> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_DOUBLE:
        {
        SimpleFuzzyConnectednessRunner<double> runner;
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

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL,"Mean");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT,  VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarMidValue( info ) );
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "This value is an estimation of the mean intensity for the pixels of the region that you expect to segment.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRange( info ) );

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL,"Standard Deviation");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT,  VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarMaximum( info ) );
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "This is the estimation of the standard deviation corresponding to the region that you expect to segment.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRange( info ) );

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL,"Threshold");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "0.5");
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "This threshold value will be used for binarizing the affinity map. Increasing its value will enlarge the region. Decreasing is value will shrink the segmented region.");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , "0.0  1.0 0.01" );

  info->SetGUIProperty(info, 3, VVP_GUI_LABEL, "Replace Value");
  info->SetGUIProperty(info, 3, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 3, VVP_GUI_DEFAULT, "255");
  info->SetGUIProperty(info, 3, VVP_GUI_HELP, "Value to assign to the binary mask of the segmented region. The rest of the image will be set to zero.");
  info->SetGUIProperty(info, 3, VVP_GUI_HINTS , "1 255.0 1.0");

  info->SetGUIProperty(info, 4, VVP_GUI_LABEL, "Produce composite output");
  info->SetGUIProperty(info, 4, VVP_GUI_TYPE, VVP_GUI_CHECKBOX);
  info->SetGUIProperty(info, 4, VVP_GUI_DEFAULT, "0");
  info->SetGUIProperty(info, 4, VVP_GUI_HELP, "This filter produce by default a binary image as output. Enabling this option will instead generate a composite output combining the input image and the binary mask as an image of two components. This is convenient for evaluating the quality of a segmentation.");

  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, "0");

  // Depending on this option, the filter will produce a 
  // single component or a double component output.
  // When single component is used, the output type is 'unsigned char'

  info->OutputVolumeScalarType = VTK_UNSIGNED_CHAR;
  info->OutputVolumeNumberOfComponents = 1;
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "1");

  const char * compositeOutputProperty = info->GetGUIProperty(info, 4, VVP_GUI_VALUE ); 

  // During the startup of the application this string is not yet defined.
  // We should then check for it before trying to use it.
  if( compositeOutputProperty )
    {
    const bool compositeOutput  = atoi( compositeOutputProperty );
    if( compositeOutput )
      {
      info->OutputVolumeScalarType = info->InputVolumeScalarType;
      info->OutputVolumeNumberOfComponents = 2;
      info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "2");
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
  
void VV_PLUGIN_EXPORT vvITKSimpleFuzzyConnectednessInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Fuzzy Connectedness (ITK) ");
  info->SetProperty(info, VVP_GROUP, "Segmentation - Region Growing");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Fuzzy Connectedness Segmentation");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filter computes an affinity map by evaluating the paths joining every pixel in the image to a seed point provided by the user. The affinity map is then thresholded in order to produce a binary image as output");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "5");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "1");// It is actually dependent of the complexity of the shape to segment

  info->OutputVolumeScalarType = VTK_UNSIGNED_CHAR;
  info->OutputVolumeNumberOfComponents = 1;
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "1");

}

}
