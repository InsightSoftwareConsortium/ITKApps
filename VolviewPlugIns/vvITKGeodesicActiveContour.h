/** This module implements the limited preprocessing for 
    the GeodesicActiveContours filter. The module expects
    two inputs. First, a binary mask, second a speed image.
    the binary mask is passed through the ReinitializeImageFilter
    in order to produce a level set. The speed image is casted
    and renormalized between 0.0 and 1.0, then connected as
    feature image to the geodesic active contour filter.  */
    

#ifndef _vvITKGeodesicActiveContour_h
#define _vvITKGeodesicActiveContour_h

#include "vvITKFilterModuleTwoInputs.h"
#include "itkGeodesicActiveContourLevelSetImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"


namespace VolView
{

namespace PlugIn
{

typedef itk::GeodesicActiveContourLevelSetImageFilter<
                          itk::Image< float,3 >,
                          itk::Image< float,3 >
                                  > GeodesicActiveContourFilterType;
 

template< class TBinaryMaskImageType, class TInputSpeedImageType>
class GeodesicActiveContour : public 
                   FilterModuleTwoInputs<
                              GeodesicActiveContourFilterType,
                              TBinaryMaskImageType,
                              TInputSpeedImageType > {

public:

  typedef TBinaryMaskImageType    BinaryMaskImageType;
  typedef TInputSpeedImageType    InputSpeedImageType;

  typedef itk::Image< float, 3 > InternalSpeedImageType;

  typedef  FilterModuleTwoInputs<
                      GeodesicActiveContourFilterType,
                      BinaryMaskImageType,
                      InputSpeedImageType > Superclass;

  typedef  itk::RescaleIntensityImageFilter< 
                      InputSpeedImageType,
                      InternalSpeedImageType 
                               > RescaleSpeedFilterType;

  typedef  itk::RescaleIntensityImageFilter< 
                      InternalSpeedImageType,
                      InternalSpeedImageType 
                               > RescaleLeveSetFilterType;

  typedef  itk::SmoothingRecursiveGaussianImageFilter<
                      BinaryMaskImageType,
                      InternalSpeedImageType 
                               > SmoothingFilterType;

  typedef unsigned char                    OutputPixelType;
  typedef itk::Image< OutputPixelType, 3 > OutputImageType;

  typedef  itk::RescaleIntensityImageFilter< 
                      InternalSpeedImageType,
                      OutputImageType 
                               > RescaleOutputFilterType;

public:

  /**  Constructor */
  GeodesicActiveContour() 
    {
    m_RescaleSpeedFilter    = RescaleSpeedFilterType::New();
    m_RescaleLevelSetFilter = RescaleLeveSetFilterType::New();
    m_SmoothingFilter       = SmoothingFilterType::New();
    m_RescaleOutputFilter   = RescaleOutputFilterType::New();

    m_RescaleSpeedFilter->SetInput( this->GetInput2() );
    m_RescaleSpeedFilter->SetOutputMaximum( 1.0 );
    m_RescaleSpeedFilter->SetOutputMinimum( 0.0 );

    m_SmoothingFilter->SetInput( this->GetInput1() );
    
    m_RescaleLevelSetFilter->SetInput( m_SmoothingFilter->GetOutput() );
    m_RescaleLevelSetFilter->SetOutputMaximum(  0.5 );
    m_RescaleLevelSetFilter->SetOutputMinimum( -0.5 );

    m_RescaleSpeedFilter->ReleaseDataFlagOn();
    m_RescaleLevelSetFilter->ReleaseDataFlagOn();
    m_SmoothingFilter->ReleaseDataFlagOn();

    m_RescaleOutputFilter->SetOutputMaximum(255);
    m_RescaleOutputFilter->SetOutputMinimum( 0 );
    }



  /**  Destructor */
  virtual ~GeodesicActiveContour() 
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

    GeodesicActiveContourFilterType * filter = this->GetFilter();

    vtkVVPluginInfo *info = this->GetPluginInfo();

    const float gaussianSigma         = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ));
    const float curvatureScaling      = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ));
    const float propagationScaling    = atof( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ));
    const float advectionScaling      = atof( info->GetGUIProperty(info, 3, VVP_GUI_VALUE ));
    const float maximumRMSError       = atof( info->GetGUIProperty(info, 4, VVP_GUI_VALUE ));

    const unsigned int maximumNumberOfIterations = atoi( info->GetGUIProperty(info, 5, VVP_GUI_VALUE ));


    filter->SetDerivativeSigma( gaussianSigma );
    filter->SetCurvatureScaling( curvatureScaling );
    filter->SetPropagationScaling( propagationScaling );
    filter->SetAdvectionScaling( advectionScaling );
    filter->SetMaximumRMSError( maximumRMSError );
    filter->SetMaximumIterations( maximumNumberOfIterations );

    m_SmoothingFilter->SetSigma( gaussianSigma );

    filter->SetInput(         m_RescaleLevelSetFilter->GetOutput()  );
    filter->SetFeatureImage(  m_RescaleSpeedFilter->GetOutput()     );

    // Execute the filter
    try
      {
      filter->Update();
      filter->ReleaseDataFlagOn();
      m_RescaleOutputFilter->SetInput( filter->GetOutput() );
      m_RescaleOutputFilter->Update();
      }
    catch( itk::ProcessAborted & )
      {
      return;
      }

    // Copy the data (with casting) to the output buffer provided by the PlugIn API
    OutputImageType::ConstPointer outputImage =
                                     m_RescaleOutputFilter->GetOutput();

    typedef itk::ImageRegionConstIterator< OutputImageType >  OutputIteratorType;

    OutputIteratorType ot( outputImage, outputImage->GetBufferedRegion() );

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

  typename SmoothingFilterType::Pointer          m_SmoothingFilter;
  typename RescaleLeveSetFilterType::Pointer     m_RescaleLevelSetFilter;
  typename RescaleSpeedFilterType::Pointer       m_RescaleSpeedFilter;
  typename RescaleOutputFilterType::Pointer      m_RescaleOutputFilter;
};


} // end namespace PlugIn

} // end namespace VolView

#endif
