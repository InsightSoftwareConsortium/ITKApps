/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    CLRegTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
//
// Test program to evaluate output of the InverseConsistentLandmarkRegistration program.
//
// Basically it ends up being a re-implementation of 
// this brains2 script 
//
// set x1 [ b2 load image test/CLReg2/WarpForward_xdisp.hdr ]
// set x2 [ b2 load image LMKREG_RESULTS/iterLMKSI/outputs/g1_To_g2_xdisp.hdr ]
// set y1 [ b2 load image test/CLReg2/WarpForward_ydisp.hdr ]
// set y2 [ b2 load image LMKREG_RESULTS/iterLMKSI/outputs/g1_To_g2_ydisp.hdr ]
// set z1 [ b2 load image test/CLReg2/WarpForward_zdisp.hdr ]
// set z2 [ b2 load image LMKREG_RESULTS/iterLMKSI/outputs/g1_To_g2_zdisp.hdr ]

// set xdiff [ b2 subtract images [ list $x1 $x2 ] ]
// set ydiff [ b2 subtract images [ list $y1 $y2 ] ]
// set zdiff [ b2 subtract images [ list $z1 $z2 ] ]

// puts "XdiffForward errors are  [b2 image min $xdiff ]  to  [b2 image max $xdiff ], anything less than 0.1 are OK."
// puts "YdiffForward errors are  [b2 image min $ydiff ]  to  [b2 image max $ydiff ], anything less than 0.1 are OK."
// puts "ZdiffForward errors are  [b2 image min $zdiff ]  to  [b2 image max $zdiff ], anything less than 0.1 are OK."


// set x1 [ b2 load image test/CLReg2/WarpReverse_xdisp.hdr ]
// set x2 [ b2 load image LMKREG_RESULTS/iterLMKSI/outputs/g2_To_g1_xdisp.hdr ]
// set y1 [ b2 load image test/CLReg2/WarpReverse_ydisp.hdr ]
// set y2 [ b2 load image LMKREG_RESULTS/iterLMKSI/outputs/g2_To_g1_ydisp.hdr ]
// set z1 [ b2 load image test/CLReg2/WarpReverse_zdisp.hdr ]
// set z2 [ b2 load image LMKREG_RESULTS/iterLMKSI/outputs/g2_To_g1_zdisp.hdr ]

// set xdiff [ b2 subtract images [ list $x1 $x2 ] ]
// set ydiff [ b2 subtract images [ list $y1 $y2 ] ]
// set zdiff [ b2 subtract images [ list $z1 $z2 ] ]

// puts "XdiffReverse errors are  [b2 image min $xdiff ]  to  [b2 image max $xdiff ], anything less than 0.1 are OK."
// puts "YdiffReverse errors are  [b2 image min $ydiff ]  to  [b2 image max $ydiff ], anything less than 0.1 are OK."
// puts "ZdiffReverse errors are  [b2 image min $zdiff ]  to  [b2 image max $zdiff ], anything less than 0.1 are OK."

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkDifferenceImageFilter.h"
#include "itkImageRegionIterator.h"
#include <iostream>
#include <string>
#include <math.h>

typedef itk::Image<float,3> ImageType;
typedef ImageType::Pointer ImagePointer;
typedef itk::DifferenceImageFilter<ImageType,ImageType> DiffImageFilterType;
typedef itk::ImageFileReader<ImageType> ImageReaderType;
typedef itk::ImageRegionConstIterator<ImageType> ConstImageIterator;

ImagePointer OpenImage(std::string path)
{
  ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(path.c_str());
  reader->Update();
  return reader->GetOutput();
}

float ErrMag(ImagePointer &im)
{
  ConstImageIterator it(im,im->GetLargestPossibleRegion());
  float max = 0.0;
  float f;
  while(!it.IsAtEnd())
    {
    f = fabs(it.Get());
    if(f > max)
      {
      max = f;
      }
    ++it;
    }
  return f;
}

int
main(int argc, char **argv)
{
  if(argc < 3)
    {
    std::cerr << "CLRegTest: Usage: CLRegTest dir1 dir2" << std::cerr;
    exit(1);
    }
  std::string Dir1(argv[1]), Dir2(argv[2]);
  const char *filenames1[3] = { "g1_To_g2_xdisp.hdr",
                                "g1_To_g2_ydisp.hdr",
                                "g1_To_g2_zdisp.hdr" };
  const char *filenames2[3] = { "WarpForward_xdisp.hdr",
                                "WarpForward_ydisp.hdr",
                                "WarpForward_zdisp.hdr" };

  const char *filenames3[3] = { "g2_To_g1_xdisp.hdr",
                                "g2_To_g1_ydisp.hdr",
                                "g2_To_g1_zdisp.hdr" };
  const char *filenames4[3] = { "WarpReverse_xdisp.hdr",
                                "WarpReverse_ydisp.hdr",
                                "WarpReverse_zdisp.hdr" };
  const char axes[3] = { 'X', 'Y', 'Z' };

  for(int i = 0; i < 3; i++)
    {
    ImagePointer Image1,Image2;
    try
      {
      Image1 = OpenImage(Dir1 + std::string("/") + std::string(filenames1[i]));
      }
    catch (itk::ExceptionObject e)
      {
      std::cerr << "can't open" << Dir1 + std::string("/") + filenames1[i]
                << std::endl;
      exit(1);
      }
    try
      {
      Image2 = OpenImage(Dir2 + std::string("/") + std::string(filenames2[i]));
      }
    catch (itk::ExceptionObject e)
      {
      std::cerr << "can't open" << Dir2 + std::string("/") + filenames2[i]
                << std::endl;
      exit(1);
      }

    DiffImageFilterType::Pointer diff = DiffImageFilterType::New();
    diff->SetValidInput(Image1);
    diff->SetTestInput(Image2);
    diff->Update();
    ImagePointer result = diff->GetOutput();
    float minmag = ErrMag(result);
    std::cout << axes[i] << "Diff Forward error is " << 
      minmag << " Less than 0.1 is OK" << std::endl;
    }

  for(int i = 0; i < 3; i++)
    {
    ImagePointer Image1,Image2;
    try
      {
      Image1 = OpenImage(Dir1 + std::string("/") + std::string(filenames3[i]));
      }
    catch (itk::ExceptionObject e)
      {
      std::cerr << "can't open" << Dir1 + std::string("/") + filenames3[i]
                << std::endl;
      exit(1);
      }
    try
      {
      Image2 = OpenImage(Dir2 + std::string("/") + std::string(filenames4[i]));
      }
    catch (itk::ExceptionObject e)
      {
      std::cerr << "can't open" << Dir2 + std::string("/") + filenames4[i]
                << std::endl;
      exit(1);
      }

    DiffImageFilterType::Pointer diff = DiffImageFilterType::New();
    diff->SetValidInput(Image1);
    diff->SetTestInput(Image2);
    diff->Update();
    ImagePointer result = diff->GetOutput();
    float minmag = ErrMag(result);
    std::cout << axes[i] << " Diff Reverse error is " << 
      minmag << " Less than 0.1 is OK" << std::endl;
    }
}
