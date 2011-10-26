/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    FEMImageRegLMEx.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for detail.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


/**
 \brief FEM Image registration example class.

 The image registration problem is modeled here with the finite element method.
 Image registration is, in general, an ill-posed problem.  Thus, we use an optimization
 scheme where the optimization criterion is given by a regularized variational energy.
 The variational energy arises from modeling the image as a physical body on which
 external forces act.  The body is allowed to deform so as to minimize the
 applied force.  The resistance of the physical body to deformation, determined by
 the physics associated with the body, serves to regularize the solution.
 The forces applied to the body are, generally, highly non-linear and so the
 body is allowed to deform slowly and incrementally.  The direction it deforms
 follows the gradient of the potential energy (the force) we define.  The potential
 energies we may choose from are given by the itk image-to-image metrics.
 The choices and the associated direction of descent are :
 Mean Squares (minimize),
 Normalized Cross-Correlation (maximize)
 Pattern Intensity  (maximize)
 Mutual Information (maximize).
 Note that we have to set the direction (SetDescentDirection) when we choose a metric.
 The forces driving the problem may also be given by user-supplied landmarks.
 The corners of the image, in this example, are always pinned.  This example is
 designed for 2D or 3D images.  A rectilinear mesh is generated automatically
 given the correct element type (Quadrilateral or Hexahedral).

 Our specific Solver for this example uses trapezoidal time stepping.  This is
 a method for solving a second-order PDE in time.  The solution is penalized
 by the zeroth (mass matrix) and first derivatives (stiffness matrix) of the
 shape functions.  There is an option to perform a line search on the energy
 after each iteration.  Optimal parameter settings require experimentation.
 The following approach tends to work well :
 Choose the relative size of density  to elasticity (e.g. Rho / E ~= 1.)
 such that the image deforms locally and slowly.
 This also affects the stability of the solution.
 Choose the time step to control the size of the deformation at each step.
 Choose enough iterations to allow the solution to converge (this may be automated).

 Reading images is up to the user.  Either set the images using
 SetReference/TargetImage or see the ReadImages function.  Outputs are raw images
 of the same type as the reference image.

 \note This code works for only 2 or 3 dimensions.
*/

// Below, we have typedefs that instantiate all necessary classes.
// Here, we instantiate the image type, load type and
// explicitly template the load implementation type.

#include "itkMacro.h"
#include "itkImage.h"
#include "itkVector.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkFEMObject.h"
#include "itkFEMObjectSpatialObject.h"
#include "itkGroupSpatialObject.h"
#include "itkSpatialObject.h"
#include "itkFEMSpatialObjectReader.h"
#include "itkFEMSpatialObjectWriter.h"
#include "itkFEMFactory.h"
#include "itkFEMRegistrationFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkHistogramMatchingImageFilter.h"
#include "metaCommand.h"

#define TWOD
//#define THREED
#ifdef TWOD
typedef itk::Image< unsigned char, 2 >                     fileImageType;
typedef itk::Image< float, 2 >                             ImageType;
typedef itk::fem::FEMObject<2>                             FEMObjectType;

// We now declare an element type and load implementation pointer for the visitor class.
typedef itk::fem::Element2DC0LinearTriangularMembrane      ElementType2;
typedef itk::fem::Element2DC0LinearQuadrilateralMembrane   ElementType;
#endif 
#ifdef THREED
typedef itk::Image< unsigned char, 3 >                     fileImageType;
typedef itk::Image< float, 3 >                             ImageType;
typedef itk::fem::FEMObject<2>                             FEMObjectType;
typedef itk::fem::Element3DC0LinearHexahedronMembrane      ElementType;
typedef itk::fem::Element3DC0LinearTetrahedronMembrane     ElementType2;
#endif
typedef itk::fem::FEMRegistrationFilter<ImageType, ImageType, FEMObjectType> RegistrationType;

