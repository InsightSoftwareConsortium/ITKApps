/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVShapeDetectionModule_h
#define _itkVVShapeDetectionModule_h

#include "vvITKFastMarchingModule.txx"

#include "itkShapeDetectionLevelSetImageFilter.h"

namespace VolView 
{

namespace PlugIn
{

template <class TInputPixelType >
class ShapeDetectionModule {

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

  // Command/Observer intended to update the progress
  typedef itk::MemberCommand< ShapeDetectionModule >  CommandType;

  // Instantiation of the Intensity windowing filter.
  // This filter is used to remove infinite times from the non-visited
  // pixels of the time-crossing map. This pixels are set to the stopping value.
  typedef itk::IntensityWindowingImageFilter< 
                                       RealImageType,
                                       OutputImageType >
                                                   IntensityWindowingFilterType;
 
  // Type of the ShapeDetection filter to use.
  typedef  itk::ShapeDetectionLevelSetImageFilter< 
                                            RealImageType, 
                                            RealImageType >    
                                                   ShapeDetectionFilterType;


public:
    ShapeDetectionModule();
   ~ShapeDetectionModule();

    void ClearSeeds();
    void AddSeed( float x, float y, float z );
    void SetLowestBorderValue( float value );
    void SetLowestBasinValue(  float value );
    void SetDistanceFromSeeds( float value );

    void ProcessData( const vtkVVProcessDataStruct * pds );
    void PostProcessData( const vtkVVProcessDataStruct * pds );
    void ProgressUpdate( itk::Object * caller, const itk::EventObject & event );
    void SetPluginInfo( vtkVVPluginInfo * info );

    const RealImageType * GetLevelSet();

private:
    FastMarchingModuleType                          m_FastMarchingModule;
    typename ShapeDetectionFilterType::Pointer      m_ShapeDetectionFilter;
    typename IntensityWindowingFilterType::Pointer  m_IntensityWindowingFilter;

    typename CommandType::Pointer                   m_CommandObserver;

    vtkVVPluginInfo                               * m_Info;

    std::string                                     m_UpdateMessage;
  
    bool                                            m_PerformPostprocessing;

};

} // end of namespace PlugIn

} // end of namespace Volview

#endif
