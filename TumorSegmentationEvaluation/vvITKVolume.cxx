/*ISIS Center Georgetown University
This plugin calculate Volume.*/
#include "vvITKFilterModule.h"
#include "itkStatisticsImageFilter.h"
template <class InputPixelType>
void vvVolumeTemplate(vtkVVPluginInfo *info,
                             vtkVVProcessDataStruct *pds, 
                             InputPixelType *)
{
  typedef itk::Image< InputPixelType, 3 > InputImageType;
  typedef  itk::StatisticsImageFilter<InputImageType>     FilterType;
  typedef  VolView::PlugIn::FilterModule< FilterType>   ModuleType;

  char tmp[1024];
  ModuleType  module;
  
  module.SetUpdateMessage("Computing Volume...");
  module.SetPluginInfo( info );
  module.InitializeProgressValue();
  module.ProcessData( pds  );
  InputImageType::SpacingType spacing =module.GetInput()->GetSpacing();
  double factor = 1.0;
  for(unsigned int d=0; d<3; d++)
    {
    factor *= spacing[d];
    }
  itk::StatisticsImageFilter<InputImageType>::RealType Volume= module.GetFilter()->GetSum();
  sprintf(tmp,"%f mm^3",Volume*factor);
  info->SetProperty( info, VVP_REPORT_TEXT, tmp );
  InputImageType::ConstPointer outputImage= module.GetInput();

  typedef itk::ImageRegionConstIterator< InputImageType >  OutputIteratorType;
  OutputIteratorType ot( outputImage, outputImage->GetBufferedRegion() );
  ot.GoToBegin(); 
  InputPixelType * outData = (InputPixelType *)(pds->outData);

  while( !ot.IsAtEnd() )
    {
    *outData = ot.Get();  // copy output pixel
    ++outData;
    ++ot;
    }
}

static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;
  
  try
    {
     switch (info->InputVolumeScalarType)
      {
      // invoke the appropriate templated function
      vtkTemplateMacro3(vvVolumeTemplate, info, pds, 
                     static_cast<VTK_TT *>(0));
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
  
void VV_PLUGIN_EXPORT vvITKVolumeInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Volume(ITK)");
  info->SetProperty(info, VVP_GROUP, "ISIS - Volume comparison");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Compute Volume");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This module calculate Volume.");

  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "8"); 
}

}
