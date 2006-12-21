#ifndef _ITK_PIPELINE_h
#define _ITK_PIPELINE_h

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


class ItkPipeline
{

public:

  typedef unsigned char                       PixelType;
  typedef itk::Image< PixelType, 2 >          ImageType;
  typedef itk::ImageFileReader< ImageType >   ReaderType;
  typedef itk::ImageFileWriter< ImageType >   WriterType;

public:

  ItkPipeline();
  ~ItkPipeline();

  void SetInputFileName(  const char * filename );
  void SetOutputFileName( const char * filename );
  void LoadInputFile();
  void SaveOutputFile();

private:

  ReaderType::Pointer   m_ImageReader; 
  WriterType::Pointer   m_ImageWriter; 

};


#endif


