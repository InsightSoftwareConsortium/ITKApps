/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    vtkWSBoundingBoxHash.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __vtkWSBoundingBoxHash_h_
#define __vtkWSBoundingBoxHash_h_

#include "itksys/hash_map.hxx"

struct bounding_box_t
{
  int x0;
  int x1;
  int y0;
  int y1;
  int z0;
  int z1;
};

typedef itksys::hash_map<unsigned long, bounding_box_t, itksys::hash<unsigned long> >
  vtkWSBoundingBoxHash;


#endif
