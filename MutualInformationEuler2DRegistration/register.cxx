#include "RegisterWindow.h"

int main(int, char*[])
{

  try 
    {
    RegisterWindow* win = new RegisterWindow();
    win->Show();
    Fl::run();
    delete win ;
    }
  catch(...)
    {
    std::cerr << "Exception caught in main" << std::endl;
    }

  return 0 ;
}




