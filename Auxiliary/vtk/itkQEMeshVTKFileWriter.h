// -------------------------------------------------------------------------
// itkQEMeshVTKFileWriter.h
// $Revision: 1.1.2.1 $
// $Author: hanfei $
// $Name: ITK-3-2 $
// $Date: 2007-10-20 16:24:57 $
// -------------------------------------------------------------------------

#ifndef __itkQuadEdgeMeshVTKFileWriter__h__
#define __itkQuadEdgeMeshVTKFileWriter__h__

#include <itkExceptionObject.h>
#include <itkProcessObject.h>

namespace itk
{
  /**
   * \class QuadEdgeMeshVTKFileWriter
   * \brief Writes from an \ref itkQE::Mesh to a .vtk file. 
   * 
   * Wrapper around a vtkUnstructuredGridWriter and a
   * \ref itk::QuadEdgeMeshToUnstructuredGridFilter to produce a writer API.
   * Requires a polydata or UGrid valid .vtk filename.
   *
   * \sa   \ref itk::VTKMeshToITKMeshFilter
   *       \ref itk::QuadEdgeMeshToUnstructuredGridFilter
   *       \ref itk::VTKMeshToQuadEdgeMeshFilter
   *       \ref itk::QuadEdgeMeshVTKFileWriter
   */
template< class TMesh >
class QuadEdgeMeshVTKFileWriter
: public itk::ProcessObject
{
public:
  /** Standard class typedefs. */
  typedef QuadEdgeMeshVTKFileWriter       Self;
  typedef itk::ProcessObject              Superclass;
  typedef itk::SmartPointer< Self >       Pointer;
  typedef itk::SmartPointer< const Self > ConstPointer;

  /** Some convenient typedefs. */
  typedef TMesh                           MeshType;
  typedef typename MeshType::Pointer      MeshPointer;
  typedef typename MeshType::ConstPointer MeshConstPointer;

public:
  /** Method for creation through the object factory. */
  itkNewMacro( Self );
           
  /** Run-time type information (and related methods). */
  itkTypeMacro( QuadEdgeMeshVTKFileWriter, itkProcessObject );

  /** Specify the name of the output file to write. */
  itkSetStringMacro( FileName );
  itkGetStringMacro( FileName );
            
  /** Set/Get the image input of this writer.  */
  void SetInput( const MeshType* input );
  const MeshType* GetInput( );
  const MeshType* GetInput( unsigned int idx );

  virtual void Write( );
  virtual void Update( ) { this->Write( ); }

protected:
  QuadEdgeMeshVTKFileWriter( );
  virtual ~QuadEdgeMeshVTKFileWriter( ) { }
  std::string m_FileName;

  /** Does the real work. */
  void GenerateData( );
  
private:
  QuadEdgeMeshVTKFileWriter( const Self& ); // Not impl.
  void operator=( const Self& ); // Not impl.

};

}

#include "itkQEMeshVTKFileWriter.txx"

#endif
