/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVDeformableModelModule_txx
#define _itkVVDeformableModelModule_txx

#include "vvITKDeformableModelModule.h"

namespace VolView 
{

namespace PlugIn
{

/*
 *    Constructor
 */
template <class TInputPixelType >
DeformableModelModule<TInputPixelType>
::DeformableModelModule()
{
    m_ImportFilter               = ImportFilterType::New();
    m_GradientMagnitudeFilter    = GradientMagnitudeFilterType::New();
    m_GradientFilter             = GradientFilterType::New();
    m_DeformableModelFilter      = DeformableModelFilterType::New();
    m_MeshSource                 = MeshSourceType::New();

    // Set up the pipeline
    m_GradientMagnitudeFilter->SetInput(  m_ImportFilter->GetOutput()             );
    m_GradientFilter->SetInput(           m_GradientMagnitudeFilter->GetOutput()  );
    m_DeformableModelFilter->SetInput(    m_MeshSource->GetOutput()               );

    // Allow progressive release of memory as the pipeline is executed
    m_GradientMagnitudeFilter->ReleaseDataFlagOn();
    m_GradientFilter->ReleaseDataFlagOn();
}


/*
 *    Destructor
 */
template <class TInputPixelType >
DeformableModelModule<TInputPixelType>
::~DeformableModelModule()
{
 
}



/*
 *    Define the center of the sphere used to initialize the deformable model.
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::SetEllipsoidCenter( const PointType & center )
{
  m_MeshSource->SetCenter( center );
}



/*
 *    Define the radius of the sphere used to initialize the deformable model.
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::SetEllipsoidRadius( float rx, float ry, float rz )
{
  PointType radius;
  radius[0] = rx;
  radius[1] = ry;
  radius[2] = rz;
  m_MeshSource->SetScale( radius );
}


/*
 *  Set the Sigma value for the Gradient Magnitude filter
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::SetSigma( float value )
{
  m_GradientMagnitudeFilter->SetSigma( value );
  m_GradientFilter->SetSigma( value );
}




/*
 *  Performs the actual filtering on the data 
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::ProcessData( const vtkVVProcessDataStruct * pds )
{

  SizeType   size;
  IndexType  start;

  double     origin[3];
  double     spacing[3];

  const vtkVVPluginInfo * info = this->GetPluginInfo();

  size[0]     =  info->InputVolumeDimensions[0];
  size[1]     =  info->InputVolumeDimensions[1];
  size[2]     =  info->InputVolumeDimensions[2];

  for(unsigned int i=0; i<3; i++)
    {
    origin[i]   =  info->InputVolumeOrigin[i];
    spacing[i]  =  info->InputVolumeSpacing[i];
    start[i]    =  0;
    }

  RegionType region;

  region.SetIndex( start );
  region.SetSize(  size  );
 
  m_ImportFilter->SetSpacing( spacing );
  m_ImportFilter->SetOrigin(  origin  );
  m_ImportFilter->SetRegion(  region  );

  const unsigned int totalNumberOfPixels = region.GetNumberOfPixels();

  const bool         importFilterWillDeleteTheInputBuffer = false;

  const unsigned int numberOfPixelsPerSlice = size[0] * size[1];

  InputPixelType *   dataBlockStart = 
                        static_cast< InputPixelType * >( pds->inData )  
                      + numberOfPixelsPerSlice * pds->StartSlice;

  m_ImportFilter->SetImportPointer( dataBlockStart, 
                                    totalNumberOfPixels,
                                    importFilterWillDeleteTheInputBuffer );

  // Set the Observer for updating progress in the GUI
  m_GradientMagnitudeFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );
  m_DeformableModelFilter->AddObserver( itk::ProgressEvent(), this->GetCommandObserver() );

  // Execute the filters and progressively remove temporary memory
  m_GradientMagnitudeFilter->Update();
  m_DeformableModelFilter->Update();

  this->PostProcessData( pds );

} // end of ProcessData



/*
 *  Performs post-processing of data. 
 *  This involves an intensity window operation and
 *  data copying into the volview provided buffer.
 */
template <class TInputPixelType >
void 
DeformableModelModule<TInputPixelType>
::PostProcessData( const vtkVVProcessDataStruct * pds )
{

   typename MeshType::ConstPointer mesh = m_DeformableModelFilter->GetOutput();


} // end of PostProcessData


} // end of namespace PlugIn

} // end of namespace Volview

#endif
