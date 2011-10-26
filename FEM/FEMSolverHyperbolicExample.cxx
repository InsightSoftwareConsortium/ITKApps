/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    FEMSolverHyperbolicExample.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkMacro.h"
#include "itkImage.h"
#include "itkVector.h"
#include "itkImageFileReader.h"
#include "itkFEMSolverHyperbolic.h"
#include "itkFEMObject.h"
#include "itkFEMObjectSpatialObject.h"
#include "itkGroupSpatialObject.h"
#include "itkSpatialObject.h"
#include "itkFEMSpatialObjectReader.h"
#include "itkFEMSpatialObjectWriter.h"
#include "itkFEMLinearSystemWrapper.h"
#include "itkFEMLinearSystemWrapperDenseVNL.h"
#include "itkFEMLinearSystemWrapperItpack.h"
#include "itkFEMLinearSystemWrapperVNL.h"
#include "itkFEMFactory.h"


typedef itk::fem::SolverHyperbolic<2> FEMSolverType;


// Print K - the global stiffness matrix
void PrintK(FEMSolverType *S)
{
  itk::fem::LinearSystemWrapper::Pointer lsw = S->GetLinearSystemWrapper();

  std::cout << std::endl << "k" << "=[";
  for( unsigned int j = 0; j < lsw->GetSystemOrder(); j++ )
    {
    std::cout << " [";
    for( unsigned int k = 0; k < lsw->GetSystemOrder(); k++ )
      {
      std::cout << lsw->GetMatrixValue(j, k);
      if( (k + 1) < lsw->GetSystemOrder() )
        {
        std::cout << ", ";
        }
      }
    if( j < lsw->GetSystemOrder() - 1 )
      {
      std::cout << " ]," << std::endl;
      }
    else
      {
      std::cout << "]";
      }
    }
  std::cout << "];" << std::endl;
}

// Print F - the global load vector
void PrintF(FEMSolverType *S)
{
  itk::fem::LinearSystemWrapper::Pointer lsw = S->GetLinearSystemWrapper();

  std::cout << std::endl << "f" << "=[";
  for( unsigned int j = 0; j < lsw->GetSystemOrder(); j++ )
    {
    if( j > 0 )
      {
      std::cout << ",  ";
      }
    std::cout << lsw->GetVectorValue(j);
    }
  std::cout << "];" << std::endl;
}

void PrintNodalCoordinates(FEMSolverType *S)
// Print the nodal coordinates
{
  std::cout << std::endl << "Nodal coordinates: " << std::endl;

  std::cout << "xyz" << "=[";

  int numberOfNodes = S->GetInput()->GetNumberOfNodes();
  for( int i = 0; i < numberOfNodes; i++ )
    {
    std::cout << " [";
    std::cout << S->GetInput()->GetNode(i)->GetCoordinates();
    std::cout << "]";
    }
  std::cout << "];" << std::endl;
}


// Useful for display purposes - lets you draw each element
// individually, instead of just a stream of nodes
void PrintElementCoordinates(FEMSolverType *S, char comment, std::ostream& of, int iter)
{
  std::cout << std::endl << "Element coordinates: " << std::endl;
  int ct = 1;

  const unsigned int invalidID = itk::fem::Element::InvalidDegreeOfFreedomID;

  int numberOfElements = S->GetInput()->GetNumberOfElements();

  for(int i = 0; i < numberOfElements; i++ )
    {
    of << "e(" << ct << ",:,:)=[";

    for (unsigned int n=0; n < S->GetInput()->GetElement(i)->GetNumberOfNodes(); n++)
      {
      itk::fem::Element::VectorType nc = S->GetInput()->GetElement(i)->GetNodeCoordinates(n);

      for (unsigned int d=0, dof; ( dof = S->GetInput()->GetElement(i)->GetNode(n)->GetDegreeOfFreedom(d) ) != invalidID; d++)
        {
        nc[d] += S->GetSolution( dof );
        }
      std::cout << nc << std::endl;
      }
    std::cout << "];" << std::endl;
    ct++;
  }
}


