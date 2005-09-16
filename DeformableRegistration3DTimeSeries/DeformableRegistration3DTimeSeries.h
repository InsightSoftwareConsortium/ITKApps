/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DeformableRegistration3DTimeSeries.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/** 
  \class DeformableRegistration3DTimeSeries
  \brief Class for handling 4DImageData.

  This class is derived from the DeformableRegistration3DTimeSeriesGUI class. It defines
  the necessary callbacks of the DeformableRegistration3DTimeSeriesGUI class.

*/
#ifndef __DeformableRegistration3DTimeSeries_h
#define __DeformableRegistration3DTimeSeries_h
#include "DeformableRegistration3DTimeSeriesGUI.h"
#include "itkImageToVTKImageFilter.h"
#include "itkImage.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkWarpImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkImageFileReader.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkDemonsRegistrationFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkBSplineDeformableTransform.h"
#include "itkLBFGSOptimizer.h"
#include "itkImageRegistrationMethod.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkSquaredDifferenceImageFilter.h"
#include "itkBSplineResampleImageFunction.h"
#include "itkIdentityTransform.h"
#include "itkBSplineDecompositionImageFilter.h"
#include "itkImageMomentsCalculator.h"
#include "itkVectorLinearInterpolateImageFunction.h"
#include "DicomImageReader.h"

#include "itkFEMRegistrationFilter.h"
#include "itkFEMImageMetricLoadImplementation.h"

#include "itkEventObject.h"
#include "itkCommand.h"

namespace fltk {

class DeformableRegistration3DTimeSeries : public DeformableRegistration3DTimeSeriesGUI
{
protected:
  int                                                         m_timeserie;
  typedef float                                               OutputPixelType;
  typedef itk::Image<unsigned short, 4>                       InputImageType;
  typedef itk::Image<float, 3>                                OutputImageType;
  typedef itk::Vector< float, 3>                              PixelType;
  typedef itk::Image<PixelType,4>                             InputDeformableImageType;
  typedef itk::Image<PixelType,3>                             OutputDeformableImageType;
  typedef itk::Image<unsigned short, 3>                       InputSegmentedImageType;
  typedef OutputImageType                                     OutputSegmentedImageType;
  typedef itk::ImageFileReader<InputImageType>                ImageReaderType;
  typedef itk::ExtractImageFilter< InputImageType, 
                                            OutputImageType > ExtractFilterType;
  typedef itk::ImageToVTKImageFilter< OutputImageType >       AdaptorFilterType;
  typedef AdaptorFilterType::Pointer                          AdaptorFilterPointer;
  ImageReaderType::Pointer                                    m_file1_reader; 
  ExtractFilterType::Pointer                                  m_extractfilter;
  AdaptorFilterPointer                                        m_AdaptorFilter;

  typedef itk::ImageFileReader<InputDeformableImageType>      DeformableReaderImageType;
  typedef itk::ExtractImageFilter<InputDeformableImageType, 
                                  OutputDeformableImageType > ExtractDeformableFilterType;
  typedef itk::ImageToVTKImageFilter<OutputDeformableImageType> 
                                                              ConnectorFilterType;
  DeformableReaderImageType::Pointer                          m_file2_reader;
  ExtractDeformableFilterType::Pointer                        m_extractdeformablefilter;
  ExtractDeformableFilterType::Pointer                        m_extractdeformablefilter2;
  ExtractDeformableFilterType::Pointer                        m_extractdeformablefiltermotion;
  ConnectorFilterType::Pointer                                m_ConnectorFilter;
  typedef itk::ImageFileReader<InputSegmentedImageType>       SegmentedImageReaderType;
  SegmentedImageReaderType::Pointer                           m_file3_reader; 
  
  typedef itk::ImageToVTKImageFilter<OutputImageType> SegmentedAdaptorFilterType;
  typedef SegmentedAdaptorFilterType::Pointer                 SegmentedAdaptorFilterPointer;
  SegmentedAdaptorFilterPointer                               m_SegmentedAdaptorFilter;
  typedef itk::WarpImageFilter<OutputSegmentedImageType,
    OutputSegmentedImageType,OutputDeformableImageType>        WarperType;
  typedef itk::NearestNeighborInterpolateImageFunction<
                                   OutputSegmentedImageType,
                                   double          >          InterpolatorType;

  //Dicom loader
  ISIS::DicomImageReader<InputImageType>                      m_DicomVolumeReader; 
  //Demons registration
  typedef itk::ExtractImageFilter< InputImageType, 
                                       OutputImageType>       ExtractFilterType;
  typedef itk::HistogramMatchingImageFilter<
                                    OutputImageType,
                                    OutputImageType >         MatchingFilterType;
  typedef itk::DemonsRegistrationFilter<
                                OutputImageType,
                                OutputImageType,
                                OutputDeformableImageType>    RegistrationFilterType;
  
