/* This plugin adapts the ThresholdSegmentationLevelSet filter.
   It does not perform any preprocessing. The user should provide
   the speed image and the initial level set as inputs. */

#include "vvITKThresholdSegmentationLevelSet.h"



template <class InputPixelType>
class ThresholdSegmentationLevelSeteRunner
  {
  public:
    typedef itk::Image< InputPixelType, 3 > InputImageType;
    typedef InputImageType                  InputFeatureImageType;
    
    typedef VolView::PlugIn::ThresholdSegmentationLevelSet< 
                                                  InputImageType,
                                                  InputFeatureImageType >   ModuleType;
    
  public:
    ThresholdSegmentationLevelSeteRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      char tmp[1024];

      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Computing Segmentation Threshold Level Set...");
      module.ProcessData( pds  );
      sprintf(tmp,"Total number of iterations = %d \n Final RMS error = %g",
                         module.GetFilter()->GetElapsedIterations(),
                         module.GetFilter()->GetRMSChange());
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

  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_CHAR:
      {
      ThresholdSegmentationLevelSeteRunner<signed char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      ThresholdSegmentationLevelSeteRunner<unsigned char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_SHORT:
      {
      ThresholdSegmentationLevelSeteRunner<signed short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      ThresholdSegmentationLevelSeteRunner<unsigned short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_INT:
      {
      ThresholdSegmentationLevelSeteRunner<signed int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_INT:
      {
      ThresholdSegmentationLevelSeteRunner<unsigned int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_LONG:
      {
      ThresholdSegmentationLevelSeteRunner<signed long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_LONG:
      {
      ThresholdSegmentationLevelSeteRunner<unsigned long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_FLOAT:
      {
      ThresholdSegmentationLevelSeteRunner<float> runner;
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

  info->UpdateProgress( info, 1.0, "Segmentation Threshold LevelSet Done !");

  return 0;
}


static int UpdateGUI(void *inf)
{
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Lower Threshold.");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarMinimum( info ) );
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Lower threshold for binarizing the feature image and induce attraction to the edges");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS, VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRange( info ) );

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Upper Threshold.");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarMinimum( info ) );
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Upper threshold for binarizing the feature image and induce attraction to the edges");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS, VolView::PlugIn::FilterModuleBase::GetInputVolumeScalarRange( info ) );

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL, "Surface Smoothness");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "1.0" );
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "Degree of smoothness of the surface. This terms affects the curvature scaling of the level set");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS, "0.0 20.0 0.1");


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
  
void VV_PLUGIN_EXPORT vvITKThresholdSegmentationLevelSetInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Segmentation Threshold Level Set (ITK)");
  info->SetProperty(info, VVP_GROUP, "Segmentation - Level Sets");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Segmentation Threshold LevelSet");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This module applies the Segmentation Threshold Level Set method for segmenting a volume. This method evolves a level set under the attraction of the edges of a binary image. This binary image is computed by thresholding the feature image provided here as the second input. No preprocessing is performed here. The user must provide as inputs an initial level set and the feature image which will be used to compute speeds. The current image being visualized will be taken as the initial level set. The speed image required  as a second input can be specified by providinng a file name in the GUI.");

  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "3");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,   "16");
  info->SetProperty(info, VVP_REQUIRES_SECOND_INPUT,        "1");

}

}
