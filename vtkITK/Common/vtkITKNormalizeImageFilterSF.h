/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKNormalizeImageFilterSF.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
*/
// .NAME vtkITKNormalizeImageFilterSF - Wrapper class around itk::NormalizeImageFilterImageFilter
// .SECTION Description
// vtkITKNormalizeImageFilterSF


#ifndef __vtkITKNormalizeImageFilterSF_h
#define __vtkITKNormalizeImageFilterSF_h


#include "vtkITKImageToImageFilterSF.h"
#include "itkNormalizeImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKNormalizeImageFilterSF : public vtkITKImageToImageFilterSF
{
 public:
  static vtkITKNormalizeImageFilterSF *New();
  vtkTypeRevisionMacro(vtkITKNormalizeImageFilterSF, vtkITKImageToImageFilterSF);

protected:
  //BTX
  typedef itk::NormalizeImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkITKNormalizeImageFilterSF() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKNormalizeImageFilterSF() {};

  //ETX
  
private:
  vtkITKNormalizeImageFilterSF(const vtkITKNormalizeImageFilterSF&);  // Not implemented.
  void operator=(const vtkITKNormalizeImageFilterSF&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKNormalizeImageFilterSF, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKNormalizeImageFilterSF);

#endif




