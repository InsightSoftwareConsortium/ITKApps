/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkregistrationcore.inl
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#ifndef ITKREGISTRATIONCORE_H
#define ITKREGISTRATIONCORE_H
#include "matitk.h"
#include "itkcore.h"
#include "seedcontainer.h"
#include "ParameterContainer.h"
#include "MATITKTemplatedVariables.h"
#include "itkregistrationcore.inl"
#endif

#include "itkregistrationcore.h"

#include "itkWarpImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkDemonsRegistrationFilter.h"
#include "itkThinPlateSplineKernelTransform.h"


template <class ITKPIXELTYPE>
class ITKRegClass{
public:
  #include "typedefs.inl"
  static typename ImageType::PixelContainerPointer pixelContainer;
  static SeedContainer<MATSEEDTYPE> seedsIndex;
  static std::vector< ITKPIXELTYPEArray<ITKPIXELTYPE> > pixelContainers;
  static typename ImportFilterType::Pointer importFilter[2];
  const static unsigned int Dimension = DIMENSION;

  static void registerThinPlateSpline(){ 
    const char* PARAM[]={""};
    const char* SUGGESTVALUE[]={""};
    const int nParam = 0;//sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    if (seedsIndex.getNumberOfSeeds()%2==1 || seedsIndex.getNumberOfSeeds()==0) 
      mexErrMsgTxt("This method requires landmarks.  Each landmark should be 3-dimensional, and there should be even number of landmarks (source->target)");
    if (emptyImportFilter[IMPORTFILTERB]){
      mexErrMsgTxt("This method requires two image volumes.  Input A should be the source image.  Input B should be a input A's target result.  ");
    }
    /////////////////////Begin Core Filter Code////////////////////////////////////
    const double epsilon = 1e-10;
    typedef itk::ThinPlateSplineKernelTransform<ITKPIXELTYPE, 3>   TPSTransform3DType;
    typedef TPSTransform3DType::InputPointType PointType3D;
    typedef TPSTransform3DType::PointsIterator Points3DIteratorType;
    PointType3D sourcePoint3D;
    PointType3D targetPoint3D;
    PointType3D mappedPoint3D;
    TPSTransform3DType::Pointer tps3D = TPSTransform3DType::New();
    /*std::cerr << "reading landmark positions..." << std::endl;
    std::ifstream input;
    input.open(pLandMarkFileName);
    int numLandmarkPairs;
    input >> numLandmarkPairs;
    if (numLandmarkPairs < 0)
    {
    std::cerr << "Error reading file." << std::endl;
    throw "Error reading landmark file."; 
    }*/
    // Reserve memory for the number of points

    tps3D->GetTargetLandmarks()->GetPoints()->Reserve( seedsIndex.getNumberOfSeeds()/2 );
    tps3D->GetSourceLandmarks()->GetPoints()->Reserve( seedsIndex.getNumberOfSeeds()/2);
    // Create landmark sets
    Points3DIteratorType tps3Ds = tps3D->GetSourceLandmarks()->GetPoints()->Begin();
    Points3DIteratorType tps3Dt = tps3D->GetTargetLandmarks()->GetPoints()->Begin();
    Points3DIteratorType tps3DsEnd  = tps3D->GetSourceLandmarks()->GetPoints()->End();
    //double x1, y1, z1,x2, y2, z2;
    for (int i=0; i< seedsIndex.getNumberOfSeeds(); i+=2){
      sourcePoint3D[0] = seedsIndex.getIndex(i)[0];
      sourcePoint3D[1] = seedsIndex.getIndex(i)[1];
      sourcePoint3D[2] = seedsIndex.getIndex(i)[2];
      tps3Ds.Value() = sourcePoint3D;
      targetPoint3D[0] = seedsIndex.getIndex(i+1)[0];;
      targetPoint3D[1] = seedsIndex.getIndex(i+1)[1];;
      targetPoint3D[2] = seedsIndex.getIndex(i+1)[2];;
      tps3Dt.Value() = targetPoint3D;
      tps3Ds++;
      tps3Dt++;
    }
    tps3D->ComputeWMatrix();
    typedef itk::ResampleImageFilter< InternalImageType, InternalImageType >    ResampleImageFilterType;
    ResampleImageFilterType::Pointer resample = ResampleImageFilterType::New();
    resample->SetTransform( tps3D );
    resample->SetInput(  importFilter[IMPORTFILTERA]->GetOutput() );
    resample->SetSize(  importFilter[IMPORTFILTERB]->GetOutput() ->GetLargestPossibleRegion().GetSize() );
    resample->SetOutputOrigin(   importFilter[IMPORTFILTERB]->GetOutput() ->GetOrigin() );
    resample->SetOutputSpacing(  importFilter[IMPORTFILTERB]->GetOutput() ->GetSpacing() );
    resample->SetDefaultPixelValue( 0 );
    resample->Update();
    pixelContainer = resample->GetOutput()->GetPixelContainer();

  }



