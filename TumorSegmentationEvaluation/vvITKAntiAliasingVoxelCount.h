#ifndef _vvITKAntiAliasingVoxelCount_h
#define _vvITKAntiAliasingVoxelCount_h
#include "vvITKFilterModule.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include <iostream>
namespace VolView
{
namespace PlugIn
{
template< class TInputImageType>
class AntiAliasedVoxelCount : public 
                        FilterModule<itk::StatisticsImageFilter< TInputImageType > >{
public:
  typedef TInputImageType    InputImageType;

  typedef itk::Image< float, 3 > InternalImageType;

  typedef  FilterModule<FilterType> Superclass;
  typedef  TInputImageType OutputImageType;
  typedef  itk::CastImageFilter< 
                      InputImageType,
                      InternalImageType 
                               > CastInputFilterType;
  typedef   itk::AntiAliasBinaryImageFilter< 
                                    itk::Image< float,3 >,  
                                    itk::Image< float,3 > >  AntiAliasFilterType;
  typedef  itk::CastImageFilter< 
                      InternalImageType,
                      OutputImageType 
                               > CastOutputFilterType;
public:
  AntiAliasedVoxelCount() 
    {
     m_CastOutputFilter   = CastOutputFilterType::New();
     m_AntiAliasFilter= AntiAliasFilterType::New();
     m_CastInputFilter   = CastInputFilterType::New();
     m_CastInputFilter->SetInput((InputImageType*)(this->GetInput()));
     m_AntiAliasFilter->SetInput(m_CastInputFilter->GetOutput());
    }
  virtual ~AntiAliasedVoxelCount() 
    {
    }
  void 
  ProcessData( const vtkVVProcessDataStruct * pds )
  {
    this->Superclass::ProcessData( pds );

    FilterType * filter = this->GetFilter();
    vtkVVPluginInfo *info = this->GetPluginInfo();
    const float maxNumberOfIterations = atoi( info->GetGUIProperty(info, 0, VVP_GUI_VALUE ) );
    const float maximumRMSError       = atof( info->GetGUIProperty(info, 1, VVP_GUI_VALUE ) );
    m_AntiAliasFilter->SetMaximumRMSError(maximumRMSError);
    m_AntiAliasFilter->SetNumberOfIterations(maxNumberOfIterations);
    m_CastOutputFilter->SetInput(m_AntiAliasFilter->GetOutput());
    filter->SetInput(m_CastOutputFilter->GetOutput());
    // Execute the filter
    try
      {
      filter->Update();
      }
    catch( itk::ProcessAborted & )
      {
      return;
      }
    m_CastOutputFilter->ReleaseDataFlagOn();
    m_AntiAliasFilter->ReleaseDataFlagOn();
} // end of ProcessData
private:
  typename CastInputFilterType::Pointer     m_CastInputFilter;
  typename CastOutputFilterType::Pointer      m_CastOutputFilter;
  typename AntiAliasFilterType::Pointer m_AntiAliasFilter;
};


} // end namespace PlugIn

} // end namespace VolView

#endif
