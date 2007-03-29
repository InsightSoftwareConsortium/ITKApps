/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    MomentRegistrator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __MomentRegistrator_h
#define __MomentRegistrator_h

#include "itkImage.h"

#include "itkImageRegistrationMethod.h"
#include "itkAffineTransform.h"
#include "itkImageMomentsCalculator.h"

namespace itk
{

template< class TImage >
class MomentRegistrator : public ImageRegistrationMethod < TImage, TImage >
  {
  public:
    typedef MomentRegistrator Self;
    typedef ImageRegistrationMethod< TImage, TImage> Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self> ConstPointer;

    itkTypeMacro(MomentRegistrator, ImageRegistrationMethod);

    itkNewMacro(Self);

    itkStaticConstMacro(ImageDimension, unsigned int, 
                        TImage::ImageDimension);
  
    typedef typename TImage::PixelType PixelType ;
    typedef typename TImage::RegionType RegionType ;

    /** preprocessing related typedefs */
    typedef AffineTransform<double, itkGetStaticConstMacro(ImageDimension)> 
                                                   TransformType ;
    typedef typename TransformType::ParametersType ParametersType ;
    typedef typename TransformType::ParametersType ScalesType ;

    typedef ImageMomentsCalculator< TImage > MomentsCalculatorType;

    void StartRegistration() ;

    TransformType * GetTypedTransform(void)
      {
      return static_cast<TransformType *>(Superclass::GetTransform());
      }

    itkSetMacro(NumberOfMoments, unsigned int) ;
    itkGetConstMacro(NumberOfMoments, unsigned int) ;

  protected:
    MomentRegistrator() ;
    virtual ~MomentRegistrator() ;
    void PrintSelf(std::ostream & os, Indent indent) const;

    virtual void Initialize() throw(ExceptionObject);

    void PrintUncaughtError() ;

    void PrintError(ExceptionObject &e) ;

  private:

    unsigned int    m_NumberOfMoments;

  } ; // end of class


#ifndef ITK_MANUAL_INSTANTIATION
#include "MomentRegistrator.txx"
#endif

} // end namespace itk

#endif //__MomentRegistrator_H

