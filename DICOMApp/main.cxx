

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <iostream>
#include <vector>

#include "DICOMParser.h"
#include "DICOMCallback.h"
#include "DICOMAppHelper.h"

#include "itkImage.h"
#include "itkDICOMImageIO2.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkDICOMImageIO2Factory.h"
#include "itkPNGImageIO.h"
#include "itkExceptionObject.h"
#include "itkRawImageIO.h"
#include "itkRescaleIntensityImageFilter.h"

#include <stdlib.h>



int main(int argc, char* argv[])
{
  if (argc < 2)
    {
    std::cout << std::endl;
    std::cout << "Usage: DICOMApp file1 file2..." << std::endl;
    std::cout << std::endl;
    std::cout << "==================================================================" << std::endl;
    std::cout << "Image data will be written to file1.raw file2.raw and file1.png file2.png" << std::endl;
    std::cout << "Some header values will be written to file1.header.txt file2.header.txt" << std::endl;
    std::cout << "==================================================================" << std::endl;
    std::cout << std::endl;
    std::cout.flush();
    return EXIT_FAILURE;
    }  

  itk::ObjectFactoryBase::RegisterFactory(itk::DICOMImageIO2Factory::New());

  itk::DICOMImageIO2::Pointer DICOMImage = 
    itk::DICOMImageIO2::New();

  typedef signed short PixelType;
  const unsigned int   Dimension = 2;

  typedef itk::Image<PixelType, Dimension > imgType;
  // typedef itk::Image<itk::RGBPixel<unsigned char>, 2> pngType;
  typedef itk::Image<unsigned char, 2> pngType;
  
  itk::ImageFileReader<imgType>::Pointer fileReader  = itk::ImageFileReader<imgType>::New();

  typedef itk::RescaleIntensityImageFilter< imgType, pngType > RescaleFilterType;

  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
  rescaler->SetInput(fileReader->GetOutput());
  rescaler->SetOutputMinimum( 0 );
  rescaler->SetOutputMaximum( 255 );

  typedef itk::ImageFileWriter<pngType> PNGImageWriterType;
  PNGImageWriterType::Pointer pngWriter = PNGImageWriterType::New();
  pngWriter->SetInput(rescaler->GetOutput());
  
  typedef itk::RawImageIO<PixelType,Dimension>  RAWImageIOType;
  RAWImageIOType::Pointer rawImageIO = RAWImageIOType::New();

  typedef itk::ImageFileWriter<imgType> RAWImageWriterType;
  RAWImageWriterType::Pointer rawWriter = RAWImageWriterType::New();
  rawWriter->SetInput(fileReader->GetOutput());
  rawWriter->SetImageIO( rawImageIO );

  for (int i = 1; i < argc; i++)
    {
    const char* filename = argv[i];

    try 
      {

      std::cout << std::endl;
      std::cout << "========== Reading " << filename << " ===========" << std::endl;
      std::cout << std::endl;

      fileReader->SetFileName(filename);
      fileReader->UpdateLargestPossibleRegion();
    
      rawImageIO->SetFileTypeToBinary();
      rawImageIO->SetByteOrderToLittleEndian();

      std::cout << std::endl;
      std::string rawFilename = std::string(filename) + ".raw";
      std::cout << "Writing: " << rawFilename << std::endl;
      rawWriter->SetFileName(rawFilename.c_str());
      rawWriter->UpdateLargestPossibleRegion();
      rawWriter->Write();
      std::cout << std::endl;

      std::string outputFilename = std::string(filename) + ".png";
      pngWriter->SetFileName(outputFilename.c_str());
      std::cout << std::endl;
      std::cout << "Writing: " << outputFilename << std::endl;

      rescaler->UpdateLargestPossibleRegion();
      pngWriter->UpdateLargestPossibleRegion();
      pngWriter->Write();
      }
    catch (itk::ExceptionObject e)
      {
      std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
      std::cerr << " Caught the following exception : " << std::endl;
      std::cerr << e << std::endl;
      std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
      }
  }  

  return EXIT_SUCCESS;
}


