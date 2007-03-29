/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    MomentRegistrator.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __MomentRegistrator_txx
#define __MomentRegistrator_txx

#include "MomentRegistrator.h"

template< class TImage >
MomentRegistrator< TImage >
::MomentRegistrator()
:ImageRegistrationMethod<TImage, TImage>()
  {
  this->SetTransform(TransformType::New());
  this->GetTypedTransform()->SetIdentity();
  this->SetInitialTransformParameters(
                  this->GetTypedTransform()->GetParameters());

  m_NumberOfMoments = 0 ;
  }

template< class TImage >
MomentRegistrator< TImage >
::~MomentRegistrator()
  {
  }

template< class TImage >
void
MomentRegistrator< TImage >
::Initialize() throw (ExceptionObject)
  {
  try
    {
    // Don't call superclass initilization since optimizer, etc aren't used
    //Superclass::Initialize();
    }
  catch(ExceptionObject e)
    {
    throw(e);
    }
  }

template< class TImage >
void
MomentRegistrator< TImage >
::StartRegistration()
  {
  try
    {   
    // Don't call superclass startRegistration since optimizer, etc isn't used
    //Superclass::StartRegistration();
    typename MomentsCalculatorType::AffineTransformType::Pointer newTransform;
    newTransform = MomentsCalculatorType::AffineTransformType::New();
    newTransform->SetIdentity();

    if(m_NumberOfMoments == 0)
      {
      typename TImage::SizeType size;
      typename TImage::IndexType fixedCenterIndex;
      Point<double, 3> fixedCenterPoint;
      size = this->GetFixedImage()->GetLargestPossibleRegion().GetSize();
      fixedCenterIndex[0] = size[0]/2;
      fixedCenterIndex[1] = size[1]/2;
      fixedCenterIndex[2] = size[2]/2;
      this->GetFixedImage()->TransformIndexToPhysicalPoint(fixedCenterIndex,
                                                           fixedCenterPoint);

      typename TImage::IndexType movingCenterIndex;
      Point<double, 3> movingCenterPoint;
      size = this->GetMovingImage()->GetLargestPossibleRegion().GetSize();
      movingCenterIndex[0] = size[0]/2;
      movingCenterIndex[1] = size[1]/2;
      movingCenterIndex[2] = size[2]/2;
      this->GetMovingImage()->TransformIndexToPhysicalPoint(movingCenterIndex,
                                                            movingCenterPoint);

      typename TransformType::OffsetType offset;
      offset = movingCenterPoint - fixedCenterPoint;

      newTransform->SetCenter(movingCenterPoint);
      newTransform->SetOffset(offset);
      }
    else 
      {
      typename MomentsCalculatorType::Pointer momCalc;
      momCalc = MomentsCalculatorType::New();
  
      momCalc->SetImage(this->GetFixedImage());
      momCalc->Compute();
      typename MomentsCalculatorType::AffineTransformType::Pointer 
            fixedImageAxesTransform;
      fixedImageAxesTransform = 
            momCalc->GetPhysicalAxesToPrincipalAxesTransform();
      typename TransformType::InputPointType fixedImageCenterOfMass;
      for(unsigned int i=0; i<ImageDimension; i++)
        {
        fixedImageCenterOfMass[i] = momCalc->GetCenterOfGravity()[i];
        }
  
      momCalc->SetImage(this->GetMovingImage());
      momCalc->Compute();
      typename MomentsCalculatorType::AffineTransformType::Pointer 
            movingImageAxesTransform;
      movingImageAxesTransform = 
            momCalc->GetPrincipalAxesToPhysicalAxesTransform();
      typename TransformType::InputPointType movingImageCenterOfMass;
      for(unsigned int i=0; i<ImageDimension; i++)
        {
        movingImageCenterOfMass[i] = momCalc->GetCenterOfGravity()[i];
        }
  
      typename TransformType::OffsetType offset;
      offset = movingImageCenterOfMass - fixedImageCenterOfMass;
      
      if(m_NumberOfMoments == 1) // Centers of mass
        {
        newTransform->SetCenter(movingImageCenterOfMass);
        newTransform->SetOffset(offset);
        }
      else  // m_NumberOfMoments == 2 // Principle axes
        {
        newTransform->SetCenter(fixedImageCenterOfMass);
        newTransform->SetMatrix(fixedImageAxesTransform->GetMatrix());
        newTransform->SetOffset(fixedImageAxesTransform->GetOffset());
        newTransform->Compose(movingImageAxesTransform, true);
        }
      }

    this->SetTransform(newTransform);

    }
  catch(ExceptionObject &e)
    {
    this->PrintError(e) ;
    }
  catch(...)
    {
    this->PrintUncaughtError() ;
    }
  }

template< class TImage >
void
MomentRegistrator< TImage >
::PrintUncaughtError()
{
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in MomentRegistrator:" << std::endl;
  std::cout << "unknown exception caught !!!" << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
}

template< class TImage >
void
MomentRegistrator< TImage >
::PrintError(ExceptionObject &e)
{
  std::cout << "-------------------------------------------------" 
            << std::endl;
  std::cout << "Exception caught in MomentRegistrator:" << std::endl;
  std::cout << e << std::endl;
  std::cout << "-------------------------------------------------" 
            << std::endl;
}

template< class TImage >
void
MomentRegistrator< TImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Number of moments = " << m_NumberOfMoments << std::endl;
}

#endif //__MomentRegistrator_txx
