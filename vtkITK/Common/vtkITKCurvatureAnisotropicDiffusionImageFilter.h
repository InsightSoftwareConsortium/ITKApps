/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKCurvatureAnisotropicDiffusionImageFilter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
*/
// .NAME vtkITKCurvatureAnisotropicDiffusionImageFilter - Wrapper class around itk::CurvatureAnisotropicDiffusionImageFilterImageFilter
// .SECTION Description
// vtkITKCurvatureAnisotropicDiffusionImageFilter


#ifndef __vtkITKCurvatureAnisotropicDiffusionImageFilter_h
#define __vtkITKCurvatureAnisotropicDiffusionImageFilter_h


#include "vtkITKImageToImageFilterFF.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKCurvatureAnisotropicDiffusionImageFilter : public vtkITKImageToImageFilterFF
{
 public:
  static vtkITKCurvatureAnisotropicDiffusionImageFilter *New();
  vtkTypeRevisionMacro(vtkITKCurvatureAnisotropicDiffusionImageFilter, vtkITKImageToImageFilterFF);

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

  void SetUseImageSpacing ( int value )
  {
    DelegateITKInputMacro ( SetUseImageSpacing, (bool) value );
  }
  void UseImageSpacingOn()
  {
    this->SetUseImageSpacing (true);
  }
  void UseImageSpacingOff()
  {
    this->SetUseImageSpacing (false);
  }
  int GetUseImageSpacing()
  {
    DelegateITKOutputMacro ( GetUseImageSpacing );
  }

protected:
  //BTX
  typedef itk::CurvatureAnisotropicDiffusionImageFilter<Superclass::InputImageType,Superclass::OutputImageType> ImageFilterType;
  vtkITKCurvatureAnisotropicDiffusionImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKCurvatureAnisotropicDiffusionImageFilter() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKCurvatureAnisotropicDiffusionImageFilter(const vtkITKCurvatureAnisotropicDiffusionImageFilter&);  // Not implemented.
  void operator=(const vtkITKCurvatureAnisotropicDiffusionImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKCurvatureAnisotropicDiffusionImageFilter, "$Revision: 1.5 $");
vtkStandardNewMacro(vtkITKCurvatureAnisotropicDiffusionImageFilter);

#endif




