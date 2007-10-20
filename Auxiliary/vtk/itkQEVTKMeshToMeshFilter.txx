// -------------------------------------------------------------------------
// itkQEVTKMeshToMeshFilter.txx
// $Revision: 1.1.2.1 $
// $Author: hanfei $
// $Name: ITK-3-2 $
// $Date: 2007-10-20 16:24:57 $
// -------------------------------------------------------------------------

#ifndef __itkVTKMeshToQuadEdgeMeshFilter__txx__
#define __itkVTKMeshToQuadEdgeMeshFilter__txx__

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif 

#include <vtkCell.h>

namespace itk
{
// ---------------------------------------------------------------------
template< class TVTKMesh, class TMesh >
void VTKMeshToQuadEdgeMeshFilter< TVTKMesh, TMesh >::
SetInput( TVTKMesh* input )
{
  if( m_Input != input )
    {
    m_Input = input;
    this->Modified( );
    } // fi
}

// ---------------------------------------------------------------------
template< class TVTKMesh, class TMesh >
VTKMeshToQuadEdgeMeshFilter< TVTKMesh, TMesh >::
VTKMeshToQuadEdgeMeshFilter( )
: Superclass( ),
m_Input( 0 )
{
  // Create the output. We use static_cast<> here because we know
  // the default output must be of type TMesh.
  MeshPointer output = TMesh::New( );

  this->Superclass::SetNumberOfRequiredOutputs( 1 );
  this->Superclass::SetNthOutput( 0, output.GetPointer( ) );
}

// ---------------------------------------------------------------------
template< class TVTKMesh, class TMesh >
void VTKMeshToQuadEdgeMeshFilter< TVTKMesh, TMesh >::
GenerateData( )
{
  if( !m_Input )
    {
    return;
    }
  MeshPointer out = this->GetOutput( );
  out->ClearFreePointAndCellIndexesLists( );
        
  // Add points
  vtkFloatingPointType vPoint[ 3 ];
  for( vtkIdType pit = 0;
       pit < m_Input->GetNumberOfPoints( );
       pit++ )
    {
    PointType iPoint;
    iPoint.Fill( 0 );
    m_Input->GetPoint( pit, vPoint );
    for( unsigned int i = 0;
         i < MeshType::PointDimension && i < 3;
         i++ )
      {
      iPoint[ i ] = CoordRepType( vPoint[ i ] );
      }
 
    out->AddPoint( iPoint );
    }

  // Add cells
  for( vtkIdType cit = 0;
       cit < m_Input->GetNumberOfCells( );
       cit++ )
    {
    vtkCell* vCell = m_Input->GetCell( cit );
    unsigned int nPoints = vCell->GetNumberOfPoints( );
    if( nPoints == 2 )
      {
      out->AddEdge( vCell->GetPointId( 0 ),
                    vCell->GetPointId( 1 ) );
      }
    else if( nPoints > 2 )
      {
      PointIdList pList;
      for( unsigned int i = 0; i < nPoints; i++ )
        {
        pList.push_back( vCell->GetPointId( i ) );
        }
      out->AddFace( pList );
      }
    } // rof
}

}

#endif
