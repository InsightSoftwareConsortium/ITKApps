/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKOtsuThresholdImageFilter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
*/
// .NAME vtkITKOtsuThresholdImageFilter - Wrapper class around itk::OtsuThresholdImageFilterImageFilter
// .SECTION Description
// vtkITKOtsuThresholdImageFilter


#ifndef __vtkITKOtsuThresholdImageFilter_h
#define __vtkITKOtsuThresholdImageFilter_h


#include "vtkITKImageToImageFilterSS.h"
#include "itkOtsuThresholdImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKOtsuThresholdImageFilter : public vtkITKImageToImageFilterSS
{
 public:
  static vtkITKOtsuThresholdImageFilter *New();
  vtkTypeRevisionMacro(vtkITKOtsuThresholdImageFilter, vtkITKImageToImageFilterSS);

  void SetOutsideValue (short value )
  {
     DelegateITKInputMacro ( SetOutsideValue, value );
  };
  short GetOutsideValue ()
  {
    DelegateITKOutputMacro(GetOutsideValue) ;
  };

  void SetInsideValue (short value )
  {
     DelegateITKInputMacro ( SetInsideValue, value );
  };
  short GetInsideValue ()
  {
    DelegateITKOutputMacro(GetInsideValue) ;
  };
  void SetNumberOfHistogramBins (unsigned long value )
  {
     DelegateITKInputMacro ( SetNumberOfHistogramBins, value );
  };
  unsigned long GetNumberOfHistogramBins ()
  {
    DelegateITKOutputMacro(GetNumberOfHistogramBins) ;
  };

  short GetThreshold ()
  {
    DelegateITKOutputMacro(GetThreshold) ;
  };


protected:
  //BTX
  typedef itk::OtsuThresholdImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkITKOtsuThresholdImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKOtsuThresholdImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKOtsuThresholdImageFilter(const vtkITKOtsuThresholdImageFilter&);  // Not implemented.
  void operator=(const vtkITKOtsuThresholdImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKOtsuThresholdImageFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKOtsuThresholdImageFilter);

#endif




