/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    Imgmath.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _ITKMATH_H_
#define _ITKMATH_H_


/* This file contains the functions to perform pixel by pixel mathematical
 * operations on 2 images. All the functions are performed by using ITK
 * filters. */

#include "itkImage.h"
#include "itkSquaredDifferenceImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkSquaredDifferenceImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include <vcl_cmath.h>


/* Iadd adds 2 images at every pixel location and outputs the resulting image.*/


template <class PixelType, int Dimension>
typename itk::Image< PixelType, Dimension >::ConstPointer 
Iadd( const itk::Image< PixelType, Dimension > * input1,
      const itk::Image< PixelType, Dimension > * input2 )
{
  typedef itk::Image< PixelType, Dimension > ImageType;

  typedef itk::AddImageFilter<ImageType,ImageType,ImageType> FilterType;

  typename FilterType::Pointer filter = FilterType::New();

  filter->SetInput1( input1 );
  filter->SetInput2( input2 );

  try
      {
      filter->Update();
      }
  catch (itk::ExceptionObject & err )
      {
      std::cout << "Exception Object caught: " << std::endl;
      std::cout << err << std::endl;
      exit(-1);
      }
  
  typename ImageType::ConstPointer image = filter->GetOutput();

  return image;

}



/* Isub subtracts 2 images at every pixel location and outputs the resulting
 * image.*/

template <class PixelType, int Dimension>
typename itk::Image< PixelType, Dimension >::ConstPointer
Isub( const itk::Image< PixelType, Dimension > * input1, 
      const itk::Image< PixelType, Dimension > * input2 )
{
  typedef itk::Image< PixelType, Dimension > ImageType;


  typedef itk::SubtractImageFilter<ImageType,ImageType,ImageType> FilterType;

  typename FilterType::Pointer filter = FilterType::New();

  filter->SetInput1( input1 );
  filter->SetInput2( input2 );

  try
      {
      filter->Update();
      }
  catch(itk::ExceptionObject & err )
      {
      std::cout << "Exception Object caught: " << std::endl;
      std::cout << err << std::endl;
      exit(-1);
      }

  typename ImageType::ConstPointer image =  filter->GetOutput();

  return image;

}


/* Imul multiplies 2 images at every pixel location and outputs the resulting
 * image.*/

template <class PixelType, int Dimension>
typename itk::Image< PixelType, Dimension >::ConstPointer 
Imul( const itk::Image< PixelType, Dimension > * input1,
      const itk::Image< PixelType, Dimension > * input2 )
{

  typedef typename itk::Image< PixelType, Dimension > ImageType;

  typedef itk::MultiplyImageFilter<ImageType,ImageType,ImageType> FilterType;
  
  typename FilterType::Pointer filter = FilterType::New();
  
  filter->SetInput1( input1 );
  filter->SetInput2( input2 );
  
  try
      {
      filter->Update();
      }
  catch (itk::ExceptionObject & err )
      {
      std::cout << "Exception Object caught: " << std::endl;
      std::cout << err << std::endl;
      exit(-1);
      }

  typename ImageType::ConstPointer image = filter->GetOutput();

  return image;

}


/* Idiv divides 2 images at every pixel location and outputs the resulting
 * image.*/

template <class PixelType, int Dimension>
typename itk::Image< PixelType, Dimension >::ConstPointer 
Idiv(const itk::Image< PixelType, Dimension > * input1,
     const itk::Image< PixelType, Dimension > * input2 )
{

  typedef itk::Image< PixelType, Dimension > ImageType;

  typedef itk::DivideImageFilter<ImageType,ImageType,ImageType> FilterType;
  
  typename FilterType::Pointer filter = FilterType::New();

  filter->SetInput1( input1  );
  filter->SetInput2( input2 );

  try
      {
      filter->Update();
      }
  catch (itk::ExceptionObject & err )
      {
      std::cout << "Exception Object caught: " << std::endl;
      std::cout << err << std::endl;
      exit(-1);
      }

  typename ImageType::ConstPointer image = filter->GetOutput();

  return image;

}


/* Iavg takes an image and the number of images as inputs , divides each pixel
 * location of the image by the number of images, and outputs the resulting
 * image.*/

template <class PixelType, int Dimension>
typename itk::Image< PixelType, Dimension >::Pointer 
Iavg( const itk::Image< PixelType, Dimension > * input1, int nimgs )
{

  typedef itk::Image< PixelType, Dimension >  ImageType;
    
  typename ImageType::Pointer image =  ImageType::New();

  image->SetRegions( input1->GetBufferedRegion() );
  image->CopyInformation( input1 );
  image->Allocate();

  typedef itk::ImageRegionIterator< ImageType > IteratorType;
  typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;

  ConstIteratorType in1( input1, input1->GetBufferedRegion() );
  IteratorType      out( image,  image->GetBufferedRegion() );

  for(in1.GoToBegin(),out.GoToBegin() ; !in1.IsAtEnd() ; ++in1,++out)
      {
      out.Set( in1.Get() / nimgs );
      }

  return image;

}                                         

/* ImageMultiplyConstant multiplies the entire image with a constant value and
 * outputs the resultant image*/

template <class PixelType, int Dimension>
typename itk::Image< PixelType, Dimension >::ConstPointer 
ImageMultiplyConstant( const itk::Image< PixelType, Dimension > * input1, 
                       PixelType constant )
{
  typedef itk::Image< PixelType, Dimension >  ImageType;
    
  typename ImageType::Pointer image =  ImageType::New();

  image->SetRegions( input1->GetBufferedRegion() );
  image->CopyInformation( input1 );
  image->Allocate();

  typedef itk::ImageRegionIterator< ImageType > IteratorType;
  typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;

  IteratorType      out1( image ,  image->GetBufferedRegion());
  ConstIteratorType in1( input1 , input1->GetBufferedRegion());

  in1.GoToBegin();
  out1.GoToBegin(); 

  while( !in1.IsAtEnd() ) 
    {
    out1.Set( (in1.Get() * constant ) );
    ++in1;
    ++out1;
    }

  typename ImageType::ConstPointer constImage = image.GetPointer();

  return constImage;
}


/* ImageDivideConstant divides the entire image with a constant value and
 * outputs the resultant image*/

template <class PixelType, int Dimension>
typename itk::Image< PixelType, Dimension >::ConstPointer 
ImageDivideConstant( const itk::Image< PixelType, Dimension > * input1, 
                     PixelType constant )
{
  typedef itk::Image< PixelType, Dimension >  ImageType;

  typename ImageType::Pointer image =  ImageType::New();

  image->SetRegions( input1->GetBufferedRegion() );
  image->CopyInformation( input1 );
  image->Allocate();

  typedef itk::ImageRegionIterator< ImageType > IteratorType;
  typedef itk::ImageRegionConstIterator< ImageType > ConstIteratorType;

  IteratorType      out1( image ,  image->GetBufferedRegion());
  ConstIteratorType in1( input1 , input1->GetBufferedRegion());

  in1.GoToBegin();
  out1.GoToBegin(); 

  while( !in1.IsAtEnd() ) 
    {
    out1.Set( (in1.Get() / constant ) );
    ++in1;
    ++out1;
    }

  typename ImageType::ConstPointer constImage = image.GetPointer();

  return constImage;
}

#endif
