// -------------------------------------------------------------------------
// itkQEMeshVTKFileWriter.txx
// $Revision: 1.1.2.1 $
// $Author: hanfei $
// $Name: ITK-3-2 $
// $Date: 2007-10-20 16:24:57 $
// -------------------------------------------------------------------------

#ifndef __itkQuadEdgeMeshVTKFileWriter__txx__
#define __itkQuadEdgeMeshVTKFileWriter__txx__

#include <vtkUnstructuredGridWriter.h>
#include "itkQEMeshToUnstructuredGridFilter.h"

namespace itk
{
// ---------------------------------------------------------------------
template< class TMesh >
void QuadEdgeMeshVTKFileWriter< TMesh >::
SetInput( const MeshType* input )
{
  // ProcessObject is not const_correct so this cast is required here.
  this->ProcessObject::SetNthInput( 0, const_cast< TMesh* >( input ) );
}

// ---------------------------------------------------------------------
template< class TMesh >
const typename QuadEdgeMeshVTKFileWriter< TMesh >::
MeshType* QuadEdgeMeshVTKFileWriter< TMesh >::
GetInput( )
{
  if( this->GetNumberOfInputs( ) < 1 )
    {
    return( (MeshType*)0 );
    }

  return( static_cast< TMesh* > ( this->ProcessObject::GetInput( 0 ) ) );
}

// ---------------------------------------------------------------------
template< class TMesh >
const typename QuadEdgeMeshVTKFileWriter< TMesh >::
MeshType* QuadEdgeMeshVTKFileWriter< TMesh >::
GetInput( unsigned int idx )
{
  return( static_cast< TMesh* > ( this->ProcessObject::GetInput( idx ) ) );
}

// ---------------------------------------------------------------------
template< class TMesh >
void QuadEdgeMeshVTKFileWriter< TMesh >::
Write( )
{
  const MeshType* input = this->GetInput( );

  // Make sure input is available
  if( input == 0 )
    {
    itkExceptionMacro( << "No input to writer!" );
    }

  // Make sure that we can write the file given the name
  if ( m_FileName == "" )
    {
    itkExceptionMacro( << "No filename was specified!" );
    }

  // Notify 'start', 'execute' and 'end' event observers
  //this->InvokeEvent( StartEvent( ) );
  this->GenerateData( );
  //this->InvokeEvent( EndEvent( ) );
}

// ---------------------------------------------------------------------
template< class TMesh >
QuadEdgeMeshVTKFileWriter< TMesh >::
QuadEdgeMeshVTKFileWriter( )
: Superclass( ),
m_FileName( "" )
{
  this->SetNumberOfRequiredInputs( 1 );
}

// ---------------------------------------------------------------------
template< class TMesh >
void QuadEdgeMeshVTKFileWriter< TMesh >::
GenerateData( )
{
  typedef MeshToUnstructuredGridFilter< MeshType > MeshToUGType;

  MeshConstPointer input = this->GetInput( );

  MeshToUGType* m2ug = MeshToUGType::New( );
  m2ug->SetInput( input );

  vtkUnstructuredGridWriter* vWriter = vtkUnstructuredGridWriter::New( );
  vWriter->SetInput( m2ug->GetOutput( ) );
  vWriter->SetFileName( m_FileName.c_str( ) );
  vWriter->Write( );
}

}
  
#endif
