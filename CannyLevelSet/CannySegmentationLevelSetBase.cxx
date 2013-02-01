/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    CannySegmentationLevelSetBase.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "CannySegmentationLevelSetBase.h"
#include <FL/fl_ask.H>


/************************************
 *
 *  Constructor
 *
 ***********************************/
CannySegmentationLevelSetBase 
::CannySegmentationLevelSetBase()
{

  m_ImageReader                  = ImageReaderType::New();

  m_CastImageFilter = CastImageFilterType::New();
  m_CastImageFilter->SetInput( m_ImageReader->GetOutput() );

  m_DerivativeFilter   = DerivativeFilterType::New();
  m_DerivativeFilter->SetInput( m_CastImageFilter->GetOutput() );

  m_SigmoidFilter  = SigmoidFilterType::New();
  m_SigmoidFilter->SetInput( m_DerivativeFilter->GetOutput() );
  m_SigmoidFilter->SetOutputMaximum( 1.0 );
  m_SigmoidFilter->SetOutputMinimum( 0.0 );

  m_TrialPoints = NodeContainer::New();

  m_FastMarchingFilter = FastMarchingFilterType::New();
  m_FastMarchingFilter->SetInput( m_SigmoidFilter->GetOutput() );
  m_FastMarchingFilter->SetTrialPoints( m_TrialPoints );

  m_InputThresholdFilter = ThresholdFilterType::New();
  m_InputThresholdFilter->SetInput( m_FastMarchingFilter->GetOutput() );
  m_InputThresholdFilter->SetUpperThreshold( itk::NumericTraits<InternalPixelType>::Zero ); 
  m_InputThresholdFilter->SetLowerThreshold( itk::NumericTraits<InternalPixelType>::NonpositiveMin() ); 
  m_InputThresholdFilter->SetInsideValue( 1 );
  m_InputThresholdFilter->SetOutsideValue(  0 );

  m_CannyFilter = CannyFilterType::New();
  m_CannyFilter->SetInput(  m_FastMarchingFilter->GetOutput() );
  m_CannyFilter->SetFeatureImage(   m_CastImageFilter->GetOutput() );

  m_CannyFilter->SetMaximumRMSError( 0.02 );
  m_CannyFilter->SetNumberOfIterations( 100 );
  m_CannyFilter->UseImageSpacingOn();
  m_CannyFilter->SetVariance( 1.0 );

  m_ThresholdFilter = ThresholdFilterType::New();
  m_ThresholdFilter->SetInput( m_CannyFilter->GetOutput() );
  m_ThresholdFilter->SetUpperThreshold( itk::NumericTraits<InternalPixelType>::Zero ); 
  m_ThresholdFilter->SetLowerThreshold( itk::NumericTraits<InternalPixelType>::NonpositiveMin() ); 
  m_ThresholdFilter->SetInsideValue(   1 );
  m_ThresholdFilter->SetOutsideValue(  0 );

  m_CannyEdgesThresholdFilter = ThresholdFilterType::New();
  m_CannyEdgesThresholdFilter->SetInput( m_CannyFilter->GetCannyImage() );
  m_CannyEdgesThresholdFilter->SetUpperThreshold( itk::NumericTraits<InternalPixelType>::Zero ); 
  m_CannyEdgesThresholdFilter->SetLowerThreshold( itk::NumericTraits<InternalPixelType>::NonpositiveMin() ); 
  m_CannyEdgesThresholdFilter->SetInsideValue( 0 );
  m_CannyEdgesThresholdFilter->SetOutsideValue( 1 );

  m_SeedImage = SeedImageType::New();

  m_SeedValue = 0; // It must be set to the minus distance of the initial level set.

  m_NumberOfSeeds = 0;

  m_InputImageIsLoaded  = false;

  m_VtkImporter1 = vtkImageImport::New();
  m_VtkImporter2 = vtkImageImport::New();

  m_ContourFilter = vtkContourFilter::New();

  m_PolyDataWriter = vtkPolyDataWriter::New();

  m_ItkExporter1 = ExportFilterType::New();
  m_ItkExporter2 = ExportFilterType::New();

  m_ProbeFilter = vtkProbeFilter::New();

  this->ConnectPipelines( m_VtkImporter1, m_ItkExporter1 );
  this->ConnectPipelines( m_VtkImporter2, m_ItkExporter2 );

  m_ItkExporter1->SetInput( m_CannyFilter->GetOutput() );
  m_ItkExporter2->SetInput( m_DerivativeFilter->GetOutput() );
 
  m_ContourFilter->SetInputConnection( m_VtkImporter1->GetOutputPort() );

  m_ContourFilter->SetValue( 0, 0.0 ); // Value of the ZeroSet.

  m_ProbeFilter->SetInputConnection( m_ContourFilter->GetOutputPort() );
  m_ProbeFilter->SetSourceConnection( m_VtkImporter2->GetOutputPort() );
  
  m_PolyDataWriter->SetInputConnection( m_ProbeFilter->GetOutputPort() );

}


void CannySegmentationLevelSetBase
::ConnectPipelines( vtkImageImport * importer, ExportFilterType * exporter )
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}




/************************************
 *
 *  Destructor
 *
 ***********************************/
