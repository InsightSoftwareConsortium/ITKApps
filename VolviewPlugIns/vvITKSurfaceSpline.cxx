/* This module encapsulates the full preprocessing required for
   applying the DeformableModel image filter for segmenting the 
   volume.  It requires seed points and the original image as inputs. */


#include "vvITKSurfaceSpline.txx"



template <class InputPixelType>
class SurfaceSplineRunner
  {
  public:
      typedef VolView::PlugIn::SurfaceSpline< 
                                            InputPixelType >   ModuleType;
  public:
    SurfaceSplineRunner() {}
    void Execute( vtkVVPluginInfo *info, vtkVVProcessDataStruct *pds )
    {
      const unsigned int pointsAlongRows  = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ));
      const unsigned int pointsAlongCols  = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ));

      ModuleType  module;
      module.SetPluginInfo( info );
      module.SetUpdateMessage("Computing Surface Spline...");
      module.SetNumberOfPointsAlongRows( pointsAlongRows );
      module.SetNumberOfPointsAlongColumns( pointsAlongCols );
      // Execute the filter
      module.ProcessData( pds  );
    }
  };



static int ProcessData(void *inf, vtkVVProcessDataStruct *pds)
{

  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;


  const unsigned int numberOfSeeds = info->NumberOfMarkers;
  if( numberOfSeeds != 9 )
    {
    info->SetProperty( info, VVP_ERROR, "This plugins requires the user to provide 9 points as 3D markers"); 
    return -1;
    }

  try 
  {
  switch( info->InputVolumeScalarType )
    {
    case VTK_CHAR:
      {
      SurfaceSplineRunner<signed char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_CHAR:
      {
      SurfaceSplineRunner<unsigned char> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_SHORT:
      {
      SurfaceSplineRunner<signed short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_SHORT:
      {
      SurfaceSplineRunner<unsigned short> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_INT:
      {
      SurfaceSplineRunner<signed int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_INT:
      {
      SurfaceSplineRunner<unsigned int> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_LONG:
      {
      SurfaceSplineRunner<signed long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_UNSIGNED_LONG:
      {
      SurfaceSplineRunner<unsigned long> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_FLOAT:
      {
      SurfaceSplineRunner<float> runner;
      runner.Execute( info, pds );
      break; 
      }
    case VTK_DOUBLE:
      {
      SurfaceSplineRunner<double> runner;
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


static int UpdateGUI( void *inf )
{
  vtkVVPluginInfo *info = (vtkVVPluginInfo *)inf;

  info->SetGUIProperty(info, 0, VVP_GUI_LABEL, "Number of points in rows");
  info->SetGUIProperty(info, 0, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 0, VVP_GUI_DEFAULT, "21.0");
  info->SetGUIProperty(info, 0, VVP_GUI_HELP, "Number of points to along the rows of the spline surface. This is used to define the resolution of the grid that resamples the spline.");
  info->SetGUIProperty(info, 0, VVP_GUI_HINTS , "3 200 1");

  info->SetGUIProperty(info, 1, VVP_GUI_LABEL, "Number of points in columns");
  info->SetGUIProperty(info, 1, VVP_GUI_TYPE, VVP_GUI_SCALE);
  info->SetGUIProperty(info, 1, VVP_GUI_DEFAULT, "21.0");
  info->SetGUIProperty(info, 1, VVP_GUI_HELP, "Number of points to along the columns of the spline surface. This is used to define the resolution of the grid that resamples the spline.");
  info->SetGUIProperty(info, 1, VVP_GUI_HINTS , "3 200 1");

  return 1;
}



extern "C" {
  
void VV_PLUGIN_EXPORT vvITKSurfaceSplineInit(vtkVVPluginInfo *info)
{
  vvPluginVersionCheck();

  // setup information that never changes
  info->ProcessData = ProcessData;
  info->UpdateGUI   = UpdateGUI;
  info->SetProperty(info, VVP_NAME, "Surface Spline (ITK)");
  info->SetProperty(info, VVP_GROUP, "Deformable - Models");
  info->SetProperty(info, VVP_TERSE_DOCUMENTATION,
                                    "Computes a Spline Surface");
  info->SetProperty(info, VVP_FULL_DOCUMENTATION,
    "This plugins generates a spline surface from controls defined by the user as 3D markers.");

  info->SetProperty(info, VVP_SUPPORTS_IN_PLACE_PROCESSING, "0");
  info->SetProperty(info, VVP_SUPPORTS_PROCESSING_PIECES,   "0");
  info->SetProperty(info, VVP_NUMBER_OF_GUI_ITEMS,          "2");
  info->SetProperty(info, VVP_REQUIRED_Z_OVERLAP,           "0");
  info->SetProperty(info, VVP_PER_VOXEL_MEMORY_REQUIRED,    "0");
  info->SetProperty(info, VVP_PRODUCES_MESH_ONLY,           "1");

}

}
