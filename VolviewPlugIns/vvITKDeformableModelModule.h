/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVDeformableModelModule_h
#define _itkVVDeformableModelModule_h

#include "vtkVVPluginAPI.h"

#include <string.h>
#include <stdlib.h>

#include "vvITKFilterModuleBase.h"

#include "itkImportImageFilter.h"
#include "itkGradientRecursiveGaussianImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkDeformableMesh3DFilter.h"
#include "itkSphereMeshSource.h"


namespace VolView 
{

namespace PlugIn
{

template <class TInputPixelType >
class DeformableModelModule : public FilterModuleBase {

public:

   // Pixel type of the input buffer
  typedef TInputPixelType                InputPixelType;
  typedef float                          RealPixelType;

  itkStaticConstMacro( Dimension, unsigned int, 3 );

  typedef itk::Image< InputPixelType,  Dimension >  InputImageType;
  typedef itk::Image< RealPixelType,   3         >  RealImageType;

  // Instantiate the ImportImageFilter
  // This filter is used for building an ITK image using 
  // the data passed in a buffer.
  typedef itk::ImportImageFilter< InputPixelType, 
                                  Dimension       > ImportFilterType;

  
  typedef ImportFilterType::RegionType    RegionType;
  typedef RegionType::SizeType            SizeType;
  typedef RegionType::IndexType           IndexType;


  // Instantiate the GradientMagnitude filter.  This filter is the first stage
  // in the generation of the image potential that will atract the deformable
  // model to the contours of structures present in the image.
  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter< 
                                InputImageType, 
                                RealImageType > GradientMagnitudeFilterType;

  // Instantiate the Gradient filter.  This filter is the second stage in the
  // generation of the image potential. It will compute the vector gradient of
  // the gradient magnitude image. The resulting image contains directional
  // information pointing to the maxima of contours as well as second order
  // information on the intensity of the original image.
  typedef itk::GradientRecursiveGaussianImageFilter< 
                                RealImageType > GradientFilterType;


  // Type of the mesh used to represent the deformable model.
  // This is by default a 3D mesh.
  typedef  itk::Mesh<double>     MeshType;


  // Type used to represent points in the Mesh
  typedef typename MeshType::PointType             PointType;


  // Instantiation of the DeformableModel filter.
  // This filter computes the deformation of the mesh under the forces
  // excerted by the image and the internal constrains of the mesh.
  typedef itk::DeformableMesh3DFilter< MeshType,
                                       MeshType >  DeformableModelFilterType;



  // Instantiation of the Sphere source. This MeshSource is used to 
  // provide an initial mesh for the deformable model. The sphere
  // center is taken from a user-provided seed point .
  typedef itk::SphereMeshSource< MeshType >        MeshSourceType;



public:
    DeformableModelModule();
   ~DeformableModelModule();

    void SetSphereCenter( const PointType & center );
    void SetSphereRadius( float radius );

    void SetSigma( float value );

    void ProcessData( const vtkVVProcessDataStruct * pds );
    void PostProcessData( const vtkVVProcessDataStruct * pds );

private:
    typename ImportFilterType::Pointer              m_ImportFilter;
    typename GradientMagnitudeFilterType::Pointer   m_GradientMagnitudeFilter;
    typename GradientFilterType::Pointer            m_GradientFilter;
    typename DeformableModelFilterType::Pointer     m_DeformableModelFilter;
    typename MeshSourceType::Pointer                m_MeshSource;

};

} // end of namespace PlugIn

} // end of namespace Volview

#endif
