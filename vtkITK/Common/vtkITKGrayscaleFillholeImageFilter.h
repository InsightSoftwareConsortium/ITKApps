/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKGrayscaleFillholeImageFilter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
*/
// .NAME vtkITKGrayscaleFillholeImageFilter - Wrapper class around itk::GrayscaleFillholeImageFilterImageFilter
// .SECTION Description
// vtkITKGrayscaleFillholeImageFilter


#ifndef __vtkITKGrayscaleFillholeImageFilter_h
#define __vtkITKGrayscaleFillholeImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkGrayscaleFillholeImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKGrayscaleFillholeImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkITKGrayscaleFillholeImageFilter *New();
  vtkTypeRevisionMacro(vtkITKGrayscaleFillholeImageFilter, vtkITKImageToImageFilterFF);

protected:
  //BTX
  typedef itk::GrayscaleFillholeImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkITKGrayscaleFillholeImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKGrayscaleFillholeImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKGrayscaleFillholeImageFilter(const vtkITKGrayscaleFillholeImageFilter&);  // Not implemented.
  void operator=(const vtkITKGrayscaleFillholeImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKGrayscaleFillholeImageFilter, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkITKGrayscaleFillholeImageFilter);

#endif




