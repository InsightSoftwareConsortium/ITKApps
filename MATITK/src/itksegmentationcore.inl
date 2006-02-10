/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itksegmentationcore.inl
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

#include "itksegmentationcore.h"

#include "itkOtsuThresholdImageFilter.h"
#include "itkGeodesicActiveContourLevelSetImageFilter.h"
#include "itkCannySegmentationLevelSetImageFilter.h"
#include "itkNeighborhoodConnectedImageFilter.h"
#include "itkIsolatedConnectedImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h" 
#include "itkConfidenceConnectedImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImage.h"
#include "itkFastMarchingImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkLaplacianSegmentationLevelSetImageFilter.h"

#include "itkWatershedImageFilter.h"
#include "itkShapeDetectionLevelSetImageFilter.h"

#include "itkBioCellularAggregate.h"
#include "itkBioCell.h"


template <class ITKPIXELTYPE>
class ITKSegClass{
public:
  #include "typedefs.inl"
  static typename ImageType::PixelContainerPointer pixelContainer;
  static SeedContainer<MATSEEDTYPE> seedsIndex;
  static std::vector< ITKPIXELTYPEArray<ITKPIXELTYPE> > pixelContainers;
  static typename ImportFilterType::Pointer importFilter[2];
  const static unsigned int Dimension = DIMENSION;

