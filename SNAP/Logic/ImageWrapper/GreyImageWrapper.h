/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    GreyImageWrapper.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __GreyImageWrapper_h_
#define __GreyImageWrapper_h_

#include "ImageWrapper.h"
#include "IRISTypes.h"

namespace itk {
    template<class TPixel,unsigned int VDimension> class Image;
    template<class TInput,class TOutput> class FunctionBase;
};

/**
 * \class GreyImageWrapper
 * \brief Image wrapper used for greyscale images in IRIS/SNAP.
 * 
 * Adds ability to remap intensity from short to byte using an
 * arbitrary function when outputing slices.
 */
class GreyImageWrapper : virtual public ImageWrapper<GreyType> 
{
public:
    // Definition of the intensity map function
    typedef itk::FunctionBase<float,float> IntensityMapType;

    // Definition for the display slice type
    typedef itk::Image<unsigned char,2> DisplaySliceType;
    typedef itk::SmartPointer<DisplaySliceType> DisplaySlicePointer;


    /**
     * Set the intensity curve to be used for mapping
     * image intensities for producing slices
     */
    virtual void SetIntensityMapFunction(IntensityMapType *curve) = 0;

    /**
     * Get the display slice in a given direction.  To change the
     * display slice, call parent's MoveToSlice() method
     */
    virtual DisplaySlicePointer GetDisplaySlice(unsigned int dim) = 0;

};

#endif // __GreyImageWrapper_h_
