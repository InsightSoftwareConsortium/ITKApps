/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVWatershedRGBModule_txx
#define _itkVVWatershedRGBModule_txx

#include "vvITKWatershedRGBModule.h"
#include <set>

#include <fstream>


namespace VolView 
{

namespace PlugIn
{

/*
 *    Constructor
 */
template <class TInputPixelType >
WatershedRGBModule<TInputPixelType>
::WatershedRGBModule()
{
    m_ImportFilter               = ImportFilterType::New();
    m_CastFilter                 = CastFilterType::New();
    m_WatershedFilter            = WatershedFilterType::New();
    m_ColorEncoder               = ColorMapFilterType::New();

    m_CastFilter->SetInput( m_ImportFilter->GetOutput() );
    m_WatershedFilter->SetInput( m_CastFilter->GetOutput() );
    m_ColorEncoder->SetInput( m_WatershedFilter->GetOutput() );

    // Allow progressive release of memory as the pipeline is executed
    m_CastFilter->ReleaseDataFlagOn();
    m_WatershedFilter->ReleaseDataFlagOn();
}


/*
 *    Destructor
 */
template <class TInputPixelType >
WatershedRGBModule<TInputPixelType>
::~WatershedRGBModule()
{
 
}




/*
 *  Set the lowest value of the basin border to be segmented
 */
template <class TInputPixelType >
void 
WatershedRGBModule<TInputPixelType>
::SetWaterLevel( float value )
{
  m_WatershedFilter->SetLevel( value );
  m_WatershedFilter->SetThreshold( value / 10.0 );
}




/*
 *  Performs the actual filtering on the data 
 */
template <class TInputPixelType >
void 
WatershedRGBModule<TInputPixelType>
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
  m_CastFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
  m_WatershedFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
  m_ColorEncoder->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );

  m_CastFilter->AddObserver( itk::StartEvent(), this->GetCommandObserver() );
  m_WatershedFilter->AddObserver( itk::StartEvent(), this->GetCommandObserver() );
  m_ColorEncoder->AddObserver( itk::StartEvent(), this->GetCommandObserver() );

  m_CastFilter->AddObserver( itk::EndEvent(), this->GetCommandObserver() );
  m_WatershedFilter->AddObserver( itk::EndEvent(), this->GetCommandObserver() );
  m_ColorEncoder->AddObserver( itk::EndEvent(), this->GetCommandObserver() );

  // Execute the filters and progressively remove temporary memory
  this->SetCurrentFilterProgressWeight( 0.1 );
  this->SetUpdateMessage("Preprocessing with casting filter...");
  m_CastFilter->Update();

  this->SetCurrentFilterProgressWeight( 0.8 );
  this->SetUpdateMessage("Computing watersheds...");
  m_WatershedFilter->Update();

  this->SetCurrentFilterProgressWeight( 0.1 );
  this->SetUpdateMessage("Postprocessing for color coding...");
  m_ColorEncoder->Update();

  this->CopyOutputData( pds );

} // end of ProcessData



/*
 *  Performs post-processing of data. 
 *  This involves an intensity window operation and
 *  data copying into the volview provided buffer.
 */
template <class TInputPixelType >
void 
WatershedRGBModule<TInputPixelType>
::CopyOutputData( const vtkVVProcessDataStruct * pds )
{

  this->SetUpdateMessage("Copying output data ...");

  // Copy the data (with casting) to the output buffer provided by the Plug In API
  typename RGBImageType::ConstPointer outputImage = m_ColorEncoder->GetOutput();


    typedef itk::ImageRegionConstIterator< RGBImageType >  OutputIteratorType;

    OutputIteratorType ot( outputImage, outputImage->GetBufferedRegion() );

    OutputPixelType * outData = (OutputPixelType *)(pds->outData);

    ot.GoToBegin(); 

    while( !ot.IsAtEnd() )
      {
      const RGBPixelType pixel = ot.Get();
      *outData = static_cast< OutputPixelType >( pixel.GetRed() );  // copy input pixel
      ++outData;
      *outData = static_cast< OutputPixelType >( pixel.GetGreen() );  // copy input pixel
      ++outData;
      *outData = static_cast< OutputPixelType >( pixel.GetBlue() );  // copy input pixel
      ++outData;
      ++ot;
      }

} // end of CopyOutputData


} // end of namespace PlugIn

} // end of namespace Volview

#endif
