/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVShapeDetectionModule_txx
#define _itkVVShapeDetectionModule_txx

#include "vvITKShapeDetectionModule.h"

namespace VolView 
{

namespace PlugIn
{

/*
 *    Constructor
 */
template <class TInputPixelType >
ShapeDetectionModule<TInputPixelType>
::ShapeDetectionModule()
{
  m_ShapeDetectionFilter       = ShapeDetectionFilterType::New();
  m_IntensityWindowingFilter   = IntensityWindowingFilterType::New();

  m_PerformPostprocessing   = true;

  // Set up the pipeline
  m_ShapeDetectionFilter->SetInput(        m_FastMarchingModule.GetLevelSet() );
  m_ShapeDetectionFilter->SetFeatureImage( m_FastMarchingModule.GetSpeedImage()   );
  m_IntensityWindowingFilter->SetInput(    m_ShapeDetectionFilter->GetOutput() );

  m_IntensityWindowingFilter->SetOutputMinimum(   0 );
  m_IntensityWindowingFilter->SetOutputMaximum( 255 );

  // Allow progressive release of memory as the pipeline is executed
  m_ShapeDetectionFilter->ReleaseDataFlagOn();

  m_ShapeDetectionFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
  m_IntensityWindowingFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
}


/*
 *    Destructor
 */
template <class TInputPixelType >
ShapeDetectionModule<TInputPixelType>
::~ShapeDetectionModule()
{
 
}



/*
 *    Add a seed point to the node container.
 *    There is a node per seed.
 */
template <class TInputPixelType >
void 
ShapeDetectionModule<TInputPixelType>
::AddSeed( float x, float y, float z )
{
  m_FastMarchingModule.AddSeed( x, y, z );
}





/*
 *  Set the initial value of the seed.
 *  This can be used to generate zero sets at
 *  a certain distance of the set of seeds.
 */
template <class TInputPixelType >
void 
ShapeDetectionModule<TInputPixelType>
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
ShapeDetectionModule<TInputPixelType>
::SetLowestBasinValue( float value )
{
  m_FastMarchingModule.SetLowestBasinValue( value );
}




/*
 *  Set the lowest value of the basin border to be segmented
 */
template <class TInputPixelType >
void 
ShapeDetectionModule<TInputPixelType>
::SetLowestBorderValue( float value )
{
  m_FastMarchingModule.SetLowestBorderValue( value );
}





/*
 *  Returns the level set resulting 
 *  from the ShapeDetection filter.
 */
template <class TInputPixelType >
const ShapeDetectionModule<TInputPixelType>::RealImageType *
ShapeDetectionModule<TInputPixelType>
::GetLevelSet()
{
   return m_ShapeDetectionFilter->GetOutput();
}





/*
 *  Performs the actual filtering on the data 
 */
template <class TInputPixelType >
void 
ShapeDetectionModule<TInputPixelType>
::ProcessData( const vtkVVProcessDataStruct * pds )
{

  m_FastMarchingModule.SetPluginInfo( this->GetPluginInfo() );

  // Set the Observer for updating progress in the GUI
  m_ShapeDetectionFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );

  // Execute the FastMarching module as preprocessing stage
  m_FastMarchingModule.ProcessData( pds );

  // Execute the filters and progressively remove temporary memory
  m_ShapeDetectionFilter->Update();

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
ShapeDetectionModule<TInputPixelType>
::PostProcessData( const vtkVVProcessDataStruct * pds )
{
 
  typedef itk::MinimumMaximumImageCalculator< 
                                  RealImageType > CalculatorType;
 
  CalculatorType::Pointer calculator = CalculatorType::New();
  calculator->SetImage( m_ShapeDetectionFilter->GetOutput() );
  calculator->Compute();

  const typename RealImageType::PixelType minimum = calculator->GetMinimum(); 
  const typename RealImageType::PixelType maximum = calculator->GetMaximum(); 
  
  m_IntensityWindowingFilter->SetWindowMaximum( maximum );
  m_IntensityWindowingFilter->SetWindowMinimum( minimum );

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
