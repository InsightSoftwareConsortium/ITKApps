// -------------------------------------------------------------------------
// itkQEMeshVTKFileReader.txx
// $Revision: 1.1.2.1 $
// $Author: hanfei $
// $Name: ITK-3-2 $
// $Date: 2007-10-20 16:24:57 $
// -------------------------------------------------------------------------

#ifndef __itkQuadEdgeMeshVTKFileReader__txx__
#define __itkQuadEdgeMeshVTKFileReader__txx__

#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include "itkQEVTKMeshToMeshFilter.h"

namespace itk
{
template< class TMesh >
QuadEdgeMeshVTKFileReader< TMesh >::
QuadEdgeMeshVTKFileReader( )
: Superclass( ),
m_FileName( "" )
{
  this->SetNumberOfRequiredOutputs( 1 );
  this->SetNthOutput( 0, NULL );
}

// ---------------------------------------------------------------------
template< class TMesh >
void QuadEdgeMeshVTKFileReader< TMesh >::
GenerateData( )
{
  typedef VTKMeshToMeshFilter< vtkUnstructuredGrid, MeshType > vtkUG2MeshType;
  
  typedef typename vtkUG2MeshType::Pointer vtkUG2MeshPointer;

  vtkUnstructuredGridReader* vReader = vtkUnstructuredGridReader::New( );
  vReader->SetFileName( m_FileName.c_str( ) );
  vReader->Update( );

  vtkUG2MeshPointer v2m = vtkUG2MeshType::New( );
  v2m->SetInput( vReader->GetOutput( ) );
  v2m->Update( );

  MeshPointer out = v2m->GetOutput( );
  this->SetNthOutput( 0, out.GetPointer( ) );
}

} // ecapseman

#endif
