/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageCoordinateTransform.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "ImageCoordinateTransform.h"

ImageCoordinateTransform
::ImageCoordinateTransform(int newX, int newY, int newZ)
{
  SetTransform(newX,newY,newZ);
}
                         
void 
ImageCoordinateTransform
::SetTransform(int newX, int newY, int newZ)
{
  assert(abs(newX) <= 3 && abs(newX) != 0);
  assert(abs(newY) <= 3 && abs(newY) != 0);
  assert(abs(newZ) <= 3 && abs(newZ) != 0);

  // Copy the mapping 
  m_Mapping[0] = newX;
  m_Mapping[1] = newY;
  m_Mapping[2] = newZ;

  // Initialize the transform
  m_Transform.fill(0);
  for(int i=0;i<3;i++)
    {
    if(m_Mapping[i] > 0)
      m_Transform(i,m_Mapping[i]-1) = 1.0;
    else
      m_Transform(i,1-m_Mapping[i]) = -1.0;
    }

  // Copy to the integer transform
  for(unsigned int r=0;r<m_Transform.rows();r++)
    for(unsigned int c=0;c<m_Transform.rows();c++)
      m_IntegerTransform(r,c) = (int) m_Transform(r,c);
}

ImageCoordinateTransform
ImageCoordinateTransform
::Inverse() const
{
  // Compute the transpose of the transform
  int newMapping[3];
  for(int i=0;i<3;i++)
    {
    if(m_Mapping[i] > 0)
      newMapping[m_Mapping[i]-1] = i+1;
    else
      newMapping[1-m_Mapping[i]] = -i-1;
    }

  return ImageCoordinateTransform(newMapping[0],newMapping[1],newMapping[2]);
}

ImageCoordinateTransform 
ImageCoordinateTransform
::Product(const ImageCoordinateTransform &t1) const
{
  // Multiply two transforms
  int newMapping[3];
  for(int i=0;i<3;i++)
    {
    if(m_Mapping[i] > 0)
      newMapping[i] = t1.m_Mapping[m_Mapping[i]-1];
    else
      newMapping[i] = -t1.m_Mapping[1-m_Mapping[i]];
    }

  return ImageCoordinateTransform(newMapping[0],newMapping[1],newMapping[2]);
}

Vector3f 
ImageCoordinateTransform
::Apply(const Vector3f &x) const 
{
  return m_Transform * x;
}

Vector3i 
ImageCoordinateTransform
::Apply(const Vector3i &x) const 
{
  return m_IntegerTransform * x;
}

unsigned int 
ImageCoordinateTransform
::GetCoordinateIndexZeroBased(unsigned int c) const 
{
  return abs(m_Mapping[c]) - 1;
}

int 
ImageCoordinateTransform
::GetCoordinateOrientation(unsigned int c) const 
{
  return m_Mapping[c] < 0 ? -1 : 1;
}
