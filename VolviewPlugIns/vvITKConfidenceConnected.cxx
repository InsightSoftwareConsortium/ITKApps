/* perform segmentation using the confidence connected image filter */

#include "vvITKFilterModule.h"

#include "itkConfidenceConnectedImageFilter.h"

static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  const unsigned int Dimension = 3;

  const unsigned int numberOfIterations = atoi( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
  const float        multiplier         = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );
  const int          replaceValue       = atoi( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ) );
  const unsigned int initialRadius      = atoi( info->GetGUIProperty(info, 3, VVP_GUI_VALUE ) );
  
  if( info->NumberOfMarkers < 1 )
    {
    info->SetProperty( info, VVP_ERROR, "Please select a seed point using the 3D Markers in the Annotation menu" ); 
    return -1;
    }

  // Take the first marker as the seed point
  const float * seedCoordinates = info->Markers;

  itk::Index<Dimension> seed;
  for(unsigned int i=0; i<3; i++)
    { 
    seed[i] =  static_cast< int >( 
         (seedCoordinates[i] - info->InputVolumeOrigin[i] ) / info->InputVolumeSpacing[i] );
    }

  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_UNSIGNED_CHAR:
      {
      typedef  unsigned char                            InputPixelType;
      typedef  itk::Image< InputPixelType, Dimension >  InputImageType; 
      typedef  unsigned char                            OutputPixelType;
      typedef  itk::Image< OutputPixelType, Dimension > OutputImageType; 
      typedef  InputImageType::IndexType            IndexType;
      typedef  itk::ConfidenceConnectedImageFilter< InputImageType,  
                                                    OutputImageType >   FilterType;
      VolView::PlugIn::FilterModule< FilterType > module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Confidence Connected Region Growing...");
      // Set the parameters on it
      module.GetFilter()->SetNumberOfIterations(        numberOfIterations );
      module.GetFilter()->SetMultiplier(                multiplier         );
      module.GetFilter()->SetReplaceValue(              replaceValue       );
      module.GetFilter()->SetInitialNeighborhoodRadius( initialRadius      );
      module.GetFilter()->SetSeed(                      seed               );
      // Execute the filter
      module.ProcessData( pds  );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      typedef  unsigned short                            InputPixelType;
      typedef  itk::Image< InputPixelType, Dimension >   InputImageType; 
      typedef  unsigned char                             OutputPixelType;
      typedef  itk::Image< OutputPixelType, Dimension >  OutputImageType; 
      typedef  InputImageType::IndexType            IndexType;
      typedef  itk::ConfidenceConnectedImageFilter< InputImageType,  
                                                    OutputImageType >   FilterType;
      VolView::PlugIn::FilterModule< FilterType > module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Confidence Connected Region Growing...");
      // Set the parameters on it
      module.GetFilter()->SetNumberOfIterations(        numberOfIterations );
      module.GetFilter()->SetMultiplier(                multiplier         );
      module.GetFilter()->SetReplaceValue(              replaceValue       );
      module.GetFilter()->SetInitialNeighborhoodRadius( initialRadius      );
      module.GetFilter()->SetSeed(                      seed               );
      // Execute the filter
      module.ProcessData( pds );
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
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Number of times that region growing process should be repeated. If initial statistical estimations are good, further iterations may improve the segmentation. After a certain number of iterations the result will not change. ");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "1 20 1");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Variance Multiplier");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, "2.5");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Factor that will multiplies the variance of the pixels region in order to define the intensity range for which new pixels will be accepted in the region.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , "0.1 10.0 0.1");

  info->SetGUIProperty(info, 2, VVP_GUI_LABEL, "Replace Value");
  info->SetGUIProperty(info, 2, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 2, VVP_GUI_DEFAULT, "255");
  info->SetGUIProperty(info, 2, VVP_GUI_HELP, "Value to assign to the binary mask of the segmented region. The rest of the image will be set to zero.");
  info->SetGUIProperty(info, 2, VVP_GUI_HINTS , "1 255.0 1.0");

  info->SetGUIProperty(info, 3, VVP_GUI_LABEL, "Initial Neighborhood Radius");
  info->SetGUIProperty(info, 3, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 3, VVP_GUI_DEFAULT, "2");
  info->SetGUIProperty(info, 3, VVP_GUI_HELP, "Size of the initial neighborhood used to compute the statistics of the region. If the region in which the seed point is placed happens to be a homogeneous intensity distribution, increasing this radius will safely improve the statistical estimation of mean and variance. Make sure that the radius is not large enough to make contours participate in the computation of the estimation. That is, from the seed point to the nearest important edge, there should be a distance larger than this radius.");
  info->SetGUIProperty(info, 3, VVP_GUI_HINTS , "1 20.0 1.0");

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
  
void VV_PLUGIN_EXPORT vvITKConfidenceConnectedInit(vtkVVPluginInfo *info)
{
  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Confidence Connected (ITK) ");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                "Confidence Connected Segmentation");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This filter applies an region growing algorithm for segmentation. The criterion for including new pixels in the region is defined by an intensity range around the mean value of the pixels existing in the region. The extent of the intensity interval is computed as the product of the variance and a multiplier provided by the user. The coordinates of a seed point are used as the initial position for start growing the region.");
  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "4");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "1");

  
}

}

