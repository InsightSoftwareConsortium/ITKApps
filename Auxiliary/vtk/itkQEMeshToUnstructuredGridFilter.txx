// -------------------------------------------------------------------------
// itkQEMeshToUnstructuredGridFilter.txx
// $Revision: 1.1.2.1 $
// $Author: hanfei $
// $Name: ITK-3-2 $
// $Date: 2007-10-20 16:24:57 $
// -------------------------------------------------------------------------

#ifndef __itkQuadEdgeMeshToUnstructuredGridFilter__txx__
#define __itkQuadEdgeMeshToUnstructuredGridFilter__txx__

#include <itkCellInterfaceVisitor.h>
#include <vtkCellArray.h>
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkUnstructuredGrid.h>
#include <vector>

namespace itk
{
  /** 
   * \class QuadEdgeITKCellsToVTKCellsMultivisitor
   *
   * \brief Internal helper class for 
   * \ref itk::QuadEdgeMeshToUnstructuredGridFilter
   * 
   * Defines a CellVisitor wich is going to visit ITK cells container
   * and call a specialized version of visit for the type of cells we
   * are interested in, namely lines and polygons (the only celltypes
   * in a QEMesh) to be transformed into VTK_LINE, VTK_TRIANGLE or
   * VTK_POLYGON.
   *
   * \note Updated version of the VistVTKCellsClass of InsightApplications
   *       /Auxiliary/vtk/vtk2itk.cxx
   *       this version is templated over the type of mesh, does not 
   *       require any type to be set prior hand, and is tuned for
   *       \ref itk::QuadEdgeMesh ( refer to the overloaded Visit( ) methods )
   */
template< class TMesh >
class QuadEdgeITKCellsToVTKCellsMultiVisitor
{
public:
  // typedef the itk cells we are interested in
  typedef TMesh                                        MeshType;
  typedef typename MeshType::QEPrimal                  QEPrimal;
  typedef typename MeshType::CellType                  CellType;
  typedef typename MeshType::CellTraits                CellTraits;
  typedef typename CellType::PointIdIterator           PointIdIterator;

  typedef typename CellTraits::PointIdInternalIterator
                                                 PointIdInternalIterator;

