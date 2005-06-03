/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    InteractorObserver.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ISISImageInteractor_h
#define __ISISImageInteractor_h

#include "vtkCommand.h"
#include "DeformableRegistration3DTimeSeriesBase.h"
#include "vtkInteractorStyleImage.h"

namespace fltk{
class InteractorObserver : public vtkCommand
{
public:

  static InteractorObserver *New() 
  {
    return new InteractorObserver;
  }
  void Execute(vtkObject * caller, unsigned long eventId, void *callData);

  void SetImageSliceViewer(DeformableRegistration3DTimeSeriesBase* viewer);
protected:
  InteractorObserver();
  
  virtual ~InteractorObserver();

private:

  DeformableRegistration3DTimeSeriesBase  * m_SliceViewer;

};
class ISISImageInteractor: public vtkInteractorStyleImage
  {
public:
  static ISISImageInteractor *New();
  vtkTypeRevisionMacro(ISISImageInteractor,vtkInteractorStyleImage);
 // void PrintSelf(ostream& os, vtkIndent indent);

  virtual void OnMiddleButtonDown  ()
  {
  }
protected:
  ISISImageInteractor();
  ~ISISImageInteractor();
private:
  ISISImageInteractor(const ISISImageInteractor&);  // Not implemented.
  void operator=(const ISISImageInteractor&);  // Not implemented.
};
}//end namespace
#endif



