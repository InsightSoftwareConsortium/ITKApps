/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVGeodesicActiveContourModule_txx
#define _itkVVGeodesicActiveContourModule_txx

#include "vvITKGeodesicActiveContourModule.h"
#include <fstream>

namespace VolView 
{

namespace PlugIn
{

/*
 *    Constructor
 */
template <class TInputPixelType >
GeodesicActiveContourModule<TInputPixelType>
::GeodesicActiveContourModule()
{
  m_GeodesicActiveContourFilter       = GeodesicActiveContourFilterType::New();
  m_IntensityWindowingFilter   = IntensityWindowingFilterType::New();

  m_PerformPostprocessing   = true;

  // Set up the pipeline
  m_GeodesicActiveContourFilter->SetInput(        m_FastMarchingModule.GetLevelSet() );
  m_GeodesicActiveContourFilter->SetFeatureImage( m_FastMarchingModule.GetSpeedImage()   );
  m_IntensityWindowingFilter->SetInput(    m_GeodesicActiveContourFilter->GetOutput() );

  m_IntensityWindowingFilter->SetOutputMinimum( 255 );
  m_IntensityWindowingFilter->SetOutputMaximum(   0 );

  // Allow progressive release of memory as the pipeline is executed
  m_GeodesicActiveContourFilter->ReleaseDataFlagOn();

  m_GeodesicActiveContourFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
  m_IntensityWindowingFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
}


/*
 *    Destructor
 */
template <class TInputPixelType >
GeodesicActiveContourModule<TInputPixelType>
::~GeodesicActiveContourModule()
{
 
}



/*
 *    Add a seed point to the node container.
 *    There is a node per seed.
 */
template <class TInputPixelType >
void 
GeodesicActiveContourModule<TInputPixelType>
::AddSeed( const IndexType & seedPosition )
{
  m_FastMarchingModule.AddSeed( seedPosition );
}





/*
 *  Set the initial value of the seed.
 *  This can be used to generate zero sets at
 *  a certain distance of the set of seeds.
 */
template <class TInputPixelType >
void 
GeodesicActiveContourModule<TInputPixelType>
::SetDistanceFromSeeds( float value )
{
  m_FastMarchingModule.SetStoppingValue( 10.0 * value );
  m_FastMarchingModule.SetInitialSeedValue( -value );
}






/*
 *  Set the lowest value of the basin to be segmented
 */
template <class TInputPixelType >
void 
GeodesicActiveContourModule<TInputPixelType>
::SetLowestBasinValue( float value )
{
  m_FastMarchingModule.SetLowestBasinValue( value );
}




/*
 *  Set the lowest value of the basin border to be segmented
 */
template <class TInputPixelType >
void 
GeodesicActiveContourModule<TInputPixelType>
::SetLowestBorderValue( float value )
{
  m_FastMarchingModule.SetLowestBorderValue( value );
}



/*
 *  Set the sigma value of the Gaussian to be used
 *  by the gradient magnitude filter.
 */
template <class TInputPixelType >
void 
GeodesicActiveContourModule<TInputPixelType>
::SetSigma( float value )
{
  m_FastMarchingModule.SetSigma( value );
}



/*
 *  Set the Advection scaling for the 
 *  GeodesicActiveContourLevelSetImageFilter.
 */
template <class TInputPixelType >
void 
GeodesicActiveContourModule<TInputPixelType>
::SetAdvectionScaling( float value )
{
  m_GeodesicActiveContourFilter->SetAdvectionScaling( value );
}




/*
 *  Set the factor that will scale curvature
 */
template <class TInputPixelType >
void 
GeodesicActiveContourModule<TInputPixelType>
::SetCurvatureScaling( float value )
{
  m_GeodesicActiveContourFilter->SetCurvatureScaling( value );
}


/*
 *  Set the factor that will scale curvature
 */
template <class TInputPixelType >
void 
GeodesicActiveContourModule<TInputPixelType>
::SetPropagationScaling( float value )
{
  m_GeodesicActiveContourFilter->SetPropagationScaling( value );
}


/*
 *  Set the threshold of RMS error that will define convergence
 */
template <class TInputPixelType >
void 
GeodesicActiveContourModule<TInputPixelType>
::SetMaximumRMSError( float value )
{
  m_GeodesicActiveContourFilter->SetMaximumRMSError( value );
}


/*
 *  Set the maximum number of iterations to attempt in order to get
 *  convergence.
 */
template <class TInputPixelType >
void 
GeodesicActiveContourModule<TInputPixelType>
::SetMaximumIterations( unsigned int value )
{
  m_GeodesicActiveContourFilter->SetMaximumIterations( value );
}


/*
 *  Get real number of iterations performed
 */
template <class TInputPixelType >
unsigned int  
GeodesicActiveContourModule<TInputPixelType>
::GetElapsedIterations() const
{
  return m_GeodesicActiveContourFilter->GetElapsedIterations();
}


/*
 *  Get the RMS error from the last iteration
 */
template <class TInputPixelType >
float 
GeodesicActiveContourModule<TInputPixelType>
::GetRMSChange() const
{
  return m_GeodesicActiveContourFilter->GetRMSChange();
}


/*
 *  Returns the level set resulting 
 *  from the GeodesicActiveContour filter.
 */
template <class TInputPixelType >
const GeodesicActiveContourModule<TInputPixelType>::RealImageType *
GeodesicActiveContourModule<TInputPixelType>
::GetLevelSet()
{
   return m_GeodesicActiveContourFilter->GetOutput();
}





/*
 *  Performs the actual filtering on the data 
 */
template <class TInputPixelType >
void 
GeodesicActiveContourModule<TInputPixelType>
::ProcessData( const vtkVVProcessDataStruct * pds )
{

  m_FastMarchingModule.SetPluginInfo( this->GetPluginInfo() );

  // Set the Observer for updating progress in the GUI
  m_GeodesicActiveContourFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );

