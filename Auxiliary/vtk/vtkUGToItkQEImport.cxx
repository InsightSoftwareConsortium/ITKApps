#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkGeometryFilter.h>
#include <vtkSphereSource.h>

#include "itkQuadEdgeMesh.h"
#include "itkQEMeshToUnstructuredGridFilter.h"
#include "itkQEVTKMeshToMeshFilter.h"

typedef itk::QuadEdgeMesh< double, 3 > MeshType;
typedef MeshType::Pointer              MeshPointer;

typedef itk::VTKMeshToQuadEdgeMeshFilter< vtkUnstructuredGrid, MeshType>
  SrcMeshType;

typedef SrcMeshType::Pointer                                  SrcMeshPointer;
typedef itk::QuadEdgeMeshToUnstructuredGridFilter< MeshType > itkToVTK;

/**
 * \example vtkUGToItkQEImport.cxx
 * Going back and forth from a vtk Unstructured Grid (UG) to a
 * \ref itk::QuadEdgeMesh. Here are the two stages:
 * \li reading a vtkUG (Unstructured Grid) and converting it to a
 *     \ref itk::QuadEdgeMesh by using itk::VTKMeshToQuadEdgeMeshFilter
 * \li Converting the \ref itk::QuadEdeMesh back to a vtkUG by using
 *     \ref itk::QuadEdgeMeshToUnstructuredGridFilter 
 */
int main( int argc, char* argv[] )
{
  SrcMeshPointer mesh = SrcMeshType::New( );

  if( argc != 2 )
    {
    std::cout << "Usage: " << argv[ 0 ] << " <ModelFile.vtk>" << std::endl;
    return( 0 );
    } // fi
     
  // When present load the vtk file:
  vtkUnstructuredGridReader* reader = vtkUnstructuredGridReader::New( );
  reader->SetFileName( argv[ 1 ] );
  reader->Update( );
  mesh->SetInput( reader->GetOutput( ) );

  mesh->Update( );

  vtkRenderer* renderer = vtkRenderer::New( );
  renderer->SetBackground( 1.0, 1.0, 1.0 );

  vtkRenderWindow* renWin = vtkRenderWindow::New( );
  renWin->AddRenderer( renderer );
  renWin->SetSize( 300, 300 );

  vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::New( );
  interactor->SetRenderWindow( renWin );

  itkToVTK* itkToVtkConvertor = itkToVTK::New( );
  itkToVtkConvertor->SetInput( mesh->GetOutput( ) );
  itkToVtkConvertor->Update( );
   
  vtkUnstructuredGridWriter* writer = vtkUnstructuredGridWriter::New( );
  writer->SetInput( itkToVtkConvertor->GetOutput( ) );
  writer->SetFileName( "junk.vtk" );
  writer->Write( );

  vtkDataSetMapper* mapper = vtkDataSetMapper::New( );
  mapper->SetInput( itkToVtkConvertor->GetOutput( ) );
   
  vtkActor* actor = vtkActor::New( );
  actor->SetMapper( mapper );
  actor->GetProperty( )->SetColor( 0, 1, 0 );
  actor->GetProperty( )->SetLineWidth( 2 );
   
  renderer->AddActor( actor );
   
  renderer->ResetCamera( );
  renWin->Render( );
  interactor->Start( );
  return( 0 );
}
