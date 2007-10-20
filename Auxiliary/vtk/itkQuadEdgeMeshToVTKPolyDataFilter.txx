// -------------------------------------------------------------------------
// itkQuadEdgeMeshToVTKPolyDataFilter.txx
// $Revision: 1.1.2.1 $
// $Author: hanfei $
// $Name: ITK-3-2 $
// $Date: 2007-10-20 16:24:57 $
// -------------------------------------------------------------------------

#ifndef __ITKQEMESHTOPOLYDATAFILTER__TXX__
#define __ITKQEMESHTOPOLYDATAFILTER__TXX__

#include <itkCellInterfaceVisitor.h>
#include <vtkCellArray.h>
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vector>

namespace itk
{

// ---------------------------------------------------------------------
template< class TMesh >
        vtkCxxRevisionMacro( QuadEdgeMeshToVTKPolyDataFilter< TMesh >,
                             "$Revision: 1.1.2.1 $" );

// ---------------------------------------------------------------------
template< class TMesh >
typename QuadEdgeMeshToVTKPolyDataFilter< TMesh >::Self*
QuadEdgeMeshToVTKPolyDataFilter< TMesh >::New( )
{
  vtkObject* ret = 
  vtkObjectFactory::CreateInstance( "vtkITKMeshToPolyDataFilter< TMesh >" );
  if( ret )
    return(static_cast< QuadEdgeMeshToVTKPolyDataFilter< MeshType >* >( ret ) );
  return( new QuadEdgeMeshToVTKPolyDataFilter< TMesh > );
}

// ---------------------------------------------------------------------
template< class TMesh >
void QuadEdgeMeshToVTKPolyDataFilter< TMesh >::
SetInput( const MeshType* input )
{
  m_Input = const_cast< MeshType* >( input );
  this->Modified( );
}

// ---------------------------------------------------------------------
template< class TMesh >
QuadEdgeMeshToVTKPolyDataFilter< TMesh >::
QuadEdgeMeshToVTKPolyDataFilter( )
: Superclass( ),
m_Input( 0 )
{
}

// ---------------------------------------------------------------------
template< class TMesh >
void QuadEdgeMeshToVTKPolyDataFilter< TMesh >::
Execute( )
{
  if( m_Input )
    {
    std::cout<<"compute Unstructured grid"<<std::endl;
    itkToVTK* itkToVtkConvertor = itkToVTK::New( );
    itkToVtkConvertor->SetInput( m_Input );
    itkToVtkConvertor->Update(  );
    vtkUnstructuredGrid* UG = itkToVtkConvertor->GetOutput( ); 

    std::cout<<"cast Unstructured grid to polydata "<<std::endl;
    vtkPolyData* output = this->GetOutput( );
    output->SetPoints(UG->GetPoints( ));
    output->SetPolys(UG->GetCells( ));
    output->BuildLinks( );
    output->BuildCells( );
    
    // Clear memory
    //UG->Delete( );
         
    } // fi
}

} // enamespace

#endif // __ITKQEMESHTOPOLYDATAFILTER__TXX__
