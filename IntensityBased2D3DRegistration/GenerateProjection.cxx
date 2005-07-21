/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    GenerateProjection.cxx
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
// This program creates a 2D projection from a 3D image using 
// the \code{RayCastInterpolateImageFunction} class.
//
// Software Guide : EndLatex 


#include <iostream>


// Software Guide : BeginLatex
//
// The transformation used is a rigid 3D Euler transform with the
// provision of a center of rotation which defaults to the center of
// the 3D volume. In practice the center of the particular feature of
// interest in the 3D volume should be used.
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
#include "itkCenteredEuler3DTransform.h"
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Ray casting is used to project the 3D volume onto pixels in the target 2D
// image.
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
#include "itkRayCastInterpolateImageFunction.h"
// Software Guide : EndCodeSnippet

#include "itkImage.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"



void usage()
{
  std::cerr << "\n";
  std::cerr << "Usage: GenerateProjection <options> inputVolume3D outputImage2D\n";
  std::cerr << "       Projects a 3D volume onto a 2D image. \n\n";
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
  std::cerr << "                                by  john.hipwell@kcl.ac.uk\n";
  std::cerr << "                                and thomas@hartkens.de\n";
  std::cerr << "                                (Imaging Sciences KCL London)\n\n";
  exit(1);
}


int main( int argc, char *argv[] )
{
  char *fileVolume3D = NULL;
  char *fileImage2D = NULL;

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

    if (ok == false) 
      {

      if (fileVolume3D == NULL) 
        {
        fileVolume3D = argv[1];
        argc--;
        argv++;
        }
      else if (fileImage2D == NULL) 
        {
        fileImage2D = argv[1];
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
    if (fileImage2D)  std::cout << "Output 2D image: " << fileImage2D  << std::endl;
    if (fileVolume3D) std::cout << "Input  3D image: " << fileVolume3D << std::endl;
    }


  
  const    unsigned int    Dimension = 3;
  typedef  short           PixelType;

  typedef itk::Image< PixelType, Dimension > ImageType2D;
  typedef itk::Image< PixelType, Dimension > ImageType3D;

  typedef   float     InternalPixelType;
  typedef itk::Image< InternalPixelType, Dimension > InternalImageType;

  typedef itk::CenteredEuler3DTransform< double >  TransformType;

  typedef itk::RayCastInterpolateImageFunction< 
                                    InternalImageType,
                                    double          >    InterpolatorType;

  TransformType::Pointer      transform     = TransformType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();

  typedef itk::ImageFileReader< ImageType3D > ImageReaderType3D;

  ImageReaderType3D::Pointer imageReader3D = ImageReaderType3D::New();

  imageReader3D->SetFileName( fileVolume3D );

  typedef itk::CastImageFilter< 
                        ImageType2D, InternalImageType > CastFilterType2D;
  typedef itk::CastImageFilter< 
                        ImageType3D, InternalImageType > CastFilterType3D;

  CastFilterType3D::Pointer caster3D = CastFilterType3D::New();


  caster3D->SetInput( imageReader3D->GetOutput() );

  caster3D->Update();


  transform->SetComputeZYX(true);

  TransformType::OutputVectorType translation;

  translation[0] = tx;
  translation[1] = ty;
  translation[2] = tz;

  const double PI = atan( 1.0 ) / 4.0;

  transform->SetTranslation(translation);
  transform->SetRotation( PI/180.0*rx, PI/180.0*ry, PI/180.0*rz);

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


  typedef ImageType2D::PointType       ImageOrigin2D;
  typedef ImageType2D::SpacingType     ImageSpacing2D;
  typedef ImageType2D::RegionType      ImageRegionType2D;
  typedef ImageRegionType2D::SizeType  SizeType2D;

  ImageOrigin2D        origin2D;
  ImageSpacing2D       resolution2D;
  SizeType2D           size2D;
  ImageRegionType2D    region2D;

  size2D[0] = 256;
  size2D[1] = 256;

  resolution2D[0] = 1.0;
  resolution2D[1] = 1.0;

  origin2D[0] = origin3D[0] + o2Dx - resolution2D[0]*((double) size2D[0] - 1.)/2.; 
  origin2D[1] = origin3D[1] + o2Dy - resolution2D[1]*((double) size2D[1] - 1.)/2.; 
  origin2D[2] = origin3D[2] + focalLength/2.; 

  InterpolatorType::InputPointType focalPoint;

  focalPoint[0] = origin3D[0];
  focalPoint[1] = origin3D[1];
  focalPoint[2] = origin3D[2] - focalLength/2.0;

  interpolator->SetFocalPoint( focalPoint );
  interpolator->SetThreshold( threshold );
  interpolator->SetTransform( transform );


  // Write out the projection image at the registration position
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  typedef itk::ResampleImageFilter< InternalImageType, ImageType2D > FilterType;

  FilterType::Pointer filter = FilterType::New();

  filter->SetInput( caster3D->GetOutput() );
  filter->SetDefaultPixelValue( 0 );

  filter->SetTransform( transform );
  filter->SetInterpolator( interpolator );

  filter->SetSize( size2D );
  filter->SetOutputOrigin(  origin2D );
  filter->SetOutputSpacing( resolution2D );

   typedef itk::ImageFileWriter< ImageType2D >  WriterType;
    WriterType::Pointer writer = WriterType::New();

    writer->SetFileName( fileImage2D );
    writer->SetInput( filter->GetOutput() );

    try 
      { 
      std::cout << "Writing image: " << fileImage2D << std::endl;
      writer->Update();
      } 
    catch( itk::ExceptionObject & err ) 
      { 
      std::cerr << "ERROR: ExceptionObject caught !" << std::endl; 
      std::cerr << err << std::endl; 
      } 


  return 0;
}

