

#include "DeformableModelApplication.h"


int main( int argc,  char * argv [] )
{

  DeformableModelApplication app;

  try
    {
    app.Show();
    Fl::run();
    }
  catch( std::exception & ex )
    {
    std::cerr << ex.what() << std::endl;
    }

  return 0;
}


