/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKCurvatureAnisotropicDiffusionImageFilterSF.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
*/
// .NAME vtkITKCurvatureAnisotropicDiffusionImageFilterSF - Wrapper class around itk::CurvatureAnisotropicDiffusionImageFilterImageFilter
// .SECTION Description
// vtkITKCurvatureAnisotropicDiffusionImageFilterSF


#ifndef __vtkITKCurvatureAnisotropicDiffusionImageFilterSF_h
#define __vtkITKCurvatureAnisotropicDiffusionImageFilterSF_h


#include "vtkITKImageToImageFilterSF.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKCurvatureAnisotropicDiffusionImageFilterSF : public vtkITKImageToImageFilterSF
{
 public:
  static vtkITKCurvatureAnisotropicDiffusionImageFilterSF *New();
  vtkTypeRevisionMacro(vtkITKCurvatureAnisotropicDiffusionImageFilterSF, vtkITKImageToImageFilterSF);

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
  vtkITKCurvatureAnisotropicDiffusionImageFilterSF() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKCurvatureAnisotropicDiffusionImageFilterSF() {};
  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKCurvatureAnisotropicDiffusionImageFilterSF(const vtkITKCurvatureAnisotropicDiffusionImageFilterSF&);  // Not implemented.
  void operator=(const vtkITKCurvatureAnisotropicDiffusionImageFilterSF&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKCurvatureAnisotropicDiffusionImageFilterSF, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKCurvatureAnisotropicDiffusionImageFilterSF);

#endif




