/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVCannySegmentationLevelSetModule_txx
#define _itkVVCannySegmentationLevelSetModule_txx

#include "vvITKCannySegmentationLevelSetModule.h"

namespace VolView 
{

namespace PlugIn
{

/*
 *    Constructor
 */
template <class TInputPixelType >
CannySegmentationLevelSetModule<TInputPixelType>
::CannySegmentationLevelSetModule()
{
  m_ImportFilter                     = ImportFilterType::New();
  m_CastFilter                       = CastFilterType::New();
  m_FastMarchingImageFilter          = FastMarchingFilterType::New();
  m_CannySegmentationLevelSetFilter  = CannySegmentationLevelSetFilterType::New();
  m_IntensityWindowingFilter         = IntensityWindowingFilterType::New();
  m_NodeContainer                    = NodeContainerType::New();

  m_PerformPostprocessing   = true;

  // Set up the pipeline
  m_CannySegmentationLevelSetFilter->SetInput(         m_FastMarchingImageFilter->GetOutput() );
  m_CastFilter->SetInput(                              m_ImportFilter->GetOutput() );
  m_CannySegmentationLevelSetFilter->SetFeatureImage(  m_CastFilter->GetOutput() );

  m_IntensityWindowingFilter->SetInput(    m_CannySegmentationLevelSetFilter->GetOutput() );

  m_IntensityWindowingFilter->SetOutputMinimum( 255 );
  m_IntensityWindowingFilter->SetOutputMaximum(   0 );

  m_FastMarchingImageFilter->SetSpeedConstant( 1.0 );

  // Allow progressive release of memory as the pipeline is executed
  m_CannySegmentationLevelSetFilter->ReleaseDataFlagOn();

  m_CannySegmentationLevelSetFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
  m_FastMarchingImageFilter->AddObserver(              itk::ProgressEvent(), this->GetCommandObserver() );
  m_IntensityWindowingFilter->AddObserver(        itk::ProgressEvent(), this->GetCommandObserver() );

  m_FastMarchingImageFilter->SetTrialPoints( m_NodeContainer );

  m_InitialSeedValue     = 0.0;
  m_CurrentNumberOfSeeds =   0;

}


/*
 *    Destructor
 */
template <class TInputPixelType >
CannySegmentationLevelSetModule<TInputPixelType>
::~CannySegmentationLevelSetModule()
{
 
}


/*
 *    Add a seed point to the node container.
 *    There is a node per seed.
 */
template <class TInputPixelType >
void 
CannySegmentationLevelSetModule<TInputPixelType>
::AddSeed( float x, float y, float z )
{
  NodeType node;
  typedef typename NodeType::IndexType          IndexType;
  typedef typename IndexType::IndexValueType    IndexValueType;
  IndexType seedPosition;
  seedPosition[0] = static_cast<IndexValueType>( x );
  seedPosition[1] = static_cast<IndexValueType>( y );
  seedPosition[2] = static_cast<IndexValueType>( z );
  node.SetValue( m_InitialSeedValue );
  node.SetIndex( seedPosition );
  m_NodeContainer->InsertElement( m_CurrentNumberOfSeeds, node );
  m_CurrentNumberOfSeeds++;
}







/*
 *  Set the initial value of the seed.
 *  This can be used to generate zero sets at
 *  a certain distance of the set of seeds.
 */
template <class TInputPixelType >
void 
CannySegmentationLevelSetModule<TInputPixelType>
::SetDistanceFromSeeds( float value )
{
  m_FastMarchingImageFilter->SetStoppingValue( 10.0 * value );
  m_InitialSeedValue = -value;
}




/*
 *  Set the variance used by the Canny edge detector
 */
template <class TInputPixelType >
void 
CannySegmentationLevelSetModule<TInputPixelType>
::SetVariance( float value )
{
  m_CannySegmentationLevelSetFilter->SetVariance( value );
}



/*
 *  Set the threshold for the Canny edge detector
 */
template <class TInputPixelType >
void 
CannySegmentationLevelSetModule<TInputPixelType>
::SetThreshold( float value )
{
  m_CannySegmentationLevelSetFilter->SetThreshold( value );
}



/*
 *  Set the Advection scaling for the 
 *  CannySegmentationLevelSetLevelSetImageFilter.
 */
template <class TInputPixelType >
void 
CannySegmentationLevelSetModule<TInputPixelType>
::SetAdvectionScaling( float value )
{
  m_CannySegmentationLevelSetFilter->SetAdvectionScaling( value );
}




/*
 *  Set the factor that will scale curvature
 */
template <class TInputPixelType >
void 
CannySegmentationLevelSetModule<TInputPixelType>
::SetCurvatureScaling( float value )
{
  m_CannySegmentationLevelSetFilter->SetCurvatureScaling( value );
}


/*
 *  Set the factor that will scale curvature
 */
template <class TInputPixelType >
void 
CannySegmentationLevelSetModule<TInputPixelType>
::SetPropagationScaling( float value )
{
  m_CannySegmentationLevelSetFilter->SetPropagationScaling( value );
}


/*
 *  Set the threshold of RMS error that will define convergence
 */
template <class TInputPixelType >
void 
CannySegmentationLevelSetModule<TInputPixelType>
::SetMaximumRMSError( float value )
{
  m_CannySegmentationLevelSetFilter->SetMaximumRMSError( value );
}


/*
 *  Set the maximum number of iterations to attempt in order to get
 *  convergence.
 */
template <class TInputPixelType >
void 
CannySegmentationLevelSetModule<TInputPixelType>
::SetMaximumIterations( unsigned int value )
{
  m_CannySegmentationLevelSetFilter->SetMaximumIterations( value );
}



/*
 *  Returns the level set resulting 
 *  from the CannySegmentationLevelSet filter.
 */
template <class TInputPixelType >
const CannySegmentationLevelSetModule<TInputPixelType>::RealImageType *
CannySegmentationLevelSetModule<TInputPixelType>
::GetLevelSet()
{
   return m_CannySegmentationLevelSetFilter->GetOutput();
}




/*
 *  Performs the actual filtering on the data 
 */
template <class TInputPixelType >
void 
CannySegmentationLevelSetModule<TInputPixelType>
::ProcessData( const vtkVVProcessDataStruct * pds )
{

  SizeType   size;
  IndexType  start;

  double     origin[3];
  double     spacing[3];

  const vtkVVPluginInfo * info = this->GetPluginInfo();

  size[0]     =  info->InputVolumeDimensions[0];
  size[1]     =  info->InputVolumeDimensions[1];
  size[2]     =  info->InputVolumeDimensions[2];

  m_FastMarchingImageFilter->SetOutputSize( size );

  for(unsigned int i=0; i<3; i++)
    {
    origin[i]   =  info->InputVolumeOrigin[i];
    spacing[i]  =  info->InputVolumeSpacing[i];
    start[i]    =  0;
    }

  RegionType region;

  region.SetIndex( start );
  region.SetSize(  size  );
 
  m_ImportFilter->SetSpacing( spacing );
  m_ImportFilter->SetOrigin(  origin  );
  m_ImportFilter->SetRegion(  region  );

  const unsigned int totalNumberOfPixels = region.GetNumberOfPixels();

  const bool         importFilterWillDeleteTheInputBuffer = false;

  const unsigned int numberOfPixelsPerSlice = size[0] * size[1];

  InputPixelType *   dataBlockStart = 
                        static_cast< InputPixelType * >( pds->inData )  
                      + numberOfPixelsPerSlice * pds->StartSlice;

  m_ImportFilter->SetImportPointer( dataBlockStart, 
                                    totalNumberOfPixels,
                                    importFilterWillDeleteTheInputBuffer );


  // Execute the filters and progressively remove temporary memory
  m_FastMarchingImageFilter->Update();

  m_CannySegmentationLevelSetFilter->Update();

  if( m_PerformPostprocessing )
    {
    this->PostProcessData( pds );
    }

} // end of ProcessData



/*
 *  Performs post-processing of data. 
 *  This involves an intensity window operation and
 *  data copying into the volview provided buffer.
 */
template <class TInputPixelType >
void 
CannySegmentationLevelSetModule<TInputPixelType>
::PostProcessData( const vtkVVProcessDataStruct * pds )
{
 
  typedef itk::MinimumMaximumImageCalculator< 
                                  RealImageType > CalculatorType;
 
  CalculatorType::Pointer calculator = CalculatorType::New();
  calculator->SetImage( m_CannySegmentationLevelSetFilter->GetOutput() );
  calculator->Compute();

  const typename RealImageType::PixelType minimum = calculator->GetMinimum(); 
  const typename RealImageType::PixelType maximum = calculator->GetMaximum(); 
  
  if( minimum < 0.0 && maximum > 0.0 )
    {
    if( -minimum > maximum )
      {
      m_IntensityWindowingFilter->SetWindowMaximum(  maximum );
      m_IntensityWindowingFilter->SetWindowMinimum( -maximum );
      }
    else
      {
      m_IntensityWindowingFilter->SetWindowMaximum( -minimum );
      m_IntensityWindowingFilter->SetWindowMinimum(  minimum );
      }
    }

  m_IntensityWindowingFilter->Update();

  // Copy the data (with casting) to the output buffer provided by the Plug In API
  typename OutputImageType::ConstPointer outputImage =
                               m_IntensityWindowingFilter->GetOutput();

  typedef itk::ImageRegionConstIterator< OutputImageType >  OutputIteratorType;

  OutputIteratorType ot( outputImage, outputImage->GetBufferedRegion() );

  InputPixelType * outData = (InputPixelType *)(pds->outData);

  ot.GoToBegin(); 
  while( !ot.IsAtEnd() )
    {
    *outData = static_cast< InputPixelType >( ot.Get() );
    ++ot;
    ++outData;
    }

} // end of PostProcessData


} // end of namespace PlugIn

} // end of namespace Volview

#endif
