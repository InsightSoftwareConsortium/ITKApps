/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVWatershedRGBModule_h
#define _itkVVWatershedRGBModule_h

#include "vtkVVPluginAPI.h"

#include <string.h>
#include <stdlib.h>

#include "vvITKFilterModuleBase.h"

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkWatershedImageFilter.h"
#include "itkImageRegionConstIterator.h"

#include "itkRGBPixel.h"
#include "itkScalarToRGBPixelFunctor.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkCastImageFilter.h"

#include <vector>


namespace VolView 
{

namespace PlugIn
{

template <class TInputPixelType >
class WatershedRGBModule : public FilterModuleBase {

public:
  typedef WatershedRGBModule             Self;
   // Pixel type of the input buffer
  typedef TInputPixelType                InputPixelType;
  typedef float                          RealPixelType;
  typedef unsigned char                  OutputPixelType;

  itkStaticConstMacro( Dimension, unsigned int, 3 );

  typedef itk::Image< InputPixelType,  
          itkGetStaticConstMacro(Dimension) >  InputImageType;
  typedef itk::Image< RealPixelType,   
          itkGetStaticConstMacro(Dimension) >  RealImageType;
  typedef itk::Image< OutputPixelType, 
          itkGetStaticConstMacro(Dimension) >  OutputImageType;

  // Instantiate the ImportImageFilter
  // This filter is used for building an ITK image using 
  // the data passed in a buffer.
  typedef itk::ImportImageFilter< InputPixelType, 
    itkGetStaticConstMacro(Dimension)       >  ImportFilterType;

  typedef FilterModuleBase::RegionType   RegionType;
  typedef FilterModuleBase::IndexType    IndexType;
  typedef FilterModuleBase::SizeType     SizeType;

  // Instantiate the Cast  filter.
  // This filter is the first stage in 
  // the generation of the watershed landscape.
  typedef itk::CastImageFilter< InputImageType, 
                                RealImageType > CastFilterType;


  // Instantiation of the Watershed filter.
  // This filter computes the propagation of the fron starting
  // at the seed points.  The input of the filter is the speed image.
  // The output is a time-crossing map.
  typedef itk::WatershedImageFilter< RealImageType >  WatershedFilterType;

  // Type of the output labeled image
  typedef typename WatershedFilterType::OutputImageType        LabeledImageType;


  // Type declaration for RGB image that will hold the color 
  // encoding of the water basins
  typedef itk::RGBPixel< unsigned char >              RGBPixelType;
  typedef itk::Image< RGBPixelType, 
          itkGetStaticConstMacro(Dimension) >         RGBImageType;

  typedef itk::Functor::ScalarToRGBPixelFunctor<unsigned long>
                                                      ColorMapFunctorType;

  typedef itk::UnaryFunctorImageFilter< LabeledImageType,
                                        RGBImageType, 
                                        ColorMapFunctorType
                                                    > ColorMapFilterType;

public:
    WatershedRGBModule();
   ~WatershedRGBModule();

    void SetWaterLevel(  float value );

    void ProcessData( const vtkVVProcessDataStruct * pds );
    void CopyOutputData( const vtkVVProcessDataStruct * pds );

private:
    typename ImportFilterType::Pointer              m_ImportFilter;
    typename CastFilterType::Pointer                m_CastFilter;
    typename WatershedFilterType::Pointer           m_WatershedFilter;
    typename ColorMapFilterType::Pointer            m_ColorEncoder;

};

} // end of namespace PlugIn

} // end of namespace Volview

#endif
