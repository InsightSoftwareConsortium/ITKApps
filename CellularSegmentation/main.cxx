

#include "CellularSegmentationApplication.h"


int main( int argc,  char * argv [] )
{

  CellularSegmentationApplication app;

  try
    {
    app.Show();

    if( argc > 1 )
      {
      Fl::check();
      app.Load( argv[1] );
      }
    
    Fl::run();
    }
  catch( std::exception & ex )
    {
    std::cerr << ex.what() << std::endl;
    }

  return 0;
}


