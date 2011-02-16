/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    imageutils.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) 2002 Insight Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __IMAGEUTILS_H
#define __IMAGEUTILS_H

#include "itkMetaImageIOFactory.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNumericTraits.h"
#include "itkOrientImageFilter.h"

#include "myutils.h"

typedef  float ImagePixelType;
typedef  short WriteImagePixelType;
typedef  unsigned char MaskPixelType;
typedef itk::Image<ImagePixelType, 3> ImageType ;
typedef itk::Image<MaskPixelType, 3> MaskType ;
typedef itk::Image<WriteImagePixelType, 3> WriteImageType ;
typedef ImageType::Pointer ImagePointer ;
typedef MaskType::Pointer MaskPointer ;
typedef itk::ImageFileReader< ImageType > ImageReaderType ;
typedef itk::ImageFileReader< MaskType > MaskReaderType ;
typedef itk::ImageFileWriter< WriteImageType > ImageWriterType ;
typedef itk::ImageFileWriter< MaskType > MaskWriterType ;

void copyImage(ImagePointer source , ImagePointer target)
{
  typedef itk::ImageRegionIteratorWithIndex<ImageType> ImageIterator ;
  ImageType::RegionType s_region = source->GetLargestPossibleRegion() ;
  ImageIterator s_iter(source, s_region ) ;


  if (target->GetLargestPossibleRegion().GetSize() != s_region.GetSize())
    {
      target->SetLargestPossibleRegion(s_region) ;
      target->SetBufferedRegion(s_region) ;
      target->Allocate() ;
    }

  ImageIterator t_iter(target, s_region) ;

  s_iter.GoToBegin() ;
  t_iter.GoToBegin() ;
  while (!s_iter.IsAtEnd())
    {
      t_iter.Set(s_iter.Get()) ;
      ++s_iter ;
      ++t_iter ;
    }
}


void logImage(ImagePointer source, ImagePointer target)
{
  print_line("Applying logarithm to the image values...") ;
  ImageType::RegionType region ;
  region = source->GetLargestPossibleRegion() ;

  if (region.GetSize() != target->GetLargestPossibleRegion().GetSize())
    {
      target->SetLargestPossibleRegion(region) ;
      target->SetBufferedRegion(region) ;
      target->Allocate() ;
    }

  itk::ImageRegionIteratorWithIndex<ImageType> s_iter(source, region) ;
  itk::ImageRegionIteratorWithIndex<ImageType> t_iter(target, region) ;

  s_iter.GoToBegin() ;
  t_iter.GoToBegin() ;

  ImageType::PixelType pixel ;
  float log_pixel ;
  while (!s_iter.IsAtEnd())
    {
      pixel = s_iter.Get() ;

      log_pixel =  log( pixel + 1 ) ;
      if (pixel < itk::NumericTraits< ImageType::PixelType>::Zero)
        t_iter.Set( itk::NumericTraits< ImageType::PixelType >::Zero ) ;
      else
        t_iter.Set( (ImageType::PixelType) log_pixel ) ;

      ++s_iter ;
      ++t_iter ;
    }
  print_line("Conversion done.") ;
}

/** Orient an image with the given orientation...
 */
template <class OrienterImageType>
typename OrienterImageType::Pointer
OrientImage(const typename OrienterImageType::Pointer & input, const typename OrienterImageType::DirectionType & DesiredDirection)
{
  //TODO:  change spatial orientation to use direction cosigns here.q

  typename itk::OrientImageFilter<OrienterImageType,OrienterImageType>::Pointer orienter =
    itk::OrientImageFilter<OrienterImageType,OrienterImageType>::New();
  orienter->SetDesiredCoordinateDirection(DesiredDirection);
  orienter->UseImageDirectionOn();
  orienter->SetInput(input);
  orienter->Update();

  typename OrienterImageType::Pointer  image = orienter->GetOutput();
  return image;
}

#endif
