/** Base class, non-templated, for the generic interface for 
    protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _vvITKFilterModuleBase_h
#define _vvITKFilterModulBasee_h

#include "vtkVVPluginAPI.h"

#include "itkCommand.h"
#include "itkProcessObject.h"
#include "itkImageRegion.h"
#include "itkSize.h"
#include "itkIndex.h"


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>   

namespace VolView
{

namespace PlugIn
{

typedef double  RealType;

class FilterModuleBase {

public:
  // Command/Observer intended to update the progress
  typedef itk::MemberCommand< FilterModuleBase >  CommandType;

  // Type used by most algorithms. These types depend only on dimension.
  typedef itk::ImageRegion<3>     RegionType;
  typedef itk::Index<3>           IndexType;
  typedef itk::Size<3>            SizeType;

  typedef float                   MarkersCoordinatesType;

public:

  /**  Constructor */
  FilterModuleBase() 
    {
    m_CommandObserver    = CommandType::New();
    m_Info               = 0;
    m_UpdateMessage      = "Processing the filter...";
    m_CommandObserver->SetCallbackFunction( this, &FilterModuleBase::ProgressUpdate );
    m_CumulatedProgress = 0.0;
    m_CurrentFilterProgressWeight = 1.0;
    m_ProcessComponentsIndependetly = true;
    }


  /**  Destructor */
  ~FilterModuleBase() 
    {
    }


  static 
  void Convert3DMarkerToIndex( const vtkVVPluginInfo  * info,
                               unsigned int markerId, IndexType & index )
  {
    const MarkersCoordinatesType * coordinates = info->Markers + 3 * markerId;
    for(unsigned int i=0; i<3; i++)
      { 
      index[i] =  static_cast< int >( 
         ( coordinates[i] - info->InputVolumeOrigin[i]   ) / 
                            info->InputVolumeSpacing[i]     );
      }
  }


  static 
  const char * GetInputVolumeScalarRange( const vtkVVPluginInfo  * info )
  {
    static char tmp[1024];
    double stepSize = 1.0;
    
  /* set the range of the sliders */
  if (info->InputVolumeScalarType == VTK_FLOAT || 
      info->InputVolumeScalarType == VTK_DOUBLE) 
    { 
    /* for float and double use a step size of 1/200 th the range */
    stepSize = info->InputVolumeScalarRange[1]*0.005 - 
      info->InputVolumeScalarRange[0]*0.005; 
    }
  const double lower = info->InputVolumeScalarRange[0];
  const double upper = info->InputVolumeScalarRange[1];
  sprintf(tmp,"%g %g %g", lower, upper, stepSize ); 
  return tmp;
  }

  static 
  const char * GetInputVolumeScalarRangeFraction( const vtkVVPluginInfo  * info , double fraction, double multiplier )
  {
    static char tmp[1024];
    const double lower = info->InputVolumeScalarRange[0];
    const double upper = info->InputVolumeScalarRange[1];
    const double resolution = upper * fraction - lower * fraction;
    const double maximumValue = resolution * multiplier;
    sprintf(tmp,"%g %g %g", resolution,maximumValue,resolution); 
    return tmp;
  }


