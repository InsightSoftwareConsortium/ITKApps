/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageWrapper.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ImageWrapper_txx_
#define __ImageWrapper_txx_

#include "itkImageRegionIterator.h"
#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkNumericTraits.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

#include "IRISSlicer.h"

#include <iostream>
using namespace std;

template <class TPixel> 
ImageWrapper<TPixel>
::ImageWrapper() 
{
  CommonInitialization();
}

template <class TPixel>
ImageWrapper<TPixel>
::~ImageWrapper() 
{
  Reset();
}

template <class TPixel>
void 
ImageWrapper<TPixel>
::CommonInitialization()
{
  // Set initial state    
  m_Initialized = false;

  // Create slicer objects
  m_Slicer[0] = SlicerType::New();
  m_Slicer[1] = SlicerType::New();
  m_Slicer[2] = SlicerType::New();

  // Set the transform to identity, which will initialize the directions of the
  // slicers
  this->SetImageToDisplayTransformsToDefault();
}

template <class TPixel>
ImageWrapper<TPixel>
::ImageWrapper(const ImageWrapper<TPixel> &copy) 
{
  CommonInitialization();

  // If the source contains an image, make a copy of that image
  if (copy.IsInitialized() && copy.GetImage())
    {
    ImagePointer newImage = 
      copy.DeepCopyRegion(copy.GetImage()->GetLargestPossibleRegion());

    UpdateImagePointer(newImage);
    }
}

template <class TPixel> 
void 
ImageWrapper<TPixel>
::PrintDebugInformation() 
{
  cout << "=== Image Properties ===" << endl;
  cout << "   Dimensions         : " << m_Image->GetLargestPossibleRegion().GetSize() << endl;
  cout << "   Origin             : " << m_Image->GetOrigin() << endl;
  cout << "   Spacing            : " << m_Image->GetSpacing() << endl;
  cout << "------------------------" << endl;
}

template <class TPixel>
void 
ImageWrapper<TPixel>
::UpdateImagePointer(ImageType *newImage) 
{
  // Check if the image size has changed
  bool hasSizeChanged = 
    (!m_Image) || 
    (newImage->GetLargestPossibleRegion().GetSize() !=
     m_Image->GetLargestPossibleRegion().GetSize());
  
  // Change the input of the slicers 
  m_Slicer[0]->SetInput(newImage);
  m_Slicer[1]->SetInput(newImage);
  m_Slicer[2]->SetInput(newImage);
    
  // If so, the coordinate transform needs to be reinitialized to identity
  if(hasSizeChanged)
    {
    // Reset the transform to identity
    this->SetImageToDisplayTransformsToDefault();

    // Reset the slice positions to zero
    this->SetSliceIndex(Vector3ui(0,0,0));
    }
  
  // Update the max-min pipeline once we have one setup
  m_MinMaxCalc = MinMaxCalculatorType::New();
  m_MinMaxCalc->SetImage(newImage);

  // Update the image
  m_Image = newImage;

  // Mark the image as Modified to enforce correct sequence of 
  // operations with MinMaxCalc
  m_Image->Modified();

  // We have been initialized
  m_Initialized = true;
}

template <class TPixel> 
void 
ImageWrapper<TPixel>
::InitializeToSize(unsigned int x, unsigned int y, unsigned int z) 
{
  itk::Size<3> size;
  size[0] = x;
  size[1] = y;
  size[2] = z;

  ImagePointer newImage = ImageType::New();
  newImage->SetRegions(size);
  newImage->Allocate();

  UpdateImagePointer(newImage);
}

template <class TPixel>
void 
ImageWrapper<TPixel>
::InitializeToImage(itk::ImageBase<3> *source) 
{
  itk::Size<3> size = source->GetLargestPossibleRegion().GetSize();
  ImagePointer newImage = ImageType::New();
  newImage->SetRegions(size);
  newImage->Allocate();

  newImage->SetOrigin(source->GetOrigin());
  newImage->SetSpacing(source->GetSpacing());

  UpdateImagePointer(newImage);
}

template <class TPixel>
void 
ImageWrapper<TPixel>
::SetImage(ImagePointer newImage) 
{
  UpdateImagePointer(newImage);
}


template <class TPixel>
void 
ImageWrapper<TPixel>
::Reset() 
{
  if (m_Initialized)
    m_Image->ReleaseData();
  m_Initialized = false;
}

