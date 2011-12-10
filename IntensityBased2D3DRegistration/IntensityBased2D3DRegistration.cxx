
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IntensityBased2D3DRegistration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


// Software Guide : BeginLatex
//
// This program implements an intensity based 2D-3D registration
// algorithm using the \code{RayCastInterpolateImageFunction} class
// and \code{GradientDifferenceImageToImageMetric} similarity measure.
//
// Software Guide : EndLatex 


#include <iostream>

// Software Guide : BeginLatex
//
// The algorithm performs a multi-resolution coarse to fine
// registration using the
// \code{MultiResolutionImageRegistrationMethod} class.
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
#include "itkMultiResolutionImageRegistrationMethod.h"
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// The transformation used is a rigid 3D Euler transform with the
// provision of a center of rotation which defaults to the center of
// the 3D volume. In practice the center of the particular feature of
// interest in the 3D volume should be used.
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
#include "itkEuler3DTransform.h"
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// We have chosen to implement the registration using the gradient
// difference similarity measure.
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
#include "itkGradientDifferenceImageToImageMetric.h"
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// This is an intensity based registration algorith so ray casting is
// used to project the 3D volume onto pixels in the target 2D image.
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
#include "itkRayCastInterpolateImageFunction.h"
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Finally a gradient descent optimizer is used to search for the
// registration position.
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
#include "itkRegularStepGradientDescentOptimizer.h"
// Software Guide : EndCodeSnippet

#include "itkImage.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkSquaredDifferenceImageFilter.h"

#include "itkCommand.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


// Software Guide : BeginLatex
//
// First we define the command class to allow us to monitor the registration.
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );

protected:
  CommandIterationUpdate() {};

public:
  typedef itk::RegularStepGradientDescentOptimizer     OptimizerType;
  typedef const OptimizerType                         *OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
  {
    Execute( (const itk::Object *)caller, event);
  }

  void Execute(const itk::Object *object, const itk::EventObject & event)
  {
    OptimizerPointer optimizer = 
                      dynamic_cast< OptimizerPointer >( object );
    if( typeid( event ) != typeid( itk::IterationEvent ) )
      {
        return;
      }
    std::cout << "Iteration: " << optimizer->GetCurrentIteration() << std::endl;
    std::cout << "Similarity: " << optimizer->GetValue() << std::endl;
    std::cout << "Position: " << optimizer->GetCurrentPosition() << std::endl;
  }
};
// Software Guide : EndCodeSnippet


// Software Guide : BeginLatex
//
// A second \emph{Command/Observer} is used to reduce the minimum
// registration step length on each occasion that the resolution of
// the multi-scale registration is increased (see
// \doxygen{MultiResImageRegistration1} for more info).
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
template <typename TRegistration>
class RegistrationInterfaceCommand : public itk::Command 
{
public:
  typedef  RegistrationInterfaceCommand   Self;
  typedef  itk::Command                   Superclass;
  typedef  itk::SmartPointer<Self>        Pointer;
  itkNewMacro( Self );

protected:
  RegistrationInterfaceCommand() {};

public:
  typedef   TRegistration                              RegistrationType;
  typedef   RegistrationType *                         RegistrationPointer;
  typedef   itk::RegularStepGradientDescentOptimizer   OptimizerType;
  typedef   OptimizerType *                            OptimizerPointer;

  void Execute(itk::Object * object, const itk::EventObject & event)
  {
    if( typeid( event ) != typeid( itk::IterationEvent ) )
      {
      return;
      }

    RegistrationPointer registration =
                            dynamic_cast<RegistrationPointer>( object );

    // If this is the first resolution level we assume that the
    // maximum step length (representing the first step size) and the
    // minimum step length (representing the convergence criterion)
    // have already been set.  At each subsequent resolution level, we
    // will reduce the minimum step length by a factor of four in order
    // to allow the optimizer to focus on progressively smaller
    // regions. The maximum step length is set to the current step
    // length. In this way, when the optimizer is reinitialized at the
    // beginning of the registration process for the next level, the
    // step length will simply start with the last value used for the
    // previous level. This will guarantee the continuity of the path
    // taken by the optimizer through the parameter space.

    OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( 
                       registration->GetOptimizer() );

    if ( registration->GetCurrentLevel() != 0 )
      {
      optimizer->SetMaximumStepLength( optimizer->GetCurrentStepLength() );
      optimizer->SetMinimumStepLength( optimizer->GetMinimumStepLength() / 4.0 );
      }

    optimizer->Print( std::cout );
  }

