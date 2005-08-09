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

int main( int argc, char ** argv )
{
  // Verify the number of parameters in the command line
  if( argc < 3 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImageFile  outputImageFile " << std::endl;
    return -1;
    }

  typedef  signed short      PixelType;
  const   unsigned int       Dimension = 2;
  typedef itk::Image< PixelType, Dimension >    ImageType;

  typedef itk::ImageFileReader< ImageType >  ReaderType;

  ReaderType::Pointer reader = ReaderType::New();

  const char * inputFilename  = argv[1];
  reader->SetFileName( inputFilename );

  typedef unsigned char WritePixelType;
  typedef itk::Image< WritePixelType, 2 > WriteImageType;
  typedef itk::RescaleIntensityImageFilter< 
               ImageType, WriteImageType > RescaleFilterType;

  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();

  rescaler->SetOutputMinimum(   0 );
  rescaler->SetOutputMaximum( 255 );
  
  typedef itk::ImageFileWriter< WriteImageType >  Writer2Type;
  Writer2Type::Pointer writer2 = Writer2Type::New();
  writer2->SetFileName( argv[2] );
 
  rescaler->SetInput( reader->GetOutput() );
  writer2->SetInput( rescaler->GetOutput() );

  try
    {
    writer2->Update();
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




