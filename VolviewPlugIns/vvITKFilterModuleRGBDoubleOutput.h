/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _vvITKFilterModuleRGBDoubleOutput_h
#define _vvITKFilterModuleRGBDoubleOutput_h

#include "vvITKFilterModule.h"
#include "itkRGBPixel.h"
#include "itkImportImageFilter.h"


namespace VolView
{

namespace PlugIn
{

template <class TFilterType >
class FilterModuleRGBDoubleOutput : public FilterModuleBase {

public:

  typedef FilterModuleBase                        Superclass;

   // Instantiate the image types
  typedef TFilterType         FilterType;
  typedef typename FilterType::InputImageType     InputImageType;
  typedef typename FilterType::OutputImageType    OutputImageType;
  typedef typename InputImageType::PixelType      InputPixelType;
  typedef unsigned char                           OutputPixelType;

  itkStaticConstMacro( Dimension, unsigned int, 
         itk::GetImageDimension< InputImageType >::ImageDimension );
  
  // Instantiate the ImportImageFilter
  // This filter is used for building an ITK image using 
  // the data passed in a buffer.
  typedef itk::ImportImageFilter< InputPixelType, 
                                  Dimension       > ImportFilterType;

  typedef typename InputImageType::SizeType           SizeType;
  typedef typename InputImageType::IndexType          IndexType;
  typedef typename InputImageType::RegionType         RegionType;



public:

  /**  Constructor */
  FilterModuleRGBDoubleOutput() 
    {
    m_ProduceDoubleOutput = false;
    m_ImportFilter       = ImportFilterType::New();
    m_Filter             = FilterType::New();
    m_Filter->SetInput( m_ImportFilter->GetOutput() );

    // Set the Observer for updating progress in the GUI
    m_Filter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
    m_Filter->AddObserver( itk::StartEvent(), this->GetCommandObserver() );
    m_Filter->AddObserver( itk::EndEvent(), this->GetCommandObserver() );
    }


  /**  Destructor */
  virtual ~FilterModuleRGBDoubleOutput() 
    {
    }

  /** Enable/Disable the production of the second output */
  void SetProduceDoubleOutput( bool value )
    {
    m_ProduceDoubleOutput = value;
    }
  

  /** Return  a pointer to the internal filter */
  FilterType * GetFilter()
  {
    return m_Filter.GetPointer();
  }



  /**  Copy the result of the processing to the output */
  virtual void CopyOutputData( const vtkVVProcessDataStruct * pds )
    {


    // Copy the data (with casting) to the output buffer provided by the PlugIn API
    typename OutputImageType::ConstPointer outputImage =
                                               this->GetFilter()->GetOutput();

    typedef itk::ImageRegionConstIterator< OutputImageType >  OutputIteratorType;

    OutputIteratorType ot( outputImage, outputImage->GetBufferedRegion() );

   typename InputImageType::ConstPointer inputImage =
                                               this->GetFilter()->GetInput();

    typedef itk::ImageRegionConstIterator< InputImageType >  InputIteratorType;

    InputIteratorType it( inputImage, inputImage->GetBufferedRegion() );


    OutputPixelType * outData = (OutputPixelType *)(pds->outData);

    ot.GoToBegin(); 

    if( !m_ProduceDoubleOutput )
      {
      while( !ot.IsAtEnd() )
        {
        *outData = ot.Get();  // copy output pixel
        ++outData;
        ++ot;
        }

      }
    else
      {
      while( !ot.IsAtEnd() )
        {
        *outData = static_cast< OutputPixelType >( it.Get().GetRed() );  // copy input pixel
        ++outData;
        *outData = static_cast< OutputPixelType >( it.Get().GetGreen() );  // copy input pixel
        ++outData;
        *outData = static_cast< OutputPixelType >( it.Get().GetBlue() );  // copy input pixel
        ++outData;
        *outData = ot.Get();  // copy output pixel
        ++outData;
        ++ot;
        ++it;
        }
    }

    }



  /**  ProcessData performs the actual filtering on the data */
  virtual void 
  ProcessData( const vtkVVProcessDataStruct * pds )
  {

    this->InitializeProgressValue();
    this->SetCurrentFilterProgressWeight( 1.0 );

    this->ImportPixelBuffer( pds );

     // Execute the filter
    try
      {
      m_Filter->Update();
      }
    catch( itk::ProcessAborted & )
      {
      return;
      }

    this->CopyOutputData( pds );

  }



  virtual void 
  ImportPixelBuffer( const vtkVVProcessDataStruct * pds )
  {

    SizeType   size;
    IndexType  start;

    double     origin[3];
    double     spacing[3];

    size[0]     =  this->GetPluginInfo()->InputVolumeDimensions[0];
    size[1]     =  this->GetPluginInfo()->InputVolumeDimensions[1];
    size[2]     =  pds->NumberOfSlicesToProcess;

    for(unsigned int i=0; i<3; i++)
      {
      origin[i]   =  this->GetPluginInfo()->InputVolumeOrigin[i];
      spacing[i]  =  this->GetPluginInfo()->InputVolumeSpacing[i];
      start[i]    =  0;
      }

    RegionType region;

    region.SetIndex( start );
    region.SetSize(  size  );
   
    m_ImportFilter->SetSpacing( spacing );
    m_ImportFilter->SetOrigin(  origin  );
    m_ImportFilter->SetRegion(  region  );

    const unsigned int totalNumberOfPixels = region.GetNumberOfPixels();



    const unsigned int numberOfPixelsPerSlice = size[0] * size[1];

    const bool         importFilterWillDeleteTheInputBuffer = false;

    InputPixelType *   dataBlockStart = 
                          static_cast< InputPixelType * >( pds->inData )  
                        + numberOfPixelsPerSlice * pds->StartSlice;

    m_ImportFilter->SetImportPointer( dataBlockStart, 
                                      totalNumberOfPixels,
                                      importFilterWillDeleteTheInputBuffer );

  } // end of ImportPixelBuffer

private:

  typename FilterType::Pointer          m_Filter;
  typename ImportFilterType::Pointer    m_ImportFilter;
  bool m_ProduceDoubleOutput;

};


} // end namespace PlugIn

} // end namespace VolView

#endif
