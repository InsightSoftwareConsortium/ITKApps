/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    GenerateThumbnail.cxx
  Language:  C++
  Date:      
  Version:     Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkExtractImageFilter.h"

int main( int argc, char ** argv )
{
  // Verify the number of parameters in the command line
  if( argc < 3 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFile  outputImageFile " << std::endl;
    return -1;
    }

  std::cout << "Running ITK Thumbnail generator on: " << argv[1] << std::endl;

  typedef  signed short   InputPixelType;
  typedef itk::Image< InputPixelType, 3 >    InputImageType;
  typedef unsigned char   OutputPixelType;
  typedef itk::Image< InputPixelType, 2 > InputImageType2;
  typedef itk::Image< OutputPixelType, 2 > OutputImageType;
 
  typedef itk::ImageFileReader< InputImageType >  ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  const char * inputFilename  = argv[1];
  reader->SetFileName( inputFilename );

  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject & e)
    {
    std::cerr << "exception in file reader " << std::endl;
    std::cerr << e.GetDescription() << std::endl;
    std::cerr << e.GetLocation() << std::endl;
    return EXIT_FAILURE;
    }

  // Extract the middle-axial slice
  typedef itk::ExtractImageFilter< InputImageType, InputImageType2 > FilterType;
  FilterType::Pointer filter = FilterType::New();
 
  InputImageType::RegionType inputRegion =
           reader->GetOutput()->GetLargestPossibleRegion();

  InputImageType::SizeType size = inputRegion.GetSize();
  size[2] = 0;

  InputImageType::IndexType start = inputRegion.GetIndex();
  const unsigned int sliceNumber = (unsigned int)((inputRegion.GetSize()[2])/2.0);
  start[2] = sliceNumber;

  std::cout << "Slice number = " << start[2] << std::endl;

  InputImageType::RegionType desiredRegion;
  desiredRegion.SetSize(  size  );
  desiredRegion.SetIndex( start );
  
  filter->SetInput(reader->GetOutput());
  filter->SetExtractionRegion( desiredRegion );
  filter->Update();

  typedef itk::RescaleIntensityImageFilter< 
               InputImageType2, OutputImageType > RescaleFilterType;

  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();

  rescaler->SetOutputMinimum(   0 );
  rescaler->SetOutputMaximum( 255 );
  
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[2] );
 
  rescaler->SetInput( filter->GetOutput() );
  writer->SetInput( rescaler->GetOutput() );

  try
    {
    writer->Update();
    }
  catch (itk::ExceptionObject & e)
    {
    std::cerr << "exception in file writer " << std::endl;
    std::cerr << e.GetDescription() << std::endl;
    std::cerr << e.GetLocation() << std::endl;
    return EXIT_FAILURE;
    }

  return 0;
}




