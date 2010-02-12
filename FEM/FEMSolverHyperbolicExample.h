/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    FEMSolverHyperbolicExample.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// disable debug warnings in MS compiler
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include "itkFEM.h"
//#include "itkFEMSolverHyperbolic.h"
#include "itkFEMLinearSystemWrappers.h"

#define OUTPUT_FIELD        1

#if OUTPUT_FIELD
#include "itkExceptionObject.h"
#include "itkImage.h"
#include "itkVector.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkVectorCastImageFilter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkWarpImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#endif

#include <iostream>
#include <fstream>
#include <exception>
#include <string>

#define DEFAULT_COMMENT     '.'
#define MATLAB_COMMENT      '%'
#define IDL_COMMENT         ';'

// Only one of the _OUTPUT variables should be nonzero, otherwise
// things will become confusing!  If both are zero, no output will be
// generated.
#define MATLAB_OUTPUT       1
#define IDL_OUTPUT          0
#define DEBUG               0
#define DEBUG_FEM_TESTS     ( ( MATLAB_OUTPUT || IDL_OUTPUT ) && DEBUG )

// Only one of these FORMAT_ variables should be nonzero!

#define FORMAT_OUTPUT_BINARY  1
#define FORMAT_OUTPUT_ASCII   0
#define OUTPUT              ( FORMAT_OUTPUT_BINARY || FORMAT_OUTPUT_ASCII )

#define FIELD_FREQ          10


void PrintK(itk::fem::SolverHyperbolic&, char, std::ostream&);
void PrintF(itk::fem::SolverHyperbolic&, char, std::ostream&);
void PrintNodalCoordinates(itk::fem::SolverHyperbolic&, char, std::ostream&);
void PrintElementCoordinates(itk::fem::SolverHyperbolic&, char, std::ostream&, int);
void PrintU(itk::fem::SolverHyperbolic&, char, std::ostream&, int);
