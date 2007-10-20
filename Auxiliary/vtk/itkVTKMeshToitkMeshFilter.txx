// -------------------------------------------------------------------------
// itkVTKMeshToitkMeshFilter.txx
// $Revision: 1.1.2.1 $
// $Author: hanfei $
// $Name: ITK-3-2 $
// $Date: 2007-10-20 16:24:57 $
// -------------------------------------------------------------------------

#ifndef __itkVTKMeshToITKFilter__txx__
#define __itkVTKMeshToITKFilter__txx__

#ifndef vtkFloatingPointType
#define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif 

#include <vtkCell.h>

namespace itk
{

// ---------------------------------------------------------------------
template< class TVTKMesh, class TMesh >
void VTKMeshToITKMeshFilter< TVTKMesh, TMesh >::
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
VTKMeshToITKMeshFilter< TVTKMesh, TMesh >::
VTKMeshToITKMeshFilter( )
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
void VTKMeshToITKMeshFilter< TVTKMesh, TMesh >::
GenerateData( )
{
  if( !m_Input )
    {
    return;
    }

  MeshPointer out = this->GetOutput( );

  // Create a compatible point container for the mesh
  PointsContainerPointer points = MeshType::PointsContainer::New();
  
  // Resize the point container to be able to fit the vtk points
  unsigned long numPoints = m_Input->GetPoints( )->GetNumberOfPoints( );
  points->Reserve( numPoints );
  // Set the point container into the output mesh
  out->SetPoints( points );
  // import the points
  vtkFloatingPointType vPoint[ 3 ];
  for(int i =0; i < numPoints; i++)
    {
    PointType iPoint;
    iPoint.Fill( 0 );
    m_Input->GetPoint( i, vPoint );
    for( unsigned int j = 0; j < MeshType::PointDimension && j < 3; j++ )
      {
      iPoint[ j ] = CoordRepType( vPoint[ j ] );
      }
    out->SetPoint( i, iPoint );
    }
  
  int cellId = 0;
  CellsContainerPointer cells = MeshType::CellsContainer::New();
  out->SetCells( cells );
  cells->Reserve( m_Input->GetNumberOfCells( ) );
  vtkIdType* pts;
  for( cellId = 0; cellId < m_Input->GetNumberOfCells( ); cellId++ )
    {
    CellAutoPointer c;
    switch( m_Input->GetCellType( cellId ) )
      {
      case VTK_TRIANGLE:
        {
        typedef itk::CellInterface< vtkFloatingPointType, CellTraits >
          CellInterfaceType;
        typedef itk::TriangleCell< CellInterfaceType >
          TriangleCellType;
        TriangleCellType * t = new TriangleCellType;
        vtkCell* vCell = m_Input->GetCell( cellId );
        pts = new vtkIdType[3];
        pts[0] = vCell->GetPointId( 0 );
        pts[1] = vCell->GetPointId( 1 );
        pts[2] = vCell->GetPointId( 2 );
        t->SetPointIds( (unsigned long*)pts );
        c.TakeOwnership( t );
        delete[] pts;
        pts = (vtkIdType*)( 0 ); 
        break;
        }  
      case VTK_QUAD:
        {
        typedef itk::CellInterface< vtkFloatingPointType, CellTraits >
          CellInterfaceType;
        typedef itk::QuadrilateralCell< CellInterfaceType >
          QuadrilateralCellType;
        QuadrilateralCellType * t = new QuadrilateralCellType;
        vtkCell* vCell = m_Input->GetCell( cellId );
        pts = new vtkIdType[4];
        pts[0] = vCell->GetPointId( 0 );
        pts[1] = vCell->GetPointId( 1 );
        pts[2] = vCell->GetPointId( 2 );
        pts[3] = vCell->GetPointId( 3 );
        t->SetPointIds( (unsigned long*)pts );
        c.TakeOwnership( t );
        delete[] pts;
        pts = (vtkIdType*)( 0 ); 
        break;
        }  
      case VTK_EMPTY_CELL:
      case VTK_VERTEX:
      case VTK_POLY_VERTEX:
      // We should be able to import that ..
      case VTK_LINE:
      // we should be able to import that
      case VTK_POLY_LINE:
      // we should be able to import that
      case VTK_TRIANGLE_STRIP:
      // we should be able to import that
      case VTK_POLYGON:
      case VTK_PIXEL:
      default:
        {
        std::cerr << "Warning unhandled cell type "; 
        std::cerr << m_Input->GetCellType( cellId ) << std::endl;
        }
          
      }
     
    out->SetCell( cellId, c );
    }
      
}

} // enamespace

#endif