  typedef itk::QuadEdgeMeshLineCell< CellType >        QECellType;
  typedef itk::QuadEdgeMeshPolygonCell< CellType >     QEPolygonCellType;

public:
  QuadEdgeITKCellsToVTKCellsMultiVisitor( ) { }
  ~QuadEdgeITKCellsToVTKCellsMultiVisitor( ) { }
  void SetMesh( MeshType* mesh ) { m_Mesh = mesh; }
  void SetTypeArray( std::vector< int >* arr ) { m_TypeArray = arr; }
  void SetCellArray( vtkCellArray* a )         { m_Cells = a; }
  void Visit( unsigned long notUsed, QECellType* e )
    {
    (void)notUsed;
    // Set point ids
    m_Cells->InsertNextCell( 2 );
    m_Cells->InsertCellPoint( e->GetQEGeom( )->GetOrigin( ) );
    m_Cells->InsertCellPoint( e->GetQEGeom( )->GetDestination( ) );

    // Set cell type
    m_TypeArray->push_back( VTK_LINE );
    }
  void Visit( unsigned long notUsed, QEPolygonCellType* e )
    {
    (void)notUsed;
    // Set point ids
    PointIdInternalIterator pit = e->InternalPointIdsBegin( );
    int nPoints = e->GetNumberOfPoints( );
    m_Cells->InsertNextCell( nPoints );
    while(pit != e->InternalPointIdsEnd( ) )
      {
      m_Cells->InsertCellPoint( *pit );
      pit++;
      }

    // Set cell type
    if( nPoints == 3 )
      {
      m_TypeArray->push_back( VTK_TRIANGLE );
      }
    else
      {
      m_TypeArray->push_back( VTK_POLYGON );
      }
    }
protected:
  MeshType* m_Mesh;
  std::vector< int >* m_TypeArray;
  vtkCellArray* m_Cells;
};

// ---------------------------------------------------------------------
template< class TMesh >
vtkCxxRevisionMacro( QuadEdgeMeshToUnstructuredGridFilter< TMesh >,
                             "$Revision: 1.1.2.1 $" );

// ---------------------------------------------------------------------
template< class TMesh >
typename QuadEdgeMeshToUnstructuredGridFilter< TMesh >::
Self*
QuadEdgeMeshToUnstructuredGridFilter< TMesh >::
New( )
{
  vtkObject* ret = vtkObjectFactory::CreateInstance(
                                     "vtkITKMeshToPolyDataFilter< TMesh >" );
  if( ret )
    {
    return( static_cast<
      QuadEdgeMeshToUnstructuredGridFilter< MeshType >* >( ret ) );
    }
  
  return( new QuadEdgeMeshToUnstructuredGridFilter< TMesh > );
}

// ---------------------------------------------------------------------
template< class TMesh >
void QuadEdgeMeshToUnstructuredGridFilter< TMesh >::
SetInput( const MeshType* input )
{
  m_Input = const_cast< MeshType* >( input );
  this->Modified( );
}

// ---------------------------------------------------------------------
template< class TMesh >
QuadEdgeMeshToUnstructuredGridFilter< TMesh >::
QuadEdgeMeshToUnstructuredGridFilter( )
: Superclass( ),
m_Input( 0 )
{
}

// ---------------------------------------------------------------------
template< class TMesh >
void QuadEdgeMeshToUnstructuredGridFilter< TMesh >::
Execute( )
{
  if( m_Input )
    {
    unsigned int nPoints = m_Input->GetPoints( )->Size( );
    if( nPoints > 0 )
      {
      vtkUnstructuredGrid* output = this->GetOutput( );

      // Create the vtkPoints object and set the number of points
      vtkPoints* vpoints = vtkPoints::New( );

      // Iterate over all the points in the itk mesh filling in
      // the vtkPoints object as we go
      PointsContainer* points = m_Input->GetPoints( );
      PointsContainerIterator pit = points->Begin( );
      while( pit != points->End( ) )
        {
        PointType p = pit.Value( );
        vpoints->InsertPoint( pit.Index( ), p[ 0 ], p[ 1 ], p[ 2 ] );
        pit++;
        }

      // Define the visitors types
      typedef itk::CellInterfaceVisitorImplementation<
        CellPixelType, CellTraits, QECellType,
        QuadEdgeITKCellsToVTKCellsMultiVisitor< MeshType > > EdgeVisitor;
      typedef itk::CellInterfaceVisitorImplementation<
        CellPixelType, CellTraits, QEPolygonCellType,
        QuadEdgeITKCellsToVTKCellsMultiVisitor< MeshType > > PolyVisitor;

      // Now create the cells using the MulitVisitor
      // 1. Create a MultiVisitor
      MultiPointer mv = MultiType::New( );
      // 2. Create an edge and Polygon visitor
      typename EdgeVisitor::Pointer ev = EdgeVisitor::New( );
      typename PolyVisitor::Pointer pv = PolyVisitor::New( );
      // 3. create vtk cells and estimate the size
      vtkCellArray* vcells = vtkCellArray::New( );
      std::vector< int > typesArray;
      // 4. Set up the visitors
      ev->SetCellArray( vcells );
      pv->SetCellArray( vcells );
      ev->SetTypeArray( &typesArray );
      pv->SetTypeArray( &typesArray );
      ev->SetMesh( m_Input );
      pv->SetMesh( m_Input );
      // 5. Add the visitors to the multivisitor
      mv->AddVisitor( ev );
      mv->AddVisitor( pv );

      // Now ask the _mesh to accept the multivisitor which
      // will Call Visit for each cell in the _mesh that matches the
      // cell types of the visitors added to the MultiVisitor
      m_Input->Accept( mv );

      // Set data to the output
      output->SetPoints( vpoints );
      output->SetCells( &*( typesArray.begin( ) ), vcells );

      // Clear memory
      vcells->Delete( );
      vpoints->Delete( );

      } // fi

    } // fi

}

} // enamespace

#endif
