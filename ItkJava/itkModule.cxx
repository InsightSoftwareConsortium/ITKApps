/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkModule.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "itkModule.h"

itkModule::itkModule()
{
  m_Reader = ReaderType::New();
  m_Writer = WriterType::New();
  m_Filter = FilterType::New();

  m_Filter->SetInput( m_Reader->GetOutput() );
  m_Writer->SetInput( m_Filter->GetOutput() );
}


itkModule::~itkModule()
{
}

void
itkModule::SetInputFileName( const char * filename )
{
  m_Reader->SetFileName( filename );
}


void
itkModule::SetOutputFileName( const char * filename )
{
  m_Writer->SetFileName( filename );
}



void
itkModule::RunPipeline()
{
}