  // Execute the FastMarching module as preprocessing stage
  m_FastMarchingModule.ProcessData( pds );

  // Execute the filters and progressively remove temporary memory
  m_GeodesicActiveContourFilter->Update();

  if( m_PerformPostprocessing )
    {
    this->PostProcessData( pds );
    }

} // end of ProcessData



/*
 *  Performs post-processing of data. 
 *  This involves an intensity window operation and
 *  data copying into the volview provided buffer.
 */
template <class TInputPixelType >
void 
GeodesicActiveContourModule<TInputPixelType>
::PostProcessData( const vtkVVProcessDataStruct * pds )
{
 
  typedef itk::MinimumMaximumImageCalculator< 
                                  RealImageType > CalculatorType;
 
  CalculatorType::Pointer calculator = CalculatorType::New();
  calculator->SetImage( m_GeodesicActiveContourFilter->GetOutput() );
  calculator->Compute();

  const typename RealImageType::PixelType minimum = calculator->GetMinimum(); 
  const typename RealImageType::PixelType maximum = calculator->GetMaximum(); 
  
  if( minimum < 0.0 && maximum > 0.0 )
    {
    if( -minimum > maximum )
      {
      m_IntensityWindowingFilter->SetWindowMaximum(  maximum );
      m_IntensityWindowingFilter->SetWindowMinimum( -maximum );
      }
    else
      {
      m_IntensityWindowingFilter->SetWindowMaximum( -minimum );
      m_IntensityWindowingFilter->SetWindowMinimum(  minimum );
      }
    }
  else
    {
    std::ofstream ofs("Azucar.txt");
    ofs << "Minimum < 0 && Maximum > 0 assertion failed";
    ofs << "Minimum = " << minimum << std::endl;
    ofs << "Maximum = " << maximum << std::endl;
    ofs.close(); 
    }

  m_IntensityWindowingFilter->Update();

  // Copy the data (with casting) to the output buffer provided by the Plug In API
  typename OutputImageType::ConstPointer outputImage =
                               m_IntensityWindowingFilter->GetOutput();

  typedef itk::ImageRegionConstIterator< OutputImageType >  OutputIteratorType;

  OutputIteratorType ot( outputImage, outputImage->GetBufferedRegion() );

  InputPixelType * outData = (InputPixelType *)(pds->outData);

  ot.GoToBegin(); 
  while( !ot.IsAtEnd() )
    {
    *outData = static_cast< InputPixelType >( ot.Get() );
    ++ot;
    ++outData;
    }

} // end of PostProcessData


} // end of namespace PlugIn

} // end of namespace Volview

#endif
