/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SimpleApp.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _SimpleApp_h
#define _SimpleApp_h

#include "MIMApplicationBase.h"
#include "itkImage.h"
#include "SimpleAppInputParser.h"
#include "MIMPreprocessor.h"
#include "MIMRegistrator.h"
#include "SimpleAppOutput.h"

#include <string>

namespace itk
{

/** \class SimpleApp
 * 
 * This class is a simple application that reads in a parameter
 * file, perform an MIM registration and outputs the 
 * target (fixed), source (moving) and registered images as PGM 
 * files.
 *
 */
template < class TInputPixel, 
           class TInputImage = Image<TInputPixel, 3>,
           class TImage = Image<float,3> >
class SimpleApp : 
public MIMApplicationBase < 
  TInputImage, TImage,
  SimpleAppInputParser<TInputImage>,
  MIMPreprocessor<TInputImage,TImage>,
  MIMRegistrator<TImage,TImage>,
  SimpleAppOutput<TInputImage> >   
{
public:

  /** Standard class typedefs. */
  typedef SimpleApp Self;
  typedef MIMApplicationBase < 
              TInputImage, TImage,
              SimpleAppInputParser<TInputImage>,
              MIMPreprocessor<TInputImage,TImage>,
              MIMRegistrator<TImage,TImage>,
              SimpleAppOutput<TInputImage> >       Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(SimpleApp, MIMApplicationBase);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Set input parameter file */
  itkSetStringMacro( ParameterFileName );

protected:

  SimpleApp()
    { 
    m_ParameterFileName = ""; 
    }

  virtual ~SimpleApp(){};

  /** Initialize the input parser. */
  virtual void InitializeParser()
    {
    this->m_Parser->SetParameterFileName( m_ParameterFileName.c_str() );
    }

  /*** Initialize the output generator. */
  virtual void InitializeGenerator()
    {
    this->m_Generator->SetFixedImage( this->m_Parser->GetFixedImage() );
    this->m_Generator->SetMovingImage( this->m_Parser->GetMovingImage() );
    this->m_Generator->SetTransform( this->m_Transform );
    this->m_Generator->SetOutputFileName( this->m_Parser->GetOutputFileName() );
    };


private:

   /*** Input parameter filename. */
   std::string          m_ParameterFileName;

};


} // namespace itk


#endif
