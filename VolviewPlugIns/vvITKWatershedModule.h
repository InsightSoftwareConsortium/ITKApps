/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVWatershedModule_h
#define _itkVVWatershedModule_h

#include "vtkVVPluginAPI.h"

#include <string.h>
#include <stdlib.h>

#include "vvITKFilterModuleBase.h"

#include "itkImage.h"
#include "itkImportImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkWatershedImageFilter.h"
#include "itkImageRegionConstIterator.h"

#include <vector>


namespace VolView 
{

namespace PlugIn
{

template <class TInputPixelType >
class WatershedModule : public FilterModuleBase {

public:

   // Pixel type of the input buffer
  typedef TInputPixelType                InputPixelType;
  typedef float                          RealPixelType;
  typedef unsigned char                  OutputPixelType;

  itkStaticConstMacro( Dimension, unsigned int, 3 );

  typedef itk::Image< InputPixelType,  Dimension >  InputImageType;
  typedef itk::Image< RealPixelType,   Dimension >  RealImageType;
  typedef itk::Image< OutputPixelType, Dimension >  OutputImageType;

  // Instantiate the ImportImageFilter
  // This filter is used for building an ITK image using 
  // the data passed in a buffer.
  typedef itk::ImportImageFilter< InputPixelType, 
                                  Dimension       > ImportFilterType;

  typedef FilterModuleBase::RegionType   RegionType;
  typedef FilterModuleBase::IndexType    IndexType;
  typedef FilterModuleBase::SizeType     SizeType;

  // Instantiate the GradientMagnitude filter.
  // This filter is the first stage in 
  // the generation of the watershed landscape.
  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter< 
                                InputImageType, 
                                RealImageType > GradientMagnitudeFilterType;


  // Instantiation of the Watershed filter.
  // This filter computes the propagation of the fron starting
  // at the seed points.  The input of the filter is the speed image.
  // The output is a time-crossing map.
  typedef itk::WatershedImageFilter< RealImageType >  WatershedFilterType;


  //  This structure will hold the seed points used to recolect basins
  //  at the end.
  typedef std::vector< IndexType >      SeedsContainerType;


public:
    WatershedModule();
   ~WatershedModule();

    void ClearSeeds();
    void AddSeed( const IndexType & seedPosition );
    void SetSigma( float value );
    void SetThreshold( float value );
    void SetWaterLevel(  float value );

    void ProcessData( const vtkVVProcessDataStruct * pds );
    void PostProcessData( const vtkVVProcessDataStruct * pds );

    /** This methods allows to disable postprocessing. Useful when 
        this module is used for initializing other level set methods */
    void SetPerformPostProcessing( bool value );
    

private:
    typename ImportFilterType::Pointer              m_ImportFilter;
    typename GradientMagnitudeFilterType::Pointer   m_GradientMagnitudeFilter;
    typename WatershedFilterType::Pointer           m_WatershedFilter;

    SeedsContainerType                              m_Seeds;
    
    bool                                            m_PerformPostprocessing;

};

} // end of namespace PlugIn

} // end of namespace Volview

#endif
