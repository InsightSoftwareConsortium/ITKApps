/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKWatershedImageFilterSUL.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
*/
// .NAME vtkITKWatershedImageFilterSUL - Wrapper class around itk::WatershedImageFilterImageFilter
// .SECTION Description
// vtkITKWatershedImageFilterSUL


#ifndef __vtkITKWatershedImageFilterSUL_h
#define __vtkITKWatershedImageFilterSUL_h


#include "vtkITKImageToImageFilterSUL.h"
#include "itkWatershedImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKWatershedImageFilterSUL : public vtkITKImageToImageFilterSUL
{
 public:
  static vtkITKWatershedImageFilterSUL *New();
  vtkTypeRevisionMacro(vtkITKWatershedImageFilterSUL, vtkITKImageToImageFilterSUL);

  void SetThreshold ( double d ) { DelegateSetMacro ( Threshold, d ); };
  double GetThreshold () { DelegateGetMacro ( Threshold ); };
  void SetLevel ( double d ) { DelegateSetMacro ( Level, d ); };
  double GetLevel () { DelegateGetMacro ( Level ); };

  // Description:
  // vtkITK filters can be told to release their bulk output data
  // during a pipeline update.  Setting the
  // ReleaseDataBeforeUpdateFlag can minimize peak memory utilization
  // during a pipeline update.
  virtual void SetReleaseDataBeforeUpdateFlag(int i)
    { DelegateSetMacro(ReleaseDataBeforeUpdateFlag, i); }
  virtual int GetReleaseDataBeforeUpdateFlag()
    { DelegateGetMacro(ReleaseDataBeforeUpdateFlag); }

protected:
  //BTX
  typedef itk::WatershedImageFilter<Superclass::InputImageType> ImageFilterType;
  vtkITKWatershedImageFilterSUL() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKWatershedImageFilterSUL() {};

  //ETX
  
private:
  vtkITKWatershedImageFilterSUL(const vtkITKWatershedImageFilterSUL&);  // Not implemented.
  void operator=(const vtkITKWatershedImageFilterSUL&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKWatershedImageFilterSUL, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKWatershedImageFilterSUL);

#endif




