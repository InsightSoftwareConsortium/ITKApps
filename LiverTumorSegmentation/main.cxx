

#include "LiverTumorSegmentation.h"


int main( int ,  char * [] )
{

  LiverTumorSegmentation app;

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