  void Execute(const itk::Object * , const itk::EventObject & )
    { return; }
};
// Software Guide : EndCodeSnippet



void usage()
{
  std::cerr << "\n";
  std::cerr << "Usage: itkMultiResIntensityBasedRegn2D3D <options> Image2D Volume3D\n";
  std::cerr << "       Registers a 3D volume to a 2D image. \n\n";
  std::cerr << "   where <options> is one or more of the following:\n\n";
  std::cerr << "       <-h>                     Display (this) usage information\n";
  std::cerr << "       <-v>                     Verbose output [default: no]\n";
  std::cerr << "       <-dbg>                   Debugging output [default: no]\n";
  std::cerr << "       <-n int>                 The number of scales to apply [default: 2]\n";
  std::cerr << "       <-maxScale int>          The scale factor corresponding to max resolution [default: 1]\n";
  std::cerr << "       <-step float float>      Maximum and minimum step sizes [default: 4 and 0.01]\n";
  std::cerr << "       <-fl float>              Focal length or source to image distance [default: 400mm]\n";
  std::cerr << "       <-t float float float>   Translation parameter of the camera \n";
  std::cerr << "       <-rx float>              Rotation around x,y,z axis in degrees \n";
  std::cerr << "       <-ry float>\n";
  std::cerr << "       <-rz float>\n";
  std::cerr << "       <-normal float float>    The 2D projection normal position [default: 0x0mm]\n";
  std::cerr << "       <-cor float float float> The centre of rotation relative to centre of volume\n";
  std::cerr << "       <-threshold float>       Threshold [default: 0]\n";
  std::cerr << "       <-diff file>             Difference image filename\n";
  std::cerr << "       <-o file>                Output image filename\n\n";
  std::cerr << "                                by  john.hipwell@kcl.ac.uk\n";
  std::cerr << "                                and thomas@hartkens.de\n";
  std::cerr << "                                (Imaging Sciences KCL London)\n\n";
  exit(1);
}


