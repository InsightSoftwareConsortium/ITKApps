/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IRISSlicer.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __IRISSlicer_h_
#define __IRISSlicer_h_

#include <ImageCoordinateTransform.h>

#include <itkImageToImageFilter.h>
#include <itkImageSliceConstIteratorWithIndex.h>
#include <itkImageRegionIterator.h>

/**
 * \class IRISSlicer
 * \brief A slice extraction filter for 3D images.  
 *
 * This filter takes a transform
 * from image space (x=pixel, y=line, z=slice) to display space.  A slice 
 * shows either x-y, y-z, or x-z in the display space.  This filter is necessary
 * because the origin in the slice can correspond to any corner of the image, not
 * just to the origin of the image.  This filter can traverse the image in different
 * directions, accomodating different positions of the display space origin in the
 * image space.
 */
template <class TPixel>
class ITK_EXPORT IRISSlicer 
: public itk::ImageToImageFilter<itk::Image<TPixel,3>, itk::Image<TPixel,2> >
{
public:
  /** Standard class typedefs. */
  typedef IRISSlicer  Self;
  typedef itk::Image<TPixel,3>  InputImageType;
  typedef itk::Image<TPixel,2>  OutputImageType;
  typedef itk::ImageToImageFilter<InputImageType,OutputImageType> Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(IRISSlicer, ImageToImageFilter);

  /** Some typedefs. */
  typedef typename InputImageType::ConstPointer  InputImagePointer;
  typedef typename InputImageType::RegionType  InputImageRegionType; 
  typedef typename OutputImageType::Pointer  OutputImagePointer;
  typedef typename OutputImageType::RegionType  OutputImageRegionType;
  typedef itk::ImageSliceConstIteratorWithIndex<InputImageType>  InputIteratorType;
  typedef itk::ImageRegionIterator<OutputImageType>  OutputIteratorType;

  /** Set the transform that maps the image space into the display space */
  void SetImageToDisplayTransform(const ImageCoordinateTransform &imageToDisplay);
  
  /** Get the transforms stored in the slicer */
  irisGetMacro(ImageToDisplay,ImageCoordinateTransform);
  irisGetMacro(DisplayToImage,ImageCoordinateTransform);

  /** Set the current slice index */
  itkSetMacro(SliceIndex,unsigned int);
  itkGetMacro(SliceIndex,unsigned int);

  /** Set the current axis direction */
  itkSetMacro(SliceAxis,unsigned int);
  itkGetMacro(SliceAxis,unsigned int);

protected:
  IRISSlicer();
  virtual ~IRISSlicer() {};
  void PrintSelf(std::ostream &s, itk::Indent indent) const;

  /** 
   * IRISSlicer can produce an image which is a different
   * resolution than its input image.  As such, IRISSlicer
   * needs to provide an implementation for
   * GenerateOutputInformation() in order to inform the pipeline
   * execution model.  The original documentation of this method is
   * below.
   *
   * \sa ProcessObject::GenerateOutputInformaton()  */
  virtual void GenerateOutputInformation();

  /**
   * This method maps an input region to an output region
   */
  virtual void CallCopyOutputRegionToInputRegion(InputImageRegionType &destRegion,
                              const OutputImageRegionType &srcRegion);

  /** 
   * IRISSlicer is not implemented as a multithreaded filter.
   * \sa ImageToImageFilter::GenerateData()  
   */
  virtual void GenerateData();

private:
  IRISSlicer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // Transform from image to the slices
  ImageCoordinateTransform m_ImageToDisplay;
   
  // Transform from slices to the image
  ImageCoordinateTransform m_DisplayToImage;

  // Current slice in each of the dimensions
  unsigned int m_SliceIndex;

  // Current axis direction
  unsigned int m_SliceAxis;

  // Axes in display space that correspond to the z,x,y axes of the slice
  // (The z axes of the slice is the dimension in which the slice moves)
  Vector3i m_DisplayAxes;

  // Axes in image space that correspond to the z,x,y axes of the slice
  // (The z axes of the slice is the dimension in which the slice moves)
  Vector3i m_ImageAxes;

  // The direction in which the image axes are traversed (+1 or -1)
  Vector3i m_AxesDirection;

  // This method computes the axes.  Since this is so fast, the method is run 
  // each time the axes are needed
  void ComputeAxes();

  // The worker method in this filter
  void CopySlice(InputIteratorType itImage, OutputIteratorType itSlice, 
                 int sliceDir, int lineDir);
};

#ifndef ITK_MANUAL_INSTANTIATION
#include "IRISSlicer.txx"
#endif

#endif //__IRISSlicer_h_
