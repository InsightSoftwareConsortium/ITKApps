/* Applies the FastMarchingImageFilter in order to perform
   segmentation. It requires seed points and a speed image
   as inputs. */

#include "vvITKFilterModule.h"

#include "itkFastMarchingImageFilter.h"


template <class InputPixelType>
class FastMarchingRunner
  {
  public:
      typedef  unsigned short                       TimePixelType;
      typedef  InputPixelType                       PixelType;
      typedef  itk::Image< PixelType, 3 >           ImageType; 
      typedef  itk::Image< TimePixelType, 3 >       TimeImageType;
      typedef  typename ImageType::IndexType        IndexType;
      typedef  typename ImageType::SizeType         SizeType;

      typedef  itk::FastMarchingImageFilter< 
                                    TimeImageType,  
                                    ImageType >     FilterType;

      typedef  VolView::PlugIn::FilterModule< FilterType >   ModuleType;

      typedef  typename FilterType::NodeContainer            NodeContainer;
      typedef  typename FilterType::NodeType                 NodeType;
      typedef  typename NodeContainer::Pointer               NodeContainerPointer;
      typedef  typename NodeType::PixelType                  NodePixelType;

  public:
    FastMarchingRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      const float stoppingValue         = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ));
      const float normalizationFactor   = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ));

      const unsigned int numberOfSeeds = info->NumberOfMarkers;
      if( numberOfSeeds < 1 )
        {
        info->SetProperty( info, VVP_ERROR, "Please select points using the 3D Markers in the Annotation menu" ); 
        return;
        }

      const double seedValue = 0.0;
      IndexType seedPosition;

      SizeType size;
      size[0] = info->OutputVolumeDimensions[0];
      size[1] = info->OutputVolumeDimensions[1];
      size[2] = info->OutputVolumeDimensions[2];


      NodeContainerPointer seeds = NodeContainer::New();
      seeds->Initialize();


      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Computing Fast Marching...");
      // Set the parameters on it
      module.GetFilter()->SetStoppingValue(  stoppingValue );
      module.GetFilter()->SetNormalizationFactor( normalizationFactor );
      NodeType node;
      node.SetValue( static_cast< NodePixelType >( seedValue ) );
      for(unsigned int i=0; i< numberOfSeeds; i++)
        {
        VolView::PlugIn::FilterModuleBase::Convert3DMarkerToIndex( info, i, seedPosition );
        node.SetIndex( seedPosition );
        seeds->InsertElement( i, node );
        }
      module.GetFilter()->SetTrialPoints( seeds );
      module.GetFilter()->SetOutputSize( size );
      // Execute the filter
      module.ProcessData( pds );
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


  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_CHAR:
      {
      FastMarchingRunner<signed char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      FastMarchingRunner<unsigned char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_SHORT:
      {
      FastMarchingRunner<signed short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      FastMarchingRunner<unsigned short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_INT:
      {
      FastMarchingRunner<signed int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_INT:
      {
      FastMarchingRunner<unsigned int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_LONG:
      {
      FastMarchingRunner<signed long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_LONG:
      {
      FastMarchingRunner<unsigned long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_FLOAT:
      {
      FastMarchingRunner<float> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_DOUBLE:
      {
      FastMarchingRunner<double> runner;
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

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Stopping Value");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "50.0");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Defines a stopping value for the time up to which the front propagation will be computed.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "1 1000 1");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Speed image normalization factor");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarMaximum( info ) );
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Factor to be used for dividing the pixel values of the speed image. This allows to use images of integer pixel type for representing the speed. The normalization should map the values of the integer image into the range [0,1]");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarTypeRange( info ) );

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
  
void VV_PLUGIN_EXPORT vvITKFastMarchingInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Fast Marching (ITK)");
  info->SetProperty(info, VVP_GROUP, "Segmentation - Level Sets");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Fast Marching Level Set");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "Fast Marcing uses a Level Set representation for propagating a front from user-provided seed points. A user-provided speed image is used for controling the front propagation");

  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "2");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "2");

}

}
