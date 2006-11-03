/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    vtkWSBoundingBox.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// .Name vtkWSBoundingBox
// .Section Description
#ifndef __vtkWSBoundingBox_
#define __vtkWSBoundingBox_

#include "vtkObject.h"

class VTK_EXPORT vtkWSBoundingBox: public vtkObject
{
public:
  static vtkWSBoundingBox *New();

  vtkTypeMacro(vtkWSBoundingBox,vtkObject);
  void PrintSelf(ostream&, vtkIndent) {}

  const int *GetExtent() const
    {
      return Extent;
    }
  void SetExtent(int x0, int x1, int y0, int y1, int z0, int z1);
  void Merge(const vtkWSBoundingBox *);
  
protected:
  vtkWSBoundingBox();
  ~vtkWSBoundingBox() {}
  
private:
  int Extent[6];
    
};

#endif
