/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface. 
    This module factorize the functionality of filters requiring two inputs */

#ifndef _vvITKFilterModuleTwoInputs_h
#define _vvITKFilterModuleTwoInputs_h

#include "vvITKFilterModule.h"

#include <string.h>
#include <stdlib.h>

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkImageRegionConstIterator.h"


namespace VolView
{

namespace PlugIn
{

//
// The types of the input images are passed as additional 
// template parameters. This may seem redundant since the
// filter type alreay has this information in the form of 
// internal traits. However the lack of standard typenames
// for the two input images makes difficult to extract such
// traits from the filter type alone.
//
// The two input images must be 3D and are expected to have
// the same extent and spacing.
// 
template <class TFilterType,class TInputImage1, class TInputImage2 >
class FilterModuleTwoInputs : public FilterModule<TFilterType> {

public:

  typedef FilterModule<TFilterType>  Superclass;

   // Instantiate the image types
  typedef TFilterType         FilterType;
  typedef TInputImage1        Input1ImageType;
  typedef TInputImage2        Input2ImageType;

  typedef typename FilterType::OutputImageType    OutputImageType;

  typedef typename Input1ImageType::PixelType     Input1PixelType;
  typedef typename Input2ImageType::PixelType     Input2PixelType;
  typedef typename OutputImageType::PixelType     OutputPixelType;

  itkStaticConstMacro( Dimension, unsigned int, 
         itk::GetImageDimension< Input1ImageType >::ImageDimension );

  // Instantiate the ImportImageFilter
  // This filter is used for building an ITK image using 
  // the data passed in a buffer.
  // Note that the importer for the first image type is inherited
  // from the base class FilterModule. We only add the importer for
  // the second input image here.
  typedef itk::ImportImage2Filter< Input2PixelType, 
                                   Dimension       > ImportFilter2Type;

  typedef typename Superclass::SizeType      SizeType;
  typedef typename Superclass::IndexType     IndexType;
  typedef typename Superclass::RegionType    RegionType;



public:

  /**  Constructor */
  FilterModuleTwoInputs() 
    {
    m_ImportFilter2      = ImportFilter2Type::New();
    }



  /**  Destructor */
  ~FilterModule() 
    {
    }


  /**  ProcessData performs the actual filtering on the data.
       In this class, this method only initialize the import
       filter for the second input, then it lets the ProcessData
       method of the base class perform the rest of the operations. */
  void 
  ProcessData( const vtkVVProcessDataStruct * pds )
  {

    // Configure the import filter for the second input
    SizeType   size;
    IndexType  start;

    double     origin[3];
    double     spacing[3];

    size[0]     =  this->GetPluginInfo()->InputVolume2Dimensions[0];
    size[1]     =  this->GetPluginInfo()->InputVolume2Dimensions[1];
    size[2]     =  pds->NumberOfSlicesToProcess;

    for(unsigned int i=0; i<3; i++)
      {
      origin[i]   =  this->GetPluginInfo()->InputVolume2Origin[i];
      spacing[i]  =  this->GetPluginInfo()->InputVolume2Spacing[i];
      start[i]    =  0;
      }

    RegionType region;

    region.SetIndex( start );
    region.SetSize(  size  );
   
    m_ImportFilter2->SetSpacing( spacing );
    m_ImportFilter2->SetOrigin(  origin  );
    m_ImportFilter2->SetRegion(  region  );

    const unsigned int totalNumberOfPixels = region.GetNumberOfPixels();

    const bool         importFilterWillDeleteTheInputBuffer = false;

    const unsigned int numberOfPixelsPerSlice = size[0] * size[1];

    Input2PixelType * dataBlockStart = 
                          static_cast< Input2PixelType * >( pds->inData )  
                        + numberOfPixelsPerSlice * pds->StartSlice;

    m_ImportFilter2->SetImportPointer( dataBlockStart, 
                                       totalNumberOfPixels,
                                       importFilterWillDeleteTheInputBuffer );

    // Now let the base class do the rest.
    this->Superclass::ProcessData( pds );
    
   
  } // end of ProcessData



private:
    typename ImportFilter2Type::Pointer    m_ImportFilter2;
};


} // end namespace PlugIn

} // end namespace VolView

#endif
