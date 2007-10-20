#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkGeometryFilter.h>
#include <vtkSphereSource.h>

#include "itkQuadEdgeMesh.h"
#include "itkQEVTKMeshToMeshFilter.h"
#include "itkQEMeshToUnstructuredGridFilter.h"

typedef itk::QuadEdgeMesh< double, 3 >                            MeshType;
typedef MeshType::Pointer                                         MeshPointer;
typedef itk::VTKMeshToQuadEdgeMeshFilter< vtkPolyData, MeshType > SrcMeshType;

typedef SrcMeshType::Pointer                                  SrcMeshPointer;
typedef itk::QuadEdgeMeshToUnstructuredGridFilter< MeshType > itkToVTK;

/**
 * \example vtkToItkQEImport.cxx
 * Importing a vtk::PolyData and converting it to an \ref itk::QuadEdgeMesh
 * by using \ref itk::QuadEdgeMeshToUnstructuredGridFilter .
 * \anchor DoxyExamplesvtkToItkQEImport
 *
 */
int main( int argc, char *argv[] )
{
  // This is a VTKMeshToMeshFilter object
  SrcMeshPointer mesh = SrcMeshType::New( );

  if (argc > 2)
    {
    std::cout << "Usage: " << argv[0] << " <ModelFile.vtk>" << std::endl;
    return 0;
    }
     
  if( argc > 1 )
    {
    // When present load the vtk file:
    vtkPolyDataReader* reader = vtkPolyDataReader::New();
    reader->SetFileName(argv[1]);
    reader->Update();
    // pipeline to tranform it to an itkQE::Mesh
    mesh->SetInput( reader->GetOutput( ) );
    }
  else 
    {
    // Default to a sphere
    vtkSphereSource* sphere = vtkSphereSource::New();
    sphere->SetRadius(1.0);
    sphere->SetThetaResolution(18);
    sphere->SetPhiResolution(18);
    // pipeline to tranform it to an itkQE::Mesh
    mesh->SetInput( sphere->GetOutput( ) );
    }

  // actually transform it
  mesh->SetDebug( true );
  mesh->Update( );

  // High level graphic pipeline
  vtkRenderer* renderer = vtkRenderer::New( );
  renderer->SetBackground( 1.0, 1.0, 1.0 );
  vtkRenderWindow* renWin = vtkRenderWindow::New( );
  renWin->AddRenderer( renderer );
  renWin->SetSize( 300, 300 );
  vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::New( );
  interactor->SetRenderWindow( renWin );

  // tranform it back to vtk 
  itkToVTK* itkToVtkConvertor = itkToVTK::New( );
  itkToVtkConvertor->SetInput( mesh->GetOutput( ) );
  itkToVtkConvertor->Update( );
  // do whatever you want with it, e.g. dump it
  vtkGeometryFilter* filter = vtkGeometryFilter::New();
  filter->SetInput( itkToVtkConvertor->GetOutput( ) );
  filter->Update();
  vtkPolyDataWriter* writer = vtkPolyDataWriter::New( );
  writer->SetInput( filter->GetOutput( ) );
  writer->SetFileName( "junk.vtk" );
  writer->Write();

  //Low level graphic pipeline for the object
  vtkDataSetMapper* mapper = vtkDataSetMapper::New( );
  mapper->SetInput( itkToVtkConvertor->GetOutput( ) );
  vtkActor* actor = vtkActor::New( );
  actor->SetMapper( mapper );
  actor->GetProperty( )->SetColor( 0, 1, 0 );
  actor->GetProperty( )->SetLineWidth( 2 );
  // connect object to High level graphic pipeline
  renderer->AddActor( actor );

  // render everything
  renderer->ResetCamera( );
  renWin->Render( );
  interactor->Start( );
  return(0);
}
