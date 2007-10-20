// -------------------------------------------------------------------------
// itkQEVTKMeshToMeshFilter.h
// $Revision: 1.1.2.1 $
// $Author: hanfei $
// $Name: ITK-3-2 $
// $Date: 2007-10-20 16:24:57 $
// -------------------------------------------------------------------------

#ifndef __itkVTKMeshToQuadEdgeMeshFilter__h__
#define __itkVTKMeshToQuadEdgeMeshFilter__h__

#include <itkMeshSource.h>
#include <vtkUnstructuredGrid.h>

namespace itk
{
  /** Transforms a vtk mesh into an \ref itk::QuadEdgeMesh
   *
   * \note Updated version of InsightApplications
   *       /Auxiliary/vtk/vtk2itk.cxx
   *       this version is templated over the type of meshes,
   *       accept both polydata and UGrid as input Meshes,
   *       does not require any type to be set (float, double),
   *       and is tuned for \ref itk::QuadEdgeMesh 
   *       ( see usage of \ref itk::QuadEdgeMesh::AddFace( ) )
   *
   * \note a version that outputs an itk::Mesh instead is available:
   *       \ref itk::VTKMeshToITKMeshFilter
   *
   * \note a version that wraps vtk reader and this class is available
   *       It is the recommanded class for VTK -> ITKQE:
   *       \ref QuadEdgeMeshVTKFileReader
   *
   * \sa   \ref itk::VTKMeshToITKMeshFilter
   *       \ref itk::QuadEdgeMeshToUnstructuredGridFilter
   *       \ref itk::QuadEdgeMeshVTKFileReader
   *       \ref itk::QuadEdgeMeshVTKFileWriter
   */
template< class TVTKMesh, class TMesh >
class VTKMeshToQuadEdgeMeshFilter
: public itk::MeshSource< TMesh >
{
public:
  typedef VTKMeshToQuadEdgeMeshFilter     Self;
  typedef itk::MeshSource< TMesh >        Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Types defined in the input mesh. */
  typedef TMesh                              MeshType;
  typedef typename MeshType::PointType       PointType;
  typedef typename MeshType::Pointer         MeshPointer;
  typedef typename MeshType::PointIdentifier PointIdentifier;
  typedef typename MeshType::PointIdList     PointIdList;
  typedef typename MeshType::Traits          MeshTraits;
  typedef typename MeshTraits::CoordRepType  CoordRepType;
  typedef typename PointType::VectorType     VectorType;

public:
  itkNewMacro( Self );
  itkTypeMacro( VTKMeshToQuadEdgeMeshFilter, itkMeshSource );
  TVTKMesh* GetInput( ) { return( m_Input ); }
  void SetInput( TVTKMesh* input );

protected:
  VTKMeshToQuadEdgeMeshFilter( );
  virtual ~VTKMeshToQuadEdgeMeshFilter( ) { }
  TVTKMesh* m_Input;
  virtual void GenerateData( );

private:
  VTKMeshToQuadEdgeMeshFilter( const Self& ); // Not impl.
  void operator=( const Self& );      // Not impl.

};

}

#include "itkQEVTKMeshToMeshFilter.txx"

#endif
