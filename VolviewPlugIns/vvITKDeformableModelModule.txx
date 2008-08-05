/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVDeformableModelModule_txx
#define _itkVVDeformableModelModule_txx

#include "vvITKDeformableModelModule.h"
#include "itkCovariantVector.h"

namespace VolView 
{

namespace PlugIn
{

/*
 *    Constructor
 */
template <class TInputPixelType >
DeformableModelModule<TInputPixelType>
::DeformableModelModule()
{
    m_ImportFilter               = ImportFilterType::New();
    m_GradientMagnitudeFilter    = GradientMagnitudeFilterType::New();
    m_GradientFilter             = GradientFilterType::New();
    m_DeformableModelFilter      = DeformableModelFilterType::New();
    m_MeshSource                 = MeshSourceType::New();

    // Set up the pipeline
    m_GradientMagnitudeFilter->SetInput(  m_ImportFilter->GetOutput()             );
    m_GradientFilter->SetInput(           m_GradientMagnitudeFilter->GetOutput()  );

    m_DeformableModelFilter->SetInput(    m_MeshSource->GetOutput()               );
    m_DeformableModelFilter->SetGradient( m_GradientFilter->GetOutput()           );

    // Allow progressive release of memory as the pipeline is executed
    m_GradientFilter->ReleaseDataFlagOn();
    m_GradientMagnitudeFilter->ReleaseDataFlagOn();

    // Set the Observer for updating progress in the GUI
    m_MeshSource->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
    m_GradientFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
    m_GradientMagnitudeFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
    m_DeformableModelFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );

    m_MeshSource->AddObserver( itk::StartEvent(), this->GetCommandObserver() );
    m_GradientFilter->AddObserver( itk::StartEvent(), this->GetCommandObserver() );
    m_GradientMagnitudeFilter->AddObserver( itk::StartEvent(), this->GetCommandObserver() );
    m_DeformableModelFilter->AddObserver( itk::StartEvent(), this->GetCommandObserver() );