CannySegmentationLevelSetBase 
::~CannySegmentationLevelSetBase()
{

  m_PolyDataWriter->Delete();

  m_ContourFilter->Delete();

  m_VtkImporter1->Delete();
  m_VtkImporter2->Delete();
}



 
/************************************
 *
 *  Load Input Image
 *
 ***********************************/
void
CannySegmentationLevelSetBase 
::LoadInputImage( const char * filename )
{
  if( !filename )
  {
    return;
  }

  m_ImageReader->SetFileName( filename );
  m_ImageReader->Update();

  // Allocate a image of seeds of the same size
  InputImageType::RegionType region 
            = m_ImageReader->GetOutput()->GetBufferedRegion();

  InputImageType::SizeType size = region.GetSize();

  for(unsigned int i=0; i<ImageDimension; i++)
    {
    if( size[i] < 10 )
      {
      this->ShowStatus("Image must be 3D and with at least 10 pixels along each Dimension.");
      itk::ExceptionObject excp;
      excp.SetDescription("Image must be 3D and with at least 10 pixels along each Dimension.");
      throw excp;
      }
    }


  m_SeedImage->SetRegions( region );
  m_SeedImage->Allocate();
  m_SeedImage->FillBuffer( itk::NumericTraits<SeedImageType::PixelType>::Zero );

  m_FastMarchingFilter->SetOutputSize( region.GetSize() );

  m_InputImageIsLoaded = true;

}



  

/********************************************
 *
 *  Set the seed. From this seed an Initial
 *  level set image is generated 
 *
 *******************************************/
void
CannySegmentationLevelSetBase 
::AddSeed( const IndexType & seedPosition  )
{

  // setup trial points
  NodeType node;
  
  node.SetValue( m_SeedValue );
  node.SetIndex( seedPosition );

  m_TrialPoints->InsertElement( m_NumberOfSeeds, node );

  m_SeedImage->SetPixel( seedPosition, 1 );

  m_NumberOfSeeds++;
  
}



  
/************************************
 *
 *  Clear Seeds
 *
 ***********************************/
void
CannySegmentationLevelSetBase 
::ClearSeeds()
{
  m_TrialPoints->Initialize(); 
  m_NumberOfSeeds = 0;
  m_FastMarchingFilter->Modified();
  m_SeedImage->FillBuffer( itk::NumericTraits<SeedImageType::PixelType>::Zero );
}


 

/********************************************
 *
 *  Set the Sigma value
 *
 *******************************************/
void
CannySegmentationLevelSetBase 
::SetSigma( double value )
{
  m_DerivativeFilter->SetSigma( value );
}



/********************************************
 *
 *  Set the Threshold value for edge strenght
 *
 *******************************************/
void
CannySegmentationLevelSetBase 
::SetThreshold( double value )
{
  m_CannyFilter->SetThreshold( value );
}




/************************************
 *
 *  Compute Gradient Magnitude
 *
 ***********************************/
void
CannySegmentationLevelSetBase
::ComputeGradientMagnitude( void )
{
  this->ShowStatus("Computing Gradient Image");
  m_DerivativeFilter->Update();
}


 
/************************************
 *
 *  Compute Fast Marching
 *
 ***********************************/
void
CannySegmentationLevelSetBase
::ComputeFastMarching( void )
{
  this->ShowStatus("Computing Fast Marching");
  m_FastMarchingFilter->Update();
}


  
/************************************
 *
 *  Compute Zero Set
 *
 ***********************************/
void
CannySegmentationLevelSetBase
::ComputeZeroSet( void )
{
  this->ShowStatus("Computing Zero Set");
  // This will also update the FastMarching
  m_InputThresholdFilter->Update();
}


 



/************************************
 *
 *  Compute Edge Potential Image
 *
 ***********************************/
void
CannySegmentationLevelSetBase
::ComputeEdgePotential( void )
{
  this->ComputeGradientMagnitude();
  this->ShowStatus("Computing Edge Potential Image");
  m_SigmoidFilter->Update();
}


  

/************************************
 *
 *  Start Segmentation
 *
 ***********************************/
void
CannySegmentationLevelSetBase
::RunCanny( void )
{

  // update the marching filter
  try
    {
    this->ComputeEdgePotential();
    this->ComputeFastMarching();
    this->ShowStatus("Computing CannySegmentationLevelSet Filter");
    m_CannyFilter->Update();
    m_CannyEdgesThresholdFilter->Update();
    }
  catch( itk::ExceptionObject & exp )
    {
    fl_alert( exp.GetDescription() );
    }

}





/************************************
 *
 *  Stop Segmentation
 *
 ***********************************/
void
CannySegmentationLevelSetBase
::SetZeroSetValue( InternalPixelType value )
{
  // By starting the FastMarching front at this value,
  // the zero set will end up being placed at distance
  // = value from the seeds. That can be seen as computing
  // a distance map from the seeds.
  m_SeedValue = - value;

  NodeContainer::Iterator nodeItr = m_TrialPoints->Begin();
  NodeContainer::Iterator endNode = m_TrialPoints->End();
  while( nodeItr != endNode )
    {
    nodeItr.Value().SetValue( m_SeedValue );
    ++nodeItr;
    }
  m_FastMarchingFilter->Modified();
}



/************************************
 *
 *  Stop Segmentation
 *
 ***********************************/
void
CannySegmentationLevelSetBase
::Stop( void )
{
  // TODO: add a Stop() method to Filters

}



