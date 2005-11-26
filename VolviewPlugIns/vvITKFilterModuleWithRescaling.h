/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVFilterModuleWithRescaling_h
#define _itkVVFilterModuleWithRescaling_h

#include "vvITKFilterModuleBase.h"

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageRegionConstIterator.h"

namespace VolView 
{

namespace PlugIn
{

template <class TFilterType, class TFinalPixelType >
class FilterModuleWithRescaling : public FilterModuleBase {

public:

   // Instantiate the image types
  typedef FilterModuleWithRescaling               Self;
  typedef TFilterType                             FilterType;

  typedef typename FilterType::InputImageType     InputImageType;
  typedef typename FilterType::OutputImageType    InternalImageType;
  typedef typename InputImageType::PixelType      InputPixelType;

  typedef TFinalPixelType                         FinalPixelType;

  itkStaticConstMacro( Dimension, unsigned int, 
         itk::GetImageDimension< InternalImageType >::ImageDimension );

  typedef itk::Image< FinalPixelType, 
            itkGetStaticConstMacro(Dimension) >   FinalImageType;

  // Instantiate the ImportImageFilter
  // This filter is used for building an ITK image using 
  // the data passed in a buffer.
  typedef itk::ImportImageFilter< InputPixelType, 
          itkGetStaticConstMacro(Dimension)   >   ImportFilterType;

  typedef typename ImportFilterType::SizeType      SizeType;
  typedef typename ImportFilterType::IndexType     IndexType;
  typedef typename ImportFilterType::RegionType    RegionType;

  // Instantiate the RescaleIntensityImageFilter
  // This filter is used for converting the pixel type from the input
  // data buffer into the input pixel type of the filter.
  typedef itk::RescaleIntensityImageFilter< 
                                        InternalImageType, 
                                        FinalImageType 
                                                        > RescaleFilterType;


public:

  /**  Constructor */
  FilterModuleWithRescaling() 
    {
    m_ImportFilter       = ImportFilterType::New();
    m_Filter             = FilterType::New();
    m_RescaleFilter      = RescaleFilterType::New();

    m_Filter->SetInput( m_ImportFilter->GetOutput() );
    m_RescaleFilter->SetInput( m_Filter->GetOutput() );

    // Set the Observer for updating progress in the GUI
    m_Filter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
    m_Filter->AddObserver( itk::StartEvent(), this->GetCommandObserver() );
    m_Filter->AddObserver( itk::EndEvent(), this->GetCommandObserver() );

    m_RescaleFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
    m_RescaleFilter->AddObserver( itk::StartEvent(), this->GetCommandObserver() );
    m_RescaleFilter->AddObserver( itk::EndEvent(), this->GetCommandObserver() );


    }



  /**  Destructor */
  virtual ~FilterModuleWithRescaling() 
    {
    }

  void SetOutputMinimum( FinalPixelType value )
  {
    m_RescaleFilter->SetOutputMinimum( value );
  }

  void SetOutputMaximum( FinalPixelType value )
  {
    m_RescaleFilter->SetOutputMaximum( value );
  }

  FilterType * GetFilter()
  {
    return m_Filter.GetPointer();
  }


  /**  ProcessData performs the actual filtering on the data */
  virtual void 
  ProcessData( const vtkVVProcessDataStruct * pds )
  {

    this->InitializeProgressValue();

    const unsigned int numberOfComponents = this->GetPluginInfo()->InputVolumeNumberOfComponents;

    for(unsigned int component=0; component < numberOfComponents; component++ )
      {


      this->ImportPixelBuffer( component, pds );


      // Execute the filter
      try
        {
        this->SetCurrentFilterProgressWeight( 0.9 );
        m_Filter->Update();
        this->SetCurrentFilterProgressWeight( 0.1 );
        m_RescaleFilter->Update();
        }
      catch( itk::ProcessAborted &  )
        {
        return;
        }

      this->CopyOutputData( component, pds );

      }

  } // end of ProcessData



  /**  Copy the output data into the volview data structure */
  virtual void 
  CopyOutputData( unsigned int component, const vtkVVProcessDataStruct * pds )
  {

    // Copy the data (with casting) to the output buffer provided by the PlugIn API
    typename FinalImageType::ConstPointer outputImage = m_RescaleFilter->GetOutput();

    const unsigned int numberOfComponents = this->GetPluginInfo()->InputVolumeNumberOfComponents;

    typedef itk::ImageRegionConstIterator< FinalImageType >  OutputIteratorType;

    OutputIteratorType ot( outputImage, outputImage->GetBufferedRegion() );

    FinalPixelType * outData = static_cast< FinalPixelType * >( pds->outData );

    outData += component;  // move to the start of the selected component;

    ot.GoToBegin(); 
    while( !ot.IsAtEnd() )
      {
      *outData = static_cast< FinalPixelType >( ot.Get() );
      ++ot;
      outData += numberOfComponents;
      }

  } // end of CopyOutputData



  virtual void 
  ImportPixelBuffer( unsigned int component, const vtkVVProcessDataStruct * pds )
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



    const unsigned int numberOfComponents = this->GetPluginInfo()->InputVolumeNumberOfComponents;

    const unsigned int numberOfPixelsPerSlice = size[0] * size[1];

    if( numberOfComponents == 1 )
      {
      const bool         importFilterWillDeleteTheInputBuffer = false;

      InputPixelType *   dataBlockStart = 
                            static_cast< InputPixelType * >( pds->inData )  
                          + numberOfPixelsPerSlice * pds->StartSlice;

      m_ImportFilter->SetImportPointer( dataBlockStart, 
                                        totalNumberOfPixels,
                                        importFilterWillDeleteTheInputBuffer );
      }
    else 
      {
      const bool         importFilterWillDeleteTheInputBuffer = true;
      
      InputPixelType *   extractedComponent = new InputPixelType[ totalNumberOfPixels ];

      InputPixelType *   dataBlockStart = 
                            static_cast< InputPixelType * >( pds->inData )  
                          + numberOfPixelsPerSlice * pds->StartSlice
                          + component;

      InputPixelType *   inputData = dataBlockStart;

      for(unsigned int i=0; i<totalNumberOfPixels; i++, inputData += numberOfComponents )
        {
        extractedComponent[i] =  *inputData;
        }

      m_ImportFilter->SetImportPointer( extractedComponent, 
                                        totalNumberOfPixels,
                                        importFilterWillDeleteTheInputBuffer );
      }

  } // end of ImportPixelBuffer

private:
    typename ImportFilterType::Pointer        m_ImportFilter;
    typename FilterType::Pointer              m_Filter;
    typename RescaleFilterType::Pointer       m_RescaleFilter;

};

} // end of namespace PlugIn

} // end of namespace Volview

#endif
