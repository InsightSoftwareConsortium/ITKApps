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
    m_GradientMagnitudeFilter->ReleaseDataFlagOn();
    m_GradientFilter->ReleaseDataFlagOn();
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
 *    Define the center of the sphere used to initialize the deformable model.
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::SetEllipsoidCenter( float centerX, float centerY, float centerZ )
{
  PointType center;
  center[0] = centerX;
  center[1] = centerY;
  center[2] = centerZ;
  m_MeshSource->SetCenter( center );
}



/*
 *    Define the radius of the sphere used to initialize the deformable model.
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::SetEllipsoidRadius( float rx, float ry, float rz )
{
  PointType radius;
  radius[0] = rx;
  radius[1] = ry;
  radius[2] = rz;
  m_MeshSource->SetScale( radius );
}


/*
 *  Set the Sigma value for the Gradient Magnitude filter
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::SetSigma( float value )
{
  m_GradientMagnitudeFilter->SetSigma( value );
  m_GradientFilter->SetSigma( value );
}



/*
 *  Set the Stiffness of the deformable model.
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::SetStiffness( float value )
{
  itk::CovariantVector<double, 2>   stiffnessVector;
  stiffnessVector[0] = value;
  stiffnessVector[1] = value;
  m_DeformableModelFilter->SetStiffness( stiffnessVector );
}


/*
 *  Set time Step
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::SetTimeStep( float value )
{
  m_DeformableModelFilter->SetTimeStep( value );
}



/*
 *  Set the factor for weighting external forces.
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::SetExternalForceWeight( float value )
{
  m_DeformableModelFilter->SetGradientMagnitude( value );
}



/*
 *  Set the number of iterations 
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::SetNumberOfIterations( unsigned int value )
{
  m_DeformableModelFilter->SetStepThreshold( value );
}




/*
 *  Performs the actual filtering on the data 
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::ProcessData( const vtkVVProcessDataStruct * pds )
{

  SizeType   size;
  IndexType  start;

  double     origin[3];
  double     spacing[3];

  const vtkVVPluginInfo * info = this->GetPluginInfo();

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

  // Set the Observer for updating progress in the GUI
  m_GradientMagnitudeFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
  m_DeformableModelFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );

  // Execute the filters and progressively remove temporary memory
  m_MeshSource->Update();
  m_GradientMagnitudeFilter->Update();
  m_GradientFilter->Update();
 // m_DeformableModelFilter->Update();

  this->PostProcessData( pds );

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
  typedef typename MeshType::PointsContainer::ConstIterator PointIterator;
  PointIterator pointItr  = mesh->GetPoints()->Begin();
  PointIterator pointsEnd = mesh->GetPoints()->End();

  while( pointItr != pointsEnd )
    {
    *outputPointsItr++ = pointItr.Value()[0]; 
    *outputPointsItr++ = pointItr.Value()[1]; 
    *outputPointsItr++ = pointItr.Value()[2]; 
    ++pointItr;
    }


  opds->NumberOfMeshCells = mesh->GetNumberOfCells();
  unsigned int numEntries = 0;
  
  typedef typename MeshType::CellsContainer::ConstIterator CellIterator;
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
    *cellsTopItr = np;
    ++cellsTopItr;
    PointIdIterator pointIdItr = cell->PointIdsBegin();
    PointIdIterator pointIdEnd = cell->PointIdsEnd();
    while( pointIdItr != pointIdEnd )
      {
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
