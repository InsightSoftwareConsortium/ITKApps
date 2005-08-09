/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DicomGenerateThumbnail.cxx
  Language:  C++
  Date:      
  Version:   

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"
#include "itkGDCMImageIO.h"

#include <list>
#include <fstream>

int main(int ac, char* av[])
{
  if(ac < 3)
    {
    std::cerr << "Usage: " << av[0] << " DicomImage OutputImage \n";
    return EXIT_FAILURE;
    }

  typedef short InputPixelType;
  typedef itk::Image< InputPixelType,2 > InputImageType;
  typedef itk::ImageFileReader< InputImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( av[1] );
  
  typedef itk::GDCMImageIO        ImageIOType;
  ImageIOType::Pointer gdcmImageIO = ImageIOType::New();
  reader->SetImageIO( gdcmImageIO );
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
       
  ImageIOType::Pointer dicomIO = ImageIOType::New();

  ReaderType::Pointer reader1 = ReaderType::New();
  reader1->SetFileName( av[1] );
  reader1->SetImageIO( dicomIO );

  try
    {
    reader1->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }
  
  typedef itk::MetaDataDictionary   DictionaryType;

  const  DictionaryType & dictionary = dicomIO->GetMetaDataDictionary();

  DictionaryType::ConstIterator itr = dictionary.Begin();
  DictionaryType::ConstIterator end = dictionary.End();

  typedef itk::MetaDataObject< std::string > MetaDataStringType;

  while( itr != end )
    {
    itk::MetaDataObjectBase::Pointer  entry = itr->second;
    MetaDataStringType::Pointer entryvalue = dynamic_cast<MetaDataStringType *>( entry.GetPointer() ) ;

    if( entryvalue )
    {
      std::string tagkey   = itr->first;
      std::string tagvalue = entryvalue->GetMetaDataObjectValue();
      std::cout << tagkey <<  " = " << tagvalue.c_str() << std::endl;
    }
    ++itr;
    }
  
  std::string entryId = "Patient's Name"; //DICOM 2003
  itk::MetaDataObjectBase::ConstPointer  entry = dictionary[entryId];

  MetaDataStringType::ConstPointer entryvalue = dynamic_cast<const MetaDataStringType *>( entry.GetPointer() ) ;

  if( entryvalue )
  {
      std::string tagvalue = entryvalue->GetMetaDataObjectValue();
      std::cout << entryId <<  " = " << tagvalue << std::endl;
  }

  typedef unsigned char WritePixelType;
  typedef itk::Image< WritePixelType, 2 > WriteImageType;
  typedef itk::RescaleIntensityImageFilter< 
               InputImageType, WriteImageType > RescaleFilterType;

  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
  rescaler->SetOutputMinimum(   0 );
  rescaler->SetOutputMaximum( 255 );
  
  typedef itk::ImageFileWriter< WriteImageType >  Writer2Type;

  Writer2Type::Pointer writer2 = Writer2Type::New();
 
  writer2->SetFileName( av[2] );
 
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
  return EXIT_SUCCESS;

}
