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

typedef itk::Image< float, 3 > FloatImageType;

typedef itk::ThresholdSegmentationLevelSetImageFilter<
                                     FloatImageType, 
                                     FloatImageType > 
                                       ThresholdSegmentationLevelSetFilterType;

template< class TInputLevelSetImageType, class TFeatureImageType>
class ThresholdSegmentationLevelSet : public 
                   FilterModuleTwoInputs<
                              ThresholdSegmentationLevelSetFilterType,
                              TInputLevelSetImageType,
                              TFeatureImageType > {

public:

  typedef TInputLevelSetImageType    InputLevelSetImageType;
  typedef TFeatureImageType          InputFeatureImageType;

  typedef  FilterModuleTwoInputs<
                      ThresholdSegmentationLevelSetFilterType,
                      InputLevelSetImageType,
                      InputFeatureImageType > Superclass;

 
  typedef typename ThresholdSegmentationLevelSetFilterType::OutputImageType  OutputImageType;
  typedef typename OutputImageType::PixelType                                OutputPixelType;

  typedef itk::CastImageFilter< InputLevelSetImageType, FloatImageType >  InputToFloatFilterType;
  typedef itk::CastImageFilter< InputFeatureImageType,  FloatImageType >  FeatureToFloatFilterType;
 
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

    const float propagationScaling    = 1.0;
    const float advectionScaling      = 1.0;
    const float maximumRMSError       = 0.001;
    const float edgeWeight            = 0.0;

    const unsigned int maximumNumberOfIterations = 500;

    filter->SetLowerThreshold( lowerThreshold );
    filter->SetUpperThreshold( upperThreshold );
    filter->SetCurvatureScaling( curvatureScaling );

    filter->SetPropagationScaling( propagationScaling );
    filter->SetAdvectionScaling( advectionScaling );
    filter->SetMaximumRMSError( maximumRMSError );
    filter->SetNumberOfIterations( maximumNumberOfIterations );
    filter->SetEdgeWeight( edgeWeight );

    typename InputToFloatFilterType::Pointer     inputCaster   = InputToFloatFilterType::New();
    typename FeatureToFloatFilterType::Pointer   featureCaster = FeatureToFloatFilterType::New();
   
    inputCaster->SetInput(    this->GetInput1()  );
    featureCaster->SetInput(  this->GetInput2()  );

    inputCaster->ReleaseDataFlagOn();
    featureCaster->ReleaseDataFlagOn();

    filter->SetInput(          inputCaster->GetOutput()  );
    filter->SetFeatureImage(  featureCaster->GetOutput() );

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

    typedef unsigned char OutputVolumePixelType;
    OutputVolumePixelType * outData = (OutputVolumePixelType *)(pds->outData);

    ot.GoToBegin(); 
    while( !ot.IsAtEnd() )
      {
      *outData = static_cast<unsigned char>( ( ot.Get() + 4.0 ) * 255.0 / 8.0 );
      ++ot;
      ++outData;
      }

  } // end of ProcessData



private:


};


} // end namespace PlugIn

} // end namespace VolView

#endif
