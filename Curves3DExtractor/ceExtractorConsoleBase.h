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
#include "itkAddImageFilter.h"
#include "itkBinaryMagnitudeImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkGradientRecursiveGaussianImageFilter.h"
#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkSymmetricEigenAnalysisImageFilter.h"
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


// Define which type of spatial function to use
// Only one of the following lines should be uncommented.
// # define SPHERE_FUNCTION
#define FRUSTUM_FUNCTION




class ceExtractorConsoleBase 
{
public:

  typedef   double                            InputPixelType;
  typedef   double                            PixelType;
  typedef   unsigned char                     OverlayPixelType;

  itkStaticConstMacro(Dimension,unsigned int,3);

  typedef   itk::Vector< PixelType, Dimension >           VectorType;
  
  typedef   itk::CovariantVector< PixelType, Dimension >  CovariantVectorType;

  typedef   itk::Image< InputPixelType, Dimension >       InputImageType;
  typedef   itk::Image< PixelType, Dimension >            ImageType;
  typedef   itk::Image< VectorType, Dimension >           VectorImageType;
  typedef   itk::Image< CovariantVectorType, Dimension >  CovariantVectorImageType;

  typedef   itk::Point<float,Dimension>                   MeshPointDataType;

  typedef   itk::Mesh< MeshPointDataType, 3 >     MeshType;

  typedef   itk::ImageFileReader< 
                            InputImageType >      VolumeReaderType;

  typedef   itk::Mesh< MeshType::PointType, Dimension >   ImageSpaceMeshType;


  typedef   itk::RecursiveGaussianImageFilter<
                            InputImageType,
                            ImageType        > InputGaussianFilterType;
  
  typedef   itk::RecursiveGaussianImageFilter<
                            ImageType,
                            ImageType         > GaussianFilterType;

  typedef   itk::SmoothingRecursiveGaussianImageFilter<
                            InputImageType,
                            ImageType         > SmoothingGaussianFilterType;

  typedef   itk::AddImageFilter< ImageType, 
                            ImageType, ImageType >  AddFilterType;

  typedef   itk::BinaryMagnitudeImageFilter< ImageType, 
                            ImageType, ImageType >  ModulusFilterType;

  typedef   itk::GradientRecursiveGaussianImageFilter< 
                            InputImageType,
                            CovariantVectorImageType >    GradientFilterType;

  typedef   itk::HessianRecursiveGaussianImageFilter< 
                            InputImageType >              HessianFilterType;
  typedef   HessianFilterType::OutputImageType            HessianImageType;
  typedef   HessianImageType::PixelType                   HessianPixelType;


  typedef   itk::FixedArray< float, 3 >                   EigenValuesArrayType;
  typedef   itk::Image< EigenValuesArrayType, 3 >         EigenValuesImageType;

  typedef   itk::SymmetricEigenAnalysisImageFilter< 
                                        HessianImageType, 
                                        EigenValuesImageType 
                                                  >       EigenFilterType;

  typedef   itk::MultiplyImageFilter< VectorImageType,
                                      VectorImageType,
                                      ImageType >  ScalarProductFilterType;

  typedef   itk::ImageToParametricSpaceFilter< ImageType, MeshType >
                                                  ParametricSpaceFilterType;

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

  typedef GaussianFilterType::RealType     RealType;

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

  InputGaussianFilterType::Pointer        m_Hx;
  InputGaussianFilterType::Pointer        m_Hy;
  InputGaussianFilterType::Pointer        m_Hz;

  SmoothingGaussianFilterType::Pointer    m_Smooth;
  GradientFilterType::Pointer             m_Gradient;
  HessianFilterType::Pointer              m_Hessian;
  EigenFilterType::Pointer                m_Eigen;

  GaussianFilterType::Pointer             m_H1x;
  GaussianFilterType::Pointer             m_H1y;
  GaussianFilterType::Pointer             m_H1z;

  GaussianFilterType::Pointer             m_H2x;
  GaussianFilterType::Pointer             m_H2y;
  GaussianFilterType::Pointer             m_H2z;

  AddFilterType::Pointer                  m_Add;

  ModulusFilterType::Pointer              m_Modulus;

  ScalarProductFilterType::Pointer        m_ScalarProduct;

  RescaleIntensityFilterType::Pointer     m_RescaleIntensitySmoothed;
  
  RescaleIntensityFilterType::Pointer     m_RescaleIntensityMaxEigen;
  
  RescaleIntensityFilterType::Pointer     m_RescaleIntensityMedialness;
  
  ParametricSpaceFilterType::Pointer      m_ParametricSpace;

  SpatialFunctionFilterType::Pointer      m_SpatialFunctionFilter;

  SpatialFunctionControlType::Pointer     m_SpatialFunctionControl;

  InverseParametricFilterType::Pointer    m_InverseParametricFilter;

  bool   m_ImageLoaded;

};



#endif