void ReadRawImageFiles( RegistrationType* X)
{

  typedef  itk::ImageFileReader< fileImageType >      FileSourceType;
  typedef  fileImageType::PixelType PixType;

  FileSourceType::Pointer reffilter = FileSourceType::New();
  reffilter->SetFileName( /*(X->GetMovingFile()).c_str()*/NULL );
  FileSourceType::Pointer tarfilter = FileSourceType::New();
  tarfilter->SetFileName( /*(X->GetFixedFile()).c_str()*/NULL );


  try
    {
    reffilter->Update();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << "Exception caught during reference file reading " << std::endl;
    std::cerr << e << std::endl;
    return ;
    }
  try
    {
    tarfilter->Update();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cerr << "Exception caught during target file reading " << std::endl;
    std::cerr << e << std::endl;
    return ;
    }

  typedef itk::RescaleIntensityImageFilter<fileImageType,ImageType> FilterType;
  FilterType::Pointer refrescalefilter = FilterType::New();
  FilterType::Pointer tarrescalefilter = FilterType::New();

  refrescalefilter->SetInput(reffilter->GetOutput());
  tarrescalefilter->SetInput(tarfilter->GetOutput());

  const double desiredMinimum =  0.0;
  const double desiredMaximum =  255.0;

  refrescalefilter->SetOutputMinimum( desiredMinimum );
  refrescalefilter->SetOutputMaximum( desiredMaximum );
  refrescalefilter->UpdateLargestPossibleRegion();
  tarrescalefilter->SetOutputMinimum( desiredMinimum );
  tarrescalefilter->SetOutputMaximum( desiredMaximum );
  tarrescalefilter->UpdateLargestPossibleRegion();


  typedef itk::HistogramMatchingImageFilter<ImageType,ImageType> HEFilterType;
  HEFilterType::Pointer IntensityEqualizeFilter = HEFilterType::New();

  IntensityEqualizeFilter->SetReferenceImage( refrescalefilter->GetOutput() );
  IntensityEqualizeFilter->SetInput( tarrescalefilter->GetOutput() );
  IntensityEqualizeFilter->SetNumberOfHistogramLevels( 100);
  IntensityEqualizeFilter->SetNumberOfMatchPoints( 15);
  IntensityEqualizeFilter->ThresholdAtMeanIntensityOn();
  IntensityEqualizeFilter->Update();

  X->SetMovingImage(refrescalefilter->GetOutput());
  X->SetFixedImage(IntensityEqualizeFilter->GetOutput()
/*tarrescalefilter->GetOutput()*/);
//  X->SetReferenceImage(reffilter->GetOutput());
//  X->SetTargetImage(tarfilter->GetOutput());
}

