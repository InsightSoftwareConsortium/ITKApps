/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    typedefs.inl
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

typedef itk::Image< ITKPIXELTYPE, DIMENSION > ImageType;
typedef ImageType InternalImageType;
//the following two lines are here out of laziness - save time during conversion since example code often use In/OutputImageType
typedef ImageType InputImageType;
typedef ImageType OutputImageType;
typedef itk::ImportImageFilter< ITKPIXELTYPE, DIMENSION >   ImportFilterType;
typedef void (*pt2Function)();
