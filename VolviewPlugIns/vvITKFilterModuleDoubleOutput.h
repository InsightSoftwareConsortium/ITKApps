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

   // Instantiate the image types
  typedef TFilterType                             FilterType;
  typedef typename FilterType::InputImageType     InputImageType;
  typedef typename FilterType::OutputImageType    OutputImageType;
  typedef typename InputImageType::PixelType      InputPixelType;
  typedef typename OutputImageType::PixelType     OutputPixelType;

  itkStaticConstMacro( Dimension, unsigned int, 
         itk::GetImageDimension< InputImageType >::ImageDimension );

  // Instantiate the ImportImageFilter
  // This filter is used for building an ITK image using 
  // the data passed in a buffer.
  typedef itk::ImportImageFilter< InputPixelType, 
                                  Dimension       > ImportFilterType;

  typedef typename ImportFilterType::SizeType      SizeType;
  typedef typename ImportFilterType::IndexType     IndexType;
  typedef typename ImportFilterType::RegionType    RegionType;



public:

  /**  Constructor */
  FilterModuleDoubleOutput() 
    {
    }



  /**  Destructor */
  virtual ~FilterModuleDoubleOutput() 
    {
    }


  /**  Copy the result of the processing to the output */
  virtual void CopyOutputData( const vtkVVProcessDataStruct * pds )
    {
    // Copy the data (with casting) to the output buffer provided by the PlugIn API
    typename OutputImageType::ConstPointer outputImage =
                                               m_Filter->GetOutput();

    typedef itk::ImageRegionConstIterator< OutputImageType >  OutputIteratorType;

    OutputIteratorType ot( outputImage, outputImage->GetBufferedRegion() );

   typename InputImageType::ConstPointer inputImage =
                                               m_Filter->GetInput();

    typedef itk::ImageRegionConstIterator< InputImageType >  InputIteratorType;

    InputIteratorType it( inputImage, inputImage->GetBufferedRegion() );


    OutputPixelType * outData = (OutputPixelType *)(pds->outData);

    ot.GoToBegin(); 
    while( !ot.IsAtEnd() )
      {
      *outData = it.Get();  // copy input pixel
      ++outData;
      *outData = ot.Get();  // copy output pixel
      ++outData;
      ++ot;
      ++it;
      }
    }

private:
    typename ImportFilterType::Pointer    m_ImportFilter;
    typename FilterType::Pointer          m_Filter;
};


} // end namespace PlugIn

} // end namespace VolView

#endif
