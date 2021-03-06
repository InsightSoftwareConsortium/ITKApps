/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DemonsPreprocessor.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _DemonsPreprocessor_h
#define _DemonsPreprocessor_h

#include "itkObject.h"

namespace itk
{

/** \class DemonsPreprocessor
 *
 * This component pre-processes the moving and fixed image before
 * registration.
 *
 * This particular, this class first normalizes both images to have
 * intensity between zero and one.
 *
 * The moving image is then normalize to the intensity of the fixed
 * image by using histogram matching. 
 *
 * The preprocessing is activatived by method Execute().
 *
 * Inputs:
 *    - pointer to original fixed image
 *    - pointer original moving image
 *    - number of histogram levels
 *    - number of match points
 *
 * Outputs:
 *    - pointer to processed fixed image
 *    - pointer to processed moving image
 *    - the minimum value of original fixed image
 *    - the minimum value of original moving image
 *
 */
template <typename TInputImage, typename TOutputImage>
class ITK_EXPORT DemonsPreprocessor : public Object
{
public:

  /** Standard class typedefs. */
  typedef DemonsPreprocessor Self;
  typedef Object Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Run-time type information (and related methods). */
  itkTypeMacro(DemonsPreprocessor, Object);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Input Image Type. */
  typedef TInputImage InputImageType;

  /** Input image pixel type. */
  typedef typename InputImageType::PixelType InputPixelType;

  /** Output Image Type. */
  typedef TOutputImage OutputImageType;

  /** Image dimension enumeration. */
  itkStaticConstMacro(ImageDimension, unsigned int, TInputImage::ImageDimension);

  /** Set the input fixed image. */
  itkSetObjectMacro( InputFixedImage, InputImageType );
  
  /** Set the input moving image. */
  itkSetObjectMacro( InputMovingImage, InputImageType );

  /** Set the number of histogram levels to use. */
  itkSetMacro( NumberOfHistogramLevels, unsigned long );

  /** Set the number of match points to use. */
  itkSetMacro( NumberOfMatchPoints, unsigned long );

  /** Method to execute the preprocessing. */
  virtual void Execute();

  /** Get the output fixed image. */
  itkGetObjectMacro( OutputFixedImage, OutputImageType );

  /** Get the output moving image. */
  itkGetObjectMacro( OutputMovingImage, OutputImageType );

  /** Get minimum value of original fixed image. */
  itkGetMacro( FixedImageMinimum, InputPixelType );

  /** Get minimum value of original moving image. */
  itkGetMacro( MovingImageMinimum, InputPixelType );


protected:
  DemonsPreprocessor();
  ~DemonsPreprocessor(){};

private:
  DemonsPreprocessor( const Self& ); //purposely not implemented
  void operator=( const Self& ); //purposely not implemented

  typename InputImageType::Pointer       m_InputFixedImage;
  typename InputImageType::Pointer       m_InputMovingImage;
  typename OutputImageType::Pointer      m_OutputFixedImage;
  typename OutputImageType::Pointer      m_OutputMovingImage;

  unsigned long                          m_NumberOfHistogramLevels;
  unsigned long                          m_NumberOfMatchPoints;

  InputPixelType                         m_FixedImageMinimum;
  InputPixelType                         m_MovingImageMinimum;

  typedef typename OutputImageType::Pointer OutputImagePointer;
  void NormalizeImage( InputImageType * input,
   OutputImagePointer & output, InputPixelType & min ); 

};

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "DemonsPreprocessor.txx"
#endif

#endif
