
#include <qpushbutton.h>
#include "itkCommand.h"

namespace itk {

  class QtLightIndicator : public QPushButton
{

  Q_OBJECT

public:

  /** Constructor */
  QtLightIndicator( QWidget *parent ):QPushButton( parent ) {}

public slots:

  void Start()
    {
    QColor yellow(255,255,0);
    QPalette pal = this->palette( );
    pal.setColor(QPalette::Window, yellow);
    }

  void Modified()
    {
    QColor red(255,0,0);
    QPalette pal = this->palette( );
    pal.setColor(QPalette::Window, red);
    }

  void End()
    {
    QColor green(0,255,0);
    QPalette pal = this->palette( );
    pal.setColor(QPalette::Window, green);
    }

};


}  // end of namespace
