/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVSurfaceSpline_txx
#define _itkVVSurfaceSpline_txx

#include "vvITKSurfaceSpline.h"

namespace VolView 
{

namespace PlugIn
{

/*
 *    Constructor
 */
template <class TInputPixelType >
SurfaceSpline<TInputPixelType>
::SurfaceSpline()
{

  const unsigned int numberOfLandmarks = 9;
 
  m_ImportFilter               = ImportFilterType::New();

  m_Spline                     = SplineType::New();

  m_SourceLandmarks            = PointSetType::New();
  m_TargetLandmarks            = PointSetType::New();


  this->SetNumberOfPointsAlongColumns( 21 );
  this->SetNumberOfPointsAlongRows( 21 );

  ofs.open("splineSpy.txt");
 
  LandmarkContainerPointer sourceLandmarks = m_SourceLandmarks->GetPoints();
  LandmarkContainerPointer targetLandmarks = m_TargetLandmarks->GetPoints();


  targetLandmarks->Reserve( numberOfLandmarks );
  sourceLandmarks->Reserve( numberOfLandmarks );

  LandmarkIterator sourceLandmarkItr = sourceLandmarks->Begin();

  LandmarkType landmark;

  landmark[0] = 0.0;
  landmark[1] = 0.0;
  landmark[2] = 0.0;
  sourceLandmarkItr.Value() = landmark;
  
  ++sourceLandmarkItr;
  landmark[0] = 0.5;
  landmark[1] = 0.0;
  landmark[2] = 0.0;
  sourceLandmarkItr.Value() = landmark;
   
  ++sourceLandmarkItr;
  landmark[0] = 1.0;
  landmark[1] = 0.0;
  landmark[2] = 0.0;
  sourceLandmarkItr.Value() = landmark;
   
  ++sourceLandmarkItr;
  landmark[0] = 0.0;
  landmark[1] = 0.5;
  landmark[2] = 0.0;
  sourceLandmarkItr.Value() = landmark;
   
  ++sourceLandmarkItr;
  landmark[0] = 0.5;
  landmark[1] = 0.5;
  landmark[2] = 0.0;
  sourceLandmarkItr.Value() = landmark;
   
  ++sourceLandmarkItr;
  landmark[0] = 1.0;
  landmark[1] = 0.5;
  landmark[2] = 0.0;
  sourceLandmarkItr.Value() = landmark;
   
  ++sourceLandmarkItr;
  landmark[0] = 0.0;
  landmark[1] = 1.0;
  landmark[2] = 0.0;
  sourceLandmarkItr.Value() = landmark;
   
  ++sourceLandmarkItr;
  landmark[0] = 0.5;
  landmark[1] = 1.0;
  landmark[2] = 0.0;
  sourceLandmarkItr.Value() = landmark;
   
  ++sourceLandmarkItr;
  landmark[0] = 1.0;
  landmark[1] = 1.0;
  landmark[2] = 0.0;
  sourceLandmarkItr.Value() = landmark;
  
}




/*
 *    Destructor
 */
template <class TInputPixelType >
SurfaceSpline<TInputPixelType>
::~SurfaceSpline()
{
   ofs.close(); 
}




/*
 *  Set the number of points along the colums
 */
template <class TInputPixelType >
void 
SurfaceSpline<TInputPixelType>
::SetNumberOfPointsAlongColumns( unsigned int num )
{
   m_SidePointsCol = num;
}



/*
 *  Set the number of points along the rows
 */
template <class TInputPixelType >
void 
SurfaceSpline<TInputPixelType>
::SetNumberOfPointsAlongRows( unsigned int num )
{
   m_SidePointsRow = num;
}


/*
 *  Set the stiffness value that allows to make the surface move from
 *  interpolation (passing through the landmarks) to approximation (not
 *  touching the landmarks).
 */
template <class TInputPixelType >
void 
SurfaceSpline<TInputPixelType>
::SetStiffness( double stiffness )
{
   m_Spline->SetStiffness( stiffness );
}




/*
 *  Performs the actual filtering on the data 
 */
template <class TInputPixelType >
void 
SurfaceSpline<TInputPixelType>
::ProcessData( const vtkVVProcessDataStruct * pds )
{

  this->SetUpdateMessage("Computing Surface Spline...");

  vtkVVPluginInfo * info = this->GetPluginInfo();

  const unsigned int numberOfSeeds = info->NumberOfMarkers;
  if( numberOfSeeds != 9  )
    {
    info->SetProperty( info, VVP_ERROR, "This plugin requires you to provide 9 points as 3D markers" ); 
    return;
    }

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
  this->SetCurrentFilterProgressWeight( 0.1 );
  this->SetUpdateMessage("Preprocessing: Spline Surface...");
   

  PointType point;
 
  point[2] =  0.0;  // always in the plane Z=0 

  for(unsigned int row=0; row < m_SidePointsRow; row++ )
    {

    point[1] =  static_cast<float>( row) / 
                static_cast<float>( m_SidePointsRow-1 );

    for(unsigned int col=0; col< m_SidePointsCol; col++ )
      {

      point[0] =  static_cast<float>(col) / 
                  static_cast<float>( m_SidePointsCol-1 );

      m_SourcePoints.push_back( point );

      }
    }
  

  LandmarkContainerPointer targetLandmarks = m_TargetLandmarks->GetPoints();
  LandmarkIterator targetLandmarkItr = targetLandmarks->Begin();

  LandmarkType landmark;

  unsigned int landmarkId = 0;

  const float * markersCoordinates = info->Markers;

  for( unsigned int i=0; i < numberOfSeeds; i++ )
    {
    landmark[0] = *markersCoordinates++; 
    landmark[1] = *markersCoordinates++;
    landmark[2] = *markersCoordinates++;
    targetLandmarks->InsertElement( landmarkId++, landmark );
    }

  m_Spline->SetTargetLandmarks( m_TargetLandmarks );
  m_Spline->SetSourceLandmarks( m_SourceLandmarks );

  m_Spline->ComputeWMatrix();


  PointIterator targetPointItr = m_TargetPoints.begin();
  PointIterator sourcePointItr = m_SourcePoints.begin();

  PointType destination;

  while( sourcePointItr != m_SourcePoints.end() )
    {
    destination = m_Spline->TransformPoint( *sourcePointItr );
    m_TargetPoints.push_back( destination );
    ++sourcePointItr;
    }

  this->SetCurrentFilterProgressWeight( 0.9 );
  this->SetUpdateMessage("Preprocessing: Marking one side of the surface...");


  this->PostProcessData( pds );


} // end of ProcessData




/*
 *  Performs post-processing of data. 
 *  This involves an intensity window operation and
 *  data copying into the volview provided buffer.
 */
template <class TInputPixelType >
void 
SurfaceSpline<TInputPixelType>
::PostProcessData( const vtkVVProcessDataStruct * pds )
{

  // A change in ProcessData signature could prevent this const_cast...
  vtkVVProcessDataStruct * opds = const_cast<vtkVVProcessDataStruct *>( pds );
  vtkVVPluginInfo * info = this->GetPluginInfo();


  // now put the results into the data structure
  const unsigned int numberOfPoints =  m_SidePointsCol * m_SidePointsRow;
  opds->NumberOfMeshPoints = numberOfPoints;

  float * points = new float[ numberOfPoints * 3 ];
  opds->MeshPoints = points;
  float * outputPointsItr = points;


  ConstPointIterator pointItr  = m_TargetPoints.begin();
  ConstPointIterator pointsEnd = m_TargetPoints.end();

  while( pointItr != pointsEnd )
    {
    *outputPointsItr++ =  (*pointItr)[0]; 
    *outputPointsItr++ =  (*pointItr)[1]; 
    *outputPointsItr++ =  (*pointItr)[2]; 
    ++pointItr;
    }

  opds->NumberOfMeshCells = ( m_SidePointsRow - 1 ) * ( m_SidePointsCol - 1 );
  
  // Cell connectivity entries follow the format of vtkCellArray:
  // n1, id1, id2,... idn1,  n2, id1, id2,.. idn2....

  unsigned int numEntries = opds->NumberOfMeshCells * 5; // each cell id + 4 points ids.
  int * cellsTopology = new int [ numEntries ];
  opds->MeshCells = cellsTopology;

  int * cellsTopItr = cellsTopology;

  for(unsigned int row=0; row < m_SidePointsRow - 1 ; row++ )
    {
    for(unsigned int col=0; col < m_SidePointsCol - 1; col++ )
      {
      const unsigned int cornerPoint = row * m_SidePointsCol + col;
      const unsigned int pointId1 = cornerPoint;
      const unsigned int pointId2 = cornerPoint + 1;
      const unsigned int pointId3 = cornerPoint + 1 + m_SidePointsCol;
      const unsigned int pointId4 = cornerPoint + m_SidePointsCol;

      *cellsTopItr++ = 4;
      *cellsTopItr++ = pointId1;
      *cellsTopItr++ = pointId2;
      *cellsTopItr++ = pointId3;
      *cellsTopItr++ = pointId4;

      }
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
