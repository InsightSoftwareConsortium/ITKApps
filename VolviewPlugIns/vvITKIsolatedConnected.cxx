/* perform segmentation using the Isolated Connected image filter */

#include "vvITKFilterModuleDoubleOutput.h"

#include "itkIsolatedConnectedImageFilter.h"



template <class InputPixelType>
class IsolatedConnectedRunner
  {
  public:
    itkStaticConstMacro( Dimension, unsigned int, 3);
    typedef  itk::Image< InputPixelType, Dimension >  InputImageType; 
    typedef  unsigned char                            OutputPixelType;
    typedef  itk::Image< OutputPixelType, Dimension > OutputImageType; 
    typedef  typename InputImageType::IndexType       IndexType;
    typedef  itk::IsolatedConnectedImageFilter< InputImageType,  
                                                OutputImageType >   FilterType;
  public:
    IsolatedConnectedRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      const float lower           = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
      const float upperLimit      = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );
      const float upperTolerance  = atof( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ) );
      const int   replaceValue    = atoi( info->GetGUIProperty(info, 3, VVP_GUI_VALUE ) );
      const bool  compositeOutput = atoi( info->GetGUIProperty(info, 4, VVP_GUI_VALUE ) );

      if( static_cast<InputPixelType>(upperTolerance) == itk::NumericTraits<InputPixelType>::Zero )
        { 
        itk::ExceptionObject excp;
        excp.SetDescription(" The current tolerance is truncated to zero. Please select a larger tolerance value." );
        excp.SetLocation("");
        throw excp;
        }

      itk::Index<Dimension> seed1;
      itk::Index<Dimension> seed2;
      VolView::PlugIn::FilterModuleBase::Convert3DMarkerToIndex( info, 0, seed1 );
      VolView::PlugIn::FilterModuleBase::Convert3DMarkerToIndex( info, 1, seed2 );

      VolView::PlugIn::FilterModuleDoubleOutput< FilterType > module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Isolated Connected Region Growing...");
      // Set the parameters on it
      module.GetFilter()->SetIsolatedValueTolerance( static_cast<InputPixelType>( upperTolerance  ) );
      module.GetFilter()->SetUpperValueLimit( static_cast<InputPixelType>( upperLimit  ) );
      module.GetFilter()->SetLower( static_cast<InputPixelType>( lower ) );
      module.GetFilter()->SetReplaceValue( replaceValue );
      module.GetFilter()->SetSeed1( seed1 );
      module.GetFilter()->SetSeed2( seed2 );
      module.SetProduceDoubleOutput( compositeOutput          );
      // Execute the filter
      module.ProcessData( pds  );
      const float isolatedValue = static_cast<float>( module.GetFilter()->GetIsolatedValue() );
      char tmp[1024];
      sprintf( tmp, "Upper threshold found = %f\n This is the intensity value that disconnects the two seeds",
               isolatedValue ); 
      info->SetProperty( info, VVP_REPORT_TEXT, tmp );
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

  if( info->NumberOfMarkers < 2 )
    {
    info->SetProperty( info, VVP_ERROR, "Please select two seed point using the 3D Markers in the Annotation menu. The first seed point in the region to segment, the second seed point in the region to reject." ); 
    return -1;
    }

  try 
    {
    switch( info->InputVolumeScalarType )
      {
      case VTK_CHAR:
        {
        IsolatedConnectedRunner<char> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_UNSIGNED_CHAR:
        {
        IsolatedConnectedRunner<unsigned char> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_SHORT:
        {
        IsolatedConnectedRunner<signed short> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_UNSIGNED_SHORT:
        {
        IsolatedConnectedRunner<unsigned short> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_INT:
        {
        IsolatedConnectedRunner<signed int> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_UNSIGNED_INT:
        {
        IsolatedConnectedRunner<unsigned int> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_LONG:
        {
        IsolatedConnectedRunner<signed long> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_UNSIGNED_LONG:
        {
        IsolatedConnectedRunner<unsigned long> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_FLOAT:
        {
        IsolatedConnectedRunner<float> runner;
        runner.Execute( info, pds );
        break; 
        }
      case VTK_DOUBLE:
        {
        IsolatedConnectedRunner<double> runner;
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
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRangeFraction( info, 0.001, 100 ) );

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
  
void VV_PLUGIN_EXPORT vvITKIsolatedConnectedInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

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
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "5");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "1");// It is actually dependent of the complexity of the shape to segment

  info->OutputVolumeScalarType = VTK_UNSIGNED_CHAR;
  info->OutputVolumeNumberOfComponents = 1;
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "1");

}

}
