/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ceExtractorConsoleBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef ce_FilterConsoleBase_h
#define ce_FilterConsoleBase_h

#include "itkImage.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkTernaryMagnitudeImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkImageToParametricSpaceFilter.h"
#include "itkMesh.h"
#include "itkImageFileReader.h"
#include "itkSphereSpatialFunction.h"
#include "itkInteriorExteriorMeshFilter.h"
#include "fltkSphereFunctionControl.h"
#include "fltkFrustumFunctionControl.h"
#include "itkParametricSpaceToImageSpaceMeshFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkSymmetricSecondRankTensor.h"
#include "itkSymmetricEigenAnalysisImageFilter.h"
#include "itkImageAdaptor.h"
#include "itkCastImageFilter.h"
#include "itkEigenMeasureSpatialFunction.h"
#include "itkPointSetToImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkJoinImageFilter.h"
#include "PixelAccessors.h"
#include "itkUnaryFunctorImageFilter.h"

// Define which type of spatial function to use
// Only one of the following lines should be uncommented.
#define  FRUSTUM_FUNCTION
//# define SPHERE_FUNCTION

// Write intermediate images for debug purposes 
//    image of eigen values.
//    ImageSpace-ParametricSpace map,
//    ExtractedCurve points image.
// #define INTERMEDIATE_OUTPUTS


class ceExtractorConsoleBase 
{
public:

  typedef   double                            InputPixelType;
  typedef   double                            PixelType;
  typedef   unsigned char                     OverlayPixelType;
  typedef   float                             MeshPixelType;
  typedef   MeshPixelType                     EigenPixelType;

  itkStaticConstMacro(Dimension,unsigned int,3);

  typedef   itk::Vector< PixelType, Dimension >           VectorType;
  
  typedef   itk::CovariantVector< PixelType, Dimension >  CovariantVectorType;

  typedef   itk::Image< InputPixelType, Dimension >       InputImageType;
  typedef   itk::Image< PixelType, Dimension >            ImageType;
  typedef   itk::Image< VectorType, Dimension >           VectorImageType;
  typedef   itk::Image< CovariantVectorType, Dimension >  CovariantVectorImageType;

  typedef   itk::Point< MeshPixelType, Dimension>         MeshPointDataType;

  typedef   itk::Mesh< MeshPointDataType, 3 >             MeshType;

  typedef   itk::ImageFileReader< 
                            InputImageType >              VolumeReaderType;

  typedef   itk::Mesh< MeshType::PointType, Dimension >   ImageSpaceMeshType;

  typedef   itk::GradientMagnitudeRecursiveGaussianImageFilter<
                            InputImageType,
                            ImageType        > GradientMagnitudeFilterType;
  
  typedef   itk::HessianRecursiveGaussianImageFilter< 
                            InputImageType >              HessianFilterType;
  typedef   HessianFilterType::OutputImageType            HessianImageType;
  typedef   HessianImageType::PixelType                   HessianPixelType;

  typedef   itk::FixedArray< double, HessianPixelType::Dimension >
                                                          EigenValueArrayType;
  typedef  itk::Image< EigenValueArrayType, HessianImageType::ImageDimension >
                                                          EigenValueImageType;
  typedef   itk::SymmetricEigenAnalysisImageFilter< 
              HessianImageType, EigenValueImageType >     EigenAnalysisFilterType;
  
  typedef itk::UnaryFunctorImageFilter< HessianImageType, ImageType,
          Functor::HessianToLaplacianFunction< HessianPixelType, PixelType > >
                                                          HessianToLaplacianImageFilter; 
  
  typedef itk::ImageAdaptor<  EigenValueImageType, 
         EigenValueAccessor< EigenValueArrayType > > ImageAdaptorType;

  typedef itk::Image< MeshPointDataType::ValueType, 
                    MeshPointDataType::PointDimension > 
                                      EachEigenValueImageType;

  typedef itk::CastImageFilter< ImageAdaptorType, 
                               EachEigenValueImageType >  CastImageFilterType;

  
  typedef itk::ImageFileWriter< CastImageFilterType::OutputImageType > WriterType;
  typedef itk::ImageFileWriter< CastImageFilterType::OutputImageType > EigenValueWriterType;
  
