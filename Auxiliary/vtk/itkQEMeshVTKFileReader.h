// -------------------------------------------------------------------------
// itkQEMeshVTKFileReader.h
// $Revision: 1.1.2.1 $
// $Author: hanfei $
// $Name: ITK-3-2 $
// $Date: 2007-10-20 16:24:57 $
// -------------------------------------------------------------------------

#ifndef __itkQuadEdgeMeshVTKFileReader__h__
#define __itkQuadEdgeMeshVTKFileReader__h__

#include <itkExceptionObject.h>
#include <itkMeshSource.h>

namespace itk
{
  /**
   * \class QuadEdgeMeshVTKFileReader
   *
   * \brief Reads a .vtk mesh file and outputs an \ref itk::QuadEdgeMesh.
   *
   * Wrapper around a vtkUnstructuredGridReader and a
   * \ref QuadEdgeVTKMeshToMeshFilter to produce a Reader API.
   * Requires a polydata or UGrid valid .vtk filename.
   *
   * \sa   \ref itk::QuadEdgeVTKMeshToITKMeshFilter
   *       \ref itk::QuadEdgeMeshToUnstructuredGridFilter
   *       \ref itk::VTKMeshToMeshFilter
   *       \ref itk::QuadEdgeMeshVTKFileWriter
   */
template< class TMesh >
class MeshVTKFileReader
: public itk::MeshSource< TMesh >
{
public:
  /** Standard class typedefs. */
  typedef QuadEdgeMeshVTKFileReader       Self;
  typedef itk::MeshSource< TMesh >        Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Some convenient typedefs. */
  typedef TMesh                      MeshType;
  typedef typename MeshType::Pointer MeshPointer;

public:
  /** Method for creation through the object factory. */
  itkNewMacro( Self );
            
  /** Run-time type information (and related methods). */
  itkTypeMacro( MeshVTKFileReader, itkMeshSource );

  /** Specify the name of the output file to write. */
  itkSetStringMacro( FileName );
  itkGetStringMacro( FileName );
            
  virtual void Read( ) { this->Update( ); }

protected:
  QuadEdgeMeshVTKFileReader( );
  virtual ~QuadEdgeMeshVTKFileReader( ) { }

  /** Does the real work. */
  void GenerateData( );
  
  std::string m_FileName;

private:
  QuadEdgeMeshVTKFileReader( const Self& ); // Not impl.
  void operator=( const Self& ); // Not impl.

};

}

#include "itkQEMeshVTKFileReader.txx"

#endif