int main(int argc, char *argv[])
{

  MetaCommand command;
  command.SetOption("MovingImage", "m", true, "Moving image <Filename>");
  command.AddOptionField("MovingImage", "Filename", MetaCommand::STRING, true);

  command.SetOption("FixedImage", "f", true, "Fixed image <Filename>");
  command.AddOptionField("FixedImage", "Filename", MetaCommand::STRING, true);

  command.SetOption("Landmarks", "l", false, "Ladmarks <Filename>");
  command.AddOptionField("Landmarks", "Filename", MetaCommand::STRING, true);

  command.SetOption("Levels", "n", false, "# of levels for registration");
  command.AddOptionField("Levels", "Number", MetaCommand::INT, true, "4");

  command.SetOption("PixelsPerElement","p",false,
      "List of # pixels in each dimension for each level <# levels> <N1> <N2>...");
  command.AddOptionField("PixelsPerElement","values",MetaCommand::LIST,true);

  command.SetOption("Elasticity","e",false,
      "List of material elasticity for each level <# levels> <E1> <E2>...");
  command.AddOptionField("Elasticity","values",MetaCommand::LIST,true);

  command.SetOption("Rho","r",false,
      "List of material rho for each level <# levels> <R1> <R2>...");
  command.AddOptionField("Rho","values",MetaCommand::LIST,true);

  command.SetOption("Points","o",false,
      "List of integration points for each level <# levels> <NP1> <NP2>...");
  command.AddOptionField("Points","values",MetaCommand::LIST,true);

  command.SetOption("Iterations","i",false,
      "List registration iterations for each level <# levels> <I1> <I2>...");
  command.AddOptionField("Iterations","values",MetaCommand::LIST,true);

  command.SetOption("Metric", "t", false,
      "Intensity similarity metric (0 = mean square, 1 = ncc, 2= pattern intensity, 3=MI)");
  command.AddOptionField("Metric", "Number", MetaCommand::INT, true, "0");

  command.SetOption("Direction", "d", false,
      "Descent Direction (1 = maximize, -1 = minimize)");
  command.AddOptionField("Direction", "Number", MetaCommand::INT, true, "-1");

  command.SetOption("LineSearch", "s", false, "Perform Line search (0=off, 1=on)");
  command.AddOptionField("LineSearch", "Number", MetaCommand::INT, true, "1");

  command.SetOption("TimeStep", "h", false, "Time Step Size");
  command.AddOptionField("TimeStep", "Number", MetaCommand::FLOAT, true, "1.0");

  command.SetOption("ReductionFactor", "c", false, "Energy Reduction Factor");
  command.AddOptionField("ReductionFactor", "Number", MetaCommand::FLOAT, true, "1.0e-15");

  command.SetOption("WarpedImage", "w", false, "WarpedImage <Filename>");
  command.AddOptionField("WarpedImage", "Filename", MetaCommand::STRING, true);

  command.SetOption("DisplacementField", "z", false, "DisplacementField <Filename>");
  command.AddOptionField("DisplacementField", "Filename", MetaCommand::STRING, true);

  command.SetOption("Mesh", "x", false, "Mesh <Filename>");
  command.AddOptionField("Mesh", "Filename", MetaCommand::STRING, true);

  if (!command.Parse(argc,argv))
    {
    return 1;
    }

  typedef itk::ImageFileReader<ImageType> ImageReaderType;

  // Read the Fixed Image
  ImageReaderType::Pointer fixedReader = ImageReaderType::New();
  fixedReader->SetFileName( command.GetValueAsString("FixedImage","Filename") );
  try
    {
    fixedReader->Update();
    }
  catch(itk::ExceptionObject& e)
    {
    std::cerr << "FEMImageRegLMEx caught an ITK exception reading fixed image:\n";
    std::cerr << e << "\n";
    return 1;
    }

  // Read the Moving Image
  ImageReaderType::Pointer movingReader = ImageReaderType::New();
  movingReader->SetFileName( command.GetValueAsString("MovingImage","Filename") );
  try
    {
    movingReader->Update();
    }
  catch(itk::ExceptionObject& e)
    {
    std::cerr << "FEMImageRegLMEx caught an ITK exception reading moving image:\n";
    std::cerr << e << "\n";
    return 1;
    }


  // Declare the registration class
  int levels = 4;
  bool useNormalizedGradient = true;
  bool employRegridding = false;
  float alpha = 1.0;
  float timeStep = 1.0;
  int metric = 0;
  int lineSearch = 0;
  std::vector<float> elasticity;
  std::vector<float> rho;
  std::vector<float> gamma;
  std::vector<int> iterations;
  std::vector<int> pixelsPerElement;
  std::vector<float> metricRegion;
  std::vector<int> integrationPoints;

  RegistrationType::Pointer registrator = RegistrationType::New();
  registrator->SetFixedImage( fixedReader->GetOutput() );
  registrator->SetMovingImage( movingReader->GetOutput() );

  if ( command.GetOptionWasSet("Levels") )
    {
    levels = command.GetValueAsInt("Levels","Number");
    }

  if ( command.GetOptionWasSet("Alpha") )
    {
    alpha = command.GetValueAsInt("Alpha","Value");
    }

  if ( command.GetOptionWasSet("TimeStep") )
    {
    timeStep = command.GetValueAsInt("TimeStep","Value");
    }

  if ( command.GetOptionWasSet("Metric") )
    {
    metric = command.GetValueAsInt("Metric","Value");
    if ((metric < 0) || (metric > 3))
      {
      std::cout << "Error: Invalid metric: " << metric << std::endl;
      std::cout << "Valid values: 0 = mean square, 1 = ncc, 2= pattern intensity, 3=MI" << std::endl;
      return 1;
      }
    }

  if ( command.GetOptionWasSet("EmployRegridding") )
    {
    int value = command.GetValueAsInt("EmployRegridding","Value");
    if (value == 1)
      {
      employRegridding = true;
      }
    }

  if ( command.GetOptionWasSet("NormalizedGradient") )
    {
    int value = command.GetValueAsInt("NormalizedGradient","Value");
    if (value == 0)
      {
      useNormalizedGradient = false;
      }
    }

  std::list< std::string > listOptions;
  std::list<std::string>::iterator it;

  listOptions.clear();
  if ( command.GetOptionWasSet("Iterations") )
    {
    listOptions = command.GetValueAsList("Iterations");
    if (listOptions.size() != levels)
      {
      std::cout << "Error: # iteration values must equal # levels" << std::endl;
      return 1;
      }
      for (it=listOptions.begin();it!=listOptions.end();it++)
        {
        iterations.push_back( atoi( (*it).c_str() ) );
        }
      listOptions.clear();
    }

  if ( command.GetOptionWasSet("PixelsPerElement") )
    {
    listOptions = command.GetValueAsList("PixelsPerElement");
    if (listOptions.size() != levels)
      {
      std::cout << "Error: # PixelsPerElement values must equal # levels" << std::endl;
      return 1;
      }
      for (it=listOptions.begin();it!=listOptions.end();it++)
        {
        pixelsPerElement.push_back( atoi( (*it).c_str() ) );
        }
      listOptions.clear();
    }

  if ( command.GetOptionWasSet("Rho") )
    {
    listOptions = command.GetValueAsList("Rho");
    if (listOptions.size() != levels)
      {
      std::cout << "Error: # Rho values must equal # levels" << std::endl;
      return 1;
      }
      for (it=listOptions.begin();it!=listOptions.end();it++)
        {
        rho.push_back( atoi( (*it).c_str() ) );
        }
      listOptions.clear();
    }

  if ( command.GetOptionWasSet("Elasticity") )
    {
    listOptions = command.GetValueAsList("Elasticity");
    if (listOptions.size() != levels)
      {
      std::cout << "Error: # Elasticity values must equal # levels" << std::endl;
      return 1;
      }
      for (it=listOptions.begin();it!=listOptions.end();it++)
        {
        elasticity.push_back( atoi( (*it).c_str() ) );
        }
      listOptions.clear();
    }

  if ( command.GetOptionWasSet("IntegrationPoints") )
    {
    listOptions = command.GetValueAsList("IntegrationPoints");
    if (listOptions.size() != levels)
      {
      std::cout << "Error: # Integration Point values must equal # levels" << std::endl;
      return 1;
      }
      for (it=listOptions.begin();it!=listOptions.end();it++)
        {
        integrationPoints.push_back( atoi( (*it).c_str() ) );
        }
      listOptions.clear();
    }

  // Set the Registration Parameters
  registrator->SetMaxLevel( levels );
  registrator->SetUseNormalizedGradient( useNormalizedGradient );
  registrator->SetEmployRegridding( employRegridding );
  registrator->ChooseMetric( metric );
  registrator->SetAlpha( alpha );
  registrator->SetDoLineSearchOnImageEnergy( lineSearch );
  registrator->SetTimeStep( timeStep );

  for (int i=0;i<levels;i++)
    {
    registrator->SetElasticity(elasticity[i], i);
    registrator->SetRho(rho[i], i);
    registrator->SetGamma(1., i);
    }

  //Need to update this - Add Support for Landmarks
  registrator->SetUseLandmarks(false);

  // Choose the material properties
  itk::fem::MaterialLinearElasticity::Pointer m;
  m=itk::fem::MaterialLinearElasticity::New();
  m->SetGlobalNumber(0);
  m->SetYoungsModulus(registrator->GetElasticity() );
  m->SetCrossSectionalArea(1.0);
  m->SetThickness(1.0);
  m->SetMomentOfInertia(1.0);
  m->SetPoissonsRatio(0.);
  m->SetDensityHeatProduct(1.0);

  // Create the element type 
  ElementType::Pointer e1 = ElementType::New();
  e1->SetMaterial(dynamic_cast<itk::fem::MaterialLinearElasticity *>( &*m ) );
  registrator->SetElement(&*e1);
  registrator->SetMaterial(m);

  // Register the images
  try
    {
    registrator->RunRegistration();
    }
  catch( ::itk::ExceptionObject & err )
    {
    std::cerr << "ITK exception detected: "  << err;
    std::cout << err << std::endl;
    return 1;
    }
  catch( ... )
    {
    std::cout << "Caught an exception: " << std::endl;
    return 1;
    }


  // Write the results
  if ( command.GetOptionWasSet("DisplacementField") )
    {
    typedef itk::ImageFileWriter<RegistrationType::FieldType>  ImageWriterType;
    ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( command.GetValueAsString("WarpedImage","Filename") );
    writer->SetInput( registrator->GetDisplacementField() );
    writer->Update();
    }

  if ( command.GetOptionWasSet("WarpedImage") )
    {
    typedef itk::ImageFileWriter<ImageType>  ImageWriterType;
    ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( command.GetValueAsString("WarpedImage","Filename") );
    writer->SetInput( registrator->GetWarpedImage() );
    writer->Update();
    }

  return 0;
}


