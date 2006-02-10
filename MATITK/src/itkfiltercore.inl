/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkfiltercore.inl
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#include "itkcore.h"
#include "itkMinMaxCurvatureFlowImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h" 
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkImageFileReader.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkBilateralImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkSigmoidImageFilter.h"

#include "itkBinaryThresholdImageFilter.h"
#include "itkBinomialBlurImageFilter.h"
#include "itkCannyEdgeDetectionImageFilter.h"
#include "itkDerivativeImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkFlipImageFilter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkGradientRecursiveGaussianImageFilter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkLaplacianRecursiveGaussianImageFilter.h"
#include "itkMeanImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkMinMaxCurvatureFlowImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageDuplicator.h"

#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkCastImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkWarpImageFilter.h"

#include "itkAntiAliasBinaryImageFilter.h"
#include "itkVnlFFTRealToComplexConjugateImageFilter.h"
#include "itkComplexToRealImageFilter.h"
#include "itkComplexToImaginaryImageFilter.h"
#include "itkScalarImageToHistogramGenerator.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkNumericTraits.h"
#include "itkOtsuMultipleThresholdsCalculator.h"


/*#include "itkBinaryMask3DMeshSource.h"
#include "itkMesh.h"
#include "itkSpatialObjectToImageFilter.h"*/

#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkSymmetricSecondRankTensor.h"
#include "itkHessian3DToVesselnessMeasureImageFilter.h"
#include "itkVotingBinaryIterativeHoleFillingImageFilter.h"



template <class ITKPIXELTYPE>
class ITKFilterClass{
public:
  #include "typedefs.inl"
  static typename ImageType::PixelContainerPointer pixelContainer;
  static std::vector< ITKPIXELTYPEArray<ITKPIXELTYPE> > pixelContainers;
  static typename ImportFilterType::Pointer importFilter[2];
  const static unsigned int Dimension = DIMENSION;

