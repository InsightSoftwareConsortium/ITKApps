/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    FEMTruss.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkFEMElementBase.h"
#include "itkFEMObject.h"
#include "itkFEMSolver.h"
#include "itkFEMLinearSystemWrapperVNL.h"
#include "itkFEMLoadBC.h"
#include "itkFEMLoadNode.h"
#include "itkFEMMaterialLinearElasticity.h"
#include "itkFEMElement2DC1Beam.h"
#include "itkFEMElement2DC0LinearLineStress.h"
#include <iostream>



/**
 * This example constructs a same problem as described in file truss.fem
 * by creating the appropriate classes.
 */
int main( int, char * [] ) {

  itk::FEMFactoryBase::GetFactory()->RegisterDefaultTypes();

  /*
   * First we create the FEM solver object. This object stores pointers
   * to all objects that define the FEM problem. One solver object
   * effectively defines one FEM problem.
   */
  typedef itk::fem::Solver<2> SolverType;
  SolverType::Pointer S = SolverType::New();

  /*
   * Set the linear system wrapper object that we wish to use.
   */
  itk::fem::LinearSystemWrapperVNL vnlSolver;
  vnlSolver.SetMaximumNonZeroValuesInMatrix(1000,1000);
  S->SetLinearSystemWrapper(&vnlSolver);


  /*
   * Below we'll define a FEM problem described in the chapter 25.3-4,
   * from the book, which can also be downloaded from 
   * http://titan.colorado.edu/courses.d/IFEM.d/IFEM.Ch25.d/IFEM.Ch25.pdf
   *
   *
   * We start by creating four Node objects. One of them is of
   * class NodeXY. It has two displacements in two degrees of freedom.
   * 3 of them are of class NodeXYrotZ, which also includes the rotation
   * around Z axis.
   */
  
  /*
   * Initialize the data members inside the node objects. Basically here
   * we only have to specify the vector of X and Y coordinate of the
   * node in global coordinate system.
   */

  itk::fem::Element::Node::Pointer n1;
  n1 = itk::fem::Element::Node::New();
  itk::fem::Element::VectorType pt(2);
  pt[0]=-4.0;
  pt[1]=3.0;
  n1->SetCoordinates(pt);

  /*
   * Add the node to the FEM Object.
   */
  typedef itk::fem::FEMObject<2> FEMObjectType;
  FEMObjectType::Pointer femObject = FEMObjectType::New();
  femObject->AddNextNode(n1.GetPointer());

  /*
   * Create three more nodes in the same way.
   */
  n1 = itk::fem::Element::Node::New();
  pt[0]=0.0;
  pt[1]=3.0;
  n1->SetCoordinates(pt);
  femObject->AddNextNode(n1.GetPointer());

  n1=itk::fem::Element::Node::New();
  pt[0]=4.0;
  pt[1]=3.0;
  n1->SetCoordinates(pt);
  femObject->AddNextNode(n1.GetPointer());

  n1=itk::fem::Element::Node::New();
  pt[0]=0.0;
  pt[1]=0.0;
  n1->SetCoordinates(pt);
  femObject->AddNextNode(n1.GetPointer());

  /*
   * Automatically assign the global numbers (IDs) to
   * all the objects in the array. (first object gets number 0,
   * second 1, and so on). We could have also specified the GN
   * member in all the created objects above, but this is easier.
   */
  femObject->RenumberNodeContainer();


  /*
   * Then we have to create the materials that will define
   * the elements.
   */
  itk::fem::MaterialLinearElasticity::Pointer m;
  m=itk::fem::MaterialLinearElasticity::New();
  m->SetGlobalNumber( 0 );
  m->SetYoungsModulus( 30000.0 );
  m->SetCrossSectionalArea( 0.02 );
  m->SetMomentOfInertia( 0.004 );
  femObject->AddNextMaterial( m );

  m=itk::fem::MaterialLinearElasticity::New();
  m->SetGlobalNumber( 1 );
  m->SetYoungsModulus( 200000.0 );
  m->SetCrossSectionalArea( 0.001 );
  m->SetMomentOfInertia( 0.0 );
  femObject->AddNextMaterial( m );

  m=itk::fem::MaterialLinearElasticity::New();
  m->SetGlobalNumber( 2 );
  m->SetYoungsModulus( 200000.0 );
  m->SetCrossSectionalArea( 0.003 );
  m->SetMomentOfInertia( 0.0 );
  femObject->AddNextMaterial( m );


  /*
   * Next we create the finite elements that use the above
   * created nodes. We'll have 3 Bar elements ( a simple
   * spring in 2D space ) and 2 Beam elements that also
   * accounts for bending.
   */
  itk::fem::Element2DC1Beam::Pointer e1;
  itk::fem::Element2DC0LinearLineStress::Pointer e2;

  e1=itk::fem::Element2DC1Beam::New();

  /*
   * Initialize the pointers to correct node objects. We use the
   * Find function of the FEMPArray to search for object (in this
   * case node) with given GN.
   */
  e1->SetGlobalNumber( 0 );
  e1->SetNode(0, femObject->GetNode(0) );
  e1->SetNode(1, femObject->GetNode(1) );
  e1->SetMaterial( femObject->GetMaterial(0).GetPointer() );
  femObject->AddNextElement( e1.GetPointer());

  /* Create the other elements */
  e1=itk::fem::Element2DC1Beam::New();
  e1->SetGlobalNumber( 1 );
  e1->SetNode(0, femObject->GetNode(1) );
  e1->SetNode(1, femObject->GetNode(2) );
  e1->SetMaterial( femObject->GetMaterial(0).GetPointer() );
  femObject->AddNextElement( e1.GetPointer());

  /*
   * Note that Bar2D element defines only two degrees of freedom
   * per node, while Beam2D defines three. In this case Bar only shares
   * the first two with Beam.
   */
  e2=itk::fem::Element2DC0LinearLineStress::New();
  e2->SetGlobalNumber( 2 );
  e2->SetNode(0, femObject->GetNode(0) );
  e2->SetNode(1, femObject->GetNode(3) );
  e2->SetMaterial( femObject->GetMaterial(1).GetPointer() );
  femObject->AddNextElement( e2.GetPointer());


  e2=itk::fem::Element2DC0LinearLineStress::New();
  e2->SetGlobalNumber( 3 );
  e2->SetNode(0, femObject->GetNode(1) );
  e2->SetNode(1, femObject->GetNode(3) );
  e2->SetMaterial( femObject->GetMaterial(2).GetPointer() );
  femObject->AddNextElement( e2.GetPointer());

  e2=itk::fem::Element2DC0LinearLineStress::New();
  e2->SetGlobalNumber( 4 );
  e2->SetNode(0, femObject->GetNode(2) );
  e2->SetNode(1, femObject->GetNode(3) );
  e2->SetMaterial( femObject->GetMaterial(1).GetPointer() );
  femObject->AddNextElement( e2.GetPointer());


  /*
   * Apply the boundary conditions and external forces (loads).
   */

  /*
   * The first node is completely fixed i.e. both displacements
   * are fixed to 0.
   * 
   * This is done by using the LoadBC class.
   */
  itk::fem::LoadBC::Pointer l1;

  l1=itk::fem::LoadBC::New();

  /*
   * Here we're saying that the first degree of freedom at first node
   * is fixed to value m_value=0.0. See comments in class LoadBC declaration
   * for more information. Note that the m_value is a vector. This is useful
   * when having isotropic elements. This is not the case here, so we only
   * have a scalar.
   */

  l1->SetElement( femObject->GetElement(0) );
  l1->SetGlobalNumber(0);
  l1->SetDegreeOfFreedom(0);
  l1->SetValue( vnl_vector<double>(1, 0.0) );
  femObject->AddNextLoad( l1 );


  /*
   * In a same way we also fix the second DOF in a first node and the
   * second DOF in a third node (it's only fixed in Y direction).
   */
  l1=itk::fem::LoadBC::New();
  l1->SetElement( femObject->GetElement(0) );
  l1->SetGlobalNumber(1);
  l1->SetDegreeOfFreedom(1);
  l1->SetValue( vnl_vector<double>(1, 0.0) );
  femObject->AddNextLoad( l1 );

  l1=itk::fem::LoadBC::New();
  l1->SetElement( femObject->GetElement(1) );
  l1->SetGlobalNumber(2);
  l1->SetDegreeOfFreedom(4);
  l1->SetValue( vnl_vector<double>(1, 0.0) );
  femObject->AddNextLoad( l1 );


  /*
   * Now we apply the external force on the fourth node. The force is specified
   * by a vector [20,-20] in global coordinate system. The force acts on tthe
   * second node of the third element in a system.
   */
  itk::fem::LoadNode::Pointer l2;
  l2=itk::fem::LoadNode::New();

  l2->SetElement( femObject->GetElement(2) );
  l2->SetGlobalNumber(3);
  l2->SetNode( 1 );
  vnl_vector<double> force(2);
  force[0] = 20.0;
  force[1] = -20.0;
  l2->SetForce( force );
  femObject->AddNextLoad( l2 );


  /*
   * The whole problem is now stored inside the FEM Object class.
   * We can now solve for displacements.
   */
  femObject->FinalizeMesh();
  S->SetInput( femObject );
  S->Update();


  /*
   * Output displacements of all nodes in a system;
   */
  std::cout<<"\nNodal displacements:\n";
  const unsigned int invalidID = itk::fem::Element::InvalidDegreeOfFreedomID;
  int numberOfNodes = S->GetInput()->GetNumberOfNodes();
  for( int i = 0; i < numberOfNodes; i++ )
    {
    itk::fem::Element::Node::Pointer n1 = S->GetInput()->GetNode(i);
    std::cout<<"Node#: ";
    std::cout << n1->GetGlobalNumber( );
    std::cout << ": ";

    for( unsigned int d=0, dof; (dof=n1->GetDegreeOfFreedom(d))!=invalidID; d++ )
      {
      std::cout << S->GetSolution(dof);
      std::cout << ",  ";
      }
    std::cout<<"\b\b\b \b\n";
    }

  std::cout << "\n";

  return 0;

}
