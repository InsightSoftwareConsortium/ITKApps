/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVGeodesicActiveContourModule_h
#define _itkVVGeodesicActiveContourModule_h

#include "vvITKFastMarchingModule.txx"

#include "itkGeodesicActiveContourLevelSetImageFilter.h"

namespace VolView 
{

namespace PlugIn
{

template <class TInputPixelType >
class GeodesicActiveContourModule : public FilterModuleBase {

public:

   // Pixel type of the input buffer
  typedef TInputPixelType                InputPixelType;
  typedef unsigned char                  OutputPixelType;

  typedef itk::Image< OutputPixelType, 3 >     OutputImageType;

  typedef FastMarchingModule< InputPixelType > FastMarchingModuleType;

  itkStaticConstMacro(Dimension, unsigned int, 
                             FastMarchingModuleType::Dimension );

  typedef typename FastMarchingModuleType::SpeedImageType     SpeedImageType;

  typedef itk::Image< float, 3 > RealImageType;

  // Instantiation of the Intensity windowing filter.
  // This filter is used to remove infinite times from the non-visited
  // pixels of the time-crossing map. This pixels are set to the stopping value.
  typedef itk::IntensityWindowingImageFilter< 
                                       RealImageType,
                                       OutputImageType >
                                                   IntensityWindowingFilterType;
 
  // Type of the GeodesicActiveContour filter to use.
  typedef  itk::GeodesicActiveContourLevelSetImageFilter< 
                                            RealImageType, 
                                            RealImageType >    
                                                   GeodesicActiveContourFilterType;

    unsigned int GetElapsedIterations() const;
    float GetRMSChange() const;

public:
    GeodesicActiveContourModule();
   ~GeodesicActiveContourModule();

    void ClearSeeds();
    void AddSeed( const IndexType & seedPosition );
    void SetLowestBorderValue( float value );
    void SetLowestBasinValue(  float value );
    void SetDistanceFromSeeds( float value );
    void SetSigma( float value );

    void SetCurvatureScaling(  float value );
    void SetPropagationScaling( float value );
    void SetAdvectionScaling( float value );
    void SetMaximumRMSError( float value );
    void SetNumberOfIterations( unsigned int iterations );

    void ProcessData( const vtkVVProcessDataStruct * pds );
    void PostProcessData( const vtkVVProcessDataStruct * pds );

    const RealImageType * GetLevelSet();

private:
    FastMarchingModuleType                              m_FastMarchingModule;
    typename GeodesicActiveContourFilterType::Pointer   m_GeodesicActiveContourFilter;
    typename IntensityWindowingFilterType::Pointer      m_IntensityWindowingFilter;
    bool                                                m_PerformPostprocessing;

};

} // end of namespace PlugIn

} // end of namespace Volview

#endif
