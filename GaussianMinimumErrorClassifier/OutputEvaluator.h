/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    OutputEvaluator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __OutputEvaluator_h
#define __OutputEvaluator_h

#include "itkImageRegionIterator.h"

#include <vector>

template< class TImage >
class OutputEvaluator 
{
public:
  OutputEvaluator() {}
  ~OutputEvaluator() {}

  typedef itk::ImageRegionIterator< TImage > ImageIterator ;
  typedef std::vector< std::vector< int > > ClassificationMatrixType ;

  void SetTruth(TImage* image) ;

  void SetTarget(TImage* image) ;
  
  void SetNumberOfClasses(unsigned int size) ;

  void GenerateData() ;

  ClassificationMatrixType& GetClassificationMatrix() const 
  { return m_ClassificationMatrix ; }

  void Print() ;

private:

  TImage* m_Truth ;
  TImage* m_Target ;
  unsigned int m_NumberOfClasses ;
  ClassificationMatrixType m_ClassificationMatrix ;
} ; // end of class

#ifndef ITK_MANUAL_INSTANTIATION
#include "OutputEvaluator.txx"
#endif

#endif