template <class TPixel>
bool 
ImageWrapper<TPixel>
::IsInitialized() const 
{
  return m_Initialized;
}

template <class TPixel>
typename ImageWrapper<TPixel>::ImagePointer
ImageWrapper<TPixel>
::DeepCopyRegion(const itk::ImageRegion<3> region) const
{
  typedef itk::RegionOfInterestImageFilter <ImageType,ImageType> FilterType;

  // Create a filter to 'cut' the image
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(m_Image);
  filter->SetRegionOfInterest(region);
  filter->Update();

  // Return the resulting image
  return filter->GetOutput();
}

template <class TPixel>
inline const TPixel& 
ImageWrapper<TPixel>
::GetVoxel(const Vector3ui &index) const 
{
  return GetVoxel(index[0],index[1],index[2]);
}

template <class TPixel>
inline TPixel& 
ImageWrapper<TPixel>
::GetVoxelForUpdate(const Vector3ui &index) 
{
  return GetVoxelForUpdate(index[0],index[1],index[2]);
}

template <class TPixel>
inline TPixel& 
ImageWrapper<TPixel>
::GetVoxelForUpdate(unsigned int x, unsigned int y, unsigned int z) 
{
  itk::Index<3> index;
  index[0] = x;
  index[1] = y;
  index[2] = z;

  // Verify that the pixel is contained by the image at debug time
  assert(m_Image && m_Image->GetLargestPossibleRegion().IsInside(index));

  // Return the pixel
  return m_Image->GetPixel(index);
}

template <class TPixel>
inline const TPixel& 
ImageWrapper<TPixel>
::GetVoxel(unsigned int x, unsigned int y, unsigned int z) const
{
  itk::Index<3> index;
  index[0] = x;
  index[1] = y;
  index[2] = z;

  // Verify that the pixel is contained by the image at debug time
  assert(m_Image && m_Image->GetLargestPossibleRegion().IsInside(index));

  // Return the pixel
  return m_Image->GetPixel(index);
}


template <class TPixel> 
void 
ImageWrapper<TPixel>
::CheckImageIntensityRange() 
{
  // Image should be loaded
  assert(m_Image && m_MinMaxCalc);

  // Check if the image has been updated since the last time that
  // the min/max has been computed
  if (m_Image->GetMTime() > m_MinMaxCalc->GetMTime())
    {
    m_MinMaxCalc->Compute();
    m_MinMaxCalc->Modified();
    m_ImageScaleFactor = 1.0 / (m_MinMaxCalc->GetMaximum() - m_MinMaxCalc->GetMinimum());

    verbose << "Computing intensity range:" << endl;
    verbose << "   " << m_MinMaxCalc->GetMinimum() << " to " << m_MinMaxCalc->GetMaximum() << endl;
    }
}

template <class TPixel> 
TPixel 
ImageWrapper<TPixel>
::GetImageMin() 
{
  // Make sure min/max are up-to-date
  CheckImageIntensityRange();

  // Return the max or min
  return m_MinMaxCalc->GetMinimum();
}

template <class TPixel>
double 
ImageWrapper<TPixel>
::GetImageScaleFactor()
{
  // Make sure min/max are up-to-date
  CheckImageIntensityRange();

  // Return the max or min
  return m_ImageScaleFactor;    
}

template <class TPixel> 
TPixel 
ImageWrapper<TPixel>
::GetImageMax()
{
  // Make sure min/max are up-to-date
  CheckImageIntensityRange();

  // Return the max or min
  return m_MinMaxCalc->GetMaximum();
}

template <class TPixel> 
typename ImageWrapper<TPixel>::ConstIterator 
ImageWrapper<TPixel>
::GetImageConstIterator() const 
{
  ConstIterator it(m_Image,m_Image->GetLargestPossibleRegion());
  it.GoToBegin();
  return it;
}

template <class TPixel> 
typename ImageWrapper<TPixel>::Iterator 
ImageWrapper<TPixel>
::GetImageIterator() 
{
  Iterator it(m_Image,m_Image->GetLargestPossibleRegion());
  it.GoToBegin();
  return it;
}

