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

    if( !m_ProduceDoubleOutput )
      {
      this->Superclass::CopyOutputData( pds );
      return;
      }

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



  /**  ProcessData performs the actual filtering on the data */
  virtual void 
  ProcessData( const vtkVVProcessDataStruct * pds )
  {
    this->Superclass::ProcessData( pds );
  }


private:

  bool m_ProduceDoubleOutput;

};


} // end namespace PlugIn

} // end namespace VolView

#endif