  typedef   itk::ImageToParametricSpaceFilter< EachEigenValueImageType, 
                                               MeshType > ParametricSpaceFilterType;
  
  typedef   itk::RescaleIntensityImageFilter< ImageType, 
                                              ImageType > RescaleIntensityFilterType;

  typedef   itk::SphereSpatialFunction< 
                                MeshType::PointDimension,
                                MeshType::PointType >  SphereSpatialFunctionType;
  
  typedef   itk::FrustumSpatialFunction< 
                                MeshType::PointDimension,
                                MeshType::PointType >  FrustumSpatialFunctionType;


            
  typedef fltk::SphereFunctionControl< 
                                  SphereSpatialFunctionType >
                                            SphereSpatialFunctionControlType;
  
  typedef fltk::FrustumFunctionControl< 
                                  FrustumSpatialFunctionType >
                                            FrustumSpatialFunctionControlType;
  
// These typedefs select the particular SpatialFunction
#ifdef SPHERE_FUNCTION
   typedef  SphereSpatialFunctionType          SpatialFunctionType;
   typedef  SphereSpatialFunctionControlType   SpatialFunctionControlType;
#endif 

#ifdef FRUSTUM_FUNCTION
   typedef  FrustumSpatialFunctionType         SpatialFunctionType;
   typedef  FrustumSpatialFunctionControlType  SpatialFunctionControlType;
#endif

                                
  typedef itk::InteriorExteriorMeshFilter<
                                        MeshType,
                                        MeshType,
                                        SpatialFunctionType  >   
                                                   SpatialFunctionFilterType;

  typedef itk::ParametricSpaceToImageSpaceMeshFilter<
                                      MeshType,
                                      ImageSpaceMeshType 
                                      >         InverseParametricFilterType;

  typedef itk::PointSetToImageFilter< MeshType, ImageType > 
                                              PointSetToImageFilterType;

  typedef PointSetToImageFilterType::OutputImageType   PointSetImageType;

  typedef itk::Image< OverlayPixelType, Dimension > OverlayImageType;
  
  typedef itk::ResampleImageFilter< PointSetImageType, PointSetImageType >
                                        OverlayImageResampleFilterType;
  
  typedef itk::BinaryThresholdImageFilter< 
    PointSetImageType,OverlayImageType >  ThresholdImageFilterType;

  typedef GradientMagnitudeFilterType::RealType     RealType;

public:
  ceExtractorConsoleBase();
  virtual ~ceExtractorConsoleBase();
  virtual void Load(const char * filename);
  virtual void ShowProgress(float);
  virtual void ShowStatus(const char * text);
  virtual void ShowSpatialFunctionControl( void );
  virtual void HideSpatialFunctionControl( void );
  virtual void Execute(void);
  virtual void SetSigma( RealType );

protected:
  VolumeReaderType::Pointer               m_Reader;

  GradientMagnitudeFilterType::Pointer    m_GradientMagnitude;
  
  HessianFilterType::Pointer              m_Hessian;

  EigenAnalysisFilterType::Pointer        m_EigenFilter;

  ImageAdaptorType::Pointer               m_EigenAdaptor1;
  ImageAdaptorType::Pointer               m_EigenAdaptor2;
  ImageAdaptorType::Pointer               m_EigenAdaptor3;

  CastImageFilterType::Pointer            m_EigenCastfilter1;
  CastImageFilterType::Pointer            m_EigenCastfilter2;
  CastImageFilterType::Pointer            m_EigenCastfilter3;

  HessianToLaplacianImageFilter::Pointer  m_Laplacian;

  ParametricSpaceFilterType::Pointer      m_ParametricSpace;
  
  SpatialFunctionFilterType::Pointer      m_SpatialFunctionFilter;

  SpatialFunctionControlType::Pointer     m_SpatialFunctionControl;

  InverseParametricFilterType::Pointer    m_InverseParametricFilter;

  PointSetToImageFilterType::Pointer      m_PointSetToImageFilter;

  OverlayImageResampleFilterType::Pointer m_OverlayResampleFilter;

  ThresholdImageFilterType::Pointer       m_ThresholdImageFilter;

  WriterType::Pointer                     m_Writer;

  EigenValueWriterType::Pointer           m_EigenValueWriter;

  bool   m_ImageLoaded;

};



#endif