static 
const char * GetInputVolumeScalarTypeRangeFraction( const vtkVVPluginInfo  * info , double fraction, double multiplier )
{
  static char tmp[1024];
  const double lower = info->InputVolumeScalarTypeRange[0];
  const double upper = info->InputVolumeScalarTypeRange[1];
  const double resolution = upper * fraction - lower * fraction;
  const double maximumValue = resolution * multiplier;
  sprintf(tmp,"%g %g %g", resolution,maximumValue,resolution); 
  return tmp;
}


  static 
  const char * GetInputVolumeScalarMidValue( const vtkVVPluginInfo  * info )
  {
    static char tmp[1024];
    const double lower = info->InputVolumeScalarRange[0];
    const double upper = info->InputVolumeScalarRange[1];
    const double midvalue = ( upper + lower ) / 2.0;
    sprintf( tmp,"%g", midvalue ); 
    return tmp;
  }


  static 
  const char * GetInputVolumeScalarTypeRange( const vtkVVPluginInfo  * info )
  {
    static char tmp[1024];
    double stepSize = 1.0;
    /* set the range of the sliders */
    if (info->InputVolumeScalarType == VTK_FLOAT || 
        info->InputVolumeScalarType == VTK_DOUBLE) 
      { 
      /* for float and double use a step size of 1/200 th the range */
      stepSize = info->InputVolumeScalarTypeRange[1]*0.005 - 
        info->InputVolumeScalarTypeRange[0]*0.005; 
      }
    const double lower = info->InputVolumeScalarTypeRange[0];
    const double upper = info->InputVolumeScalarTypeRange[1];
    sprintf(tmp,"%g %g %g", lower, upper, stepSize ); 
    return tmp;
  }

  static 
  const char * GetInputVolumeScalarTypeMidValue( const vtkVVPluginInfo  * info )
  {
    static char tmp[1024];
    const double lower = info->InputVolumeScalarTypeRange[0];
    const double upper = info->InputVolumeScalarTypeRange[1];
    const double midvalue = upper*0.5 + lower*0.5;
    sprintf( tmp,"%g", midvalue ); 
    return tmp;
  }

  static 
  const char * GetInputVolumeScalarMinimum( const vtkVVPluginInfo  * info )
  {
    static char tmp[1024];
    const double value = info->InputVolumeScalarRange[0];
    sprintf(tmp,"%g", value); 
    return tmp;
  }


  static 
  const char * GetInputVolumeScalarMaximum( const vtkVVPluginInfo  * info )
  {
    static char tmp[1024];
    const double value = info->InputVolumeScalarRange[1];
    sprintf(tmp,"%g", value); 
    return tmp;
  }

  static 
  const char * GetInputVolumeScalarTypeMinimum( const vtkVVPluginInfo  * info )
  {
    static char tmp[1024];
    const double value = info->InputVolumeScalarTypeRange[0];
    sprintf(tmp,"%g", value); 
    return tmp;
  }

  static 
  const char * GetInputVolumeScalarTypeMaximum( const vtkVVPluginInfo  * info )
  {
    static char tmp[1024];
    const double value = info->InputVolumeScalarTypeRange[1];
    sprintf(tmp,"%g", value); 
    return tmp;
  }


  static 
  const char * GetInputVolumeDimension( const vtkVVPluginInfo  * info, int axis )
  {
    static char tmp[1024];
    const float limit = info->InputVolumeDimensions[ axis ];
    sprintf(tmp,"%g %g %g", 0.0, limit, 1.0 ); 
    return tmp;
  }

  void SetUpdateMessage( const char * message )
  {
     m_UpdateMessage = message;
  }

  void InitializeProgressValue()
  {
     m_CumulatedProgress = 0.0;
     m_Info->UpdateProgress(m_Info, m_CumulatedProgress, m_UpdateMessage.c_str()); 
  }


  void SetCumulatedProgress( float progress )
  {
     m_CumulatedProgress = progress;
  }


  void SetCurrentFilterProgressWeight( float weight )
  {
     m_CurrentFilterProgressWeight = weight;
  }

  float GetCurrentFilterProgressWeight() const
  {
     return m_CurrentFilterProgressWeight;
  }


  void SetProcessComponentsIndependetly( bool independentProcessing )
  {
     m_ProcessComponentsIndependetly = independentProcessing;
  }

  CommandType *
  GetCommandObserver()
  {
     return m_CommandObserver;
  }



  void 
  ProgressUpdate( itk::Object * caller, const itk::EventObject & event )
  {

    bool  updateGUI = false;
    float progressForGUI;

    itk::ProcessObject::Pointer process =
              dynamic_cast< itk::ProcessObject *>( caller );

    if( typeid( itk::EndEvent ) == typeid( event ) )
      {
      m_CumulatedProgress += m_CurrentFilterProgressWeight;
      progressForGUI = m_CumulatedProgress;
      updateGUI = true;
      }

    if( typeid( itk::ProgressEvent ) == typeid( event ) )
      {
      const float currentFilterProgress = process->GetProgress();
      progressForGUI = m_CumulatedProgress + 
                       currentFilterProgress * m_CurrentFilterProgressWeight;
      updateGUI = true;
      }


    if( updateGUI )
      {
      if( m_ProcessComponentsIndependetly )
        {
        progressForGUI /= m_Info->InputVolumeNumberOfComponents;
        }
      m_Info->UpdateProgress(m_Info, progressForGUI, m_UpdateMessage.c_str()); 
      // Test whether during the GUI update, the Abort button was pressed
      int abort = atoi( m_Info->GetProperty( m_Info, VVP_ABORT_PROCESSING ) );
      if( abort )
        {
        process->SetAbortGenerateData(true);
        }
      }
  }


  /**  Set the Plugin Info structure */
  void
  SetPluginInfo( vtkVVPluginInfo * info )
  {
    m_Info = info;
  }


  /**  Get the Plugin Info structure */
  vtkVVPluginInfo *
  GetPluginInfo()
  {
    return m_Info;
  }


private:
    CommandType::Pointer         m_CommandObserver;
    vtkVVPluginInfo            * m_Info;
    std::string                  m_UpdateMessage;
    float                        m_CumulatedProgress;
    float                        m_CurrentFilterProgressWeight;
    bool                         m_ProcessComponentsIndependetly;

};


} // end namespace PlugIn

} // end namespace VolView

#endif
