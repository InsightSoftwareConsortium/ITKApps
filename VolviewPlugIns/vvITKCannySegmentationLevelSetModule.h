/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVCannySegmentationLevelSetModule_h
#define _itkVVCannySegmentationLevelSetModule_h

#include "vvITKFilterModuleBase.h"
#include "itkImportImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkFastMarchingImageFilter.h"
#include "itkCannySegmentationLevelSetImageFilter.h"
#include "itkIntensityWindowingImageFilter.h"

namespace VolView 
{

namespace PlugIn
{

template <class TInputPixelType >
class CannySegmentationLevelSetModule : public FilterModuleBase {

public:

   // Pixel type of the input buffer
  typedef TInputPixelType                InputPixelType;
  typedef unsigned char                  OutputPixelType;
  typedef float                          RealPixelType;

  itkStaticConstMacro(Dimension, unsigned int, 3 );

  typedef itk::Image< InputPixelType,  Dimension >      InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >      OutputImageType;
  typedef itk::Image< RealPixelType,   Dimension >      RealImageType;

  // Instantiate the ImportImageFilter
  // This filter is used for building an ITK image using 
  // the data passed in a buffer.
  typedef itk::ImportImageFilter< InputPixelType, Dimension > ImportFilterType;

  typedef typename FilterModuleBase::SizeType      SizeType;
  typedef typename FilterModuleBase::IndexType     IndexType;
  typedef typename FilterModuleBase::RegionType    RegionType;

  // Instantiate the CastImageFilter
  // This filter is used for converting the pixel type from the input
  // data buffer into the input pixel type of the filter.
  typedef itk::CastImageFilter< InputImageType, 
                                RealImageType > CastFilterType;

  // Instantiation of the FastMarching filter.
  // This filter computes the propagation of the fron starting
  // at the seed points.  The input of the filter is the speed image.
  // The output is a time-crossing map.
  typedef itk::FastMarchingImageFilter< RealImageType,
                                        RealImageType >  FastMarchingFilterType;

  // Type of the CannySegmentationLevelSet filter to use.
  typedef  itk::CannySegmentationLevelSetImageFilter< 
                                itk::Image< RealPixelType,   3 >,
                                itk::Image< RealPixelType,   3 >
                                                  > CannySegmentationLevelSetFilterType;

  // Instantiation of the Intensity windowing filter.
  // This filter is used to remove infinite times from the non-visited
  // pixels of the time-crossing map. This pixels are set to the stopping value.
  typedef itk::IntensityWindowingImageFilter< 
                                       RealImageType,
                                       OutputImageType >
                                                   IntensityWindowingFilterType;
 
   typedef typename FastMarchingFilterType::NodeType         NodeType;
   typedef typename FastMarchingFilterType::NodeContainer    NodeContainerType;

public:
    CannySegmentationLevelSetModule();
   ~CannySegmentationLevelSetModule();

    void ClearSeeds();
    void AddSeed( const IndexType & seedPosition );
    void SetDistanceFromSeeds( float value );

    void SetCurvatureScaling(  float value );
    void SetPropagationScaling( float value );
    void SetAdvectionScaling( float value );
    void SetMaximumRMSError( float value );
    void SetMaximumIterations( unsigned int iterations );

    void SetThreshold( float value );
    void SetVariance( float value );

    void ProcessData( const vtkVVProcessDataStruct * pds );
    void PostProcessData( const vtkVVProcessDataStruct * pds );

    const RealImageType * GetLevelSet();

    unsigned int GetElapsedIterations() const;
    float GetRMSChange() const;

private:

    typename ImportFilterType::Pointer              m_ImportFilter;
    typename CastFilterType::Pointer                m_CastFilter;
    typename FastMarchingFilterType::Pointer        m_FastMarchingImageFilter;
    typename NodeContainerType::Pointer             m_NodeContainer;

    typename CannySegmentationLevelSetFilterType::Pointer      
                                                    m_CannySegmentationLevelSetFilter;

    typename IntensityWindowingFilterType::Pointer  m_IntensityWindowingFilter;

    double                                          m_InitialSeedValue;
 
    unsigned long                                   m_CurrentNumberOfSeeds;

    bool                                            m_PerformPostprocessing;

};

} // end of namespace PlugIn

} // end of namespace Volview

#endif
