/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    TestImageWrapper.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __TestImageWrapper_h_
#define __TestImageWrapper_h_

#include "TestBase.h"
#include "ImageWrapper.h"
#include "ImageWrapper.txx"

/**
 * This class is used to test the functionality in the ImageWrapper class
 */
template<class TPixel>
class TestImageWrapper : public TestBaseOneImage<TPixel>
{
public:
  typedef TestBaseOneImage<TPixel> Superclass;
  typedef ImageWrapper<TPixel> WrapperType;

  void PrintUsage();
  void Run();
    
  const char *GetTestName() 
  { 
    return "ImageWrapper"; 
  }
  
  const char *GetDescription()
  { 
    return "A suite of ImageWrapper tests"; 
  }
};

template<class TPixel> 
void TestImageWrapper<TPixel> 
::PrintUsage() 
{
  // Run the parent's part of the test
  Superclass::PrintUsage();

  // RAI may be passed to this test
  cout << "  rai CODE : Pass in an RAI anatomy-image code" << endl;
}

template<class TPixel> 
void TestImageWrapper<TPixel> 
::Run() 
{
  // Run the parent's part of the test (loads image)
  Superclass::Run();

  // Do the rest
  cout << "Testing code in ImageWrapper.h" << endl;

  // Create an image wrapper
  WrapperType *wrapper = new WrapperType;

  // Insert image into the wrapper
  wrapper->SetImage(m_Image);
  
  // Set the cursor position in the slice wrapper
  wrapper->SetSliceIndex(wrapper->GetSize() / ((unsigned int)2));

  // Create a transform that specifies the image-slice geometry
  ImageCoordinateTransform ict;
  ict.SetTransform(Vector3i(-2,1,-3),wrapper->GetSize());
  
  // Get a slice in the z-direction in the image
  typename WrapperType::SlicePointer slice = wrapper->GetSlice(0);

  // Report min/max intensities
  cout << "Max intensity: " << wrapper->GetImageMax() << endl;
  cout << "Min intensity: " << wrapper->GetImageMin() << endl;

  // We are finished testing
  cout << "Testing complete" << endl;
}

#endif //__TestImageWrapper_h_




