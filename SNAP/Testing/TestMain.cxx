/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    TestMain.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
// Define the verbose output stream
#include "SNAPTestDriver.h"

std::ostream &verbose = std::cout;

extern void testVTKImport();

void test01()
{
  // We are converting read data from RGB pixel image
  typedef itk::RGBPixel<unsigned char> RGBPixelType;
  typedef itk::Image<RGBPixelType,2> RGBImageType;

  // Typedefs
  typedef itk::ImageFileReader<RGBImageType> ReaderType;
  typedef itk::PNGImageIO IOType;
  typedef itk::ImageRegionIterator<RGBImageType> RGBIteratorType;
  typedef itk::ImageRegionIterator<ExampleImageType> IteratorType;

  // Load a 2D grey level image
  typedef itk::Image<unsigned char,2> InputImageType;

  // Scale and shift constants
  const float scale[2] = {1.0f / 255.0f, 2.0f / 255.0f };
  const float shift[2] = {0.0f, -1.0f };

  // Load each of these images
  ReaderType::Pointer reader = ReaderType::New();
  IOType::Pointer io = IOType::New();
  reader->SetImageIO(io);
  reader->SetFileName("c:/grant/app/InsightApplications/SNAP/ProgramData/Images2D/Outline.png");
  reader->Update();

  // Allocate the example image
  InputImageType::Pointer image = ExampleImageType::New();
  image->SetRegions(reader->GetOutput()->GetBufferedRegion());
  image->Allocate();

  // Scale the image into the range (image is RGB 0..255)
  RGBIteratorType 
    itColor(reader->GetOutput(),reader->GetOutput()->GetBufferedRegion());
  IteratorType it(image,image->GetBufferedRegion());

  for(itColor.GoToBegin();!itColor.IsAtEnd();++it,++itColor)
    {
    it.Value() = itColor.Value().GetLuminance() * scale[i] + shift[i];
    }
}

int main(int argc, char *argv[]) 
{
  SNAPTestDriver driver;
  driver.Run(argc,argv);
  return 0;
}
