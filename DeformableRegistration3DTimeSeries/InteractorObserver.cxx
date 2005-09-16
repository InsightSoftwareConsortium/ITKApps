/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    InteractorObserver.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "InteractorObserver.h"

#include <FL/Fl.H>
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"
namespace fltk{

InteractorObserver
::InteractorObserver()
{
  m_SliceViewer = NULL;
}

InteractorObserver
::~InteractorObserver()
{
  m_SliceViewer = NULL;
}

void
InteractorObserver
::SetImageSliceViewer(DeformableRegistration3DTimeSeriesBase * viewer )
{
  m_SliceViewer = viewer;
}

void 
InteractorObserver
::Execute(vtkObject * caller, unsigned long eventId, void *callData)
{
  const int x = Fl::event_x();
  const int y = Fl::event_y();

  if( m_SliceViewer &&(eventId == ::vtkCommand::MiddleButtonPressEvent))
  {
    m_SliceViewer->SelectPoint( x, y ,1);
    //A very simple window and level
    //For a real one see vtk source code for vtkImagViewer.cxx
    //m_SliceViewer->SelectWindowLevel( x, y );
  }
  if( m_SliceViewer&&(eventId == ::vtkCommand::LeftButtonPressEvent))  
  {
    m_SliceViewer->SelectPoint( x, y ,0);
  }
if( m_SliceViewer&&(eventId == ::vtkCommand::RightButtonPressEvent))  
  {

    m_SliceViewer->SelectPoint( x, y ,1);
  }
}

vtkCxxRevisionMacro(ISISImageInteractor, "$Revision: 1.2 $");
vtkStandardNewMacro(ISISImageInteractor);

ISISImageInteractor::ISISImageInteractor() 
{

}
ISISImageInteractor::~ISISImageInteractor() 
{
}

}//end namespace




