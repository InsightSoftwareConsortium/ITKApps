#include "InteractorObserver.h"

#include <FL/Fl.H>




InteractorObserver
::InteractorObserver()
{
  m_SliceViewer = NULL;
  m_Motion = false;
}




InteractorObserver
::~InteractorObserver()
{
  m_SliceViewer = NULL;
}



void
InteractorObserver
::SetImageSliceViewer( ImageSliceViewer * viewer )
{
  m_SliceViewer = viewer;
}



void 
InteractorObserver
::Execute(vtkObject * caller, unsigned long eventId, void *callData)
{
  if ( eventId == ::vtkCommand::LeftButtonPressEvent )
    {
    m_Motion = true;
    }
  if ( eventId == ::vtkCommand::LeftButtonReleaseEvent )
    {
    m_Motion = false;
    }
  if( m_SliceViewer && m_Motion )
    {
      //point selected when mouse button pressed!!
    const int x = Fl::event_x();
    const int y = Fl::event_y();
    m_SliceViewer->SelectPoint( x, y );
    }
}






