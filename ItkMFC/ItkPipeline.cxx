#include "ItkPipeline.h"


ItkPipeline
::ItkPipeline()
{
   m_ImageReader = ReaderType::New();
   m_ImageWriter = WriterType::New();
}



ItkPipeline
::~ItkPipeline()
{

}


void
ItkPipeline
::SetInputFileName( const char * filename )
{
  m_ImageReader->SetFileName( filename );
}



void
ItkPipeline
::SetOutputFileName( const char * filename )
{
  m_ImageWriter->SetFileName( filename );
}


void
ItkPipeline
::LoadInputFile()
{
  m_ImageReader->Update();
}

