/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    UnaryFunctorCache.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __UnaryFunctorCache_h_
#define __UnaryFunctorCache_h_

#include <cassert>
#include <cstdio>

#include "IRISTypes.h"
#include "itkObject.h"

/**
 * \class UnaryFunctorCache
 * \brief A cache for unary functors operating on types like short and char.
 *
 * This object wraps around a Functor and remembers the output values for 
 * the input values that is receives.  Do not use this class with non-integral
 * types and with types like int and long, or you will run out of memory!
 */
template <class TInput, class TOutput, class TFunctor> 
class ITK_EXPORT UnaryFunctorCache : public itk::Object
{
public:

  /** Standard class typedefs. */
  typedef UnaryFunctorCache Self;
  typedef itk::Object Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(UnaryFunctorCache,itk::Object);

  /** New macro */
  itkNewMacro(UnaryFunctorCache);

  /** Evaluate the function using cache lookup */
  TOutput Evaluate(const TInput &in) const {
    return m_Cache[in - m_CacheBegin];
  }

  /**
   * Set the function instance to call evaluate on
   */
  irisSetMacro(InputFunctor,TFunctor *);

  /**
   * Get the function instance
   */
  irisGetMacro(InputFunctor,TFunctor *);

  /**
   * Set the evaluation bounds, if you use these and these are small enough,
   * you can use int or long.
   */
  void SetEvaluationRange(TInput begin, TInput length) {
    m_CacheBegin = begin;
    m_CacheLength = length;
  }

  /** Compute the cache */
  void ComputeCache();

  /**
   * A functor is a lightweight object that has an Evaluate function and 
   * can be passed on to itk::UnaryFunctorFilter
   */
  class CachingFunctor {
  public:
    TOutput operator()(const TInput &in) {
      // assert(m_Parent);
      return m_Parent->Evaluate(in);
    }

    CachingFunctor(UnaryFunctorCache *parent) {
      m_Parent = parent;
    }

    CachingFunctor() {
      m_Parent = NULL;
    }
  private:
    Pointer m_Parent;
  };

  /**
   * This method returns the lightweight functor (it can be copied)
   */
  irisGetMacro(CachingFunctor,const CachingFunctor &);

protected:

  UnaryFunctorCache();
  virtual ~UnaryFunctorCache();
  void PrintSelf(std::ostream &s, itk::Indent indent) const;

  /**
   * The function being cached
   */
  TFunctor *m_InputFunctor;

  /**
   * The storage for the cache
   */
  TOutput *m_Cache;

  /**
   * The bounds of the cache
   */
  TInput m_CacheBegin,m_CacheLength;

  /**
   * The functor
   */
  CachingFunctor m_CachingFunctor;
};

#ifndef ITK_MANUAL_INSTANTIATION
#include "UnaryFunctorCache.txx"
#endif

#endif // __UnaryFunctorCache_h_
