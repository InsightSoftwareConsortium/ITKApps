/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKRelabelComponentImageFilter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
*/
// .NAME vtkITKRelabelComponentImageFilter - Wrapper class around itk::RelabelComponentImageFilterImageFilter
// .SECTION Description
// vtkITKRelabelComponentImageFilter


#ifndef __vtkITKRelabelComponentImageFilter_h
#define __vtkITKRelabelComponentImageFilter_h


#include "vtkITKImageToImageFilterULUL.h"
#include "itkRelabelComponentImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKRelabelComponentImageFilter : public vtkITKImageToImageFilterULUL
{
 public:
  static vtkITKRelabelComponentImageFilter *New();
  vtkTypeRevisionMacro(vtkITKRelabelComponentImageFilter, vtkITKImageToImageFilterULUL);

  
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
  // Get the number of objects in the label map (excluding the
  // background). Can only be called after an Update().
  unsigned long GetNumberOfObjects() { return this->GetImageFilterPointer()->GetNumberOfObjects(); }

  // Description:
  // Get the size of an object in pixels. Can only be
  // called after an Update().vtkITKRelabelComponentImageFilter
  unsigned long GetSizeOfObjectInPixels( unsigned long obj )
    { return this->GetImageFilterPointer()->GetSizeOfObjectInPixels(obj); }

  // Description:
  // Get the size of an object in physical units. Can
  // only be called after an Update().
  float GetSizeOfObjectInPhysicalUnits( unsigned long obj )
    { return this->GetImageFilterPointer()->GetSizeOfObjectInPhysicalUnits(obj); }
  
protected:
  //BTX
  typedef itk::RelabelComponentImageFilter<Superclass::InputImageType, Superclass::OutputImageType> ImageFilterType;
  vtkITKRelabelComponentImageFilter() : Superclass ( ImageFilterType::New() ){};
  ~vtkITKRelabelComponentImageFilter() {};

  ImageFilterType* GetImageFilterPointer() { return dynamic_cast<ImageFilterType*> ( m_Filter.GetPointer() ); }

  //ETX
  
private:
  vtkITKRelabelComponentImageFilter(const vtkITKRelabelComponentImageFilter&);  // Not implemented.
  void operator=(const vtkITKRelabelComponentImageFilter&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkITKRelabelComponentImageFilter, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKRelabelComponentImageFilter);

#endif