int main(int ac, char** av)
{

  if (ac < 5)
    {
    std::cout << "Usage: " << av[0];
    std::cout << " input-file inputImage iterations lsw (0=VNL, 1=Dense VNL, 2=Itpack)";
    std::cout << std::endl;
    return 1;
    }

  itk::FEMFactoryBase::GetFactory()->RegisterDefaultTypes();

  unsigned int niter = atoi ( av[3] );
  unsigned int w = atoi( av[4] );

  typedef itk::FEMSpatialObjectReader<2>      FEMSpatialObjectReaderType;
  typedef FEMSpatialObjectReaderType::Pointer FEMSpatialObjectReaderPointer;
  FEMSpatialObjectReaderPointer SpatialReader = FEMSpatialObjectReaderType::New();
  SpatialReader->SetFileName( av[1] );
  try
    {
    SpatialReader->Update();
    }
  catch (::itk::fem::FEMException e)
    {
    std::cout<<"Error reading FEM problem: "<< av[1] <<"!\n";
    e.Print(std::cout);
    return EXIT_FAILURE;
    }

  typedef itk::FEMObjectSpatialObject<2>      FEMObjectSpatialObjectType;
  typedef FEMObjectSpatialObjectType::Pointer FEMObjectSpatialObjectPointer;
  FEMObjectSpatialObjectType::ChildrenListType* children = SpatialReader->GetGroup()->GetChildren();
  FEMObjectSpatialObjectType::Pointer femSO =
    dynamic_cast<FEMObjectSpatialObjectType *>( (*(children->begin() ) ).GetPointer() );
  delete children;


  typedef itk::Image<unsigned char,2> ByteImageType;
  typedef itk::ImageFileReader<ByteImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( av[2] );
  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject &e)
    {
    std::cout << "Error: Failed to read image " << av[2] << std::endl;
    e.Print(std::cout);
    return EXIT_FAILURE;
    }

  /**
   * Third, create the FEM solver object and generate the solution
   */
  femSO->GetFEMObject()->FinalizeMesh();

  FEMSolverType::Pointer SH = FEMSolverType::New();
  SH->SetInput( femSO->GetFEMObject() );
  SH->SetTimeStep( .5 );
  SH->SetNumberOfIterations( niter );

  itk::fem::LinearSystemWrapperDenseVNL lsw_dvnl;
  itk::fem::LinearSystemWrapperItpack   lsw_itpack;
  itk::fem::LinearSystemWrapperVNL      lsw_vnl;

  SH->SetOrigin( reader->GetOutput()->GetOrigin() );
  SH->SetSpacing( reader->GetOutput()->GetSpacing() );
  SH->SetRegion( reader->GetOutput()->GetLargestPossibleRegion() );
  SH->SetDirection( reader->GetOutput()->GetDirection() );
  SH->InitializeInterpolationGrid(reader->GetOutput()->GetLargestPossibleRegion(),
                                  reader->GetOutput()->GetOrigin(),
                                  reader->GetOutput()->GetSpacing(),
                                  reader->GetOutput()->GetDirection() );
  switch (w)
    {
    case 0:
      // VNL
      std::cout << std::endl << ">>>>>Using LinearSystemWrapperVNL" << std::endl;
      SH->SetLinearSystemWrapper(&lsw_vnl);
      break;
    case 1:
      // Dense VNL
      std::cout << std::endl << ">>>>>Using LinearSystemWrapperDenseVNL" << std::endl;
      SH->SetLinearSystemWrapper(&lsw_dvnl);
      break;
    case 2:
      // IT Pack
      std::cout << std::endl << ">>>>>Using LinearSystemWrapperItpack" << std::endl;
      SH->SetLinearSystemWrapper(&lsw_itpack);
      break;
    default:
      // Sparse VNL - default
      std::cout << std::endl << ">>>>>Using LinearSystemWrapperVNL" << std::endl;
      SH->SetLinearSystemWrapper(&lsw_vnl);
      break;
    }

  try
    {
    SH->Update();
    }
  catch (itk::ExceptionObject &err)
    {
    std::cerr << "ITK exception detected: "  << err;
    return EXIT_FAILURE;
    }


  PrintK( SH );
  PrintF( SH );
  PrintNodalCoordinates( SH );

  return EXIT_SUCCESS;
}





