/** This module implemetes the limited preprocessing for 
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

namespace VolView
{

namespace PlugIn
{

typedef itk::GeodesicActiveContourLevelSetImageFilter<
                          itk::Image< float,3 >,
                          itk::Image< float,3 >
                                  > GeodesicActiveContourFilterType;
 
typedef itk::Image< unsigned char, 3 > BinaryMaskImageType;
typedef itk::Image< unsigned char, 3 > InputSpeedImageType;

typedef itk::Image< float, 3 > InternalSpeedImageType;

class GeodesicActiveContour : public 
                   FilterModuleTwoInputs<
                              GeodesicActiveContourFilterType,
                              BinaryMaskImageType,
                              InputSpeedImageType > {

public:

  typedef  FilterModuleTwoInputs<
                      GeodesicActiveContourFilterType,
                      BinaryMaskImageType,
                      InputSpeedImageType > Superclass;

  typedef  itk::RescaleIntensityImageFilter< 
                      InputSpeedImageType,
                      InternalSpeedImageType 
                               > RescaleIntensityFilterType;


public:

  /**  Constructor */
  GeodesicActiveContour() 
    {
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

    GeodesicActiveContourFilterType * filter = this->GetFilter();

    vtkVVPluginInfo *info = this->GetPluginInfo();

    const float zeroSetValue          = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ));
    const float gaussianSigma         = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ));
    const float curvatureScaling      = atof( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ));
    const float propagationScaling    = atof( info->GetGUIProperty(info, 3, VVP_GUI_VALUE ));
    const float advectionScaling      = atof( info->GetGUIProperty(info, 4, VVP_GUI_VALUE ));
    const float maximumRMSError       = atof( info->GetGUIProperty(info, 5, VVP_GUI_VALUE ));

    const unsigned int maximumNumberOfIterations = atoi( info->GetGUIProperty(info, 6, VVP_GUI_VALUE ));

    m_RescaleIntensityFilter->SetInput( this->GetSecondInput() );

    filter->SetDerivativeSigma( gaussianSigma );
    filter->SetCurvatureScaling( curvatureScaling );
    filter->SetPropagationScaling( propagationScaling );
    filter->SetAdvectionScaling( advectionScaling );
    filter->SetMaximumRMSError( maximumRMSError );
    filter->SetMaximumIterations( maximumNumberOfIterations );
    filter->SetFeatureImage( m_RescaleIntensityFilter->GetOutput() );

    // Now let the base class do the rest.
    this->Superclass::ProcessData( pds );
    
   
  } // end of ProcessData



private:

  // Add here the RescaleIntensity filter and the reinitialize filter.
  RescaleIntensityFilterType::Pointer   m_RescaleIntensityFilter;

};


} // end namespace PlugIn

} // end namespace VolView

#endif
