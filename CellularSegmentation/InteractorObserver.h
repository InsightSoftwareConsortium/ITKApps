#include "vtkCommand.h"
#include "ImageSliceViewer.h"

/** 
  \class InteractorObserver
  \brief This class defines observer command for user event
         (Keyboard/Mouse)in a ImageSliceViewer window. 
*/


class InteractorObserver : public vtkCommand
{
public:

  static InteractorObserver *New() 
    {return new InteractorObserver;};

  /** The SetImageSliceViewer method inputs pointer of the ImageSliceViewer
      serviced.
  */
  void SetImageSliceViewer( ImageSliceViewer * viewer );

  /** The Execute method defines the code to be run when the 
  command is invoked.
  */
  void Execute(vtkObject * caller, unsigned long eventId, void *callData);
   
protected:

  InteractorObserver();
  virtual ~InteractorObserver();

private:

  ImageSliceViewer   * m_SliceViewer;
  bool                 m_Motion;

};






