/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    MATITKTemplatedVariables.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef MATITKTEMPLATEDVARIABLES_H
#define MATITKTEMPLATEDVARIABLES_H
#include "seedcontainer.h"
template<class ITKPIXELTYPE>
class MATITKTemplatedVariables{
#include "typedefs.inl"
public:
  std::vector< ITKPIXELTYPEArray<ITKPIXELTYPE> > pixelContainers;
  typename ImageType::PixelContainerPointer pixelContainer; 
  MATPARAMTYPE* pParameters;  
  typename ImportFilterType::Pointer importFilter[2]; 
  SeedContainer<MATSEEDTYPE> seedsIndex;
};
#endif
