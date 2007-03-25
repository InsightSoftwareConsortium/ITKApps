/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    BiasImageGenerator.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) 2002 Insight Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even 
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <string>
#include <vector>
#include <vnl/vnl_math.h>

#include "imageutils.h"
#include "OptionList.h"
#include "itkMultivariateLegendrePolynomial.h"
#include <itkCastImageFilter.h>

typedef itk::MultivariateLegendrePolynomial BiasField ;

void print_usage()
{
  print_line("MRIBiasCorrection 1.0 (21.June.2001)");

  print_line("usage: BiasImageGenerator [--input file] --output file" ) ;
  print_line("       --dimension int --size long long long") ;
  print_line("       --degree int --useLog [yes|no]") ;
  print_line("       --coefficients c0,..,cn" ) ;

  print_line("");

  print_line("--input file") ;
  print_line("        input image file name [meta image format] (optional)" );
  print_line("--output file") ;
  print_line("        output image file [meta image format]" );
  print_line("--dimension int") ;
  print_line("        output image dimension") ;
  print_line("--size x y z") ;
  print_line("        size of each image dimension") ;
  print_line("--degree int") ;
  print_line("        degree of legendre polynomial used for the") ;
  print_line("        approximation of the bias field" );
  print_line("--coefficients c(0),..,c(n)") ;
  print_line("        coefficients of the polynomial") ;
  print_line("        (used for generating bias field)") ;
  print_line("--use-log [yes|no]") ;
  print_line("        if yes, assume a multiplicative bias field" );

  print_line("") ;

  print_line("example: BiasImageGenerator --output biasfromsample.mhd") ;
  print_line("         --dimension 3 --size 256 256 100 --degree 3") ;
  print_line("         --coefficients 0.056789 -1.00004 0.78945 ... -0.02345");

}

    
int main(int argc, char* argv[])
{

  if (argc <= 1)
    {
      print_usage() ;
      exit(0) ;
    }

  OptionList options(argc, argv) ;

  // get image file options
  std::string outputFileName ;
  std::string inputFileName ;
  int dimension ;
  int degree ;
  itk::Array<double> coefficientVector ;
  BiasField::DomainSizeType biasSize ;
  bool useLog ;
  try
    {
      options.GetStringOption("input", &inputFileName, false) ;
      options.GetStringOption("output", &outputFileName, true) ;
      // get bias field options
      useLog = options.GetBooleanOption("use-log", true, false) ;
      // get bias field options
      std::vector<double> coefficients ;
      options.GetMultiDoubleOption("coefficients", &coefficients, true) ;
      int length = coefficients.size() ;
      coefficientVector.set_size(length) ;
      for (int i = 0 ; i < length ; i++)
        coefficientVector[i] = coefficients[i] ;
      degree = options.GetIntOption("degree", 3, true) ;
      dimension = options.GetIntOption("dimension", 3,  false) ;
      if(dimension != static_cast<int>(ImageType::ImageDimension))
        {
        std::cout << "Currently compiled to only support "
                  << ImageType::ImageDimension << " dimensional images."
                  << std::endl;
        exit(0);
        }

      std::vector<int> sizes ;
      options.GetMultiIntOption("size", &sizes, false) ;
      if (sizes.size() > 0 ) {
   biasSize = BiasField::DomainSizeType(sizes.size()) ;
   for (unsigned int i = 0 ; i < sizes.size() ; i++)
     {
       biasSize[i] = sizes[i] ;
     }
      }
    }
  catch(OptionList::RequiredOptionMissing e)
    {
      std::cout << "Error: The '" << e.OptionTag 
                << "' option is required but missing." 
                << std::endl ;
      exit(0) ;
    }
  
  
  ImagePointer input = NULL;
  if(inputFileName != "")
    {
    ImageReaderType::Pointer imageReader = ImageReaderType::New() ;
    try
      {
      std::cout << "Loading images..." << std::endl ;
      imageReader->SetFileName(inputFileName.c_str()) ;
      imageReader->Update() ;
      input = imageReader->GetOutput() ;
      std::cout << "Images loaded." << std::endl ;
      if (useLog)
        {
        logImage(input, input) ;
        }
      }
    catch (itk::ExceptionObject e)
      {
      e.Print(std::cout);
      exit(0) ;
      }
    }
      

  if (input) {
    ImageType::SizeType size = input->GetLargestPossibleRegion().GetSize() ;
    biasSize.clear() ;
    int biasDimension = 0  ;
    
    for(unsigned int dim = 0 ; dim < ImageType::ImageDimension ; dim++)
      {
   if (size[dim] > 1)
     {
       biasSize.resize(biasDimension + 1) ;
       biasSize[biasDimension] = size[dim] ;
       biasDimension++ ;
     }
      }
  }

  BiasField biasField(biasSize.size(), degree, biasSize) ;
  
  try
    {
      biasField.SetCoefficients(coefficientVector) ;
    }
  catch(BiasField::CoefficientVectorSizeMismatch m)
    {
      std::cout << "Error: Invalid Coefficients for the bias fiedl" 
                << std::endl ;
      std::cout << "given size: " << m.m_Given 
                << "required size: " 
                << m.m_Required  
                << std::endl ;
      exit(0) ;
    }
  

  // generates the bias field image
  ImagePointer output = ImageType::New() ;
  typedef ImageType::PixelType Pixel ;

  ImageType::IndexType index ;
  ImageType::SizeType size ;
  ImageType::RegionType region ;
  
  BiasField::DomainSizeType biasSize2 = biasField.GetDomainSize() ;
  for (unsigned int i = 0 ; i < ImageType::ImageDimension ; i++)
    {
      if (i < biasField.GetDimension())
   {
     size[i] = biasSize2[i] ;
   }
      else
   {
     size[i] = 0 ;
   }
      index[i] = 0 ;
    }

  region.SetSize(size) ;
  region.SetIndex(index) ;

  output->SetLargestPossibleRegion(region) ;
  output->SetBufferedRegion(region) ;
  output->Allocate() ;

  BiasField::SimpleForwardIterator bIter(&biasField) ;
  bIter.Begin() ;
  itk::ImageRegionIterator<ImageType> oIter(output, region) ;
  oIter.GoToBegin() ;

  if (input) {
    itk::ImageRegionIterator<ImageType> iIter(input, region) ;
    iIter.GoToBegin() ;
    while (!bIter.IsAtEnd())
      {
   double diff = iIter.Get() + bIter.Get() ;
   if (useLog)
     {
       oIter.Set( (Pixel) ( exp(diff) - 1)) ;
     }
   else
     {
       oIter.Set( (Pixel) diff) ;
     }
   ++iIter ;
   ++oIter ;
   ++bIter ;
      }
  } else {
    while (!bIter.IsAtEnd())
      {
   oIter.Set( (Pixel) bIter.Get()) ;
   ++oIter ;
   ++bIter ;
      }
  }

  // writes the bias field image
  std::cout << "Writing  image..." << std::endl ;
  typedef itk::CastImageFilter< ImageType,  WriteImageType> castFilterType;
  castFilterType::Pointer convFilter = castFilterType::New();
  convFilter->SetInput(output);
  convFilter->Update();
  ImageWriterType::Pointer writer = ImageWriterType::New() ;
  writer->SetInput(convFilter->GetOutput()) ;
  writer->SetFileName(outputFileName.c_str()) ;
  writer->Write() ;

  return 0 ;
}
