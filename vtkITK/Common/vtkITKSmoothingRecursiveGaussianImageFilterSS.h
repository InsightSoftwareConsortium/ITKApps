/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKSmoothingRecursiveGaussianImageFilterSS.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
*/
// .NAME vtkITKSmoothingRecursiveGaussianImageFilterSS - Wrapper class around itk::SmoothingRecursiveGaussianImageFilterImageFilter
// .SECTION Description
// vtkITKSmoothingRecursiveGaussianImageFilterSS


#ifndef __vtkITKSmoothingRecursiveGaussianImageFilterSS_h
#define __vtkITKSmoothingRecursiveGaussianImageFilterSS_h


#include "vtkITKImageToImageFilterSS.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKSmoothingRecursiveGaussianImageFilterSS : public vtkITKImageToImageFilterSS
{
 public:
  static vtkITKSmoothingRecursiveGaussianImageFilterSS *New();
  vtkTypeRevisionMacro(vtkITKSmoothingRecursiveGaussianImageFilterSS, vtkITKImageToImageFilterSS);

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
  typedef itk::SmoothingRecursiveGaussianImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkITKSmoothingRecursiveGaussianImageFilterSS() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKSmoothingRecursiveGaussianImageFilterSS() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKSmoothingRecursiveGaussianImageFilterSS(const vtkITKSmoothingRecursiveGaussianImageFilterSS&);  // Not implemented.
  void operator=(const vtkITKSmoothingRecursiveGaussianImageFilterSS&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKSmoothingRecursiveGaussianImageFilterSS, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkITKSmoothingRecursiveGaussianImageFilterSS);

#endif




