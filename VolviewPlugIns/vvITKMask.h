/** This module implements Masking by setting to zero in the current image all
 * the pixels that are non-zero in a second image provided as mask.  */
    

#ifndef _vvITKMask_h
#define _vvITKMask_h

#include "vvITKFilterModuleTwoInputs.h"
#include "itkMaskImageFilter.h"


namespace VolView
{

namespace PlugIn
{

template< class TInputImageType, class TMaskImageType>
class Mask : public 
                   FilterModuleTwoInputs<
                              itk::MaskImageFilter<
                                    TInputImageType,
                                    TMaskImageType,
                                    TInputImageType>,
                              TInputImageType,
                              TMaskImageType > {

public:

  typedef  itk::MaskImageFilter<
                      TInputImageType,
                      TMaskImageType,
                      TInputImageType> MaskImageFilterType;

  typedef  FilterModuleTwoInputs<
                      MaskImageFilterType,
                      TInputImageType,
                      TMaskImageType > Superclass;

  typedef typename MaskImageFilterType::OutputImageType  OutputImageType;

public:

  /**  Constructor */
  Mask() 
    {
    }



  /**  Destructor */
  virtual ~Mask() 
    {
    }


  /**  ProcessData performs the actual filtering on the data.
       In this class, this method only initialize the import
       filter for the second input, then it lets the ProcessData
       method of the base class perform the rest of the operations. */
  void 
  ProcessData( const vtkVVProcessDataStruct * pds )
  {
    // Let superclass perform initial connections
    this->Superclass::ProcessData( pds );

    MaskImageFilterType * maskingFilter = this->GetFilter();

    vtkVVPluginInfo *info = this->GetPluginInfo();

    maskingFilter->SetInput1(  this->GetInput1()  );
    maskingFilter->SetInput2(  this->GetInput2()  );

    // Execute the filter
    try
      {
      maskingFilter->Update();
      }
    catch( itk::ProcessAborted & )
      {
      return;
      }

    // Copy the data (with casting) to the output buffer provided by the PlugIn API
    typename OutputImageType::ConstPointer outputImage = maskingFilter->GetOutput();

    typedef itk::ImageRegionConstIterator< OutputImageType >  OutputIteratorType;

    OutputIteratorType ot( outputImage, outputImage->GetBufferedRegion() );

    typedef typename OutputImageType::PixelType OutputPixelType;
    OutputPixelType * outData = (OutputPixelType *)(pds->outData);

    ot.GoToBegin(); 
    while( !ot.IsAtEnd() )
      {
      *outData = ot.Get();
      ++ot;
      ++outData;
      }

  } // end of ProcessData



private:


};


} // end namespace PlugIn

} // end namespace VolView

#endif
