/* perform smoothing using an anisotropic diffusion filter */

#include "vvITKFilterModuleWithCasting.h"

#include "itkCurvatureFlowImageFilter.h"


template <class InputPixelType>
class CurvatureFlowRunner
  {
  public:
      typedef   float       InternalPixelType;
      typedef   itk::Image< InternalPixelType, 3 > InternalImageType; 

      typedef   itk::CurvatureFlowImageFilter< 
                                        InternalImageType,  
                                        InternalImageType >   FilterType;
 
      typedef  InputPixelType                            OutputPixelType;                                          

      typedef  VolView::PlugIn::FilterModuleWithCasting< InputPixelType, 
                                                         FilterType,
                                                         OutputPixelType > ModuleType;
  public:
    CurvatureFlowRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      const unsigned int numberOfIterations = atoi(  info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
      const float        timeStep           = atof(  info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );

      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Smoothing with Curvature Flow...");
      // Set the parameters on it
      module.GetFilter()->SetNumberOfIterations( numberOfIterations );
      module.GetFilter()->SetTimeStep( timeStep );
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
      CurvatureFlowRunner<signed char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      CurvatureFlowRunner<unsigned char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_SHORT:
      {
      CurvatureFlowRunner<signed short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      CurvatureFlowRunner<unsigned short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_INT:
      {
      CurvatureFlowRunner<signed int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_INT:
      {
      CurvatureFlowRunner<unsigned int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_LONG:
      {
      CurvatureFlowRunner<signed long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_LONG:
      {
      CurvatureFlowRunner<unsigned long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_FLOAT:
      {
      CurvatureFlowRunner<float> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_DOUBLE:
      {
      CurvatureFlowRunner<double> runner;
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

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Number of Iterations");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "5");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Number of times that the diffusion approximation will be computed. The more iterations, the stronger the smoothing");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "1 100 1");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Time Step");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, "0.05");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Discretization of time for approximating the diffusion process.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , "0.01 1.0 0.005");

  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, "0");
  
  info->OutputVolumeScalarType = info->InputVolumeScalarType;
  info->OutputVolumeNumberOfComponents = info->InputVolumeNumberOfComponents; 
  memcpy(info->OutputVolumeDimensions,info->InputVolumeDimensions,
         3*sizeof(int));
  memcpy(info->OutputVolumeSpacing,info->InputVolumeSpacing,
         3*sizeof(float));
  memcpy(info->OutputVolumeOrigin,info->InputVolumeOrigin,
         3*sizeof(float));

  return 1;
}


extern "C" {
  
void VV_PLUGIN_EXPORT vvITKCurvatureFlowInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Curvature Flow (ITK)");
  info->SetProperty(info, VVP_GROUP, "Noise Suppression");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                 "Anisotropic diffusion smoothing");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filter applies an edge-preserving smoothing to a volume by computing the evolution of an anisotropic diffusion partial differential equation. Diffusion is regulated by the curvature of iso-contours in the image. This filter processes the whole image in one piece, and does not change the dimensions, data type, or spacing of the volume.");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "2");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "8");
  
}

}

