/** This module implements the limited preprocessing for 
    the ThresholdSegmentationLevelSets filter. The module expects
    two inputs. First, a binary mask, second a speed image.
    the binary mask is passed through the ReinitializeImageFilter
    in order to produce a level set. The speed image is casted
    and renormalized between 0.0 and 1.0, then connected as
    feature image to the geodesic active contour filter.  */
    

#ifndef _vvITKThresholdSegmentationLevelSet_h
#define _vvITKThresholdSegmentationLevelSet_h

#include "vvITKFilterModuleTwoInputs.h"
#include "itkThresholdSegmentationLevelSetImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"


namespace VolView
{

namespace PlugIn
{


template< class TInputLevelSetImageType, class TFeatureImageType>
class ThresholdSegmentationLevelSet : public 
                   FilterModuleTwoInputs<
                          itk::ThresholdSegmentationLevelSetImageFilter<
                                            TInputLevelSetImageType,
                                            TFeatureImageType >,
                              TInputLevelSetImageType,
                              TFeatureImageType > {

public:

  typedef TInputLevelSetImageType    InputLevelSetImageType;
  typedef TFeatureImageType          InputFeatureImageType;

  typedef  FilterModuleTwoInputs<
                      itk::ThresholdSegmentationLevelSetImageFilter<
                                        TInputLevelSetImageType,
                                        TFeatureImageType >,
                      InputLevelSetImageType,
                      InputFeatureImageType > Superclass;

  typedef itk::ThresholdSegmentationLevelSetImageFilter<
                          TInputLevelSetImageType,
                          TFeatureImageType
                                  > ThresholdSegmentationLevelSetFilterType;
 
  typedef typename ThresholdSegmentationLevelSetFilterType::OutputImageType  OutputImageType;
  typedef typename OutputImageType::PixelType                                OutputPixelType;

public:

  /**  Constructor */
  ThresholdSegmentationLevelSet() 
    {
    }



  /**  Destructor */
  virtual ~ThresholdSegmentationLevelSet() 
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

    ThresholdSegmentationLevelSetFilterType * filter = this->GetFilter();

    vtkVVPluginInfo *info = this->GetPluginInfo();

    const float upperThreshold        = atof( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ));
    const float lowerThreshold        = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ));
    const float curvatureScaling      = atof( info->GetGUIProperty(info, 2, VVP_GUI_VALUE ));
    const float propagationScaling    = atof( info->GetGUIProperty(info, 3, VVP_GUI_VALUE ));
    const float advectionScaling      = atof( info->GetGUIProperty(info, 4, VVP_GUI_VALUE ));
    const float maximumRMSError       = atof( info->GetGUIProperty(info, 5, VVP_GUI_VALUE ));

    const unsigned int maximumNumberOfIterations = atoi( info->GetGUIProperty(info, 6, VVP_GUI_VALUE ));

    filter->SetLowerThreshold( lowerThreshold );
    filter->SetUpperThreshold( upperThreshold );
    filter->SetCurvatureScaling( curvatureScaling );
    filter->SetPropagationScaling( propagationScaling );
    filter->SetAdvectionScaling( advectionScaling );
    filter->SetMaximumRMSError( maximumRMSError );
    filter->SetNumberOfIterations( maximumNumberOfIterations );

    filter->SetInput(        this->GetInput1() );
    filter->SetFeatureImage( this->GetInput2() );

    // Execute the filter
    try
      {
      filter->Update();
      }
    catch( itk::ProcessAborted & )
      {
      return;
      }

    // Copy the data (with casting) to the output buffer provided by the PlugIn API
    OutputImageType::ConstPointer outputImage = filter->GetOutput();

    typedef itk::ImageRegionConstIterator< OutputImageType >  OutputIteratorType;

    OutputIteratorType ot( outputImage, outputImage->GetBufferedRegion() );

    OutputPixelType * outData = (OutputPixelType *)(pds->outData);

    ot.GoToBegin(); 
    while( !ot.IsAtEnd() )
      {
      *outData = static_cast<unsigned char>( ( ot.Get() + 5.0 ) * 255.0 / 10.0 );
      ++ot;
      ++outData;
      }
  } // end of ProcessData



private:


};


} // end namespace PlugIn

} // end namespace VolView

#endif
