/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _vvITKFilterModuleDoubleOutput_h
#define _vvITKFilterModuleDoubleOutput_h

#include "vvITKFilterModule.h"



namespace VolView
{

namespace PlugIn
{

template <class TFilterType >
class FilterModuleDoubleOutput : public FilterModule<TFilterType> {

public:

  typedef FilterModule<TFilterType>               Superclass;

   // Instantiate the image types
  typedef typename Superclass::FilterType         FilterType;
  typedef typename Superclass::InputImageType     InputImageType;
  typedef typename Superclass::OutputImageType    OutputImageType;
  typedef typename Superclass::InputPixelType     InputPixelType;
  typedef typename Superclass::OutputPixelType    OutputPixelType;

  itkStaticConstMacro( Dimension, unsigned int, 
         itk::GetImageDimension< InputImageType >::ImageDimension );

  typedef typename Superclass::ImportFilterType   ImportFilterType;
  typedef typename Superclass::SizeType           SizeType;
  typedef typename Superclass::IndexType          IndexType;
  typedef typename Superclass::RegionType         RegionType;



public:

  /**  Constructor */
  FilterModuleDoubleOutput() 
    {
    m_ProduceDoubleOutput = false;
    }


  /**  Destructor */
  virtual ~FilterModuleDoubleOutput() 
    {
    }

  /** Enable/Disable the production of the second output */
  void SetProduceDoubleOutput( bool value )
    {
    m_ProduceDoubleOutput = value;
    }




  /**  Copy the result of the processing to the output */
  virtual void CopyOutputData( const vtkVVProcessDataStruct * pds )
    {
    
    // Copy the data (with casting) to the output buffer provided by the PlugIn API
    typename OutputImageType::ConstPointer outputImage =
                                               this->GetFilter()->GetOutput();

    typedef itk::ImageRegionConstIterator< OutputImageType >  OutputIteratorType;

    OutputIteratorType ot( outputImage, outputImage->GetBufferedRegion() );
    ot.GoToBegin(); 


    if( m_ProduceDoubleOutput )
      {
      typename InputImageType::ConstPointer inputImage =
                                                 this->GetFilter()->GetInput();

      typedef itk::ImageRegionConstIterator< InputImageType >  InputIteratorType;

      InputIteratorType it( inputImage, inputImage->GetBufferedRegion() );
      it.GoToBegin();

      // When producing composite output, the output image must have the same
      // type as the input image. Therefore, we use InputPixelType instead of
      // OutputPixelType.
      InputPixelType * outData = (InputPixelType *)(pds->outData);

      while( !ot.IsAtEnd() )
        {
        *outData = static_cast< OutputPixelType >( it.Get() );  // copy input pixel
        ++outData;
        *outData = ot.Get();  // copy output pixel
        ++outData;
        ++ot;
        ++it;
        }
      }
    else
      {
      OutputPixelType * outData = (OutputPixelType *)(pds->outData);

      while( !ot.IsAtEnd() )
        {
        *outData = ot.Get();  // copy output pixel
        ++outData;
        ++ot;
        }

      }
    }



  /**  ProcessData performs the actual filtering on the data */
  virtual void 
  ProcessData( const vtkVVProcessDataStruct * pds )
  {
    this->InitializeProgressValue();
    this->SetCurrentFilterProgressWeight( 1.0 );

    const unsigned int numberOfComponents = this->GetPluginInfo()->InputVolumeNumberOfComponents;

    if( numberOfComponents != 1 )
      {
      itk::ExceptionObject excp;
      excp.SetDescription("This filter is intendended to be used with single-componente data only");
      throw excp;
      }

    this->ImportPixelBuffer( 0, pds );

    if( !m_ProduceDoubleOutput )
      {
      this->ExportPixelBuffer( 0, pds );
      }

    // Execute the filter
    try
      {
      this->GetFilter()->Update();
      }
    catch( itk::ProcessAborted & )
      {
      return;
      }

    this->CopyOutputData( pds );

  }


private:

  bool m_ProduceDoubleOutput;

};


} // end namespace PlugIn

} // end namespace VolView

#endif