    m_MeshSource->AddObserver( itk::EndEvent(), this->GetCommandObserver() );
    m_GradientFilter->AddObserver( itk::EndEvent(), this->GetCommandObserver() );
    m_GradientMagnitudeFilter->AddObserver( itk::EndEvent(), this->GetCommandObserver() );
    m_DeformableModelFilter->AddObserver( itk::EndEvent(), this->GetCommandObserver() );
}


/*
 *    Destructor
 */
template <class TInputPixelType >
DeformableModelModule<TInputPixelType>
::~DeformableModelModule()
{
 
}




/*
 *  Performs the actual filtering on the data 
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::ProcessData( const vtkVVProcessDataStruct * pds )
{

  this->SetUpdateMessage("Computing Deformable Model...");

  vtkVVPluginInfo * info = this->GetPluginInfo();

  const float radiusX               = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ));
  const float radiusY               = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ));
  const float radiusZ               = atof( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ));
  const float sigma                 = atof( info->GetGUIProperty(info, 3, VVP_GUI_VALUE ));
  const float stiffness             = atof( info->GetGUIProperty(info, 4, VVP_GUI_VALUE ));
  const float externalForceWeight   = atof( info->GetGUIProperty(info, 5, VVP_GUI_VALUE ));
  const float timeStep              = atof( info->GetGUIProperty(info, 6, VVP_GUI_VALUE ));

  const unsigned int numberOfIterations  = atoi( info->GetGUIProperty(info, 7, VVP_GUI_VALUE ));
  const unsigned int resolutionX         = atoi( info->GetGUIProperty(info, 8, VVP_GUI_VALUE ));
  const unsigned int resolutionY         = atoi( info->GetGUIProperty(info, 9, VVP_GUI_VALUE ));

  const unsigned int numberOfSeeds = info->NumberOfMarkers;
  if( numberOfSeeds < 1 )
    {
    info->SetProperty( info, VVP_ERROR, "Please select the center of the initial spherical model using the 3D Markers in the Annotation menu" ); 
    return;
    }

  PointType center;
  center[0] = info->Markers[0];
  center[1] = info->Markers[0];
  center[2] = info->Markers[0];
  m_MeshSource->SetCenter( center );

  PointType radius;
  radius[0] = radiusX;
  radius[1] = radiusY;
  radius[2] = radiusZ;
  m_MeshSource->SetScale( radius );

  m_GradientMagnitudeFilter->SetSigma( sigma );
  m_GradientFilter->SetSigma( sigma );

  itk::CovariantVector<double, 2>   stiffnessVector;
  stiffnessVector[0] = stiffness;
  stiffnessVector[1] = stiffness;
  m_DeformableModelFilter->SetStiffness( stiffnessVector );

  m_DeformableModelFilter->SetGradientMagnitude( externalForceWeight );
  m_DeformableModelFilter->SetTimeStep( timeStep );
  m_DeformableModelFilter->SetStepThreshold( numberOfIterations );

  m_MeshSource->SetResolutionX( resolutionX );
  m_MeshSource->SetResolutionY( resolutionY );

  ofs.open("track.txt");

  SizeType   size;
  IndexType  start;

  double     origin[3];
  double     spacing[3];


  size[0]     =  info->InputVolumeDimensions[0];
  size[1]     =  info->InputVolumeDimensions[1];
  size[2]     =  info->InputVolumeDimensions[2];

  for(unsigned int i=0; i<3; i++)
    {
    origin[i]   =  info->InputVolumeOrigin[i];
    spacing[i]  =  info->InputVolumeSpacing[i];
    start[i]    =  0;
    }

  RegionType region;

  region.SetIndex( start );
  region.SetSize(  size  );
 
  m_ImportFilter->SetSpacing( spacing );
  m_ImportFilter->SetOrigin(  origin  );
  m_ImportFilter->SetRegion(  region  );

  const unsigned int totalNumberOfPixels = region.GetNumberOfPixels();

  const bool         importFilterWillDeleteTheInputBuffer = false;

  const unsigned int numberOfPixelsPerSlice = size[0] * size[1];

  InputPixelType *   dataBlockStart = 
                        static_cast< InputPixelType * >( pds->inData )  
                      + numberOfPixelsPerSlice * pds->StartSlice;

  m_ImportFilter->SetImportPointer( dataBlockStart, 
                                    totalNumberOfPixels,
                                    importFilterWillDeleteTheInputBuffer );

  // Execute the filters and progressively remove temporary memory
  this->SetCurrentFilterProgressWeight( 0.02 );
  this->SetUpdateMessage("Preprocessing: Generating Initial Mesh...");
  m_MeshSource->Update();
  
  this->SetCurrentFilterProgressWeight( 0.19 );
  this->SetUpdateMessage("Preprocessing: computing gradient magnitude...");
  m_GradientMagnitudeFilter->Update();

  this->SetCurrentFilterProgressWeight( 0.19 );
  this->SetUpdateMessage("Preprocessing: computing gradient...");
  m_GradientFilter->Update();
  
  this->SetCurrentFilterProgressWeight( 0.60 );
  this->SetUpdateMessage("Computing Deformable Model...");
  m_DeformableModelFilter->Update();

  this->PostProcessData( pds );

  char tmp[1024];
  sprintf( tmp, "The resulting mesh has\n %ld Points \n %ld Cells ", 
                                m_DeformableModelFilter->GetOutput()->GetNumberOfPoints(),
                                m_DeformableModelFilter->GetOutput()->GetNumberOfCells() );
  info->SetProperty( info, VVP_REPORT_TEXT, tmp );

  ofs.close();

} // end of ProcessData



/*
 *  Performs post-processing of data. 
 *  This involves an intensity window operation and
 *  data copying into the volview provided buffer.
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::PostProcessData( const vtkVVProcessDataStruct * pds )
{


  // A change in ProcessData signature could prevent this const_cast...
  vtkVVProcessDataStruct * opds = const_cast<vtkVVProcessDataStruct *>( pds );
  vtkVVPluginInfo * info = this->GetPluginInfo();

  // Temporarily use the sphere output, just to debug the convertion from ITK mesh
  // to Plugin mesh.
  typedef typename MeshType::Pointer  MeshPointer;
  MeshPointer mesh = m_DeformableModelFilter->GetOutput();

  // now put the results into the data structure
  const unsigned int numberOfPoints = mesh->GetNumberOfPoints();
  opds->NumberOfMeshPoints = numberOfPoints;

  float * points = new float[ numberOfPoints * 3 ];
  opds->MeshPoints = points;
  float * outputPointsItr = points;
  typedef typename MeshType::PointsContainer      PointsContainer;
  typedef typename PointsContainer::ConstIterator PointIterator;
  PointIterator pointItr  = mesh->GetPoints()->Begin();
  PointIterator pointsEnd = mesh->GetPoints()->End();

  while( pointItr != pointsEnd )
    {
    ofs << pointItr.Value() << std::endl;
    *outputPointsItr++ = pointItr.Value()[0]; 
    *outputPointsItr++ = pointItr.Value()[1]; 
    *outputPointsItr++ = pointItr.Value()[2]; 
    ++pointItr;
    }


  opds->NumberOfMeshCells = mesh->GetNumberOfCells();
  unsigned int numEntries = 0;
  
  typedef typename MeshType::CellsContainer         CellsContainer;
  typedef typename CellsContainer::ConstIterator    CellIterator;
  CellIterator cellItr = mesh->GetCells()->Begin();
  CellIterator cellEnd = mesh->GetCells()->End();

  // Cell connectivity entries follow the format of vtkCellArray:
  // n1, id1, id2,... idn1,  n2, id1, id2,.. idn2....
  while( cellItr != cellEnd )
    {
    // one position for the number of points
    numEntries += 1;  
    // plus one position per each point Id
    numEntries += cellItr.Value()->GetNumberOfPoints();
    ++cellItr;
    }

  int * cellsTopology = new int [ numEntries ];
  opds->MeshCells = cellsTopology;

  typedef typename MeshType::CellType               CellType;
  typedef typename CellType::PointIdConstIterator   PointIdIterator;

  cellItr = mesh->GetCells()->Begin();
  int * cellsTopItr = cellsTopology;

  while( cellItr != cellEnd )
    {
    const CellType * cell = cellItr.Value();
    const unsigned int np = cell->GetNumberOfPoints();
    ofs << std::endl << np << "  ";
    *cellsTopItr = np;
    ++cellsTopItr;
    PointIdIterator pointIdItr = cell->PointIdsBegin();
    PointIdIterator pointIdEnd = cell->PointIdsEnd();
    while( pointIdItr != pointIdEnd )
      {
      ofs <<  *pointIdItr << "  ";
      *cellsTopItr = *pointIdItr;
      ++cellsTopItr;
      ++pointIdItr;
      }
    ++cellItr;
    }
    


  // return the polygonal data
  info->AssignPolygonalData(info, opds);

  // clean up
  delete [] cellsTopology;
  delete [] points;
  
} // end of PostProcessData


} // end of namespace PlugIn

} // end of namespace Volview

#endif
