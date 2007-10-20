// -------------------------------------------------------------------------
// itkVTKMeshToitkMeshFilter.h
// $Revision: 1.1.2.1 $
// $Author: hanfei $
// $Name: ITK-3-2 $
// $Date: 2007-10-20 16:24:57 $
// -------------------------------------------------------------------------

#ifndef __itkVTKMeshToITKMeshFilter__h__
#define __itkVTKMeshToITKMeshFilter__h__

#include <itkMeshSource.h>
#include <vtkUnstructuredGrid.h>
#include <itkQuadrilateralCell.h>

namespace itk
{
/**
 * \class VTKMeshToITKMeshFilter
 *
 * \brief Transforms a vtk mesh into an \ref itk::Mesh
 *
 * \note Updated version of InsightApplications
 *       /Auxiliary/vtk/vtk2itk.cxx
 *       this version is templated over the type of meshes,
 *       accept both polydata and Unstructured Grid as input Meshes,
 *       does not require any type to be set (float, double),
 *
 * \note a version that outputs an itk::QuadEdgeMesh instead is available:
 *       \ref itk::VTKMeshToQuadEdgeMeshFilter
 *
 * \sa   \ref itk::VTKMeshToQuadEdgeMeshFilter
 *       \ref itk::QuadEdgeMeshToUnstructuredGridFilter
 *       \ref itk::QuadEdgeMeshVTKFileReader
 *       \ref itk::QuadEdgeMeshVTKFileWriter
 */
template< class TVTKMesh, class TMesh >
class VTKMeshToITKMeshFilter
: public itk::MeshSource< TMesh >
{
public:
  typedef VTKMeshToITKMeshFilter          Self;
  typedef itk::MeshSource< TMesh >        Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Types defined in the output mesh. */
  typedef TMesh                              MeshType;
  typedef typename MeshType::Pointer         MeshPointer;
  typedef typename MeshType::PointIdentifier PointIdentifier;
  typedef typename MeshType::CoordRepType    CoordRepType;
  typedef typename MeshType::PointType       PointType;
  typedef typename MeshType::CellAutoPointer CellAutoPointer;
  typedef typename MeshType::CellTraits      CellTraits;
  typedef typename PointType::VectorType     VectorType;

  typedef typename MeshType::PointsContainer::Pointer PointsContainerPointer;
  typedef typename MeshType::CellsContainerPointer    CellsContainerPointer;

  itkNewMacro( Self );
  itkTypeMacro( VTKMeshToITKMeshFilter, itkMeshSource );

  TVTKMesh* GetInput( ) { return( m_Input ); }
  void SetInput( TVTKMesh* input );

protected:
  VTKMeshToITKMeshFilter( );
  virtual ~VTKMeshToITKMeshFilter( ) { };
  virtual void GenerateData( );
  TVTKMesh* m_Input;

private:
  VTKMeshToITKMeshFilter( const Self& ); // Not impl.
  void operator=( const Self& );      // Not impl.
};

}

#include "itkVTKMeshToitkMeshFilter.txx"

#endif
