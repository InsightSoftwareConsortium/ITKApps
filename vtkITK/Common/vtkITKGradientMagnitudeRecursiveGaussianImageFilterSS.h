/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
*/
// .NAME vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS - Wrapper class around itk::GradientMagnitudeRecursiveGaussianImageFilterImageFilter
// .SECTION Description
// vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS


#ifndef __vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS_h
#define __vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS_h


#include "vtkITKImageToImageFilterSS.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS : public vtkITKImageToImageFilterSS
{
 public:
  static vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS *New();
  vtkTypeRevisionMacro(vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS, vtkITKImageToImageFilterSS);


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
  vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS(const vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS&);  // Not implemented.
  void operator=(const vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS);

#endif




