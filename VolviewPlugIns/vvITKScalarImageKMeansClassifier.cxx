/* perform a pixel-wise intensity transformation using a ScalarImageKMeansClassifier function */

#include "vvITKFilterModule.h"

#include "itkScalarImageKmeansImageFilter.h"



template <class InputPixelType>
class ScalarImageKMeansClassifierRunner
  {
  public:
      typedef  InputPixelType                       PixelType;
      typedef  itk::Image< PixelType, 3 >           ImageType; 
      typedef  itk::ScalarImageKmeansImageFilter< 
                                                  ImageType  >    FilterType;
      typedef  VolView::PlugIn::FilterModule< FilterType >        ModuleType;

  public:
    ScalarImageKMeansClassifierRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Performing Classification with a K-Means algorithm");
      const unsigned int numberOfClasses = atoi( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
      for(unsigned int k=0; k<numberOfClasses; k++)
        {
        module.GetFilter()->AddClassWithInitialMean( 0.0 );
        }

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
      ScalarImageKMeansClassifierRunner<signed char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      ScalarImageKMeansClassifierRunner<unsigned char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_SHORT:
      {
      ScalarImageKMeansClassifierRunner<signed short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      ScalarImageKMeansClassifierRunner<unsigned short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_INT:
      {
      ScalarImageKMeansClassifierRunner<signed int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_INT:
      {
      ScalarImageKMeansClassifierRunner<unsigned int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_LONG:
      {
      ScalarImageKMeansClassifierRunner<signed long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_LONG:
      {
      ScalarImageKMeansClassifierRunner<unsigned long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_FLOAT:
      {
      ScalarImageKMeansClassifierRunner<float> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_DOUBLE:
      {
      ScalarImageKMeansClassifierRunner<double> runner;
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

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Number of Classes");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "4");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Number of classes to be used for the K-Means classification. The number of classes is a critical decision in this algorithm, you want to make sure that you add at least one class more to the expected number of classes.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "1 20 1");


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
  
void VV_PLUGIN_EXPORT vvITKScalarImageKMeansClassifierInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Scalar Image K-Means");
  info->SetProperty(info, VVP_GROUP, "Segmentation Statistics");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
    "Performs classification of a Scalar image using the K-Means algorithm");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filters performs statistical classification in a scalar image by applying the K-Means algortihm. The use must provide a number of classes in order to initialize the classification. The output is a labeled image encoded in 8 bits. It is assumed that no more than 256 class will be expected as output.");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "1");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "0"); 
}

}

