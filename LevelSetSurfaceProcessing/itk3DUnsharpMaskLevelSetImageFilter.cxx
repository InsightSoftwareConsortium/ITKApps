#include <iostream>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkUnsharpMaskLevelSetImageFilter.h"

int main( int argc, char **argv )
{
  if (argc < 6 )
    {
      std::cerr << "Missing Parameters " << std::endl;
      std::cerr << "Usage: " << argv[0];
      std::cerr <<
        " inputVolume outputVolume isosurface_value number_of_iterations unsharp_mask_weight"
                << std::endl;
      return 1;
    }
 
  typedef float PixelType;
  typedef itk::Image < PixelType, 3 >  ImageType;
  typedef itk::ImageFileReader < ImageType > FileReaderType;
  typedef itk::ImageFileWriter < ImageType > FileWriterType;
  typedef itk::UnsharpMaskLevelSetImageFilter
    <ImageType, ImageType> FilterType;
  
  FilterType::Pointer filter = FilterType::New();

  filter->SetIsoSurfaceValue( atof(argv[3]) );
  filter->SetMaxFilterIteration( atoi(argv[4]) );
  filter->SetNormalProcessUnsharpWeight( ::atof (argv[5]) );
  
  FileReaderType::Pointer reader = FileReaderType::New();
  reader->SetFileName(argv[1]);
   
  FileWriterType::Pointer writer = FileWriterType::New();
  writer->SetFileName(argv[2]);
  
  filter->SetInput(reader->GetOutput());
  writer->SetInput(filter->GetOutput());
  
  try {
    writer->Update();
  }
  catch (itk::ExceptionObject &e)
    {
      std::cerr << e << std::endl;
    }
 
 
  return 0;
}