int main( int argc, char *argv[] )
{
  char *fileImage2D = NULL;
  char *fileVolume3D = NULL;
  char *fileOutput = NULL;
//  char *fileDifference;

  bool ok;
  bool verbose = false;
  bool debug = false;

  unsigned int nScales = 2;
  int maxScale = 1;

  double rx = 0.;
  double ry = 0.;
  double rz = 0.;

  double tx = 0.;
  double ty = 0.;
  double tz = 0.;

  double cx = 0.;
  double cy = 0.;
  double cz = 0.;

  double focalLength = 400.;

  double maxStepSize = 4.;
  double minStepSize = 1.;

  double o2Dx = 0;
  double o2Dy = 0;

  double threshold=0;


  // Parse command line parameters

  if (argc <= 1)
    usage();

  while (argc > 1) 
    {
    ok = false;

    if ((ok == false) && (strcmp(argv[1], "-h") == 0))
      {
      argc--; argv++;
      ok = true;
      usage();      
      }

    if ((ok == false) && (strcmp(argv[1], "-v") == 0))
      {
      argc--; argv++;
      ok = true;
      verbose = true;
      }

    if ((ok == false) && (strcmp(argv[1], "-dbg") == 0))
      {
      argc--; argv++;
      ok = true;
      debug = true;
      }

    if ((ok == false) && (strcmp(argv[1], "-n") == 0))
      {
      argc--; argv++;
      ok = true;
      nScales=atoi(argv[1]);
      argc--; argv++;
      }

    if ((ok == false) && (strcmp(argv[1], "-maxScale") == 0))
      {
      argc--; argv++;
      ok = true;
      maxScale=atoi(argv[1]);
      argc--; argv++;
      }

    if ((ok == false) && (strcmp(argv[1], "-fl") == 0))
      {
      argc--; argv++;
      ok = true;
      focalLength=atof(argv[1]);
      argc--; argv++;
      }

    if ((ok == false) && (strcmp(argv[1], "-step") == 0))
      {
      argc--; argv++;
      ok = true;
      maxStepSize=atof(argv[1]);
      argc--; argv++;
      minStepSize=atof(argv[1]);
      argc--; argv++;
      }

    if ((ok == false) && (strcmp(argv[1], "-t") == 0))
      {
      argc--; argv++;
      ok = true;
      tx=atof(argv[1]);
      argc--; argv++;
      ty=atof(argv[1]);
      argc--; argv++;
      tz=atof(argv[1]);
      argc--; argv++;
      }

    if ((ok == false) && (strcmp(argv[1], "-rx") == 0))
      {
      argc--; argv++;
      ok = true;
      rx=atof(argv[1]);
      argc--; argv++;
      }

    if ((ok == false) && (strcmp(argv[1], "-ry") == 0))
      {
      argc--; argv++;
      ok = true;
      ry=atof(argv[1]);
      argc--; argv++;
      }

    if ((ok == false) && (strcmp(argv[1], "-rz") == 0))
      {
      argc--; argv++;
      ok = true;
      rz=atof(argv[1]);
      argc--; argv++;
      }

    if ((ok == false) && (strcmp(argv[1], "-normal") == 0))
      {
      argc--; argv++;
      ok = true;
      o2Dx=atof(argv[1]);
      argc--; argv++;
      o2Dy=atof(argv[1]);
      argc--; argv++;
      }

    if ((ok == false) && (strcmp(argv[1], "-cor") == 0))
      {
      argc--; argv++;
      ok = true;
      cx=atof(argv[1]);
      argc--; argv++;
      cy=atof(argv[1]);
      argc--; argv++;
      cz=atof(argv[1]);
      argc--; argv++;
      }

    if ((ok == false) && (strcmp(argv[1], "-threshold") == 0))
      {
      argc--; argv++;
      ok = true;
      threshold=atof(argv[1]);
      argc--; argv++;
      }

    if ((ok == false) && (strcmp(argv[1], "-diff") == 0))
      {
      argc--; argv++;
      ok = true;
//      fileDifference = argv[1];
      argc--; argv++;
      }

    if ((ok == false) && (strcmp(argv[1], "-o") == 0))
      {
      argc--; argv++;
      ok = true;
      fileOutput = argv[1];
      argc--; argv++;
      }

    if (ok == false) 
      {

      if (fileImage2D == NULL) 
        {
        fileImage2D = argv[1];
        argc--;
        argv++;
        }
      
      else if (fileVolume3D == NULL) 
        {
        fileVolume3D = argv[1];
        argc--;
        argv++;
        }
      
      else 
        {
        std::cerr << "ERROR: Can not parse argument " << argv[1] << std::endl;
        usage();
        }
      }
    } 

  if (verbose) 
    {
    if (fileImage2D)  std::cout << "Input 2D image: " << fileImage2D  << std::endl;
    if (fileVolume3D) std::cout << "Input 3D image: " << fileVolume3D << std::endl;
    if (fileOutput)   std::cout << "Output image: "   << fileOutput   << std::endl;
    }


  
  // Software Guide : BeginLatex
  //
  // We begin the program proper by defining the 2D and 3D images. The
  // \code{MultiResolutionImageRegistrationMethod} requires that both
  // images have the same dimension so the 2D image is given
  // dimension 3 and the size of the \emph{z} dimension is set to unity.
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  const    unsigned int    Dimension = 3;
  typedef  short           PixelType;

  typedef itk::Image< PixelType, Dimension > ImageType2D;
  typedef itk::Image< PixelType, Dimension > ImageType3D;
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // The following lines define each of the components used in the
  // registration: The transform, optimizer, metric, interpolator and
  // the multi-resolution registration method itself.
  //
  // Due to the recursive nature of the process by which the
  // downsampled images are computed by the image pyramids of the
  // multi-resolution registration, the output images are required to
  // have real pixel types. We declare this internal image type to be
  // \code{InternalPixelType}:
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  typedef   float     InternalPixelType;
  typedef itk::Image< InternalPixelType, Dimension > InternalImageType;

  typedef itk::Euler3DTransform< double >  TransformType;

  typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;

  typedef itk::GradientDifferenceImageToImageMetric< 
                                    InternalImageType, 
                                    InternalImageType >    MetricType;

  typedef itk::RayCastInterpolateImageFunction< 
                                    InternalImageType,
                                    double          >    InterpolatorType;

  
  typedef itk::MultiResolutionImageRegistrationMethod< 
                                    InternalImageType, 
                                    InternalImageType >   RegistrationType;
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // In the multi-resolution framework, a
  // \doxygen{MultiResolutionPyramidImageFilter} is used to create a
  // pyramid of downsampled images. The size of each downsampled image
  // is specified by the user in the form of a schedule of shrink
  // factors. A description of the filter and the format of the
  // schedules are found in Section \ref{sec:ImagePyramids}.
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  typedef itk::MultiResolutionPyramidImageFilter<
                                    InternalImageType,
                                    InternalImageType > ImagePyramidType2D;
  typedef itk::MultiResolutionPyramidImageFilter<
                                    InternalImageType,
                                    InternalImageType > ImagePyramidType3D;
  // Software Guide : EndCodeSnippet


  // Software Guide : BeginLatex
  //
  // Each of the registration components are instantiated in the
  // usual way...
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  ImagePyramidType2D::Pointer imagePyramid2D = 
      ImagePyramidType2D::New();
  ImagePyramidType3D::Pointer imagePyramid3D =
      ImagePyramidType3D::New();

  MetricType::Pointer         metric        = MetricType::New();
  TransformType::Pointer      transform     = TransformType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // and passed to the registration method:
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetInterpolator(  interpolator  );

  registration->SetFixedImagePyramid( imagePyramid2D );
  registration->SetMovingImagePyramid( imagePyramid3D );
  // Software Guide : EndCodeSnippet
  
  if (debug) 
    {
    //metric->DebugOn();
    //transform->DebugOn();   
    optimizer->DebugOn();   
    //interpolator->DebugOn();
    //registration->DebugOn();
    }



  // Software Guide : BeginLatex
  //
  //  The 2- and 3-D images are read from files, 
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  typedef itk::ImageFileReader< ImageType2D > ImageReaderType2D;
  typedef itk::ImageFileReader< ImageType3D > ImageReaderType3D;

  ImageReaderType2D::Pointer imageReader2D = ImageReaderType2D::New();
  ImageReaderType3D::Pointer imageReader3D = ImageReaderType3D::New();

  imageReader2D->SetFileName( fileImage2D );
  imageReader3D->SetFileName( fileVolume3D );
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  //  but before connecting these images to the registration we need
  //  to cast them to the internal image type using
  //  \doxygen{CastImageFilters}.
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  typedef itk::CastImageFilter< 
                        ImageType2D, InternalImageType > CastFilterType2D;
  typedef itk::CastImageFilter< 
                        ImageType3D, InternalImageType > CastFilterType3D;

  CastFilterType2D::Pointer caster2D = CastFilterType2D::New();
  CastFilterType3D::Pointer caster3D = CastFilterType3D::New();
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  //  The output of the readers is connected as input to the cast
  //  filters. The inputs to the registration method are taken from the
  //  cast filters. 
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  caster2D->SetInput( imageReader2D->GetOutput() );
  caster3D->SetInput( imageReader3D->GetOutput() );

  caster2D->Update();
  caster3D->Update();

  registration->SetFixedImage(  caster2D->GetOutput() );
  registration->SetMovingImage( caster3D->GetOutput() );

  registration->SetFixedImageRegion( 
       caster2D->GetOutput()->GetBufferedRegion() );
  // Software Guide : EndCodeSnippet
   

  // Initialise the transform
  // ~~~~~~~~~~~~~~~~~~~~~~~~

  transform->SetComputeZYX(true);

  // Software Guide : BeginLatex
  //
  // The transform is initialised with the translation [tx,ty,tz] and
  // rotation [rx,ry,rz] specified on the command line
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  TransformType::OutputVectorType translation;

  translation[0] = tx;
  translation[1] = ty;
  translation[2] = tz;

  transform->SetTranslation(translation);
  transform->SetRotation(M_PI/180.0*rx, M_PI/180.0*ry, M_PI/180.0*rz);
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // The centre of rotation is set by default to the centre of the 3D
  // volume but can be offset from this position using a command
  // line specified translation [cx,cy,cz]
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  double origin3D[ Dimension ];

  const itk::Vector<double, 3> resolution3D = imageReader3D->GetOutput()->GetSpacing();

  typedef ImageType3D::RegionType      ImageRegionType3D;
  typedef ImageRegionType3D::SizeType  SizeType3D;

  ImageRegionType3D region3D = caster3D->GetOutput()->GetBufferedRegion();
  SizeType3D        size3D   = region3D.GetSize();

  origin3D[0] = resolution3D[0]*((double) size3D[0])/2.; 
  origin3D[1] = resolution3D[1]*((double) size3D[1])/2.; 
  origin3D[2] = resolution3D[2]*((double) size3D[2])/2.; 

  TransformType::InputPointType center;
  center[0] = cx + origin3D[0];
  center[1] = cy + origin3D[1];
  center[2] = cz + origin3D[2];

  transform->SetCenter(center);
  // Software Guide : EndCodeSnippet

  if (verbose) 
    {
    std::cout << "3D image size: "
              << size3D[0] << ", " << size3D[1] << ", " << size3D[2] << std::endl
              << "   resolution: "
              << resolution3D[0] << ", " << resolution3D[1] << ", " << resolution3D[2] << std::endl
              << "Transform: " << transform << std::endl;
    }


  // Set the origin of the 2D image
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // Software Guide : BeginLatex
  //
  // For correct (perspective) projection of the 3D volume, the 2D
  // image needs to be placed at a certain distance (the focal length
  // or source-to-image distance \emph{focalLength}) from the focal
  // point, and the normal from the imaging plane to the focal point
  // needs to be specified.
  //
  // By default, ie. if [tx,ty,tz] is [0,0,0], the 3D volume is placed
  // half-way between the focal point and the imaging plane. Similarly
  // the imaging plane normal is set by default to the center of the
  // 2D image but may be modified from this using the command line
  // parameters [o2Dx,o2Dy].
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  double origin2D[ Dimension ];

  const itk::Vector<double, 3> resolution2D = imageReader2D->GetOutput()->GetSpacing();

  typedef ImageType2D::RegionType      ImageRegionType2D;
  typedef ImageRegionType2D::SizeType  SizeType2D;

  ImageRegionType2D region2D = caster2D->GetOutput()->GetBufferedRegion();
  SizeType2D        size2D   = region2D.GetSize();

  origin2D[0] = origin3D[0] + o2Dx - resolution2D[0]*((double) size2D[0] - 1.)/2.; 
  origin2D[1] = origin3D[1] + o2Dy - resolution2D[1]*((double) size2D[1] - 1.)/2.; 
  origin2D[2] = origin3D[2] + focalLength/2.; 

  imageReader2D->GetOutput()->SetOrigin( origin2D );

  registration->SetFixedImageRegion( caster2D->GetOutput()->GetBufferedRegion() );
  // Software Guide : EndCodeSnippet

  if (verbose)
    {
    std::cout << "2D image size: "
              << size2D[0] << ", " << size2D[1] << ", " << size2D[2] << std::endl
              << "   resolution: "
              << resolution2D[0] << ", " << resolution2D[1] << ", " << resolution2D[2] << std::endl
              << "   and position: " 
              << origin2D[0] << ", " << origin2D[1] << ", " << origin2D[2] << std::endl;
    }


  // Set the pyramid schedule for the 2D image
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // Software Guide : BeginLatex
  //
  // The multi-resolution hierachy is specified with respect to the 2D
  // image. The number of scales or levels defaults to two, with each level
  // differing in resolution by a factor of two. The highest resolution (final)
  // level corresponds to the resolution of the input 2D image but
  // this can be altered by the user via the \emph{maxScale} command
  // line parameter. The number of scales \emph{nScales} can also be
  // specified by the user.
  //
  // Software Guide : EndLatex
  std::vector<double> sampledRes2D;
  std::vector<ImageRegionType2D> imageRegionPyramid2D;

  imagePyramid2D->SetNumberOfLevels( nScales );
  
  sampledRes2D.reserve( nScales );
  imageRegionPyramid2D.reserve( nScales );

  typedef ImagePyramidType2D::ScheduleType   ScheduleType2D;
  ScheduleType2D schedule2D = imagePyramid2D->GetSchedule();

  for ( unsigned int dim = 0; dim < ImageType2D::ImageDimension; dim++ )
    {
    schedule2D[ nScales-1 ][ dim ] = maxScale;
    }

  for ( int level=nScales-2; level >= 0; level-- )
    {
    for ( unsigned int dim = 0; dim < ImageType2D::ImageDimension; dim++ )
      {
      schedule2D[ level ][ dim ] = 2*schedule2D[ level+1 ][ dim ];
      }
    }

  // Compute the 2D ImageRegion corresponding to each level of the 
  // pyramid.

  typedef ImageRegionType2D::IndexType       IndexType2D;
  IndexType2D       inputStart2D = region2D.GetIndex();

  for ( unsigned int level=0; level < nScales; level++ )
    {
    SizeType2D  size;
    IndexType2D start;
    sampledRes2D[ level ] = 0.;
    for ( unsigned int dim = 0; dim < ImageType2D::ImageDimension; dim++ )
      {
      float scaleFactor = static_cast<float>( schedule2D[ level ][ dim ] );

      size[ dim ] = static_cast<SizeType2D::SizeValueType>(
        floor( static_cast<float>( size2D[ dim ] ) / scaleFactor ) );

      if( size[ dim ] < 1 )
        {
        size[ dim ] = 1;
        schedule2D[ level ][ dim ] = 1;
        }

       std::cout << level << " " << dim << " " 
                << size[ dim ] << " " << schedule2D[ level ][ dim ] 
                << std::endl;

       scaleFactor = static_cast<float>( schedule2D[ level ][ dim ] );
       start[ dim ] = static_cast<IndexType2D::IndexValueType>(
         ceil(  static_cast<float>( inputStart2D[ dim ] ) / scaleFactor ) ); 

      if ( dim < 2 ) 
        {
        sampledRes2D[ level ] +=  scaleFactor*resolution2D[ dim ]
                                 *scaleFactor*resolution2D[ dim ];
        }
      }

    sampledRes2D[ level ] = sqrt( sampledRes2D[ level ] );

    imageRegionPyramid2D[ level ].SetSize( size );
    imageRegionPyramid2D[ level ].SetIndex( start );
    }

  imagePyramid2D->SetSchedule( schedule2D );


  // Set the pyramid schedule for the 3D image
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // Software Guide : BeginLatex
  //
  // The 3D image pyramid naturally contains the same number of levels
  // as the multi-resolution schedule for the 2D image. In addition
  // the sub-sampling factors for each level are set such that
  // resolution of the 3D image in each case is reduced to no greater
  // than the corresponding 2D image resolution at that scale. This
  // ensures that the 3D volume is reduced in size as far as possible,
  // minimising ray-casting computation time, whilst retaining
  // sufficient information to ensure accurate production of the DRR
  // at the resolution of the 2D image.
  //
  // Software Guide : EndLatex

  std::vector<ImageRegionType3D> imageRegionPyramid3D;

  imagePyramid3D->SetNumberOfLevels( nScales );
  
  imageRegionPyramid3D.reserve( nScales );

  typedef ImagePyramidType3D::ScheduleType   ScheduleType3D;
  ScheduleType3D schedule3D = imagePyramid3D->GetSchedule();
  
  // Compute the 2D image pyramid schedule such that the 3D volume
  // resolution is no greater than the 2D image resolution.

  for ( unsigned int level=0; level < nScales; level++ )
    {
    for ( unsigned int dim = 0; dim < ImageType3D::ImageDimension; dim++)
      {
      double res = resolution3D[ dim ];
      schedule3D[ level ][ dim ] = 1;
      while ( res*2. < sampledRes2D[ level ] ) 
        {
        schedule3D[ level ][ dim ] *= 2;
        res *= 2.;
        }
      }
    }
   
  // Compute the 3D ImageRegion corresponding to each level of the 
  // pyramid.

  typedef ImageRegionType3D::IndexType       IndexType3D;
  IndexType3D       inputStart3D = region3D.GetIndex();

  for ( unsigned int level=0; level < nScales; level++ )
    {
    SizeType3D  size;
    IndexType3D start;
    for ( unsigned int dim = 0; dim < ImageType3D::ImageDimension; dim++)
      {
      float scaleFactor = static_cast<float>( schedule3D[ level ][ dim ] );

      size[ dim ] = static_cast<SizeType3D::SizeValueType>(
        floor( static_cast<float>( size3D[ dim ] ) / scaleFactor ) );

      if( size[ dim ] < 1 )
        {
        size[ dim ] = 1;
        schedule3D[ level ][ dim ] = 1;
        }

      scaleFactor = static_cast<float>( schedule3D[ level ][ dim ] );
      start[ dim ] = static_cast<IndexType3D::IndexValueType>(
        ceil(  static_cast<float>( inputStart3D[ dim ] ) / scaleFactor ) ); 
      }
    imageRegionPyramid3D[ level ].SetSize( size );
    imageRegionPyramid3D[ level ].SetIndex( start );
    }

  imagePyramid3D->SetSchedule( schedule3D );


  // Initialise the ray cast interpolator
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  // Software Guide : BeginLatex
  //
  // The ray cast interpolator is used to project the 3D volume. It
  // does this by casting rays from the (transformed) focal point to
  // each (transformed) pixel coordinate in the 2D image.
  //
  // In addition a threshold may be specified to ensure that only
  // intensities greater than a given value contribute to the
  // projected volume. This can be used, for instance, to remove soft
  // tissue from projections of CT data and force the registration
  // to find a match which aligns bony structures in the images.
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  InterpolatorType::InputPointType focalPoint;

  focalPoint[0] = origin3D[0];
  focalPoint[1] = origin3D[1];
  focalPoint[2] = origin3D[2] - focalLength/2.;

  interpolator->SetFocalPoint(focalPoint);
  interpolator->SetThreshold(threshold);
  interpolator->SetTransform(transform);
  // Software Guide : EndCodeSnippet

  // NB. Interpolator input image set in itkImageToImageMatric

  if (verbose)
    {
    std::cout << "Focal point: " 
              << focalPoint[0] << ", " << focalPoint[1] << ", " << focalPoint[2] << std::endl
              << "Threshold: " << threshold << std::endl;
    }


  // Set up the transform and start position
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  // Software Guide : BeginLatex
  //
  // The registration start position is intialised using the
  // transformation parameters.
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  registration->SetInitialTransformParameters( transform->GetParameters() );
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // We wish to maximise the gradient difference similarity measure.
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  optimizer->SetMaximize( true );  
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // The maximum and minimum step lengths are specified. The default
  // values for these parameters are 4 and 1 (degrees or mm, depending
  // on the parameter) respectively, for the coarsest resolution (so
  // step sizes of 4, 2 and 1 will be used). For finer scales the
  // minimum step size is reduced by a factor of four, so the second
  // scale will be performed with step sizes of 1, 0.5 and 0.25 mm or
  // degrees.
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  optimizer->SetMaximumStepLength( maxStepSize );  
  optimizer->SetMinimumStepLength( minStepSize );
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // The maximum number of iterations is set to 200.
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  optimizer->SetNumberOfIterations( 200 );
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // The optimizer weightings are set such that one degree equates to
  // one millimeter.
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  itk::Optimizer::ScalesType weightings( 6 );

  weightings[0] = 180./M_PI;
  weightings[1] = 180./M_PI;
  weightings[2] = 180./M_PI;
  weightings[3] = 1.;
  weightings[4] = 1.;
  weightings[5] = 1.;

  optimizer->SetScales( weightings );
  // Software Guide : EndCodeSnippet

  if (verbose)
    {
    optimizer->Print( std::cout );
    }


  // Create the observers
  // ~~~~~~~~~~~~~~~~~~~~

  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();

  optimizer->AddObserver( itk::IterationEvent(), observer );

  // Software Guide : BeginLatex
  //
  //  Once all the registration components are in place we can create
  //  an instance of the interface command and connect it to the
  //  registration object using the \code{AddObserver()} method.
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  typedef RegistrationInterfaceCommand<RegistrationType> CommandType;
  CommandType::Pointer command = CommandType::New();
  registration->AddObserver( itk::IterationEvent(), command );
  // Software Guide : EndCodeSnippet

  // Software Guide : BeginLatex
  //
  // Finally we set the number of multi-resolution levels:
  //
  // Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  registration->SetNumberOfLevels( nScales );
  // Software Guide : EndCodeSnippet

  imagePyramid3D->Print(std::cout);
  imagePyramid2D->Print(std::cout);


  // Start the registration
  // ~~~~~~~~~~~~~~~~~~~~~~

  if (verbose)
    {
    std::cout << "Starting the registration now..." << std::endl;
    }

  try 
    { 
    // Software Guide : BeginLatex
    //
    // and start the registration.
    //
    // Software Guide : EndLatex
    // Software Guide : BeginCodeSnippet
    registration->Update();
    // Software Guide : EndCodeSnippet
    } 
  catch( itk::ExceptionObject & err ) 
    { 
      std::cout << "ExceptionObject caught !" << std::endl; 
      std::cout << err << std::endl; 
      return -1;
    } 

  typedef RegistrationType::ParametersType ParametersType;
  ParametersType finalParameters = registration->GetLastTransformParameters();

  const double TranslationAlongX = finalParameters[0];
  const double TranslationAlongY = finalParameters[1];
  const double TranslationAlongZ = finalParameters[2];

  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();

  const double bestValue = optimizer->GetValue();

  std::cout << "Result = " << std::endl;
  std::cout << " Translation X = " << TranslationAlongX  << std::endl;
  std::cout << " Translation Y = " << TranslationAlongY  << std::endl;
  std::cout << " Translation Z = " << TranslationAlongZ  << std::endl;
  std::cout << " Iterations    = " << numberOfIterations << std::endl;
  std::cout << " Metric value  = " << bestValue          << std::endl;

  
  // Write out the projection image at the registration position
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  typedef itk::ResampleImageFilter< InternalImageType, InternalImageType > FilterType;

  FilterType::Pointer filter = FilterType::New();

  filter->SetInput( caster3D->GetOutput() );
  filter->SetDefaultPixelValue( 0 );

  filter->SetTransform( transform );
  filter->SetInterpolator( interpolator );

  filter->SetSize( imageReader2D->GetOutput()->GetLargestPossibleRegion().GetSize() );
  filter->SetOutputOrigin(  imageReader2D->GetOutput()->GetOrigin() );
  filter->SetOutputSpacing( imageReader2D->GetOutput()->GetSpacing() );

  typedef itk::CastImageFilter< InternalImageType, ImageType2D > OutputCastFilterType;
  OutputCastFilterType::Pointer outputCaster   = OutputCastFilterType::New();
  outputCaster->SetInput( filter->GetOutput() );

   typedef itk::ImageFileWriter< ImageType2D >  WriterType;
    WriterType::Pointer writer = WriterType::New();

    writer->SetFileName( fileOutput );
    writer->SetInput( outputCaster->GetOutput() );

    try { 
      std::cout << "Writing image: " << fileOutput << std::endl;
      writer->Update();
    } 
    catch( itk::ExceptionObject & err ) { 
      
      std::cerr << "ERROR: ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
    } 


  return 0;
}

