/** 

Generic interface for protocol communication between an ITK filter and the
VolView Plugin Interface.  This module factorize the functionality of filters
requiring two inputs.  Note that the two input image types are not taken from
the filter type traits since some filters requires preprocessing (sometimes
casting) to be applied on the imported images.  Also the lack of standard
typenames for the two input images makes difficult to extract such traits from
the filter type alone. The two input images must be 3D and are expected to have
the same extent and spacing.

*/


#ifndef _vvITKFilterModuleTwoInputs_h
#define _vvITKFilterModuleTwoInputs_h

#include "vvITKFilterModuleBase.h"

#include <string.h>
#include <stdlib.h>

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkImageRegionConstIterator.h"


namespace VolView
{

namespace PlugIn
{

 
template <class TFilterType,class TInputImage1, class TInputImage2 >
class FilterModuleTwoInputs : public FilterModuleBase {

public:

  typedef FilterModuleBase   Superclass;

   // Instantiate the image types
  typedef TFilterType         FilterType;
  typedef TInputImage1        Input1ImageType;
  typedef TInputImage2        Input2ImageType;

  typedef typename Input1ImageType::PixelType     Input1PixelType;
  typedef typename Input2ImageType::PixelType     Input2PixelType;

  itkStaticConstMacro( Dimension, unsigned int, 
         itk::GetImageDimension< Input1ImageType >::ImageDimension );


  // Instantiate the two ImportImageFilters These filters are used for building
  // ITK images using the data passed in a buffer.

  typedef itk::ImportImageFilter< Input1PixelType, 
                                  Dimension       > ImportFilter1Type;

  typedef itk::ImportImageFilter< Input2PixelType, 
                                  Dimension       > ImportFilter2Type;


  typedef typename Superclass::SizeType      SizeType;
  typedef typename Superclass::IndexType     IndexType;
  typedef typename Superclass::RegionType    RegionType;



public:

  /**  Constructor */
  FilterModuleTwoInputs() 
    {
    m_ImportFilter1      = ImportFilter1Type::New();
    m_ImportFilter2      = ImportFilter2Type::New();
    m_Filter             = FilterType::New();

    // Set the Observer for updating progress in the GUI
    m_Filter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
    m_Filter->AddObserver( itk::StartEvent(), this->GetCommandObserver() );
    m_Filter->AddObserver( itk::EndEvent(), this->GetCommandObserver() );
    }



  /**  Destructor */
  virtual ~FilterModuleTwoInputs() 
    {
    }


  /** Give access to the input images */
  const Input1ImageType * GetInput1() 
    {  return m_ImportFilter1->GetOutput(); }

  const Input2ImageType * GetInput2() 
    {  return m_ImportFilter2->GetOutput(); }

  /** Return  a pointer to the internal filter */
  FilterType * GetFilter()
  {
    return m_Filter.GetPointer();
  }


  /**  ProcessData performs the actual filtering on the data.
       In this class, this method only initialize the import
       filter for the second input, then it lets the ProcessData
       method of the base class perform the rest of the operations. */
  virtual void 
  ProcessData( const vtkVVProcessDataStruct * pds )
  {

    vtkVVPluginInfo *info = this->GetPluginInfo();

    this->InitializeProgressValue();
    // Methods must be called by derived classes at the 
    // beginning of their corresponding ProcessData().

    // Configure the import filter for the second input
    SizeType   size;
    IndexType  start;

    double     origin[3];
    double     spacing[3];

    size[0]     =  info->InputVolumeDimensions[0];
    size[1]     =  info->InputVolumeDimensions[1];
    size[2]     =  pds->NumberOfSlicesToProcess;

    for(unsigned int i=0; i<3; i++)
      {
      origin[i]   =  info->InputVolumeOrigin[i];
      spacing[i]  =  info->InputVolumeSpacing[i];
      start[i]    =  0;
      }

    RegionType region;

    region.SetIndex( start );
    region.SetSize(  size  );
   
    m_ImportFilter1->SetSpacing( spacing );
    m_ImportFilter1->SetOrigin(  origin  );
    m_ImportFilter1->SetRegion(  region  );

    const unsigned int totalNumberOfPixels1 = region.GetNumberOfPixels();

    const bool         importFilterWillDeleteTheInputBuffer = false;

    const unsigned int numberOfPixelsPerSlice1 = size[0] * size[1];

    Input1PixelType * dataBlock1Start = 
                          static_cast< Input1PixelType * >( pds->inData )  
                        + numberOfPixelsPerSlice1 * pds->StartSlice;

    m_ImportFilter1->SetImportPointer( dataBlock1Start, 
                                       totalNumberOfPixels1,
                                       importFilterWillDeleteTheInputBuffer );

    m_ImportFilter1->Update();


    size[0]     =  info->InputVolume2Dimensions[0];
    size[1]     =  info->InputVolume2Dimensions[1];
    size[2]     =  pds->NumberOfSlicesToProcess;

    for(unsigned int j=0; j<3; j++)
      {
      origin[j]   =  info->InputVolume2Origin[j];
      spacing[j]  =  info->InputVolume2Spacing[j];
      start[j]    =  0;
      }

    region.SetIndex( start );
    region.SetSize(  size  );

    m_ImportFilter2->SetSpacing( spacing );
    m_ImportFilter2->SetOrigin(  origin  );
    m_ImportFilter2->SetRegion(  region  );

    const unsigned int totalNumberOfPixels2 = region.GetNumberOfPixels();

    const unsigned int numberOfPixelsPerSlice2 = size[0] * size[1];

    Input2PixelType * dataBlock2Start = 
                          static_cast< Input2PixelType * >( pds->inData2 )  
                        + numberOfPixelsPerSlice2 * pds->StartSlice;

    m_ImportFilter2->SetImportPointer( dataBlock2Start, 
                                       totalNumberOfPixels2,
                                       importFilterWillDeleteTheInputBuffer );

    m_ImportFilter2->Update();
    // Note that the filter has not been connected yet to the 
    // importers. This can not be done in this class since in the
    // generic case the filter input types may be different from
    // the types used in the input filters. It is up to the derived 
    // class to connect the pipeline.
   
  } // end of ProcessData



private:
    typename ImportFilter1Type::Pointer    m_ImportFilter1;
    typename ImportFilter2Type::Pointer    m_ImportFilter2;
    typename FilterType::Pointer           m_Filter;
};


} // end namespace PlugIn

} // end namespace VolView

#endif
