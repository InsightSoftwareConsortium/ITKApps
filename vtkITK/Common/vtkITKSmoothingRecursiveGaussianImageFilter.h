/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKSmoothingRecursiveGaussianImageFilter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
*/
// .NAME vtkITKSmoothingRecursiveGaussianImageFilter - Wrapper class around itk::SmoothingRecursiveGaussianImageFilterImageFilter
// .SECTION Description
// vtkITKSmoothingRecursiveGaussianImageFilter


#ifndef __vtkITKSmoothingRecursiveGaussianImageFilter_h
#define __vtkITKSmoothingRecursiveGaussianImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKSmoothingRecursiveGaussianImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkITKSmoothingRecursiveGaussianImageFilter *New();
  vtkTypeRevisionMacro(vtkITKSmoothingRecursiveGaussianImageFilter, vtkITKImageToImageFilterFF);

  void SetSigma( double s )
    {
    this->GetImageFilterPointer()->SetSigma(s);
    }
  void SetNormalizeAcrossScale( int n )
    {
    this->GetImageFilterPointer()->SetNormalizeAcrossScale(n);
    }


protected:
  //BTX
  typedef itk::SmoothingRecursiveGaussianImageFilter<Superclass::InputImageType,Superclass::InputImageType> ImageFilterType;
  vtkITKSmoothingRecursiveGaussianImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKSmoothingRecursiveGaussianImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKSmoothingRecursiveGaussianImageFilter(const vtkITKSmoothingRecursiveGaussianImageFilter&);  // Not implemented.
  void operator=(const vtkITKSmoothingRecursiveGaussianImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKSmoothingRecursiveGaussianImageFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKSmoothingRecursiveGaussianImageFilter);

#endif




