/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    LabelImageWrapper.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __LabelImageWrapper_h_
#define __LabelImageWrapper_h_

#include "ImageWrapper.h"
#include "itkRGBAPixel.h"

// Forward references
class ColorLabel;

/**
 * \class LabelImageWrapper
 * \brief ImageWrapper for segmentation images in SNAP/IRIS.
 * 
 * An extension of the ImageWrapper class for dealing with segmentation
 * images.  Using a table of color labels, it is possible to get RGBA 
 * slices from these images.
 *
 * \sa ImageWrapper, LabelImageWrapperImplementation
 */
class LabelImageWrapper : virtual public ImageWrapper<LabelType>
{
public:
  // Type of color slice returned by this class
  typedef itk::RGBAPixel<unsigned char> DisplayPixelType;
  // typedef vnl_vector_fixed<unsigned char,4> DisplayPixelType;
  typedef itk::Image<DisplayPixelType,2> DisplaySliceType;
  typedef itk::SmartPointer<DisplaySliceType> DisplaySlicePointer;
    
  /**
   * Set the table of color labels used to produce color slice images
   */  
  virtual void SetLabelColorTable(ColorLabel *labels) = 0;

  /**
   * Get the color label table
   */
  virtual ColorLabel *GetLabelColorTable() const = 0;

  /**
   * Tell the object to update it's color mapping cache
   * TODO: Implement this with ModifiedTime stuff
   */
  virtual void UpdateColorMappingCache() = 0;

  /**
   * Get a color slice for display purposes
   */
  virtual DisplaySlicePointer GetDisplaySlice(unsigned int dim) = 0;
};
















#endif // __SegmentationImageWrapper_h_
