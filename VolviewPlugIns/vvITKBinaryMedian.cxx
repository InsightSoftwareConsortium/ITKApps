/* perform an intensity transformation by computing 
   the median value of a neighborhood  */

#include "vvITKFilterModule.h"

#include "itkBinaryMedianImageFilter.h"




template <class InputPixelType>
class BinaryMedianRunner
  {
  public:
      typedef  InputPixelType                       PixelType;
      typedef  itk::Image< PixelType, 3 >           ImageType; 
      typedef  itk::BinaryMedianImageFilter< ImageType,  ImageType >   FilterType;
      typedef  VolView::PlugIn::FilterModule< FilterType >       ModuleType;

  public:
    BinaryMedianRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      itk::Size< 3 > radius;
      radius[0] = atoi( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
      radius[1] = atoi( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );
      radius[2] = atoi( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ) );

      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Transforming intensities with a BinaryMedian filter...");
      module.GetFilter()->SetRadius( radius );
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
      BinaryMedianRunner<signed char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      BinaryMedianRunner<unsigned char> runner;
      runner.Execute( info, pds );
      break; 
      }
    default:
      info->SetProperty( info, VVP_ERROR, "This filter is intended for 8 bits binary images only" ); 
      return -1;
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
  char tmp[1024];
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Radius X");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "2");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Integer radius along the X axis of the neighborhood used to compute the BinaryMedian. The neighborhood is a rectangular region that extends this number of pixels around the pixel being computed. Setting a radius of 2 will use a neighborhood of size 5.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "1 5 1");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Radius Y");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, "2");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Integer radius along the Y axis of the neighborhood used to compute the BinaryMedian. The neighborhood is a rectangular region that extends this number of pixels around the pixel being computed. Setting a radius of 2 will use a neighborhood of size 5.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , "1 5 1");

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL, "Radius Z");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "2");
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "Integer radius along the Z axis of the neighborhood used to compute the BinaryMedian. The neighborhood is a rectangular region that extends this number of pixels around the pixel being computed. Setting a radius of 2 will use a neighborhood of size 5.");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , "1 5 1");

  const char * text = info->GetGUIProperty(info,2,VVP_GUI_VALUE);
  if( text )
    {
    sprintf(tmp,"%d", atoi( text ) ); 
    info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, tmp);
    }
  else
    {
    info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, "0");
    }
  
  
  info->OutputVolumeScalarType = info->InputVolumeScalarType;
  info->OutputVolumeNumberOfComponents = 
    info->InputVolumeNumberOfComponents;
  memcpy(info->OutputVolumeDimensions,info->InputVolumeDimensions,
         3*sizeof(int));
  memcpy(info->OutputVolumeSpacing,info->InputVolumeSpacing,
         3*sizeof(float));
  memcpy(info->OutputVolumeOrigin,info->InputVolumeOrigin,
         3*sizeof(float));

  // if multi component we have 1 scalar for input and 1 scalar for output
  if (info->InputVolumeNumberOfComponents > 1)
    {
    int sizeReq = 2*info->InputVolumeScalarSize;
    char tmps[500];
    sprintf(tmps,"%i",sizeReq);
    info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED, tmps); 
    }
  else
    {
    // otherwise no memory is required
    info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED, "0"); 
    }

  return 1;
}


extern "C" {
  
void VV_PLUGIN_EXPORT vvITKBinaryMedianInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Binary Median (ITK)");
  info->SetProperty(info, VVP_GROUP, "Noise Suppression");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
          "Replace each voxel with the median of its neighborhood");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filter is intended to be used in Binary images ONLY. It applies an a binary median filter to the image. This is equivalent to a voting filter in terms of a Cellular Automata. This filter is commonly used for removing noise from masks resulting from segmentation methods. You may find useful to apply the filter multiple times.");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "1");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "3");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "1");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "0");
}

}
