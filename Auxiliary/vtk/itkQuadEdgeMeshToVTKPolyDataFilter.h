// -------------------------------------------------------------------------
// itkQuadEdgeMeshToVTKPolyDataFilter.h
// $Revision: 1.1.2.1 $
// $Author: hanfei $
// $Name: ITK-3-2 $
// $Date: 2007-10-20 16:24:57 $
// -------------------------------------------------------------------------


#ifndef __ITKQEMESHTOPOLYDATAFILTER__H__
#define __ITKQEMESHTOPOLYDATAFILTER__H__

#include <vtkPolyDataSource.h>
#include "itkQuadEdgeMeshLineCell.h"
#include "itkQuadEdgeMeshPolygonCell.h"
#include "vtkPoints.h"
#include "itkQEMeshToUnstructuredGridFilter.h"

namespace itk
{

  /**
  * \class QuadEdgeMeshToVTKPolyDataFilter
  *
  * \brief Transforms a \ref itkQE::Mesh into a VTK's Unstructured Grid.
  * 
  * \note Updated version of InsightApplications
  *       /Auxiliary/vtk/vtk2itk.cxx
  *       this version is templated over the type of meshes,
  *       does not require any type to be set (float, double),
  *       and is tuned for \ref itkQE::Mesh 
  *       ( see \ref itkQE::ITKCellsToVTKCellsMultivisitor )
  *
  */

template< class TMesh >
class QuadEdgeMeshToVTKPolyDataFilter
: public vtkPolyDataSource
{
public:
  typedef QuadEdgeMeshToVTKPolyDataFilter Self;

  /** Useful typedefs. */
  typedef TMesh                        MeshType;
    
  typedef typename MeshType::PointsContainerIterator PointsContainerIterator;
  typedef typename MeshType::PointsContainer         PointsContainer;
  typedef typename MeshType::PointType               PointType;
  typedef typename MeshType::PixelType               PixelType;
  typedef typename MeshType::CoordRepType            CoordRepType;
 
public:
  /** Standard vtk interface. */
  vtkTypeRevisionMacro(QuadEdgeMeshToVTKPolyDataFilter<TMesh>,
                       vtkPolyDataSource );
  static Self* New( );

  /** Input type. */
  void SetInput( const MeshType* input );

protected:
  QuadEdgeMeshToVTKPolyDataFilter( );
  ~QuadEdgeMeshToVTKPolyDataFilter( ) { }

  void Execute( );

  MeshType* m_Input;

private:
  QuadEdgeMeshToVTKPolyDataFilter( const Self& ) { }
  void operator=( const Self& ) { }
};

} // enamespace

#include "itkQuadEdgeMeshToVTKPolyDataFilter.txx"

#endif
