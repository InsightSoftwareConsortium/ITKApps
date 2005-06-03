/* ISIS Center Georgetown University
This plugin calculate Hausdorff Distance.*/
#include "vvITKFilterModuleTwoInputs.h"
#include "itkHausdorffDistanceImageFilter.h"

template <class InputPixelType1, class InputPixelType2>
void vvHausdorffDistanceTemplate2(vtkVVPluginInfo *info,
                             vtkVVProcessDataStruct *pds, 
                             InputPixelType1 *, InputPixelType2 *)
{
  typedef itk::Image< InputPixelType1, 3 > InputImageType1;
  typedef itk::Image< InputPixelType2, 3 > InputImageType2;
  typedef  itk::HausdorffDistanceImageFilter<InputImageType1,InputImageType2>     FilterType;
  typedef  VolView::PlugIn:: FilterModuleTwoInputs< FilterType,InputImageType1,InputImageType2 >   ModuleType;

  char tmp[1024];
  ModuleType  module;
  
  module.SetUpdateMessage("Computing Hausdorff Distance...");
  module.SetPluginInfo( info );
  module.InitializeProgressValue();
  module.SetUpdateMessage("Passed...");
  module.GetFilter()->SetInput1((InputImageType1 *)module.GetInput1());
  module.GetFilter()->SetInput2((InputImageType2 *)module.GetInput2() );
  module.GetFilter()->ReleaseDataFlagOn();
  module.ProcessData( pds  );
  module.GetFilter()->Update();
  itk::HausdorffDistanceImageFilter<InputImageType1,InputImageType2>::RealType distance = module.GetFilter()->GetHausdorffDistance();
    
  InputImageType1::SpacingType spacing =module.GetInput1()->GetSpacing();
  double factor = 1.0;
  for(unsigned int d=0; d<3; d++)
    {
    factor *= spacing[d];
    }
  sprintf(tmp,"%f mm^3",distance*factor);
  info->SetProperty( info, VVP_REPORT_TEXT, tmp );
  InputImageType1::ConstPointer outputImage= module.GetInput1();
  typedef itk::ImageRegionConstIterator< InputImageType1 >  OutputIteratorType;
  OutputIteratorType ot( outputImage, outputImage->GetBufferedRegion() );
  ot.GoToBegin(); 
  InputPixelType1 * outData = (InputPixelType1 *)(pds->outData);
  while( !ot.IsAtEnd() )
    {
    *outData = ot.Get();  // copy output pixel
    ++outData;
    ++ot;
    }
}

template <class IT>
void vvHausdorffDistanceTemplate(vtkVVPluginInfo *info,
                            vtkVVProcessDataStruct *pds, 
                            IT *)
{
  switch (info->InputVolume2ScalarType)
    {
    // invoke the appropriate templated function
    vtkTemplateMacro4(vvHausdorffDistanceTemplate2, info, pds, 
                      static_cast<IT *>(0), static_cast<VTK_TT *>(0));
    }
  return;
}

static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;
  try
    {
    switch (info->InputVolumeScalarType)
      {
      // invoke the appropriate templated function
      vtkTemplateMacro3(vvHausdorffDistanceTemplate, info, pds, 
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
  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Testing value.");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "1.0");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "No help yet");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "0.1 10.0 0.1");

  //info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP, "0");
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
  
void VV_PLUGIN_EXPORT vvITKHausdorffDistanceInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Hausdorff Distance(ITK)");
  info->SetProperty(info, VVP_GROUP, "ISIS - Volume comparison");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Hausdorff Distance calculation");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This module calculate Hausdorff Distance between two segmented volumes.");

  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "1");
  info->SetProperty(info,  VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "1"); 
  info->SetProperty(info, VVP_REQUIRES_SECOND_INPUT,        "1");

}

}