  typedef itk::ImageRegionConstIterator
                                <OutputDeformableImageType>   Iterator3D;
  typedef itk::ImageRegionIterator
                                <InputDeformableImageType>    Iterator4D;
  RegistrationFilterType::Pointer                             m_DeformableFilter;

  //BSpline 
 // const unsigned int                                          SplineOrder = 3;
  typedef double                                              CoordinateRepType;
  typedef itk::BSplineDeformableTransform<CoordinateRepType,3,
          3 >                                       TransformType;
  typedef itk::LBFGSOptimizer                                OptimizerType;
  typedef itk::MeanSquaresImageToImageMetric< 
                                    OutputImageType, 
                                    OutputImageType >         MetricType;
  typedef itk::LinearInterpolateImageFunction< 
                                    OutputImageType,
                                    double>                   BSInterpolatorType;
  typedef itk::ImageRegistrationMethod< 
                                    OutputImageType, 
                                    OutputImageType   >       RegistrationType;
  BSInterpolatorType::Pointer                                 BSplineInterpolator;
  MetricType::Pointer                                         metric;
  OptimizerType::Pointer                                      optimizer;
  RegistrationType::Pointer                                   registration;
  //FEM Registration
  
  typedef itk::fem::Element3DC0LinearHexahedronMembrane       Element3DType;
  typedef itk::fem::Element3DC0LinearTetrahedronMembrane      Element3DType2;
  typedef itk::fem::FiniteDifferenceFunctionLoad
                   <OutputImageType,OutputImageType>          ImageLoadType;

  template class itk::fem::ImageMetricLoadImplementation<ImageLoadType>;
  typedef Element3DType::LoadImplementationFunctionPointer    LoadImpFP;
  typedef Element3DType::LoadType                             ElementLoadType;

  typedef Element3DType2::LoadImplementationFunctionPointer   LoadImpFP2;
  typedef Element3DType2::LoadType                            ElementLoadType2;

  typedef itk::fem::VisitorDispatcher<Element3DType,
    ElementLoadType, LoadImpFP>                               DispatcherType;

  typedef itk::fem::VisitorDispatcher<Element3DType2,
    ElementLoadType2, LoadImpFP2>                             DispatcherType2;
  typedef itk::fem::FEMRegistrationFilter<OutputImageType,
    OutputImageType>                                          FEMRegistrationType;
  
  //Warp Image 
  InterpolatorType::Pointer                                   m_interpolator; 
  WarperType::Pointer                                         m_WarperField;
  WarperType::Pointer                                         m_WarperField2;
  InputDeformableImageType::Pointer                           m_VectorImage4D;
  InputImageType::Pointer                                     m_Image4D;
  OutputImageType::Pointer                                    m_SegmentedImage3D;
  InputImageType::IndexType index;
  InputImageType::SizeType size;
  InputImageType::RegionType desiredRegion;

  //Compute the center of mass for the segmented data
  typedef itk::ImageMomentsCalculator<OutputSegmentedImageType> ImageMomentsType;

  ImageMomentsType::Pointer                                     calculator;

  //Compute the motion path of a selected point
   typedef itk::VectorLinearInterpolateImageFunction<
                                   OutputDeformableImageType,
                                   double          >  VectorInterpolatorType;

  VectorInterpolatorType::Pointer interpolator;
  typedef VectorInterpolatorType::OutputType  OutputType;
  typedef OutputDeformableImageType::PixelType::ValueType ValueType;

public:
   /** ProcessDicomReaderInteraction implements user interaction required
   for selecting DICOM data for read. */

   virtual void ProcessDicomReaderInteraction( void );
   itk::SimpleMemberCommand<DeformableRegistration3DTimeSeries>::Pointer      m_DicomReaderCommand;



  virtual void SetImage(OutputImageType * img);
  virtual void LoadRawData();
  virtual void SelectTime(int );
  virtual void LoadDeformableFieldData();
  virtual void LoadSegmentedData();
  void ManualSegmentationStart();
  void ManualSegmentationStop();
  void RasterizePolygon();

  /**
   LoadDicomSeries method loads the volume data from DICOM files. It first props up 
   a window for choosing the directory containing the DICOM data files. Once the
   directory name is provided, it props up another window listing all the "series 
   ids" of serieses whose files are in the chosen directory.The pixel type of the data 
   has be "unsigned char". */
  virtual void LoadDicomSeries();
  void StartDemonsDeformation();
  void StartBSplineDeformation();
  void StartFEMDeformation();
  void Save4DDeformationImage();
  void Save4DSegmentedImage();
  void Save3DSegmentedImage();
  void ViewVolume();
  void ViewSlice();
  void AbortDeformation();
  void ComputePathSelectedPoint();
  void ComputePathSelectedVolume();
  void Show();
  void SetInteractor(int i);
  void Hide();
  DeformableRegistration3DTimeSeries();
  virtual ~DeformableRegistration3DTimeSeries();
  //Aurora tracking functions - Funtionality disabled
private:
  //Aurora tracking variables - Funtionality disabled
};
} // end namespace fltk

#endif
