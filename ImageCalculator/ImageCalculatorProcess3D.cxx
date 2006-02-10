#include "ImageCalculatorTemplates.h"

//This program calls the 3d function to process the algorithm.
void ImageCalculatorProcess3D(const std::string & InType,MetaCommand command)
{
    ImageCalculatorProcessND<3>(InType,command);
}