  static void registerDemon(){ 
    const char* PARAM[]={"NumberOfHistogramLevels","NumberOfMatchPoints","DemonNumberofIterations",
      "DemonStandardDeviations"};
    const char* SUGGESTVALUE[]={"1024","7","150","1.0"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    if (emptyImportFilter[IMPORTFILTERB]){
      mexErrMsgTxt("This method requires two image volumes.  Input A should be the fixed image.  Input B should be the moving image.");
    }
    /////////////////////Begin Core Filter Code////////////////////////////////////
    unsigned int NumberOfHistogramLevels=(unsigned int)paramIterator.getCurrentParam(0);
    unsigned int NumberOfMatchPoints=(unsigned int)paramIterator.getCurrentParam(1);
    unsigned int DemonNumberofIterations=(unsigned int)paramIterator.getCurrentParam(2);
    double DemonStandardDeviations=(double)paramIterator.getCurrentParam(3);
    typedef itk::HistogramMatchingImageFilter<
      InternalImageType,
      InternalImageType >   MatchingFilterType;
    MatchingFilterType::Pointer matcher = MatchingFilterType::New();
    matcher->SetInput( importFilter[IMPORTFILTERB]->GetOutput());
    matcher->SetReferenceImage( importFilter[IMPORTFILTERA]->GetOutput() );
    matcher->SetNumberOfHistogramLevels( NumberOfHistogramLevels );
    matcher->SetNumberOfMatchPoints( NumberOfMatchPoints );
    matcher->ThresholdAtMeanIntensityOn();
    typedef itk::Vector< float, Dimension >    VectorPixelType;
    typedef itk::Image<  VectorPixelType, Dimension > DeformationFieldType;
    typedef itk::DemonsRegistrationFilter<InternalImageType,InternalImageType,DeformationFieldType>   RegistrationFilterType;
    RegistrationFilterType::Pointer filter = RegistrationFilterType::New();
    filter->SetFixedImage( importFilter[IMPORTFILTERA]->GetOutput());
    filter->SetMovingImage( matcher->GetOutput() );
    filter->SetNumberOfIterations( DemonNumberofIterations );
    filter->SetStandardDeviations( DemonStandardDeviations);
    filter->Update();
    typedef itk::WarpImageFilter<InternalImageType, InternalImageType,DeformationFieldType  > WarperType;
    typedef itk::LinearInterpolateImageFunction<InternalImageType,double>  InterpolatorType;
    WarperType::Pointer warper = WarperType::New();
    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    warper->SetInput( importFilter[IMPORTFILTERB]->GetOutput());
    warper->SetInterpolator( interpolator );
    warper->SetOutputSpacing( importFilter[IMPORTFILTERA]->GetOutput()->GetSpacing() );
    warper->SetOutputOrigin( importFilter[IMPORTFILTERA]->GetOutput()->GetOrigin() );
    warper->SetDeformationField( filter->GetOutput() );
    warper->Update();
    pixelContainer=warper->GetOutput()->GetPixelContainer(); 
  }


  static void ITKRegistrationEntry(MATITKTemplatedVariables<ITKPIXELTYPE>& GTV)
  {
    pixelContainer = GTV.pixelContainer;
    seedsIndex = GTV.seedsIndex;
    pixelContainers = GTV.pixelContainers;
    importFilter[0] = GTV.importFilter[0];
    importFilter[1] = GTV.importFilter[1];

    ////////////////////////////////start need to append if additing a new function/////////////////////////
    const FunctionCall operations[]={
      FunctionCall("RD","registerDemon",&ITKRegClass<ITKPIXELTYPE>::registerDemon),
      FunctionCall("RTPS","registerThinPlateSpline",&ITKRegClass<ITKPIXELTYPE>::registerThinPlateSpline)
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
      mexPrintf("\nThe following registration functions are implemented:\n");   
      for (int i=0;i<nFcn;i++){
        mexPrintf("%s: %s\n",operations[i].OpCode,operations[i].OpName);
      }
      //if (pstrzOp[0]=='R') mexErrMsgTxt("Unknown Opcode");
    }

    GTV.pixelContainer = pixelContainer;
    GTV.pixelContainers = pixelContainers;
    GTV.importFilter[0] = importFilter[0];
    GTV.importFilter[1] = importFilter[1];
    return;
  }
};


template <class ITKPIXELTYPE>
typename ITKRegClass<ITKPIXELTYPE>::ImageType::PixelContainerPointer ITKRegClass<ITKPIXELTYPE>::pixelContainer;
template<class ITKPIXELTYPE>
SeedContainer<MATSEEDTYPE> ITKRegClass<ITKPIXELTYPE>::seedsIndex;
template<class ITKPIXELTYPE>
  std::vector< ITKPIXELTYPEArray<ITKPIXELTYPE> > ITKRegClass<ITKPIXELTYPE>::pixelContainers;
template<class ITKPIXELTYPE>
typename ITKRegClass<ITKPIXELTYPE>::ImportFilterType::Pointer ITKRegClass<ITKPIXELTYPE>::importFilter[2];

template<>
void ITKRegClass<unsigned char>::registerThinPlateSpline(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
}

template<>
void ITKRegClass<float>::registerThinPlateSpline(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
}

template<>
void ITKRegClass<int>::registerThinPlateSpline(){ 
  mexPrintf("This method is not supported with this data type! Try converting to double first.");
}
