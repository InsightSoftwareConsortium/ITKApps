/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    RegionGrowingSegmentationBase2D.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef REGIONGROWINGSEGMENTATIONBASE
#define REGIONGROWINGSEGMENTATIONBASE

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <itkImage.h>
#include <itkCastImageFilter.h>

#include <itkConnectedThresholdImageFilter.h>
#include <itkConfidenceConnectedImageFilter.h>

#include <itkBilateralImageFilter.h>
#include <itkCurvatureFlowImageFilter.h>
#include <itkGradientAnisotropicDiffusionImageFilter.h>
#include <itkCurvatureAnisotropicDiffusionImageFilter.h>

#include <itkSobelEdgeDetectionImageFilter.h>
#include <itkMaximumImageFilter.h>



/**
 * \brief RegionGrowingSegmentationBase2D class that instantiate
 * the elements required for a Region Growing approach for segmentation
 *
 */
class RegionGrowingSegmentationBase2D
{
public:

  /** Dimension of the images to be registered */ 
  enum { ImageDimension = 2 };

  /** Pixel type used for reading the input image */
  typedef   signed short                         InputPixelType;

  /** Pixel type used for writing the output image */
  typedef   unsigned char                        OutputPixelType;

  /** Pixel type to be used internally */
  typedef   float                                 InternalPixelType;
  
  /** Type of the input image */
  typedef   itk::Image<InputPixelType,ImageDimension>    InputImageType;

  /** Type of the output image */
  typedef   itk::Image<OutputPixelType,ImageDimension>   OutputImageType;

  /** Type of the internal image */
  typedef   itk::Image<InternalPixelType,ImageDimension> InternalImageType;

  /** Filter for reading the input image */
  typedef   itk::ImageFileReader< InputImageType >       ImageReaderType;

  /** Filter for writing the input image */
  typedef   itk::ImageFileWriter< OutputImageType >      ImageWriterType;

  /** Cast filter needed because Curvature flow expects double images */
  typedef   itk::CastImageFilter< 
                 InputImageType, 
                 InternalImageType >     Cast1ImageFilterType;

  /** Cast filter needed because Sobel expects double images */
  typedef   itk::CastImageFilter< 
                 OutputImageType, 
                 InternalImageType >     Cast2ImageFilterType;

  /** Null filter used to select smoothing filter */
  typedef   itk::CastImageFilter< 
                 InternalImageType, 
                 InternalImageType >     NullImageFilterType;


  /** Curvature flow image filter for producing homogeneous regions */
  typedef   itk::BilateralImageFilter< 
                 InternalImageType, 
                 InternalImageType >     BilateralImageFilterType;

  /** Curvature flow image filter for producing homogeneous regions */
  typedef   itk::CurvatureFlowImageFilter< 
                 InternalImageType, 
                 InternalImageType >     CurvatureFlowImageFilterType;

  /** Curvature Anisotropic Diffusion Image Filter */
  typedef   itk::CurvatureAnisotropicDiffusionImageFilter< 
                 InternalImageType, 
                 InternalImageType > CurvatureAnisotropicDiffusionImageFilterType;

  /** Gradient Anisotropic Diffusion Image Filter */
  typedef   itk::GradientAnisotropicDiffusionImageFilter< 
                 InternalImageType, 
                 InternalImageType > GradientAnisotropicDiffusionImageFilterType;



  /** Threshold Connected Image Filter */
  typedef   itk::ConnectedThresholdImageFilter< 
                 InternalImageType, 
                 OutputImageType >     ConnectedThresholdImageFilterType;

  /** Confidence Connected Image Filter */
  typedef   itk::ConfidenceConnectedImageFilter< 
                 InternalImageType, 
                 OutputImageType >     ConfidenceConnectedImageFilterType;

  /** Sobel Filter for extracting the contour of a segmented region */
  typedef   itk::SobelEdgeDetectionImageFilter<
                 InternalImageType, 
                 InternalImageType >     SobelImageFilterType;

  /** Maximum Filter for composing the edge with the original image */
  typedef   itk::MaximumImageFilter<
                 InternalImageType, 
                 InternalImageType,
                 InternalImageType >     MaximumImageFilterType;


public:
  RegionGrowingSegmentationBase2D();
  virtual ~RegionGrowingSegmentationBase2D();

  virtual void LoadInputImage(void)=0;
  virtual void LoadInputImage(const char * filename);

  virtual void WriteOutputImage()=0;
  virtual void WriteOutputImage(const char * filename);
  virtual void WriteConnectedThresholdImage()=0;
  virtual void WriteConfidenceConnectedImage()=0;

  virtual void ShowStatus(const char * text)=0;

  virtual void Stop(void);

  virtual void SelectSmoothingFilter( unsigned int );


protected:

  ImageReaderType::Pointer                    m_ImageReader;

  ImageWriterType::Pointer                    m_ImageWriter;

  bool                                        m_InputImageIsLoaded;

  Cast1ImageFilterType::Pointer               m_Cast1ImageFilter;

  Cast2ImageFilterType::Pointer               m_Cast2ImageFilter;

  NullImageFilterType::Pointer                m_NullImageFilter;

  BilateralImageFilterType::Pointer           m_BilateralImageFilter;

  CurvatureFlowImageFilterType::Pointer       m_CurvatureFlowImageFilter;

  CurvatureAnisotropicDiffusionImageFilterType::Pointer  m_CurvatureAnisotropicDiffusionImageFilter;

  GradientAnisotropicDiffusionImageFilterType::Pointer   m_GradientAnisotropicDiffusionImageFilter;

  ConnectedThresholdImageFilterType::Pointer  m_ConnectedThresholdImageFilter;

  ConfidenceConnectedImageFilterType::Pointer m_ConfidenceConnectedImageFilter;

  SobelImageFilterType::Pointer               m_SobelImageFilter;

  MaximumImageFilterType::Pointer             m_MaximumImageFilter;

};



#endif

