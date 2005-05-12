/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
*/
// .NAME vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF - Wrapper class around itk::GradientMagnitudeRecursiveGaussianImageFilterImageFilter
// .SECTION Description
// vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF


#ifndef __vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF_h
#define __vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF : public vtkITKImageToImageFilterFF
{
 public:
  static vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF *New();
  vtkTypeRevisionMacro(vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF, vtkITKImageToImageFilterFF);


  // Description:
  // vtkITK filters can be told to release their bulk output data
  // during a pipeline update.  Setting the
  // ReleaseDataBeforeUpdateFlag can minimize peak memory utilization
  // during a pipeline update.
  virtual void SetReleaseDataBeforeUpdateFlag(int i)
    { DelegateSetMacro(ReleaseDataBeforeUpdateFlag, i); }
  virtual int GetReleaseDataBeforeUpdateFlag()
    { DelegateGetMacro(ReleaseDataBeforeUpdateFlag); }

  // Description:
  // Set the standard deviation of the gaussian used for smoothing
  // (measured in mm).
  void SetSigma( double s )
    { DelegateSetMacro(Sigma, s); }

  // Description:
  // Normalize the gaussian kernel for consistent smoothed values across
  // scale space.
  void SetNormalizeAcrossScale( int n )
    { DelegateSetMacro(NormalizeAcrossScale,n); }

protected:
  //BTX
  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF(const vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF&);  // Not implemented.
  void operator=(const vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKGradientMagnitudeRecursiveGaussianImageFilterFF);

#endif




