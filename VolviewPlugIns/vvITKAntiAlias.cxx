/* perform smoothing using an anisotropic diffusion filter */

#include "vvITKFilterModuleWithCasting.h"

#include "itkAntiAliasBinaryImageFilter.h"



template <class InputPixelType>
class AntiAliasRunner
  {
  public:
      typedef  InputPixelType                       PixelType;
      typedef  itk::Image< PixelType, 3 >           ImageType; 

      typedef   float       InternalPixelType;
      typedef   itk::Image< InternalPixelType,3 > InternalImageType; 

      typedef   itk::AntiAliasBinaryImageFilter< 
                                    InternalImageType,  
                                    InternalImageType >   FilterType;
     
      typedef  InputPixelType                        OutputPixelType;

      typedef  VolView::PlugIn::FilterModuleWithCasting< 
                                                InputPixelType,
                                                FilterType,
                                                OutputPixelType > ModuleType;

  public:
    AntiAliasRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      const unsigned int maxNumberOfIterations = atoi( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
      const float        maximumRMSError       = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );

      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Reducing aliasing effects...");
      // Set the parameters on it
      module.GetFilter()->SetMaximumIterations(     maxNumberOfIterations );
      module.GetFilter()->SetMaximumRMSError(       maximumRMSError    );
      module.GetFilter()->SetNumberOfLayers(  3 );
      // Execute the filter
      module.ProcessData( pds  );
    }
  };


static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  // make sure there is only one component of input data
  if (info->InputVolumeNumberOfComponents != 1)
    {
    info->SetProperty( info, VVP_ERROR, 
                       "The AntiAlias filter only works with single component data" ); 
    return -1;
    }
  

  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_CHAR:
      {
      AntiAliasRunner<signed char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      AntiAliasRunner<unsigned char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_SHORT:
      {
      AntiAliasRunner<signed short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      AntiAliasRunner<unsigned short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_INT:
      {
      AntiAliasRunner<signed int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_INT:
      {
      AntiAliasRunner<unsigned int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_LONG:
      {
      AntiAliasRunner<signed long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_LONG:
      {
      AntiAliasRunner<unsigned long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_FLOAT:
      {
      AntiAliasRunner<float> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_DOUBLE:
      {
      AntiAliasRunner<double> runner;
      runner.Execute( info, pds );
      break; 
      }
    default:
      info->SetProperty( info, VVP_ERROR, "Pixel Type Unknown for the AntiAlias filter" ); 
      return -1;
      break;
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

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Number of Iterations ");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "5");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Number of times that the diffusion approximation will be computed. The more iterations, the stronger the smoothing");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "1 100 1");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Maximum RMS Error");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, "0.05");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Maximum RMS error allows. This value defines the convergence criterion for the smoothing.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , "0.001 0.1 0.001");

  const char * stringValue = info->GetGUIProperty(info, 0, VVP_GUI_VALUE );
  if( !stringValue )
    {
    info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, "0");
    }
  else
    {
    info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, stringValue);
    }
  

  info->OutputVolumeScalarType = info->InputVolumeScalarType;
  info->OutputVolumeNumberOfComponents = 1;

  info->OutputVolumeDimensions[0] = info->InputVolumeDimensions[0]; // + 2 * numberOfIterations;
  info->OutputVolumeDimensions[1] = info->InputVolumeDimensions[1]; // + 2 * numberOfIterations;
  info->OutputVolumeDimensions[2] = info->InputVolumeDimensions[2]; // + 2 * numberOfIterations;

  memcpy(info->OutputVolumeSpacing,info->InputVolumeSpacing,
         3*sizeof(float));
  memcpy(info->OutputVolumeOrigin,info->InputVolumeOrigin,
         3*sizeof(float));

  return 1;
}

extern "C" {
  
void VV_PLUGIN_EXPORT vvITKAntiAliasInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Anti-Aliasing (ITK)");
  info->SetProperty(info, VVP_GROUP, "Surface Generation");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                  "Reduction of aliasing effects");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filter applies a level set evolution over a binary image in order to produce a smoother contour that is suitable for extracting iso-surfaces. The resulting contour is encoded as the zero-set of the output level set. The zero set will be rescaled as the mid-value of the intensity range corresponding to the pixel type used. This filter processes the whole image in one piece, and does not change the dimensions, data type, or spacing of the volume.");

  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "2");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "8"); 

}

}
