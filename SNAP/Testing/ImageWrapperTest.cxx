/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageWrapperTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "ImageWrapperImplementation.h"

/**
 * This class is used to test the functionality in the ImageWrapper class
 * (This is my attempt to keep the template code in CXX files)
 */
template<class TPixel> class ImageWrapperTester {
public:
  void RunAllTests(const char *fname) 
    {
    cout << "Testing code in ImageWrapper.h" << endl;

    // Create an image wrapper
    typedef ImageWrapperImplementation<TPixel> WrapperType;
    WrapperType wrapper;

    // Initialize with zeroes
    wrapper.InitializeToSize(10,10,10);

    // Access a pixel
    cout << "Pixel at 2,3,4 is " << wrapper.GetVoxel(2,3,4) << endl;

    // Get a slice out of the image
    wrapper.SetSliceIndex(Vector3i(5,5,5));
    typename WrapperType::SlicePointer slice = wrapper.GetSlice(0);

    // Load an image from a file
    try {
      wrapper.LoadFromFile(fname);
      cout << "File loaded OK" << endl;
    } catch(...) {
      cout << "File did not load" << endl;
    }

    // Report min/max
    cout << "Max intensity " << wrapper.GetImageMax() << endl;

    // Test replication
    ImageWrapperImplementation<TPixel> ident;
    ident.InitializeToImage(wrapper.GetImage());

    // We are finished testing
    cout << "Testing complete" << endl;
    }
};

void TestTemplateImageWrapper(const char *fname) {
  ImageWrapperTester<short> tstShort;
  ImageWrapperTester<unsigned char> tstUChar;
  ImageWrapperTester<float> tstFloat;

  tstShort.RunAllTests(fname);
  tstUChar.RunAllTests(fname);
  tstFloat.RunAllTests(fname);
}