template <class TPixel>    
void 
ImageWrapper<TPixel>
::RemapIntensityToRange(TPixel min, TPixel max)
{
  typedef itk::RescaleIntensityImageFilter<ImageType> FilterType;
  typedef typename FilterType::Pointer FilterPointer;

  // Create a filter to remap the intensities
  FilterPointer filter = FilterType::New();
  filter->SetInput(m_Image);
  filter->SetOutputMinimum(min);
  filter->SetOutputMaximum(max);

  // Run the filter
  filter->Update();

  // Store the output and point everything to it
  UpdateImagePointer(filter->GetOutput());
}

template <class TPixel>    
void 
ImageWrapper<TPixel>
::RemapIntensityToMaximumRange()
{
  // Only allowed on integer data types, o/w makes no sense!
  assert(itk::NumericTraits<TPixel>::is_integer);

  RemapIntensityToRange(itk::NumericTraits<TPixel>::min(),
                        itk::NumericTraits<TPixel>::max());
}

template <class TPixel>    
void 
ImageWrapper<TPixel>
::SetSliceIndex(const Vector3ui &cursor)
{
  // Save the cursor position
  m_SliceIndex = cursor;

  // Select the appropriate slice for each slicer
  for(unsigned int i=0;i<3;i++)
  {
    // Which axis does this slicer slice?
    unsigned int axis = m_Slicer[i]->GetSliceDirectionImageAxis();

    // Set the slice using that axis
    m_Slicer[i]->SetSliceIndex(cursor[axis]);
  }
}

template <class TPixel>    
void 
ImageWrapper<TPixel>
::SetImageToDisplayTransformsToDefault()
{
  ImageCoordinateTransform id[3];
  id[0].SetTransform(Vector3i(1,2,3),Vector3ui(0,0,0));
  id[1].SetTransform(Vector3i(1,3,2),Vector3ui(0,0,0));
  id[2].SetTransform(Vector3i(2,3,1),Vector3ui(0,0,0));
  SetImageToDisplayTransform(0,id[0]);
  SetImageToDisplayTransform(1,id[1]);
  SetImageToDisplayTransform(2,id[2]);
}


template <class TPixel>    
void 
ImageWrapper<TPixel>
::SetImageToDisplayTransform(unsigned int iSlice,
                             const ImageCoordinateTransform &transform)
{
  // Get the transform and its inverse
  m_ImageToDisplayTransform[iSlice] = transform;
  m_DisplayToImageTransform[iSlice] = transform.Inverse();

  // Tell slicer in which directions to slice
  m_Slicer[iSlice]->SetSliceDirectionImageAxis(
    m_DisplayToImageTransform[iSlice].GetCoordinateIndexZeroBased(2));
  
  m_Slicer[iSlice]->SetLineDirectionImageAxis(
    m_DisplayToImageTransform[iSlice].GetCoordinateIndexZeroBased(1));

  m_Slicer[iSlice]->SetPixelDirectionImageAxis(
    m_DisplayToImageTransform[iSlice].GetCoordinateIndexZeroBased(0));

  m_Slicer[iSlice]->SetPixelTraverseForward(
    m_DisplayToImageTransform[iSlice].GetCoordinateOrientation(0) > 0);

  m_Slicer[iSlice]->SetLineTraverseForward(
    m_DisplayToImageTransform[iSlice].GetCoordinateOrientation(1) > 0);
}


  /** For each slicer, find out which image dimension does is slice along */

template <class TPixel>    
unsigned int 
ImageWrapper<TPixel>
::GetDisplaySliceImageAxis(unsigned int iSlice)
{
  return m_Slicer[iSlice]->GetSliceDirectionImageAxis();
}

template <class TPixel>    
Vector3ui
ImageWrapper<TPixel>
::GetSliceIndex() const
{
  return m_SliceIndex;
}

template <class TPixel>
ImageWrapper<TPixel>::SliceType*
ImageWrapper<TPixel>
::GetSlice(unsigned int dimension)
{
  return m_Slicer[dimension]->GetOutput();
}

template <class TPixel>
const TPixel *
ImageWrapper<TPixel>
::GetVoxelPointer() const
{
  return m_Image->GetBufferPointer();
}

template <class TPixel>
Vector3ui
ImageWrapper<TPixel>
::GetSize() const
{
  // Cast the size to our vector format
  itk::Size<3> size = m_Image->GetLargestPossibleRegion().GetSize();      
  return Vector3ui(
    (unsigned int) size[0],
    (unsigned int) size[1],
    (unsigned int) size[2]);
}

#endif // __ImageWrapper_txx_
