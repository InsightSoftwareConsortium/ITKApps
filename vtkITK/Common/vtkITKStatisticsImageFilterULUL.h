/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKStatisticsImageFilterULUL.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
*/
// .NAME vtkITKStatisticsImageFilterULUL - Wrapper class around itk::StatisticsImageFilterULUL
// .SECTION Description
// vtkITKStatisticsImageFilterULUL


#ifndef __vtkITKStatisticsImageFilterULUL_h
#define __vtkITKStatisticsImageFilterULUL_h


#include "vtkITKImageToImageFilterULUL.h"
#include "itkStatisticsImageFilter.h"
#include "vtkObjectFactory.h"

class VTK_EXPORT vtkITKStatisticsImageFilterULUL : public vtkITKImageToImageFilterULUL
{
 public:
  static vtkITKStatisticsImageFilterULUL *New();
  vtkTypeRevisionMacro(vtkITKStatisticsImageFilterULUL, vtkITKImageToImageFilterULUL);

  double GetMinimum()
  {
    DelegateITKOutputMacro(GetMinimum);
  }
  double GetMaximum()
  {
    DelegateITKOutputMacro(GetMaximum);
  }
  double GetMean()
  {
    DelegateITKOutputMacro(GetMean);
  }
  double GetSigma()
  {
    DelegateITKOutputMacro(GetSigma);
  }
  double GetVariance()
  {
    DelegateITKOutputMacro(GetVariance);
  }
  double GetSum()
  {
    DelegateITKOutputMacro(GetSum);
  }

protected:
  //BTX
  typedef itk::StatisticsImageFilter<Superclass::InputImageType> ImageFilterType;
  
  vtkITKStatisticsImageFilterULUL() : Superclass ( ImageFilterType::New() ) {};
  ~vtkITKStatisticsImageFilterULUL() {};
  //ETX

private:
  vtkITKStatisticsImageFilterULUL(const vtkITKStatisticsImageFilterULUL&);  // Not implemented.
  void operator=(const vtkITKStatisticsImageFilterULUL&);  //
                                                                          // Not implemented
  
};

vtkCxxRevisionMacro(vtkITKStatisticsImageFilterULUL, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKStatisticsImageFilterULUL);

#endif


