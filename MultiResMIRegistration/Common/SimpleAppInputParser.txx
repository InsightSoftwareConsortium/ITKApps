/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SimpleAppInputParser.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _SimpleAppInputParser_txx
#define _SimpleAppInputParser_txx

#include "SimpleAppInputParser.h"
#include "itkImageFileReader.h"
#include <stdio.h>

namespace itk
{

template <typename TImage>
SimpleAppInputParser<TImage>
::SimpleAppInputParser()
{
  m_ParameterFileName = "";
  m_FixedImage = NULL;
  m_MovingImage = NULL;

  for ( unsigned int j = 0; j < ImageDimension; j++ )
    {
    m_PermuteOrder[j] = j;
    }
  m_FlipAxes.Fill( false );

  m_NumberOfLevels = 1;
  m_FixedImageShrinkFactors.Fill( 1 );
  m_MovingImageShrinkFactors.Fill( 1 );

  m_NumberOfIterations = IterationsArrayType(1);
  m_NumberOfIterations.Fill( 10 );

  m_LearningRates = RatesArrayType(1);
  m_LearningRates.Fill( 1e-4 );

  m_TranslationScale = 100.0;

  m_OutputFileName = "";

}


template <typename TImage>
void
SimpleAppInputParser<TImage>
::Execute()
{

  // open up the parameter file
  FILE * paramFile;
  paramFile = fopen( m_ParameterFileName.c_str(), "r" );
  if( !paramFile )
    {
    itkExceptionMacro( << "Could not open parameter file. " );
    }
    
  // parse the file
  char         currentLine[150];
  unsigned int uNumber;
  float        fNumber;


  /************************************
   * Parse fixed image information
   ************************************/
  // get fixed image file name
  if( fscanf( paramFile, "%s", currentLine ) != 1 ) 
    {
    itkExceptionMacro( << "Could not find fixed image filename." );
    }
  std::string fixedImageFileName = currentLine;
  std::cout << "Fixed image filename: " << fixedImageFileName << std::endl;

  /************************************
   * Parse moving image information
   ************************************/
  // get moving image file name
  if( fscanf( paramFile, "%s", currentLine ) != 1 ) 
    {
    itkExceptionMacro( << "Could not find moving image filename." );
    }
  std::string movingImageFileName = currentLine;
  std::cout << "Moving image filename: " << movingImageFileName << std::endl;

  /************************************
   * Parse permute and flip parameters
   ************************************/
  // get permute order
  for ( unsigned int j = 0; j < ImageDimension; j++ )
    {
    if( fscanf( paramFile, "%d", &uNumber ) != 1 )
      {
      itkExceptionMacro( << "Could not find permute order." );
      }

    m_PermuteOrder[j] = uNumber;
    }
  std::cout << "Permute order: " << m_PermuteOrder << std::endl;


  // get which axes to flip
  for ( unsigned int j = 0; j < ImageDimension; j++ )
    {
    if( fscanf( paramFile, "%d", &uNumber ) != 1 )
      {
      itkExceptionMacro( << "Could not find permute order." );
      }

    m_FlipAxes[j] = !(!uNumber);
    }
  std::cout << "Flip axes: " << m_FlipAxes << std::endl;


  /************************************
   * Parse registration parameters
   ************************************/
  // get number of levels
  std::cout << "Number of levels: ";
  if( fscanf( paramFile, "%d", &uNumber) != 1 )
    {
    itkExceptionMacro( << "Could not find the number of levels." );
    }
  m_NumberOfLevels = uNumber;
  std::cout << uNumber << std::endl;

  // get fixed image shrink factors
  for( unsigned int j = 0; j < ImageDimension; j++ )
    {
    if( fscanf( paramFile, "%d", &uNumber ) != 1 )
      {
      itkExceptionMacro( << "Could not find fixed image shrink factors. " );
      }

    m_FixedImageShrinkFactors[j] = uNumber;
    }
  std::cout << "Fixed image shrink factors: " << m_FixedImageShrinkFactors;
  std::cout << std::endl;
 
  // get moving image shrink factors
  for( unsigned int j = 0; j < ImageDimension; j++ )
    {
    if( fscanf( paramFile, "%d", &uNumber ) != 1 )
      {
      itkExceptionMacro( << "Could not find moving image shrink factors. " );
      }
    m_MovingImageShrinkFactors[j] = uNumber;
    }
  std::cout << "Moving image shrink factors: " << m_MovingImageShrinkFactors;
  std::cout << std::endl;

  // get number of iterations
  {
  itk::Array<unsigned int> temp( m_NumberOfLevels );
  temp.Fill( 0 );
  m_NumberOfIterations = temp;
  }
  for( unsigned int j = 0; j < m_NumberOfLevels; j++ )
    {
    if( fscanf( paramFile, "%d", &uNumber ) != 1 )
     {
     itkExceptionMacro( << "Could not find number of iterations per level. " );
     }
    m_NumberOfIterations[j] = uNumber;
    }
  std::cout << "Number of iterations: " << m_NumberOfIterations;
  std::cout << std::endl;

  // get learning rates
  {
  itk::Array<double> temp( m_NumberOfLevels );
  temp.Fill( 0 );
  m_LearningRates = temp;
  }
  for( unsigned int j = 0; j < m_NumberOfLevels; j++ )
    {
    if( fscanf( paramFile, "%f", &fNumber ) != 1 )
     {
     itkExceptionMacro( << "Could not find learning rates per level. " );
     }
    m_LearningRates[j] = fNumber;
    }
  std::cout << "Learning rates: " << m_LearningRates;
  std::cout << std::endl;

  // get translation scale
  if( fscanf( paramFile, "%f", &fNumber) != 1 )
    {
    itkExceptionMacro( << "Could not find the translation scale." );
    }
  m_TranslationScale = fNumber;
  std::cout << "Translation scale: " << fNumber << std::endl;


  /************************************
   * Parse output related parameters
   ************************************/
  // output filename
  if( fscanf( paramFile, "%s", currentLine ) != 1 ) 
  {
  itkExceptionMacro( << "Could not find the output filename." );
  }

  m_OutputFileName = currentLine;

  std::cout << "Output filename: " << m_OutputFileName << std::endl;
  std::cout << std::endl;


  /***********************
   * Read in the images
   ***********************/
   typedef typename ImageType::PixelType PixelType;
   typedef itk::ImageFileReader<ImageType> ReaderType;
   typename ReaderType::Pointer fixedReader  = ReaderType::New();
   typename ReaderType::Pointer movingReader = ReaderType::New();
  
   try 
    {
    fixedReader->SetFileName( fixedImageFileName.c_str() );
    fixedReader->Update();
    
    m_FixedImage = fixedReader->GetOutput();
    }
   catch(...)
    {
    std::cout << "Error while reading in fixed image." << std::endl;
    throw;
    }


   try 
    {
    movingReader->SetFileName( movingImageFileName.c_str() );
    movingReader->Update();
    
    m_MovingImage = movingReader->GetOutput();
    }
   catch(...)
    {
    std::cout << "Error while reading in moving image." << std::endl;
    throw;
    }

 
}


} // namespace itk

#endif
