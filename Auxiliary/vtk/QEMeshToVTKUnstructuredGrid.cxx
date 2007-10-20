#include <iostream>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include "itkQuadEdgeMesh.h"
#include "itkQuadEdgeMeshLineCell.h"
#include "itkQuadEdgeMeshPolygonCell.h"
#include "itkQEMeshToUnstructuredGridFilter.h"

/**
 * \example QEMeshToVTKUnstructuredGrid.cxx
 * Exporting a \ref itk::QuadEdgeMesh to a vtk Unstructured Grid
 *  by using \ref itk::QuadEdgeMeshToUnstructuredGridFilter 
 *
 */
int main( )
{
  typedef itk::QuadEdgeMesh< double, 3 >                     TMesh;
  typedef itk::QuadEdgeMeshLineCell< TMesh::CellType >       TLineCell;
  typedef itk::QuadEdgeMeshPolygonCell< TMesh::CellType >    TPolyCell;
  typedef itk::QuadEdgeMeshToUnstructuredGridFilter< TMesh > Titk2vtk;

  typedef TMesh::CellAutoPointer CellAutoPointer;
  typedef TMesh::Pointer         TMeshPointer;
  typedef TMesh::PointType       TPoint;

  TPoint points0;
  TPoint points1;
  TPoint points2;
  TPoint points3;

  points0[ 0 ] = 0;
  points0[ 1 ] = 10;
  points0[ 2 ] = 15;

  points1[ 0 ] = 7;
  points1[ 1 ] = 0;
  points1[ 2 ] = 0;

  points2[ 0 ] = 15;
  points2[ 1 ] = 6;
  points2[ 2 ] = 5;

  points3[ 0 ] = 5;
  points3[ 1 ] = 20;
  points3[ 2 ] = 0;

  // Mesh creation
  TMeshPointer mesh = TMesh::New( );
  TMesh::PointIdentifier pid0 = mesh->AddPoint( TPoint( points0 ) );
  TMesh::PointIdentifier pid1 = mesh->AddPoint( TPoint( points1 ) );
  TMesh::PointIdentifier pid2 = mesh->AddPoint( TPoint( points2 ) );
  TMesh::PointIdentifier pid3 = mesh->AddPoint( TPoint( points3 ) );
    
  CellAutoPointer line0;
  line0.TakeOwnership( new TLineCell );
  line0->SetPointId( 0, pid0 );
  line0->SetPointId( 1, pid1 );
  mesh->SetCell( 0, line0 );

  CellAutoPointer line1;
  line1.TakeOwnership( new TLineCell );
  line1->SetPointId( 0, pid1 );
  line1->SetPointId( 1, pid2 );
  mesh->SetCell( 1, line1 );

  CellAutoPointer line2;
  line2.TakeOwnership( new TLineCell );
  line2->SetPointId( 0, pid2 );
  line2->SetPointId( 1, pid3 );
  mesh->SetCell( 2, line2 );

  CellAutoPointer line3;
  line3.TakeOwnership( new TLineCell );
  line3->SetPointId( 0, pid3 );
  line3->SetPointId( 1, pid0 );
  mesh->SetCell( 3, line3 );
    
  CellAutoPointer line4;
  line4.TakeOwnership( new TLineCell );
  line4->SetPointId( 0, pid0 );
  line4->SetPointId( 1, pid2 );
  mesh->SetCell( 4, line4 );

  mesh->AddFaceTriangle( pid0, pid1, pid2 );
  mesh->AddFaceTriangle( pid2, pid3, pid0 );

  // itk2vtk connection
  Titk2vtk* itk2vtk = Titk2vtk::New( );
  itk2vtk->SetInput( mesh );
  itk2vtk->Update( );

  vtkDataSetMapper* mapper = vtkDataSetMapper::New( );
  mapper->SetInput( itk2vtk->GetOutput( ) );

  vtkActor* actor = vtkActor::New( );
  actor->SetMapper( mapper );
  actor->GetProperty( )->SetColor( 1.0, 0.0, 0.0 );
  actor->GetProperty( )->SetLineWidth( 2 );

  // Visualization
  vtkRenderer* renderer = vtkRenderer::New( );
  vtkRenderWindow* renWin = vtkRenderWindow::New( );
  vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::New( );

  renderer->SetBackground( 0.5, 0.5, 0.5 );
  renWin->AddRenderer( renderer );
  renWin->SetSize( 500, 500 );
  interactor->SetRenderWindow( renWin );

  renderer->AddActor( actor );

  renderer->ResetCamera( );
  renWin->Render( );
  interactor->Start( );
  return( 0 );
}
