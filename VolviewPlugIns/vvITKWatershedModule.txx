/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVWatershedModule_txx
#define _itkVVWatershedModule_txx

#include "vvITKWatershedModule.h"
#include <set>


namespace VolView 
{

namespace PlugIn
{

/*
 *    Constructor
 */
template <class TInputPixelType >
WatershedModule<TInputPixelType>
::WatershedModule()
{
    m_ImportFilter               = ImportFilterType::New();
    m_GradientMagnitudeFilter    = GradientMagnitudeFilterType::New();
    m_WatershedFilter            = WatershedFilterType::New();

    m_PerformPostprocessing   = true;


    // Set up the pipeline
    m_GradientMagnitudeFilter->SetInput(  m_ImportFilter->GetOutput() );
    m_GradientMagnitudeFilter->SetNormalizeAcrossScale( true );

    m_WatershedFilter->SetInput( m_GradientMagnitudeFilter->GetOutput() );

    // Allow progressive release of memory as the pipeline is executed
    m_GradientMagnitudeFilter->ReleaseDataFlagOn();
    if( m_PerformPostprocessing )
      {
      m_WatershedFilter->ReleaseDataFlagOn();
      }
}


/*
 *    Destructor
 */
template <class TInputPixelType >
WatershedModule<TInputPixelType>
::~WatershedModule()
{
 
}



/*
 *    Add a seed point to the node container.
 *    There is a node per seed.
 */
template <class TInputPixelType >
void 
WatershedModule<TInputPixelType>
::AddSeed( const IndexType & seedPosition )
{
  m_Seeds.push_back( seedPosition );
}





/*
 *  Set the Sigma value for the Gradient Magnitude filter
 */
template <class TInputPixelType >
void 
WatershedModule<TInputPixelType>
::SetSigma( float value )
{
  m_GradientMagnitudeFilter->SetSigma( value );
}




/*
 *  Set the lowest value of the basin to be segmented
 */
template <class TInputPixelType >
void 
WatershedModule<TInputPixelType>
::SetThreshold( float value )
{
  m_WatershedFilter->SetThreshold( value );
}




/*
 *  Set the lowest value of the basin border to be segmented
 */
template <class TInputPixelType >
void 
WatershedModule<TInputPixelType>
::SetWaterLevel( float value )
{
  m_WatershedFilter->SetLevel( value );
}




/*
 *  Set the boolean flag controlling whether 
 *  post-processing will be performed or not.
 */
template <class TInputPixelType >
void 
WatershedModule<TInputPixelType>
::SetPerformPostProcessing( bool value )
{
  m_PerformPostprocessing = value;
  if( m_PerformPostprocessing )
    {
    m_WatershedFilter->ReleaseDataFlagOn();
    }
  else 
    {
    m_WatershedFilter->ReleaseDataFlagOff();
    }
}



/*
 *  Performs the actual filtering on the data 
 */
template <class TInputPixelType >
void 
WatershedModule<TInputPixelType>
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

  // Set the Observer for updating progress in the GUI
  m_GradientMagnitudeFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
  m_WatershedFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );

  // Execute the filters and progressively remove temporary memory
  m_GradientMagnitudeFilter->Update();
  m_WatershedFilter->Update();

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
WatershedModule<TInputPixelType>
::PostProcessData( const vtkVVProcessDataStruct * pds )
{

  // Copy the data (with casting) to the output buffer provided by the Plug In API
  typedef typename WatershedFilterType::OutputImageType   LabeledImageType;
  typedef typename LabeledImageType::PixelType            LabelType;

  typename LabeledImageType::ConstPointer outputImage =
                               m_WatershedFilter->GetOutput();


  // Collect the label values associated with all the seed points.
  typedef std::set<unsigned long>   LabelsTable;
  LabelsTable  labels;

  SeedsContainerType::const_iterator seed = m_Seeds.begin();
  SeedsContainerType::const_iterator last = m_Seeds.end();
  std::ofstream ofs("seedlabels.txt");
  while( seed != last )
    {
    labels.insert( outputImage->GetPixel( *seed ) );
    ofs << outputImage->GetPixel( *seed ) << std::endl;
    ++seed;
    }
  ofs << "size = " << labels.size() << std::endl;  
  ofs.close();
  

  typedef itk::ImageRegionConstIterator< LabeledImageType >  OutputIteratorType;

  OutputIteratorType ot( outputImage, outputImage->GetBufferedRegion() );

  OutputPixelType * outData = (OutputPixelType *)(pds->outData);

  LabelsTable::const_iterator firstLabel = labels.begin();
  LabelsTable::const_iterator lastLabel  = labels.end();
  LabelsTable::const_iterator label;

  bool found = false;

  ot.GoToBegin(); 
  while( !ot.IsAtEnd() )
    {
    const OutputPixelType labeledPixel =  ot.Get();

    label = firstLabel;
    found = false;
    while( label != lastLabel )
      {
      if( *label == labeledPixel )
        {
        found = true;
        break;
        }
      ++label;
      }

    if( found )
      { 
      *outData = 0;
      }
    else
      {
      *outData = 255;
      }
    
    ++ot;
    ++outData;
    }

} // end of PostProcessData


} // end of namespace PlugIn

} // end of namespace Volview

#endif
