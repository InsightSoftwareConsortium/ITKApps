/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    FEMRead.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkFEMSolver.h"
#include "itkFEMObject.h"
#include "itkFEMObjectSpatialObject.h"
#include "itkGroupSpatialObject.h"
#include "itkSpatialObject.h"
#include "itkFEMSpatialObjectReader.h"

#include <iostream>
#include <fstream>
#include <string>


/**
 * This example reads a FEM problem from a *.meta file that is specified on
 * command line. If no argument is provided on command line, file
 * 'truss.meta' is read from the current folder.
 *
 * External forces, which are also specified inside a *.meta, are applied
 * and the problem is solved for displacements. Solution vector is output
 * on stdout.
 *
 * You'll probably have to check the problem definiton in a file, in order
 * to understand what these displacements mean.
 */
int main( int argc, char *argv[] ) {

const char* filename;

  if (argc>=2)
  {
    filename=argv[1];
  }
  else
  {
    /**
     * If the argument was not specified, we try default file name.
     */
    filename="truss.meta";
  }


  /**
   * First load tyhe FEM Object from the file
   */
  typedef itk::FEMSpatialObjectReader<2>      FEMSpatialObjectReaderType;
  typedef FEMSpatialObjectReaderType::Pointer FEMSpatialObjectReaderPointer;
  FEMSpatialObjectReaderPointer SpatialReader = FEMSpatialObjectReaderType::New();
  SpatialReader->SetFileName( argv[1] );
  try 
  {
    SpatialReader->Update();
  }
  catch (::itk::fem::FEMException e)
  {
    std::cout<<"Error reading FEM problem: "<<filename<<"!\n";
    e.Print(std::cout);
    return 1;
  }

  /** Second, get the FEM Object from the Spatial Object */
  typedef itk::FEMObjectSpatialObject<2>      FEMObjectSpatialObjectType;
  typedef FEMObjectSpatialObjectType::Pointer FEMObjectSpatialObjectPointer;
  FEMObjectSpatialObjectType::ChildrenListType* children = SpatialReader->GetGroup()->GetChildren();
  FEMObjectSpatialObjectType::Pointer femSO =
    dynamic_cast<FEMObjectSpatialObjectType *>( (*(children->begin() ) ).GetPointer() );
  delete children;

  femSO->GetFEMObject()->FinalizeMesh();

  /**
   * Third, create the FEM solver object and generate the solution
   */
  typedef itk::fem::Solver<2> FEMSolverType;
  FEMSolverType::Pointer S = FEMSolverType::New();
  S->SetInput( femSO->GetFEMObject() );
  S->Update();

  /**
   * Output displacements of all nodes in a system;
   */
  typedef itk::fem::FEMObject<2>      FEMObjectType;
  FEMObjectType::Pointer fem = S->GetInput();

  std::cout<<"\nNodal displacements:\n";
  unsigned int numberOfNodes = fem->GetNumberOfNodes();

  for(unsigned int i = 0; i<numberOfNodes; i++)
    {
    itk::fem::Element::Node::Pointer node = fem->GetNode( i );
    std::cout<<"Node#: "<< node->GetGlobalNumber() <<": ";
    /** For each DOF in the node... */
    for( unsigned int d=0, dof; (dof=node->GetDegreeOfFreedom(d))!=itk::fem::Element::InvalidDegreeOfFreedomID; d++ )
      {
      std::cout<<S->GetSolution(dof);
      std::cout<<",  ";
      }
    std::cout<<"\b\b\b \b\n";
    }

  return 0;

}
