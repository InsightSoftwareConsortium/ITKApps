/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkModule.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkModule__H
#define __itkModule__H

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMeanImageFilter.h"
#include "itkImage.h"


class itkModule
{
public:
  void SetInputFileName( const char * filename );
  void SetOutputFileName( const char * filename );
  void RunPipeline(); 
 
  itkModule();
  ~itkModule();
private:
    
  typedef itk::Image<unsigned char, 2> ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::ImageFileWriter< ImageType > WriterType;
  typedef itk::MeanImageFilter< ImageType, ImageType > FilterType;

  
private:

  ReaderType::Pointer m_Reader;
  WriterType::Pointer m_Writer;
  FilterType::Pointer m_Filter;

};



#endif


