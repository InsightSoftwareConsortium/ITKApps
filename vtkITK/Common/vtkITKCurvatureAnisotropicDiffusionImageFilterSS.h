/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKCurvatureAnisotropicDiffusionImageFilterSS.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
*/
// .NAME vtkITKCurvatureAnisotropicDiffusionImageFilterSS - Wrapper class around itk::CurvatureAnisotropicDiffusionImageFilterImageFilter
// .SECTION Description
// vtkITKCurvatureAnisotropicDiffusionImageFilterSS


#ifndef __vtkITKCurvatureAnisotropicDiffusionImageFilterSS_h
#define __vtkITKCurvatureAnisotropicDiffusionImageFilterSS_h


#include "vtkITKImageToImageFilterSS.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKCurvatureAnisotropicDiffusionImageFilterSS : public vtkITKImageToImageFilterSS
{
 public:
  static vtkITKCurvatureAnisotropicDiffusionImageFilterSS *New();
  vtkTypeRevisionMacro(vtkITKCurvatureAnisotropicDiffusionImageFilterSS, vtkITKImageToImageFilterSS);

  double GetTimeStep ()
  {
    DelegateITKOutputMacro(GetTimeStep) ;
  };

  double GetConductanceParameter ()
  {
    DelegateITKOutputMacro(GetConductanceParameter) ;
  };

  unsigned int GetNumberOfIterations ()
  {
    DelegateITKOutputMacro(GetNumberOfIterations) ;
  };

  void SetNumberOfIterations( unsigned int value )
  {
    DelegateITKInputMacro ( SetNumberOfIterations, value );
  };

  void SetTimeStep ( double value )
  {
    DelegateITKInputMacro ( SetTimeStep, value );
  };

  void SetConductanceParameter ( double value )
  {
    DelegateITKInputMacro ( SetConductanceParameter, value );
  };

protected:
  //BTX
  typedef itk::CurvatureAnisotropicDiffusionImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkITKCurvatureAnisotropicDiffusionImageFilterSS() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKCurvatureAnisotropicDiffusionImageFilterSS() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKCurvatureAnisotropicDiffusionImageFilterSS(const vtkITKCurvatureAnisotropicDiffusionImageFilterSS&);  // Not implemented.
  void operator=(const vtkITKCurvatureAnisotropicDiffusionImageFilterSS&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKCurvatureAnisotropicDiffusionImageFilterSS, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKCurvatureAnisotropicDiffusionImageFilterSS);

#endif




