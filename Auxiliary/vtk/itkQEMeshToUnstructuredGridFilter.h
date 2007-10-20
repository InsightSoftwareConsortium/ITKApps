// -------------------------------------------------------------------------
// itkQEMeshToUnstructuredGridFilter.h
// $Revision: 1.1.2.1 $
// $Author: hanfei $
// $Name: ITK-3-2 $
// $Date: 2007-10-20 16:24:57 $
// -------------------------------------------------------------------------

#ifndef __itkQuadEdgeMeshToUnstructuredGridSource__h__
#define __itkQuadEdgeMeshToUnstructuredGridSource__h__

#include <vtkUnstructuredGridSource.h>
#include "itkQuadEdgeMeshLineCell.h"
#include "itkQuadEdgeMeshPolygonCell.h"

namespace itk
{

/**
 * \class QuadEdgeMeshToUnstructuredGridFilter
 *
 * \brief Transforms a \ref itk::QuadEdgeMesh into a VTK's Unstructured Grid.
 * 
 * \note Updated version of InsightApplications
 *       /Auxiliary/vtk/vtk2itk.cxx
 *       this version is templated over the type of meshes,
 *       does not require any type to be set (float, double),
 *       and is tuned for \ref itk::QuadEdgeMesh 
 *       ( see itk::QuadEdgeITKCellsToVTKCellsMultivisitor )
 *
 * \note a version that outputs an itk::Mesh instead is available:
 *       \ref itk::QuadEdgeVTKMeshToITKMeshFilter
 *
 * \note a version that wraps vtk reader and this class is available
 *       It is the recommanded class for VTK -> ITKQE:
 *       \ref QuadEdgeMeshVTKFileReader
 *
 * \sa   \ref itk::VTKMeshToITKMeshFilter
 *       \ref itk::QuadEdgeMeshToUnstructuredGridFilter
 *       \ref itk::QuadEdgeMeshVTKFileReader
 *       \ref itk::QuadEdgeMeshVTKFileWriter
 *       itk::QuadEdgeITKCellsToVTKCellsMultivisitor
 */
template< class TMesh >
class QuadEdgeMeshToUnstructuredGridFilter
: public vtkUnstructuredGridSource
{
public:
    typedef TMesh                                MeshType;
    typedef QuadEdgeMeshToUnstructuredGridFilter Self;

    /** Useful typedefs. */
    typedef typename MeshType::PointsContainerIterator PointsContainerIterator;
    typedef typename MeshType::PointsContainer         PointsContainer;
    typedef typename MeshType::PointType               PointType;
    typedef typename MeshType::PixelType               PixelType;
    typedef typename MeshType::CoordRepType            CoordRepType;
    typedef typename MeshType::CellTraits              CellTraits;
    typedef typename MeshType::QEPrimal                QEPrimal;
    typedef typename MeshType::CellType                CellType;
    typedef typename MeshType::CellPixelType           CellPixelType;
    typedef typename CellType::MultiVisitor            MultiType;
    typedef typename MultiType::Pointer                MultiPointer;
    typedef itk::QuadEdgeMeshLineCell< CellType >      QECellType;
    typedef itk::QuadEdgeMeshPolygonCell< CellType >   QEPolygonCellType;

public:
    /** Standard vtk interface. */
    vtkTypeRevisionMacro( QuadEdgeMeshToUnstructuredGridFilter< TMesh >,
                          vtkUnstructuredGridSource );
    static Self* New( );

    /** Input type. */
    void SetInput( const MeshType* input );

protected:
    QuadEdgeMeshToUnstructuredGridFilter( );
    ~QuadEdgeMeshToUnstructuredGridFilter( ) { }

    void Execute( );

    MeshType* m_Input;

private:
    QuadEdgeMeshToUnstructuredGridFilter( const Self& ) { }
    void operator=( const Self& ) { }
};

} // enamespace

#include "itkQEMeshToUnstructuredGridFilter.txx"

#endif
