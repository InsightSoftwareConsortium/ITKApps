/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageCoordinateTransform.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ImageCoordinateTransform_h_
#define __ImageCoordinateTransform_h_

#include "IRISTypes.h"
#include <vnl/vnl_matrix_fixed.h>

/**
 * \class ImageCoordinateTransform
 * Coordinate transform used to map between image space, anatomy space 
 * and display space (formerly known as RAI).
 */
class ImageCoordinateTransform 
{
public:
  /**
   * Constructor that calls SetTransform
   */
  ImageCoordinateTransform(int mapX = 1, int mapY = 2, int mapZ = 3);

  /**
   * Initialize the transform with new singed coordinate mappings 
   * (1-based signed indices)
   */
  void SetTransform(int newX, int newY, int newZ);

  /**
   * Compute the inverse of this transform
   */
  ImageCoordinateTransform Inverse() const;

  /**
   * Multiply by another transform
   */
  ImageCoordinateTransform Product(const ImageCoordinateTransform &t1) const;

  /**
   * Apply transform to a vector
   */
  Vector3f Apply(const Vector3f &x) const;

  /**
   * Apply to an integer vector (this is legal because the transform maps
   * integers to integers
   */
  Vector3i Apply(const Vector3i &x) const;

  /**
   * Get the index of a particular coordinate
   */
  unsigned int GetCoordinateIndexZeroBased(unsigned int c) const;

  /**
   * Get the orientation of a particular coordinate (returns 1 or -1)
   */
  int GetCoordinateOrientation(unsigned int c) const;

private:
  typedef vnl_matrix_fixed<float,3,3> MatrixType;
  typedef vnl_matrix_fixed<int,3,3> IntegerMatrixType;

  // A transform matrix
  MatrixType m_Transform;
  IntegerMatrixType m_IntegerTransform;

  // A mapping from coordinate to signed coordinate index
  // e.g. [-2,-1,0] means x->-z, y->-y, z->x
  int m_Mapping[3];
};

#endif