  static void filterAntiAliasBinaryImage(){
    const char* PARAM[]={"maximumRMSError","numberOfIterations","numberOfLayers"};
    const char* SUGGESTVALUE[]={"0.01","50","2"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    ///////////////begin core filter code///////////////////////
    double maximumRMSError=(double)paramIterator.getCurrentParam(0);
    unsigned int numberOfIterations=(unsigned int)paramIterator.getCurrentParam(1);
    unsigned int numberOfLayers=(unsigned int)paramIterator.getCurrentParam(2);
    typedef itk::AntiAliasBinaryImageFilter<InternalImageType, InternalImageType> AntiAliasFilterType;
    typedef itk::RescaleIntensityImageFilter<InternalImageType, InternalImageType > RescaleFilter;
    AntiAliasFilterType::Pointer antiAliasFilter = AntiAliasFilterType::New();
    antiAliasFilter->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    antiAliasFilter->SetMaximumRMSError( maximumRMSError );
    antiAliasFilter->SetNumberOfIterations( numberOfIterations );
    antiAliasFilter->SetNumberOfLayers( numberOfLayers );

    RescaleFilter::Pointer rescale = RescaleFilter::New();
    //The output of an edge filter is 0 or 1
    rescale->SetOutputMinimum(   0 );
    rescale->SetOutputMaximum( 255 );
    rescale->SetInput( antiAliasFilter->GetOutput() );
    rescale->Update();

    pixelContainer = rescale->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }




  static void filterGaussian(){
    const char* PARAM[]={"gaussianVariance","maxKernelWidth"};
    const char* SUGGESTVALUE[]={"",""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    double gaussianVariance=(double)paramIterator.getCurrentParam(0);
    unsigned int maxKernelWidth=(unsigned int)paramIterator.getCurrentParam(1);
    typedef itk::DiscreteGaussianImageFilter<InternalImageType, InternalImageType > FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    filter->SetVariance( gaussianVariance );
    filter->SetMaximumKernelWidth( maxKernelWidth );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer();
    /////////////////////End Core Filter Code////////////////////////////////////
  }

  static void filterCurvatureAnsio(){
    const char* PARAM[]={"numberOfIterations","timeStep","conductance"};
    const char* SUGGESTVALUE[]={"","0.0625","3.0"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    unsigned int NUMITERATION=(unsigned int)paramIterator.getCurrentParam(0);
    double TIMESTEP=(double)paramIterator.getCurrentParam(1);
    double CONDUCTANCE=(double)paramIterator.getCurrentParam(2);
    typedef itk::CurvatureAnisotropicDiffusionImageFilter<InternalImageType, InternalImageType >  FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    filter->SetNumberOfIterations( NUMITERATION );
    filter->SetTimeStep( TIMESTEP );
    filter->SetConductanceParameter( CONDUCTANCE );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////
  }

  static void filterCurvatureFlow(){
    const char* PARAM[]={"numberOfIterations","timeStep"};
    const char* SUGGESTVALUE[]={"10","0.0625"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    unsigned int NUMITERATION=(unsigned int)paramIterator.getCurrentParam(0);
    double TIMESTEP=(double)paramIterator.getCurrentParam(1);
    typedef itk::CurvatureFlowImageFilter<
      InputImageType, OutputImageType >  FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    filter->SetNumberOfIterations( NUMITERATION );
    filter->SetTimeStep( TIMESTEP );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////
  }


  static void filterMinMaxCurvatureFlow(){
    const char* PARAM[]={"numberOfIterations","timeStep","stencilRadius"};
    const char* SUGGESTVALUE[]={"10","0.0625","1"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    unsigned int NUMITERATION=(unsigned int)paramIterator.getCurrentParam(0);
    double TIMESTEP=(double)paramIterator.getCurrentParam(1);
    double RADIUS=(double)paramIterator.getCurrentParam(2);
    typedef itk::MinMaxCurvatureFlowImageFilter<
      InputImageType, OutputImageType >  FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    filter->SetNumberOfIterations( NUMITERATION );
    filter->SetStencilRadius( RADIUS );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////
  }


  static void filterGradientMagnitude(){
    const char* PARAM[]={""};
    const char* SUGGESTVALUE[]={""};
    const int nParam = 0;//sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    typedef itk::GradientMagnitudeImageFilter< InternalImageType, InternalImageType >  FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////
  }

  static void filterGradientMagnitudeWithSmoothing(){
    const char* PARAM[]={"Sigma"};
    const char* SUGGESTVALUE[]={""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    double sigma=(double)paramIterator.getCurrentParam(0);
    typedef   itk::GradientMagnitudeRecursiveGaussianImageFilter< InternalImageType, 
      InternalImageType >  GradientFilterType;
    GradientFilterType::Pointer  gradientMagnitude = GradientFilterType::New();
    gradientMagnitude->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    gradientMagnitude->SetSigma(  sigma  );
    gradientMagnitude->Update();
    pixelContainer = gradientMagnitude->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////
  }


  static void filterSigmoidNonlinearMapping(){
    const char* PARAM[]={"outputMinimum","outputMaximum","alpha","beta"};
    const char* SUGGESTVALUE[]={"10","240","10","170"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    ITKPIXELTYPE MIN=paramIterator.getCurrentParam(0);
    ITKPIXELTYPE MAX=paramIterator.getCurrentParam(1);
    double ALPHA=(double)paramIterator.getCurrentParam(2);
    double BETA=(double)paramIterator.getCurrentParam(3);
    typedef itk::SigmoidImageFilter<
      InputImageType, OutputImageType >  FilterType;
    FilterType::Pointer sigmoidFilter = FilterType::New();
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    filter->SetOutputMinimum(   MIN  );
    filter->SetOutputMaximum(   MAX  );
    filter->SetAlpha(  ALPHA  );
    filter->SetBeta(   BETA   );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////
  }


  static void filterDilate(){
    const char* PARAM[]={"DilationRadius","ValueOverWhichDilateWillApply"};
    const char* SUGGESTVALUE[]={"","255"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    unsigned int  DilationRadius=(unsigned int)paramIterator.getCurrentParam(0);
    unsigned int  ValueOverWhichDilateWillApply=(unsigned int)paramIterator.getCurrentParam(1);
    typedef itk::BinaryBallStructuringElement< InternalImageType::PixelType,
      Dimension  >             StructuringElementType;
    typedef itk::BinaryDilateImageFilter<InternalImageType,InternalImageType, 
      StructuringElementType >  DilateFilterType;
    DilateFilterType::Pointer binaryDilate = DilateFilterType::New();
    StructuringElementType  structuringElementDilation;
    structuringElementDilation.SetRadius(DilationRadius );  
    structuringElementDilation.CreateStructuringElement();
    binaryDilate->SetKernel(structuringElementDilation );
    binaryDilate->SetDilateValue( ValueOverWhichDilateWillApply );
    binaryDilate->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    binaryDilate->Update();
    pixelContainer = binaryDilate->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////

  }

  static void filterErode(){
    const char* PARAM[]={"ErosionRadius","ValueOverWhichErodeWillApply"};
    const char* SUGGESTVALUE[]={"","255"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    unsigned int  ErosionRadius=(unsigned int)paramIterator.getCurrentParam(0);
    unsigned int  ValueOverWhichErodeWillApply=(unsigned int)paramIterator.getCurrentParam(1);
    typedef itk::BinaryBallStructuringElement< InternalImageType::PixelType,
      Dimension  >             StructuringElementType;
    typedef itk::BinaryErodeImageFilter<InternalImageType,InternalImageType, 
      StructuringElementType >  ErodeFilterType;
    ErodeFilterType::Pointer binaryErode = ErodeFilterType::New();
    StructuringElementType  structuringElementErosion;
    structuringElementErosion.SetRadius(ErosionRadius );  
    structuringElementErosion.CreateStructuringElement();
    binaryErode->SetKernel(structuringElementErosion );
    binaryErode->SetErodeValue( ValueOverWhichErodeWillApply );
    binaryErode->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    binaryErode->Update();
    pixelContainer = binaryErode->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////

  }


  static void filterDanielssonDistanceMapImageFilter(){
    const char* PARAM[]={"",""};
    const char* SUGGESTVALUE[]={"",""};
    const int nParam = 0;//sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    typedef itk::DanielssonDistanceMapImageFilter<
      InputImageType, OutputImageType >  FilterType;
    typedef itk::RescaleIntensityImageFilter< 
      OutputImageType, OutputImageType > RescalerType;
    RescalerType::Pointer scaler = RescalerType::New();
    FilterType::Pointer filter = FilterType::New();
    filter->InputIsBinaryOn();
    filter->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    scaler->SetOutputMaximum( 65535L );
    scaler->SetOutputMinimum(     0L );
    scaler->SetInput( filter->GetOutput() );
    scaler->Update();
    pixelContainer = scaler->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////

  }

  static void filterDanielssonDistanceMapImageFilterGetVoronoiMap(){
    const char* PARAM[]={"",""};
    const char* SUGGESTVALUE[]={"",""};
    const int nParam = 0;//sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    typedef itk::DanielssonDistanceMapImageFilter<
      InputImageType, OutputImageType >  FilterType;
    typedef itk::RescaleIntensityImageFilter< 
      OutputImageType, OutputImageType > RescalerType;
    RescalerType::Pointer scaler = RescalerType::New();
    FilterType::Pointer filter = FilterType::New();
    filter->InputIsBinaryOn();
    filter->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    scaler->SetOutputMaximum( 65535L );
    scaler->SetOutputMinimum(     0L );
    scaler->SetInput( filter->GetVoronoiMap() );
    scaler->Update();
    pixelContainer = scaler->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////

  }

  static void filterBilateral(){
    const char* PARAM[]={"domainSigma","rangeSigma"};
    const char* SUGGESTVALUE[]={"5","5"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    double DOMAINSIGMA=(double)paramIterator.getCurrentParam(0);
    double RANGESIGMA=(double)paramIterator.getCurrentParam(1);
    double domainSigmas[ DIMENSION ];
    for(unsigned int i=0; i<Dimension; i++)
    {
      domainSigmas[i] = DOMAINSIGMA;
    }
    typedef itk::BilateralImageFilter<
      InputImageType, OutputImageType >  FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    filter->SetDomainSigma( domainSigmas );
    filter->SetRangeSigma(  RANGESIGMA   );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////
  }

  static void BinaryThresholdImageFilter(){
    const char* PARAM[]={"outsideValue","insideValue","lowerThreshold","upperThreshold"};
    const char* SUGGESTVALUE[]={"","","",""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    ITKPIXELTYPE outsideValue=paramIterator.getCurrentParam(0);
    ITKPIXELTYPE insideValue=paramIterator.getCurrentParam(1);
    ITKPIXELTYPE lowerThreshold=paramIterator.getCurrentParam(2);
    ITKPIXELTYPE upperThreshold=paramIterator.getCurrentParam(3);
    ///////////////begin core filter code///////////////////////
    typedef itk::BinaryThresholdImageFilter<
      InputImageType, OutputImageType >  FilterType;
    typedef itk::ImageFileWriter< InputImageType >  WriterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    filter->SetOutsideValue( outsideValue );
    filter->SetInsideValue(  insideValue  );
    filter->SetLowerThreshold( lowerThreshold );
    filter->SetUpperThreshold( upperThreshold );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }

  static void BinomialBlurImageFilter(){
    const char* PARAM[]={"repetitions"};
    const char* SUGGESTVALUE[]={""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    unsigned int repetitions=(unsigned int)paramIterator.getCurrentParam(0);
    ///////////////begin core filter code///////////////////////

    typedef itk::BinomialBlurImageFilter<
      InputImageType, OutputImageType >  FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    filter->SetRepetitions( repetitions );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }

  static void DerivativeImageFilter(){
    const char* PARAM[]={"SETORDER","SETDIRECTION"};
    const char* SUGGESTVALUE[]={"",""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    unsigned int SETORDER=(unsigned int)paramIterator.getCurrentParam(0);
    double SETDIRECTION=(double)paramIterator.getCurrentParam(1);
    ///////////////begin core filter code///////////////////////
    typedef itk::DerivativeImageFilter<
      InputImageType, OutputImageType >  FilterType;

    FilterType::Pointer filter = FilterType::New();
    filter->SetOrder(SETORDER);
    filter->SetDirection(SETDIRECTION);
    filter->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }

  static void DiscreteGaussianImageFilter(){
    const char* PARAM[]={"gaussianVariance","maxKernelWidth"};
    const char* SUGGESTVALUE[]={"",""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    double gaussianVariance=(double)paramIterator.getCurrentParam(0);
    double maxKernelWidth=(double)paramIterator.getCurrentParam(1);
    ///////////////begin core filter code///////////////////////

    typedef itk::DiscreteGaussianImageFilter<
      InputImageType, OutputImageType >  FilterType;

    FilterType::Pointer filter = FilterType::New();
    filter->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    filter->SetVariance( gaussianVariance );
    filter->SetMaximumKernelWidth( maxKernelWidth );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }

  static void FlipImageFilter(){
    const char* PARAM[]={"XDIRECTION","YDIRECTION","ZDIRECTION"};
    const char* SUGGESTVALUE[]={"","",""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    double XDIRECTION=(double)paramIterator.getCurrentParam(0);
    double YDIRECTION=(double)paramIterator.getCurrentParam(1);
    double ZDIRECTION=(double)paramIterator.getCurrentParam(2);
    ///////////////begin core filter code///////////////////////

    typedef itk::FlipImageFilter< InternalImageType >  FilterType;

    FilterType::Pointer filter = FilterType::New();
    typedef FilterType::FlipAxesArrayType     FlipAxesArrayType;

    FlipAxesArrayType flipArray;

    //  flipArray[0] = atoi( argv[3] );
    flipArray[0] = (unsigned int)( XDIRECTION );
    //  flipArray[1] = atoi( argv[4] );
    flipArray[1] = (unsigned int)( YDIRECTION );
    flipArray[2] = (unsigned int)( ZDIRECTION );

    filter->SetFlipAxes( flipArray );
    filter->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }

  static void GradientAnisotropicDiffusionImageFilter(){
    const char* PARAM[]={"numberOfIterations","timeStep","conductance"};
    const char* SUGGESTVALUE[]={"","",""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    unsigned int numberOfIterations=(unsigned int)paramIterator.getCurrentParam(0);
    double timeStep=paramIterator.getCurrentParam(1);
    double conductance=paramIterator.getCurrentParam(2);
    ///////////////begin core filter code///////////////////////

    typedef itk::GradientAnisotropicDiffusionImageFilter<
      InputImageType, OutputImageType >  FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    filter->SetNumberOfIterations( numberOfIterations );
    filter->SetTimeStep( timeStep );
    filter->SetConductanceParameter( conductance );

    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }

  static void GradientMagnitudeRecursiveGaussianImageFilter(){
    const char* PARAM[]={"sigma"};
    const char* SUGGESTVALUE[]={""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    double sigma=(double)paramIterator.getCurrentParam(0);
    ///////////////begin core filter code///////////////////////
    typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<
      InputImageType, OutputImageType >  FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    filter->SetSigma( sigma );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  static void LaplacianRecursiveGaussianImageFilter(){
    const char* PARAM[]={"sigma"};
    const char* SUGGESTVALUE[]={""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    double sigma=(double)paramIterator.getCurrentParam(0);
    ///////////////begin core filter code///////////////////////
    typedef itk::LaplacianRecursiveGaussianImageFilter<
      InputImageType, OutputImageType >  FilterType;
    FilterType::Pointer laplacian = FilterType::New();
    laplacian->SetNormalizeAcrossScale( false );
    laplacian->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    laplacian->SetSigma( sigma );
    laplacian->Update();
    pixelContainer = laplacian->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }

  static void MeanImageFilter(){
    const char* PARAM[]={"XRADIUS","YRADIUS","ZRADIUS"};
    const char* SUGGESTVALUE[]={"","",""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    double XRADIUS=(double)paramIterator.getCurrentParam(0);
    double YRADIUS=(double)paramIterator.getCurrentParam(1);
    double ZRADIUS=(double)paramIterator.getCurrentParam(2);
    ///////////////begin core filter code///////////////////////

    typedef itk::MeanImageFilter<
      InputImageType, OutputImageType >  FilterType;

    FilterType::Pointer filter = FilterType::New();
    InputImageType::SizeType indexRadius;

    indexRadius[0] = XRADIUS; // radius along x
    indexRadius[1] = YRADIUS; // radius along y
    indexRadius[2] = ZRADIUS; // radius along z

    filter->SetRadius( indexRadius );
    filter->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }

  static void MedianImageFilter(){
    const char* PARAM[]={"XRADIUS","YRADIUS","ZRADIUS"};
    const char* SUGGESTVALUE[]={"","",""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    double XRADIUS=(double)paramIterator.getCurrentParam(0);
    double YRADIUS=(double)paramIterator.getCurrentParam(1);
    double ZRADIUS=(double)paramIterator.getCurrentParam(2);
    ///////////////begin core filter code///////////////////////

    typedef itk::MedianImageFilter<
      InputImageType, OutputImageType >  FilterType;

    FilterType::Pointer filter = FilterType::New();
    InputImageType::SizeType indexRadius;

    indexRadius[0] = XRADIUS; // radius along x
    indexRadius[1] = YRADIUS; // radius along y
    indexRadius[2] = ZRADIUS; // radius along z

    filter->SetRadius( indexRadius );
    filter->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }

  static void FFTImageFilter(){
    const char* PARAM[]={"Real Or Complex Output"};
    const char* SUGGESTVALUE[]={"0 for Real, 1 for Complex"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    int ISCOMPLEX=(int)paramIterator.getCurrentParam(0);
    ///////////////begin core filter code///////////////////////
    typedef ITKPIXELTYPE  PixelType;
    const unsigned int Dimension = 3;
    typedef itk::VnlFFTRealToComplexConjugateImageFilter< 
      PixelType, Dimension >  FFTFilterType;

    FFTFilterType::Pointer fftFilter = FFTFilterType::New();
    fftFilter->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    fftFilter->Update();

    typedef FFTFilterType::OutputImageType    ComplexImageType;

    if (!ISCOMPLEX){
      typedef itk::ComplexToRealImageFilter< 
        ComplexImageType, OutputImageType > RealFilterType;

      RealFilterType::Pointer realFilter = RealFilterType::New();
      realFilter->SetInput( fftFilter->GetOutput() );
      typedef itk::RescaleIntensityImageFilter< 
        OutputImageType, 
        OutputImageType > RescaleFilterType;

      RescaleFilterType::Pointer intensityRescaler = RescaleFilterType::New();

      intensityRescaler->SetInput( realFilter->GetOutput() );
      intensityRescaler->SetOutputMinimum(  0  );
      intensityRescaler->SetOutputMaximum( 255 );

      intensityRescaler->Update();
      pixelContainer = intensityRescaler->GetOutput()->GetPixelContainer();
    }
    else{
      typedef itk::ComplexToImaginaryImageFilter< 
        ComplexImageType, OutputImageType > ImaginaryFilterType;
      ImaginaryFilterType::Pointer imaginaryFilter = ImaginaryFilterType::New();
      imaginaryFilter->SetInput( fftFilter->GetOutput() );
      imaginaryFilter->Update();
      pixelContainer = imaginaryFilter->GetOutput()->GetPixelContainer();
    }
    ///////////////end core filter code///////////////////////
  }


  static void OtsuMultipleThresholdImageFilter(){
    const char* PARAM[]={"numberOfThresholds","numberOfBins"};
    const char* SUGGESTVALUE[]={"less than 5","128"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    unsigned int SETNUMBEROFTHRESHOLDS=(unsigned int)paramIterator.getCurrentParam(0);
    unsigned int SETNUMBEROFBINS=(unsigned int)paramIterator.getCurrentParam(1);
    ///////////////begin core filter code///////////////////////
    typedef itk::Statistics::ScalarImageToHistogramGenerator< InputImageType > 
      ScalarImageToHistogramGeneratorType;
    typedef itk::OtsuMultipleThresholdsCalculator< 
      ScalarImageToHistogramGeneratorType::HistogramType >   CalculatorType;
    typedef itk::BinaryThresholdImageFilter< InputImageType, OutputImageType >  
      FilterType;
    typedef itk::CastImageFilter<
      InputImageType, InputImageType >  CastFilterType;
    CastFilterType::Pointer       castFilter       = CastFilterType::New();
    castFilter->SetInput(importFilter[IMPORTFILTERA]->GetOutput());

    ScalarImageToHistogramGeneratorType::Pointer scalarImageToHistogramGenerator = 
      ScalarImageToHistogramGeneratorType::New();
    CalculatorType::Pointer calculator = CalculatorType::New();
    FilterType::Pointer filter = FilterType::New();
    scalarImageToHistogramGenerator->SetNumberOfBins(SETNUMBEROFBINS);    
    scalarImageToHistogramGenerator->SetInput(castFilter->GetOutput());
    castFilter->Update();
    scalarImageToHistogramGenerator->Compute();

    calculator->SetNumberOfThresholds(SETNUMBEROFTHRESHOLDS);
    calculator->SetInputHistogram(scalarImageToHistogramGenerator->GetOutput());
    calculator->Update();

    filter->SetInput(castFilter->GetOutput()  );
    const ITKPIXELTYPE outsideValue = 0;
    const ITKPIXELTYPE insideValue = 255;
    filter->SetOutsideValue( outsideValue );
    filter->SetInsideValue(  insideValue  );

    const CalculatorType::OutputType &thresholdVector = calculator->GetOutput();  
    CalculatorType::OutputType::const_iterator itNum = thresholdVector.begin();
    ITKPIXELTYPE lowerThreshold = 0;
    ITKPIXELTYPE upperThreshold;
    //vector<InputImageType::Pointer> stupid;
    for(; itNum < thresholdVector.end(); itNum++) 
    {
      upperThreshold = (*itNum);
      filter->SetLowerThreshold( static_cast<ITKPIXELTYPE> (lowerThreshold) );
      filter->SetUpperThreshold( static_cast<ITKPIXELTYPE> (upperThreshold) );
      lowerThreshold = upperThreshold;

      filter->Update();
      /*ImageType::PixelContainerPointer containerCopy = filter->GetOutput()->GetPixelContainer();
      unsigned long numberOfPixels = filter->GetOutput()->GetPixelContainer()->Size();
      pixelContainers.push_back( ITKPIXELTYPEArray<ITKPIXELTYPE> (containerCopy->GetImportPointer(),numberOfPixels));   
      double* pbuffer = (double *) mxMalloc(numberOfPixels * sizeof(double));
      filter->GetOutput()->GetPixelContainer()->SetImportPointer( pbuffer, numberOfPixels, 
        false );*/
      unsigned long numberOfPixels = filter->GetOutput()->GetPixelContainer()->Size();
      ITKPIXELTYPE* pSource = filter->GetOutput()->GetPixelContainer()->GetImportPointer();
      ITKPIXELTYPE* pBuffer = (ITKPIXELTYPE *) mxMalloc(numberOfPixels * sizeof(ITKPIXELTYPE));
      for (int i=0;i<numberOfPixels;i++) pBuffer[i] = pSource[i];
      pixelContainers.push_back( ITKPIXELTYPEArray<ITKPIXELTYPE> (pBuffer,numberOfPixels));

    }
    ///////////////end core filter code///////////////////////
  }

  /*static void SurfaceExtractionFilter(){
  const char* PARAM[]={"membershipObjectValues"};
  const char* SUGGESTVALUE[]={""};
  const int nParam = sizeof(PARAM)/sizeof(*PARAM);
  ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
  double membershipObjectValues=paramIterator.getCurrentParam(0);
  ///////////////begin core filter code///////////////////////  

  typedef itk::Mesh<double>                         MeshType;
  typedef itk::BinaryMask3DMeshSource< InputImageType, MeshType >   MeshSourceType;

  MeshSourceType::Pointer meshSource = MeshSourceType::New();
  const double objectValue = static_cast<double>(membershipObjectValues);

  meshSource->SetObjectValue( objectValue );
  meshSource->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
  meshSource->Update(); 

  mexPrintf("%s has completed.\n",pstrzOp);
  mexPrintf("%s has completed.\n",pstrzOp);
  std::cout << "Nodes = " << meshSource << std::endl;
  std::cout << "Cells = " << meshSource->GetNumberOfCells() << std::endl;
  typedef itk::SpatialObjectToImageFilter< MeshSourceType, InputImageType >     SpatialObjectToImageFilterType;
  SpatialObjectToImageFilterType::Pointer spatialImageFilter = SpatialObjectToImageFilterType::New();
  spatialImageFilter->SetInput(meshSource->GetOutput());
  spatialImageFilter->Update();
  pixelContainer = spatialImageFilter->GetOutput()->GetPixelContainer();
  ///////////////end core filter code///////////////////////
  }*/

  static void VesselnessMeasureImageFilter(){
    const char* PARAM[]={"SetSigma","SetAlpha1","SetAlpha2"};
    const char* SUGGESTVALUE[]={"","",""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    double SETSIGMA=(double)paramIterator.getCurrentParam(0);
    double SETALPHA1=(double)paramIterator.getCurrentParam(1);
    double SETALPHA2=(double)paramIterator.getCurrentParam(2);
    ///////////////begin core filter code///////////////////////
    typedef   itk::HessianRecursiveGaussianImageFilter< 
      InputImageType >              HessianFilterType;
    HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
    hessianFilter->SetInput( importFilter[IMPORTFILTERA]->GetOutput());
    hessianFilter->SetSigma( SETSIGMA );
    hessianFilter->Update();
    typedef   itk::Hessian3DToVesselnessMeasureImageFilter<
      ITKPIXELTYPE > VesselnessMeasureFilterType;
    VesselnessMeasureFilterType::Pointer vesselnessFilter = 
      VesselnessMeasureFilterType::New();
    vesselnessFilter->SetInput( hessianFilter->GetOutput());
    vesselnessFilter->SetAlpha1( SETALPHA1);
    vesselnessFilter->SetAlpha2( SETALPHA2);
    vesselnessFilter->Update();
    pixelContainer = vesselnessFilter->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }

  static void VotingBinaryIterativeHoleFillingImageFilter(){
    const char* PARAM[]={"radiusX","radiusY","radiusZ","binaryImageBackgroundColor","binaryImageForegroundColor","SetMajorityThreshold","numberOfIterations"};
    const char* SUGGESTVALUE[]={"","","","","","",""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    unsigned int radiusX=(unsigned int)paramIterator.getCurrentParam(0);
    unsigned int radiusY=(unsigned int)paramIterator.getCurrentParam(1);
    unsigned int radiusZ=(unsigned int)paramIterator.getCurrentParam(2);
    unsigned int binaryImageBackgroundColor=(unsigned int)paramIterator.getCurrentParam(3);
    unsigned int binaryImageForegroundColor=(unsigned int)paramIterator.getCurrentParam(4);
    unsigned int SetMajorityThreshold=(unsigned int)paramIterator.getCurrentParam(5);
    unsigned int numberOfIterations=(unsigned int)paramIterator.getCurrentParam(6);
    ///////////////begin core filter code///////////////////////
    typedef   unsigned char  PixelType;
    typedef itk::VotingBinaryIterativeHoleFillingImageFilter<
      InputImageType >  FilterType;

    FilterType::Pointer filter = FilterType::New();
    ImageType::SizeType indexRadius;

    indexRadius[0] = radiusX; // radius along x
    indexRadius[1] = radiusY; // radius along y
    indexRadius[2] = radiusZ; // radius along y

    filter->SetRadius( indexRadius );
    filter->SetBackgroundValue(  binaryImageBackgroundColor );
    filter->SetForegroundValue( binaryImageForegroundColor );
    filter->SetMajorityThreshold( SetMajorityThreshold );
    filter->SetMaximumNumberOfIterations( numberOfIterations );
    filter->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    filter->Update();
    pixelContainer = filter->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }

  static void ITKFilterClassEntry(MATITKTemplatedVariables<ITKPIXELTYPE>& GTV){
    ////////////////////////////////start need to append if additing a new function/////////////////////////
    pixelContainer = GTV.pixelContainer;
    pixelContainers = GTV.pixelContainers;
    importFilter[0] = GTV.importFilter[0];
    importFilter[1] = GTV.importFilter[1];
    
    const FunctionCall operations[]={
        FunctionCall("FAAB","AntiAliasBinaryImageFilter",&ITKFilterClass<ITKPIXELTYPE>::filterAntiAliasBinaryImage),
        FunctionCall("FBB","BinomialBlurImageFilter",&ITKFilterClass<ITKPIXELTYPE>::BinomialBlurImageFilter),
        FunctionCall("FBD","BinaryDilateFilter",&ITKFilterClass<ITKPIXELTYPE>::filterDilate),
        FunctionCall("FBE","BinaryErodeFilter",&ITKFilterClass<ITKPIXELTYPE>::filterErode),
        FunctionCall("FBL","BilateralFilter",&ITKFilterClass<ITKPIXELTYPE>::filterBilateral),
        FunctionCall("FBT","BinaryThresholdImageFilter",&ITKFilterClass<ITKPIXELTYPE>::BinaryThresholdImageFilter),
        FunctionCall("FCA","CurvatureAnsioFilter",&ITKFilterClass<ITKPIXELTYPE>::filterCurvatureAnsio),
        FunctionCall("FCF","CurvatureFlowFilter",&ITKFilterClass<ITKPIXELTYPE>::filterCurvatureFlow),
        FunctionCall("FD","DerivativeImageFilter",&ITKFilterClass<ITKPIXELTYPE>::DerivativeImageFilter),
        FunctionCall("FDG","DiscreteGaussianImageFilter",&ITKFilterClass<ITKPIXELTYPE>::DiscreteGaussianImageFilter),
        FunctionCall("FDM","DanielssonDistanceMapImageFilter",&ITKFilterClass<ITKPIXELTYPE>::filterDanielssonDistanceMapImageFilter),
        FunctionCall("FDMV","DanielssonDistanceMapImageFilterGetVoronoiMap",&ITKFilterClass<ITKPIXELTYPE>::filterDanielssonDistanceMapImageFilterGetVoronoiMap),
        FunctionCall("FF","FlipImageFilter",&ITKFilterClass<ITKPIXELTYPE>::FlipImageFilter),
        FunctionCall("FFFT","FFTImageFilter",&ITKFilterClass<ITKPIXELTYPE>::FFTImageFilter),
        FunctionCall("FGA","GaussianFilter",&ITKFilterClass<ITKPIXELTYPE>::filterGaussian),
        FunctionCall("FGAD","GradientAnisotropicDiffusionImageFilter",&ITKFilterClass<ITKPIXELTYPE>::GradientAnisotropicDiffusionImageFilter),
        FunctionCall("FGM","GradientMagnitudeFilter",&ITKFilterClass<ITKPIXELTYPE>::filterGradientMagnitude),
        FunctionCall("FGMRG","GradientMagnitudeRecursiveGaussianImageFilter",&ITKFilterClass<ITKPIXELTYPE>::GradientMagnitudeRecursiveGaussianImageFilter),
        FunctionCall("FGMS","GradientMagnitudeWithSmoothingFilter",&ITKFilterClass<ITKPIXELTYPE>::filterGradientMagnitudeWithSmoothing),
        FunctionCall("FLS","LaplacianRecursiveGaussianImageFilter",&ITKFilterClass<ITKPIXELTYPE>::LaplacianRecursiveGaussianImageFilter),
        FunctionCall("FMEAN","MeanImageFilter",&ITKFilterClass<ITKPIXELTYPE>::MeanImageFilter),
        FunctionCall("FMEDIAN","MedianImageFilter",&ITKFilterClass<ITKPIXELTYPE>::MedianImageFilter),
        FunctionCall("FMMCF","MinMaxCurvatureFlowFilter",&ITKFilterClass<ITKPIXELTYPE>::filterMinMaxCurvatureFlow),
        FunctionCall("FOMT","OtsuMultipleThresholdImageFilter",&ITKFilterClass<ITKPIXELTYPE>::OtsuMultipleThresholdImageFilter),
        FunctionCall("FSN","SigmoidNonlinearMappingFilter",&ITKFilterClass<ITKPIXELTYPE>::filterSigmoidNonlinearMapping),
        FunctionCall("FVBIH","VotingBinaryIterativeHoleFillingImageFilter",&ITKFilterClass<ITKPIXELTYPE>::VotingBinaryIterativeHoleFillingImageFilter),
        FunctionCall("FVMI","VesselnessMeasureImageFilter",&ITKFilterClass<ITKPIXELTYPE>::VesselnessMeasureImageFilter) 
    };    

    ////////////////////////////////end need to append if additing a new function/////////////////////////
    const int nFcn = sizeof(operations)/sizeof(*operations);
    bool listFunctions=false;

    //dispatch the correct one
    bool found=false;
    for (int i=0;i<nFcn;i++){
      if (!stricmp(operations[i].OpCode,pstrzOp)) {
        mexPrintf("\n%s is being executed...\n",pstrzOp);
        mexEvalString("drawnow");
        operations[i].ptrFcn();       
        mexPrintf("%s has completed.\n",pstrzOp);
        found=true;
        break;
      }
    }
    if (!found)   
    {
      mexPrintf("\nThe following filtering functions are implemented:\n");  
      for (int i=0;i<nFcn;i++){
        mexPrintf("%s: %s\n",operations[i].OpCode,operations[i].OpName);
      }
      //if (pstrzOp[0]=='F') mexErrMsgTxt("Unknown Opcode");
    }

    GTV.pixelContainer = pixelContainer;
    GTV.pixelContainers = pixelContainers;
    GTV.importFilter[0] = importFilter[0];
    GTV.importFilter[1] = importFilter[1];
    
    return; 

  }
};

template <class ITKPIXELTYPE>
typename ITKFilterClass<ITKPIXELTYPE>::ImageType::PixelContainerPointer ITKFilterClass<ITKPIXELTYPE>::pixelContainer;
template <class ITKPIXELTYPE>
  std::vector< ITKPIXELTYPEArray<ITKPIXELTYPE> > ITKFilterClass<ITKPIXELTYPE>::pixelContainers;
template <class ITKPIXELTYPE>
typename ITKFilterClass<ITKPIXELTYPE>::ImportFilterType::Pointer ITKFilterClass<ITKPIXELTYPE>::importFilter[2];

template<>
void ITKFilterClass<unsigned char>::DerivativeImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::FFTImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<int>::DerivativeImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<int>::FFTImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 


/*
template<>
void ITKFilterClass<unsigned char>::BinomialBlurImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::filterDilate(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::filterErode(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} */
/*
template<>
void ITKFilterClass<unsigned char>::filterBilateral(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::BinaryThresholdImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::filterCurvatureAnsio(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::filterCurvatureFlow(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 
*/

/*
template<>
void ITKFilterClass<unsigned char>::DiscreteGaussianImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::filterDanielssonDistanceMapImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::filterDanielssonDistanceMapImageFilterGetVoronoiMap(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::FlipImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 
*/

/*
template<>
void ITKFilterClass<unsigned char>::filterGaussian(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::GradientAnisotropicDiffusionImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::filterGradientMagnitude(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::GradientMagnitudeRecursiveGaussianImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::filterGradientMagnitudeWithSmoothing(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::LaplacianRecursiveGaussianImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::MeanImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::MedianImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::filterMinMaxCurvatureFlow(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::OtsuMultipleThresholdImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::filterSigmoidNonlinearMapping(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::VotingBinaryIterativeHoleFillingImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} 

template<>
void ITKFilterClass<unsigned char>::VesselnessMeasureImageFilter(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
} */