  static void segmentationConfidenceConnected(){
    const char* PARAM[]={"multiplier","NumberOfIteration","ReplaceValue"};
    const char* SUGGESTVALUE[]={"2.5","5","100"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    //SeedContainer seedsIndex(1); //Inserted for filters that require seeding
    /////////////////////Begin Core Filter Code////////////////////////////////////
    double multiplier=(double)paramIterator.getCurrentParam(0);
    unsigned int NumberOfIteration=(unsigned int)paramIterator.getCurrentParam(1);
    ITKPIXELTYPE ReplaceValue=paramIterator.getCurrentParam(2);
    typedef itk::ConfidenceConnectedImageFilter<InternalImageType, InternalImageType> ConnectedFilterType;
    ConnectedFilterType::Pointer confidenceConnected = ConnectedFilterType::New();
    confidenceConnected->SetMultiplier(multiplier );
    confidenceConnected->SetNumberOfIterations( NumberOfIteration );
    confidenceConnected->SetReplaceValue( ReplaceValue );
    confidenceConnected->SetSeed(seedsIndex.getIndex (0));
    confidenceConnected->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    confidenceConnected->Update();
    pixelContainer = confidenceConnected->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////

  }


  static void segmentationIsolatedConnected(){
    const char* PARAM[]={"LowerThreshold","ReplaceValue"};
    const char* SUGGESTVALUE[]={"","255",""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    InternalImageType::PixelType LowerThreshold=(InternalImageType::PixelType)paramIterator.getCurrentParam(0);
    unsigned int ReplaceValue=(unsigned int)paramIterator.getCurrentParam(1);
    typedef itk::IsolatedConnectedImageFilter<InternalImageType, InternalImageType> ConnectedFilterType;
    ConnectedFilterType::Pointer isolatedConnected = ConnectedFilterType::New();
    isolatedConnected->SetLower(  LowerThreshold  );
    isolatedConnected->SetReplaceValue( ReplaceValue );
    isolatedConnected->SetSeed1( seedsIndex.getIndex(0) );
    isolatedConnected->SetSeed2( seedsIndex.getIndex(1) );
    isolatedConnected->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    isolatedConnected->Update();
    mexPrintf("\nIsolated Value Found = %f\n",(double)(isolatedConnected->GetIsolatedValue()) );
    pixelContainer = isolatedConnected->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////

  }

  static void segmentationNeighbourhoodConnected(){
    const char* PARAM[]={"RadiusX","RadiusY","RadiusZ","LowerThreshold","UpperThreshold","ReplaceValue"};
    const char* SUGGESTVALUE[]={"2","2","2","","","255"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    unsigned int radiusX=(unsigned int)paramIterator.getCurrentParam(0);
    unsigned int radiusY=(unsigned int)paramIterator.getCurrentParam(1);
    unsigned int radiusZ=(unsigned int)paramIterator.getCurrentParam(2);
    unsigned int LowerThreshold=(unsigned int)paramIterator.getCurrentParam(3);
    unsigned int UpperThreshold=(unsigned int)paramIterator.getCurrentParam(4);
    unsigned int ReplaceValue=(unsigned int)paramIterator.getCurrentParam(5);
    typedef itk::NeighborhoodConnectedImageFilter<InternalImageType,InternalImageType > ConnectedFilterType;  
    ConnectedFilterType::Pointer neighborhoodConnected = ConnectedFilterType::New();
    InternalImageType::SizeType radius;
    radius[0] = radiusX; 
    radius[1] = radiusY; 
    radius[2] = radiusZ; 
    neighborhoodConnected->SetLower( LowerThreshold );
    neighborhoodConnected->SetUpper( UpperThreshold );
    neighborhoodConnected->SetSeed( seedsIndex.getIndex(0) );
    neighborhoodConnected->SetReplaceValue( ReplaceValue );
    neighborhoodConnected->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    neighborhoodConnected->Update();
    pixelContainer = neighborhoodConnected->GetOutput()->GetPixelContainer(); 
  }

  static void segmentationConnectedThreshold(){
    const char* PARAM[]={"LowerThreshold","UpperThreshold"};
    const char* SUGGESTVALUE[]={"",""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    unsigned int LowerThreshold=(unsigned int)paramIterator.getCurrentParam(0);
    unsigned int UpperThreshold=(unsigned int)paramIterator.getCurrentParam(1);
    typedef itk::ConnectedThresholdImageFilter< InternalImageType, InternalImageType > ConnectedFilterType;
    ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();
    connectedThreshold->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    connectedThreshold->SetLower(  LowerThreshold  );
    connectedThreshold->SetUpper(  UpperThreshold  );
    connectedThreshold->SetReplaceValue( 255 );
    connectedThreshold->SetSeed( seedsIndex.getIndex(0) );
    connectedThreshold->Update();
    pixelContainer = connectedThreshold->GetOutput()->GetPixelContainer(); 
  }

  static void segmentationFastMarch(){
    const char* PARAM[]={"stoppingTime"};
    const char* SUGGESTVALUE[]={""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    double stoppingTime=(double)paramIterator.getCurrentParam(0);
    typedef  itk::FastMarchingImageFilter< InternalImageType, 
      InternalImageType >    FastMarchingFilterType;
    typedef FastMarchingFilterType::NodeContainer           NodeContainer;
    typedef FastMarchingFilterType::NodeType                NodeType;
    FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();
    fastMarching->SetOutputSize(importFilter[IMPORTFILTERA]->GetOutput()->GetBufferedRegion().GetSize() );
    fastMarching->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    fastMarching->SetStoppingValue(  stoppingTime  );
    NodeContainer::Pointer seedsITK = NodeContainer::New();
    NodeType node;
    for (int i=0;i<seedsIndex.getNumberOfSeeds();i++){
      node.SetIndex(seedsIndex.getIndex(i));
      node.SetValue(0.0);
      seedsITK->InsertElement(i,node);
    }
    fastMarching->SetTrialPoints(  seedsITK  );
    fastMarching->Update();
    pixelContainer = fastMarching->GetOutput()->GetPixelContainer(); 
  }

  static void segmentationOtsuThreshold(){
    const char* PARAM[]={"numberOfHistogram"};
    const char* SUGGESTVALUE[]={"128"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    /////////////////////Begin Core Filter Code////////////////////////////////////
    unsigned int NUMHISTOGRAM=(unsigned int)paramIterator.getCurrentParam(0);
    typedef itk::OtsuThresholdImageFilter<InputImageType, OutputImageType > FilterType;   
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    filter->SetOutsideValue( 255);
    filter->SetInsideValue( 0);
    filter->SetNumberOfHistogramBins( NUMHISTOGRAM );
    filter->Update();
    mexPrintf("\nCalculated Threshold Value Found = %f\n",(double)(filter->GetThreshold()) );
    pixelContainer = filter->GetOutput()->GetPixelContainer(); 

    /////////////////////End Core Filter Code////////////////////////////////////
  }

  static void segmentationGeodesicActiveContourLevelSet(){
    const char* PARAM[]={"propagationScaling", "CurvatureScaling",
      "AdvectionScaling","MaximumRMSError","MaxIteration"};
    const char* SUGGESTVALUE[]={"","1.0","1.0","0.02","800"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    if (emptyImportFilter[IMPORTFILTERB]){
      mexErrMsgTxt("This method requires two image volumes.  Input A should be the feature image.  Input B should be a input A's gradient result.  ");
    }
    mexPrintf("\nThis method requires two image volumes.\nInput A will be used as feature image. Input B will be used as input A's gradient.\n"); 
    /////////////////////Begin Core Filter Code////////////////////////////////////
    double propagationScaling=(double)paramIterator.getCurrentParam(0); 
    double CurvatureScaling=(double)paramIterator.getCurrentParam(1);
    double AdvectionScaling=(double)paramIterator.getCurrentParam(2);
    double MaximumRMSError=(double)paramIterator.getCurrentParam(3);
    unsigned int maxiteration=(unsigned int)paramIterator.getCurrentParam(4);
    typedef  itk::GeodesicActiveContourLevelSetImageFilter< InputImageType, 
      OutputImageType >    GeodesicActiveContourFilterType;
    GeodesicActiveContourFilterType::Pointer filter = GeodesicActiveContourFilterType::New();

    filter->SetPropagationScaling( propagationScaling );
    filter->SetCurvatureScaling( CurvatureScaling );
    filter->SetAdvectionScaling( AdvectionScaling );
    filter->SetMaximumRMSError( MaximumRMSError );
    filter->SetNumberOfIterations( maxiteration );
    filter->SetInput(importFilter[IMPORTFILTERB]->GetOutput());
    filter->SetFeatureImage( importFilter[IMPORTFILTERA]->GetOutput() );
    filter->Update();
    typedef itk::CastImageFilter< itk::Image<float,3>, OutputImageType >   CastFilterType;
    CastFilterType::Pointer caster = CastFilterType::New();
    caster->SetInput( filter->GetOutput() );

    typedef itk::BinaryThresholdImageFilter< 
      InternalImageType, 
      OutputImageType    >    ThresholdingFilterType;

    ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();                     
    thresholder->SetLowerThreshold( -1000.0 );
    thresholder->SetUpperThreshold(     0.0 );
    thresholder->SetOutsideValue(  0  );
    thresholder->SetInsideValue(  255 );
    thresholder->SetInput( caster->GetOutput() );
    thresholder->Update();
    pixelContainer = thresholder->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////
  }

  static void segmentationLaplacianLevelSetLevelSet(){
    const char* PARAM[]={"IsoSurfaceValue","PropagationScaling","CurvatureScaling","MaximumRMSError","MaxIteration"};
    const char* SUGGESTVALUE[]={"","","1.0","0.02","800"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    if (emptyImportFilter[IMPORTFILTERB]){
      mexErrMsgTxt("This method requires two image volumes.  Input A should be the feature image.  Input B should be a input A's gradient result.  ");
    }
    mexPrintf("\nThis method requires two image volumes.\nInput A will be used as feature image. Input B will be used as input A's gradient.\n"); 
    /////////////////////Begin Core Filter Code////////////////////////////////////
    double IsoSurfaceValue=(double)paramIterator.getCurrentParam(0);  
    double PropagationScaling=(double)paramIterator.getCurrentParam(1);
    double CurvatureScaling=(double)paramIterator.getCurrentParam(2);
    double MaximumRMSError=(double)paramIterator.getCurrentParam(3);
    unsigned int maxiteration=(unsigned int)paramIterator.getCurrentParam(4);
    typedef itk::LaplacianSegmentationLevelSetImageFilter< InternalImageType, 
      InternalImageType > LaplacianSegmentationLevelSetImageFilterType;
    LaplacianSegmentationLevelSetImageFilterType::Pointer filter =
      LaplacianSegmentationLevelSetImageFilterType::New();
    filter->SetMaximumRMSError( MaximumRMSError );
    filter->SetCurvatureScaling( CurvatureScaling );
    filter->SetPropagationScaling( PropagationScaling );
    filter->SetNumberOfIterations(maxiteration );
    filter->SetIsoSurfaceValue( IsoSurfaceValue );
    filter->SetInput(importFilter[IMPORTFILTERB]->GetOutput());
    filter->SetFeatureImage( importFilter[IMPORTFILTERA]->GetOutput() );
    filter->Update();
    typedef itk::CastImageFilter< itk::Image<float,3>, OutputImageType >   CastFilterType;
    CastFilterType::Pointer caster = CastFilterType::New();
    caster->SetInput( filter->GetOutput() );

    typedef itk::BinaryThresholdImageFilter< 
      InternalImageType, 
      OutputImageType    >    ThresholdingFilterType;

    ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();                     
    thresholder->SetLowerThreshold( -1000.0 );
    thresholder->SetUpperThreshold(     0.0 );
    thresholder->SetOutsideValue(  0  );
    thresholder->SetInsideValue(  255 );
    thresholder->SetInput( caster->GetOutput() );
    thresholder->Update();
    pixelContainer = thresholder->GetOutput()->GetPixelContainer(); 
    /////////////////////End Core Filter Code////////////////////////////////////
  }

  static void WatershedSegmentation(){
    const char* PARAM[]={"SETLEVEL","SETTHRESHOLD"};
    const char* SUGGESTVALUE[]={"0.0-1.0","0.0-1.0"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);

    double SETLEVEL=(double)paramIterator.getCurrentParam(0);
    double SETTHRESHOLD=(double)paramIterator.getCurrentParam(1);
    ///////////////begin core filter code///////////////////////

    typedef itk::WatershedImageFilter<InputImageType> WatershedFilterType;  
    WatershedFilterType::Pointer watershed = WatershedFilterType::New();
    watershed->SetLevel(SETLEVEL);
    watershed->SetThreshold(SETTHRESHOLD);
    watershed->SetInput(importFilter[IMPORTFILTERA]->GetOutput());
    typedef itk::CastImageFilter< itk::Image<unsigned long,3>, OutputImageType >   CastFilterType;
    CastFilterType::Pointer caster = CastFilterType::New();
    caster->SetInput( watershed->GetOutput() );
    caster->Update();
    pixelContainer = caster->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }

  static void ShapeDetectionLevelSetFilter(){
    const char* PARAM[]={"propagationScaling","curvatureScaling","SetMaximumRMSError","SetNumberOfIterations"};
    const char* SUGGESTVALUE[]={"","","0.02","800"};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    double propagationScaling=(double)paramIterator.getCurrentParam(0);
    double curvatureScaling=(double)paramIterator.getCurrentParam(1);
    double SetMaximumRMSError=(double)paramIterator.getCurrentParam(2);
    unsigned int SetNumberOfIterations=(unsigned int)paramIterator.getCurrentParam(3);
    if (emptyImportFilter[IMPORTFILTERB]){
      mexErrMsgTxt("This method requires two image volumes.  Input A should be the feature image.  Input B should be a input A's gradient result.  ");
    }
    mexPrintf("\nThis method requires two image volumes.\nInput A will be used as feature image. Input B will be used as input A's gradient.\n"); 
    ///////////////begin core filter code///////////////////////  
    typedef  itk::ShapeDetectionLevelSetImageFilter< InternalImageType, 
      InternalImageType >    ShapeDetectionFilterType;
    ShapeDetectionFilterType::Pointer 
      shapeDetection = ShapeDetectionFilterType::New();                              
    shapeDetection->SetInput( importFilter[IMPORTFILTERB]->GetOutput() );
    shapeDetection->SetFeatureImage( importFilter[IMPORTFILTERA]->GetOutput() );

    shapeDetection->SetPropagationScaling(  propagationScaling );
    shapeDetection->SetCurvatureScaling( curvatureScaling ); 
    shapeDetection->SetMaximumRMSError( SetMaximumRMSError );
    shapeDetection->SetNumberOfIterations( SetNumberOfIterations );

    typedef itk::CastImageFilter< itk::Image<float,3>, OutputImageType >   CastFilterType;
    CastFilterType::Pointer caster = CastFilterType::New();
    caster->SetInput( shapeDetection->GetOutput() );
    caster->Update();

    pixelContainer = caster->GetOutput()->GetPixelContainer();
    ///////////////end core filter code///////////////////////
  }


  static void CellularSegmentationSegmentation(){
    const char* PARAM[]={"SetChemoAttractantLowThreshold","SetChemoAttractantHighThreshold","numberOfIterations"};
    const char* SUGGESTVALUE[]={"","",""};
    const int nParam = sizeof(PARAM)/sizeof(*PARAM);
    ParameterContainer<ITKPIXELTYPE> paramIterator(PARAM,SUGGESTVALUE,nParam);
    double SetChemoAttractantLowThreshold=(double)paramIterator.getCurrentParam(0);
    double SetChemoAttractantHighThreshold=(double)paramIterator.getCurrentParam(1);
    unsigned int numberOfIterations=(unsigned int)paramIterator.getCurrentParam(2);
    ///////////////begin core filter code///////////////////////
    typedef itk::bio::CellularAggregate< Dimension >  CellularAggregateType;
    typedef CellularAggregateType::BioCellType        CellType;
    CellularAggregateType::Pointer cellularAggregate = CellularAggregateType::New();
    typedef itk::CastImageFilter< InputImageType, itk::Image<float,3> >   CastFilterType;
    CastFilterType::Pointer caster = CastFilterType::New();
    caster->SetInput( importFilter[IMPORTFILTERA]->GetOutput() );
    caster->Update();
    cellularAggregate->AddSubstrate( caster->GetOutput() );
    CellType::PointType  position;
    caster->GetOutput()->TransformIndexToPhysicalPoint( seedsIndex.getIndex(0), position ); 
    CellType * egg = CellType::CreateEgg();
    CellType::SetChemoAttractantLowThreshold(  SetChemoAttractantLowThreshold);
    CellType::SetChemoAttractantHighThreshold( SetChemoAttractantHighThreshold);

    cellularAggregate->SetEgg( egg, position );

    ITKPIXELTYPE* pbuffer = (ITKPIXELTYPE *) mxMalloc(numberOfIterations * sizeof(ITKPIXELTYPE));
    pixelContainers.push_back( ITKPIXELTYPEArray<ITKPIXELTYPE> (pbuffer,numberOfIterations,false));   
    for(unsigned int i=0; i<numberOfIterations; i++)
    {
      cellularAggregate->AdvanceTimeStep();
      pbuffer[i] = (ITKPIXELTYPE)(cellularAggregate->GetNumberOfCells());
    }
    mexPrintf("Final number of Cells = %u", cellularAggregate->GetNumberOfCells());
    ///////////////end core filter code///////////////////////
  }



  static void ITKSegmentationEntry(MATITKTemplatedVariables<ITKPIXELTYPE>& GTV)
  {
    ////////////////////////////////start need to append if additing a new function/////////////////////////
    pixelContainer = GTV.pixelContainer;
    seedsIndex = GTV.seedsIndex;
    pixelContainers = GTV.pixelContainers;
    importFilter[0] = GTV.importFilter[0];
    importFilter[1] = GTV.importFilter[1];

    const FunctionCall operations[]={
      FunctionCall("SCC","ConfidenceConnectedSegmentation",&ITKSegClass<ITKPIXELTYPE>::segmentationConfidenceConnected),
        FunctionCall("SCSS","CellularSegmentationSegmentation(Debug)",&ITKSegClass<ITKPIXELTYPE>::CellularSegmentationSegmentation),      
        FunctionCall("SCT","ConnectedThresholdSegmentation",&ITKSegClass<ITKPIXELTYPE>::segmentationConnectedThreshold),
        FunctionCall("SFM","FastMarchSegmentation",&ITKSegClass<ITKPIXELTYPE>::segmentationFastMarch),
        FunctionCall("SGAC","GeodesicActiveContourLevelSetSegmentation",&ITKSegClass<ITKPIXELTYPE>::segmentationGeodesicActiveContourLevelSet),
        FunctionCall("SIC","IsolatedConnectedSegmentation",&ITKSegClass<ITKPIXELTYPE>::segmentationIsolatedConnected),
        FunctionCall("SLLS","LaplacianLevelSetLevelSetSegmentation",&ITKSegClass<ITKPIXELTYPE>::segmentationLaplacianLevelSetLevelSet),
        FunctionCall("SNC","NeighbourhoodConnectedSegmentation",&ITKSegClass<ITKPIXELTYPE>::segmentationNeighbourhoodConnected),
        FunctionCall("SOT","OtsuThresholdSegmentation",&ITKSegClass<ITKPIXELTYPE>::segmentationOtsuThreshold),
        FunctionCall("SSDLS","ShapeDetectionLevelSetFilter",&ITKSegClass<ITKPIXELTYPE>::ShapeDetectionLevelSetFilter),
        FunctionCall("SWS","WatershedSegmentation",&ITKSegClass<ITKPIXELTYPE>::WatershedSegmentation)
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
      mexPrintf("\nThe following segmentation functions are implemented:\n");   
      for (int i=0;i<nFcn;i++){
        mexPrintf("%s: %s\n",operations[i].OpCode,operations[i].OpName);
      }
      //if (pstrzOp[0]=='S') mexErrMsgTxt("Unknown Opcode");
    }

    GTV.pixelContainer = pixelContainer;
    GTV.pixelContainers = pixelContainers;
    GTV.importFilter[0] = importFilter[0];
    GTV.importFilter[1] = importFilter[1];
    return;
  }
};

template <class ITKPIXELTYPE>
typename ITKSegClass<ITKPIXELTYPE>::ImageType::PixelContainerPointer ITKSegClass<ITKPIXELTYPE>::pixelContainer;
template <class ITKPIXELTYPE>
SeedContainer<MATSEEDTYPE> ITKSegClass<ITKPIXELTYPE>::seedsIndex;
template <class ITKPIXELTYPE>
  std::vector< ITKPIXELTYPEArray<ITKPIXELTYPE> > ITKSegClass<ITKPIXELTYPE>::pixelContainers;
template <class ITKPIXELTYPE>
typename ITKSegClass<ITKPIXELTYPE>::ImportFilterType::Pointer ITKSegClass<ITKPIXELTYPE>::importFilter[2];
