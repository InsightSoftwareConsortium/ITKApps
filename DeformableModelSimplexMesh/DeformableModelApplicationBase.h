#ifndef __DeformableModelApplicationBase__h__
#define __DeformableModelApplicationBase__h__


#include "itkImage.h"
#include "itkMesh.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkDefaultDynamicMeshTraits.h"
#include "itkSimplexMesh.h"
#include "itkRegularSphereMeshSource.h"
#include "itkTriangleMeshToSimplexMeshFilter.h"
#include "itkVertexCell.h"
#include "itkCellInterfaceVisitor.h"

#include "itkDeformableSimplexMesh3DBalloonForceFilter.h"
#include "itkGradientRecursiveGaussianImageFilter.h"

class Leila
{
 public:
  int a;

 protected:
  double b;
};



class DeformableModelApplicationBase 
{

public:
  
  typedef unsigned short   PixelType;

  typedef unsigned char  VisualizationPixelType;
  
  typedef itk::Image< PixelType, 3 >                  VolumeType;
  
  //typedef itk::Image< PixelType, 2 >                  ImageType;
  
  typedef itk::ImageFileReader< VolumeType >          VolumeReaderType;
  
  typedef itk::Image< VisualizationPixelType, 3 >     VisualizationVolumeType;
  
  typedef itk::RescaleIntensityImageFilter< 
                          VolumeType,
                          VisualizationVolumeType >   RescaleIntensityFilterType;

  typedef itk::ImageToVTKImageFilter< 
                          VisualizationVolumeType >   ITK2VTKAdaptorFilterType;


 
   // Declare the type of the input and output mesh
  typedef itk::DefaultDynamicMeshTraits<double, 3, 3,double,double> TriangleMeshTraits;
  typedef itk::DefaultDynamicMeshTraits<double, 3, 3, double,double> SimplexMeshTraits;
  typedef itk::Mesh<double,3, TriangleMeshTraits> TriangleMeshType;
  typedef itk::SimplexMesh<double,3, SimplexMeshTraits> SimplexMeshType;


  // declare triangle mesh source (inital mesh must be replaced with creating
  // mesh from binary image using marching cubes
  typedef itk::RegularSphereMeshSource<TriangleMeshType>  SphereMeshSourceType;
  typedef SphereMeshSourceType::PointType PointType;
  typedef SphereMeshSourceType::VectorType VectorType;


  // declare the triangle to simplex mesh filter
  typedef itk::TriangleMeshToSimplexMeshFilter<TriangleMeshType, SimplexMeshType> SimplexFilterType;

  typedef SimplexMeshType::CellsContainer::ConstIterator  CellIterator;
  typedef itk::CellInterface<
                      SimplexMeshType::PixelType, 
                      SimplexMeshType::CellTraits >  CellInterfaceType;
  typedef itk::VertexCell<CellInterfaceType>      vertexCell;
  typedef itk::LineCell<CellInterfaceType> lineCell;
  
 
  typedef SimplexMeshType::LineType  lineType;
  typedef SimplexMeshType::CellType  cellType;


  //deformation stuff

   typedef itk::DeformableSimplexMesh3DBalloonForceFilter<SimplexMeshType,SimplexMeshType> DeformFilterType;
 

  
   typedef DeformFilterType::GradientImageType       GradientImageType;

   typedef itk::GradientRecursiveGaussianImageFilter<VolumeType,GradientImageType> GradientFilterType;

public:
  
  DeformableModelApplicationBase();
  
  virtual ~DeformableModelApplicationBase();
  
  virtual void SetSeedPoint( double x, double y, double z );
  
  virtual void  GetSeedPoint(double data[3]); 
  
  
  
protected:
  
  double                                  m_SeedPoint[3];
  
  VolumeReaderType::Pointer               m_VolumeReader;
  
  RescaleIntensityFilterType::Pointer     m_RescaleIntensity;

  ITK2VTKAdaptorFilterType::Pointer       m_ITK2VTKAdaptor;
  
  VolumeType::ConstPointer                m_LoadedVolume;

  SphereMeshSourceType::Pointer           m_SphereMeshSource;

  SimplexFilterType::Pointer              m_SimplexFilter;

  SimplexMeshType::Pointer                m_SimplexMesh;
 
  DeformFilterType::Pointer               m_DeformFilter;

  GradientFilterType::Pointer             m_GradientFilter;
  
};


#endif
