/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    OutputEvaluator.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __OutputEvaluator_txx
#define __OutputEvaluator_txx

#include "OutputEvaluator.h"

template< class TImage >
void
OutputEvaluator< TImage > 
::SetTruth(TImage* image)
{
  m_Truth = image ;
}

template< class TImage >
void
OutputEvaluator< TImage > 
::SetTarget(TImage* image)
{
  m_Target = image ;
}

template< class TImage >
void
OutputEvaluator< TImage > 
::SetNumberOfClasses(unsigned int size)
{
  m_NumberOfClasses = size ;
  m_ClassificationMatrix.resize(m_NumberOfClasses) ;
  for ( int i = 0 ; i < m_NumberOfClasses ; i++ )
    {
      m_ClassificationMatrix[i].resize(m_NumberOfClasses) ;
    }
}

template< class TImage >
void
OutputEvaluator< TImage >
::GenerateData()
{
  for ( int i = 0 ; i < m_NumberOfClasses ; i++ )
    {
      std::fill(m_ClassificationMatrix[i].begin(), 
                m_ClassificationMatrix[i].end(), 0) ;
    }

  ImageIterator t_iter(m_Truth, m_Truth->GetLargestPossibleRegion()) ;
  ImageIterator e_iter(m_Target, m_Target->GetLargestPossibleRegion()) ;
  unsigned int trueLabel ;
  unsigned int estimatedLabel ;
  while ( !t_iter.IsAtEnd() )
    {
      trueLabel = t_iter.Get() ;
      estimatedLabel = e_iter.Get() ;

      m_ClassificationMatrix[estimatedLabel][trueLabel] += 1 ;
      ++t_iter ;
      ++e_iter ;
    }
}

template< class TImage >
void
OutputEvaluator< TImage > 
::Print()
{
  unsigned int i, j ;

  for ( i = 0 ; i < m_NumberOfClasses ; i++ )
    {
      for ( j = 0 ; j < m_NumberOfClasses ; j++ )
        {
          std::cout << m_ClassificationMatrix[i][j] << " " ; 
        }
      std::cout << std::endl ;
    }
}

#endif
