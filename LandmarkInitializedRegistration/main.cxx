#include "itkWin32Header.h"
#include "guiMainImplementation.h"

int main()
{
  guiMainImplementation* gui = new guiMainImplementation();
  gui->Show();
  return Fl::run();
}
