#include "ImageCalculatorTemplates.h"

//This program calls the 2d function to process the algorithm.
void ImageCalculatorProcess2D(const std::string & InType,MetaCommand command)
{
    ImageCalculatorProcessND<2>(InType,command);
}
