/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    main.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkWin32Header.h"
#include "guiMainImplementation.h"
#include "ImageRegistrationApp.h"
#include <time.h>
#include <string>
#include "metaCommand.h"
#include "itkTransformBase.h"
#include "itkTransformFactory.h"

int main(int argc, char **argv)
  {
  if(argc > 1)
    {

    MetaCommand command;

    command.SetOption("InitMass", "InitMass", false,
                      "Initialize registrations using Centers of Mass");
    command.SetOption("InitCenters", "InitCenters", false,
                      "Initialize registration using image centers");
    command.SetOption("InitMoments", "InitMoments", false,
                      "Initialize registration using image moments");
    command.SetOption("InitTransform", "InitTransform", false,
                      "Initialize using transforms <numberOfTransforms> <ListOfTransforms>*");
    command.AddOptionField("InitTransform", "ListOfTransforms",
                           MetaCommand::LIST, true);
    command.SetOption("InitInvTransform", "InitInvTransform", false,
                    "Initialize using inverse of the transforms <numberOfTransforms> <ListOfTransforms>*");
    command.AddOptionField("InitInvTransform", "ListOfInvTransforms",
                           MetaCommand::LIST, true);
    command.SetOption("InitLandmarks", "InitLandmarks", false,
      "Initialize using landmarks <fixedLandmarksfile> <movingLandmarksfile>");
    command.AddOptionField("InitLandmarks", "FixedLandmarksFile",
                           MetaCommand::STRING, true);
    command.AddOptionField("InitLandmarks", "MovingLandmarksFile",
                           MetaCommand::STRING, true);

    command.SetOption("TfmNone", "TfmNone", false,
                      "Do not perform registration");
    command.SetOption("TfmRigid", "TfmRigid", false,
                      "Perform Rigid registration");
    command.SetOption("TfmAffine", "TfmAffine", false,
                      "Perform Affine registration");
    command.SetOption("TfmRigidAffine", "TfmRigidAffine", false,
                      "Perform Rigid and then Affine registration");
    command.SetOption("TfmDeformable", "TfmDeformable", false,
                      "Perform Deformable registration");
    
    command.SetOption("OptOnePlusOne", "OptOnePlusOne", false,
                      "Use One plus One optimizer");
    command.SetOption("OptGradient", "OptGradient", false,
                      "Use gradient (with powell line search) optimizer");
    command.SetOption("OptOnePlusOneGradient", "OptOnePlusOneGradient", false,
                      "Use One plus One and then gradient optimizers");
    command.SetOption("OptIterations", "OptIterations", false,
                      "# of iterations for optimizer");
    command.AddOptionField("OptIterations", "Number",
                           MetaCommand::INT, true, "500");

    command.SetOption("MetricSamples", "MetricSamples", false,
                   "# of samples for mutual information metric");
    command.AddOptionField("MetricSamples", "Number",
                           MetaCommand::INT, true, "20000");


    command.SetOption("SaveTransform", "SaveTransform", false,
                      "Save registration transform <Filename>");
    command.AddOptionField("SaveTransform", "Filename",
                            MetaCommand::STRING, true);

    command.SetOption("SaveMovingImage", "SaveMovingImage", false,
                      "Save registered moving image <Filename>");
    command.AddOptionField("SaveMovingImage", "Filename",
                           MetaCommand::STRING, true);

    // Fields are added in order
    command.AddField("FixedImage", "FixedImage's Filename",
                     MetaCommand::STRING, true);
    command.AddField("MovingImage","MovingImage's Filename",
                     MetaCommand::STRING, true);
    
    if(!command.Parse(argc,argv))
      {
      return 1;
      }

    typedef itk::Image<short, 3>            ImageType;
    typedef ImageRegistrationApp<ImageType> ImageRegistrationAppType;

    /** Create the registration app **/
    ImageRegistrationAppType::Pointer imageRegistrationApp =
                                      ImageRegistrationAppType::New();

    typedef itk::LandmarkSpatialObject<3>   LandmarkType;
    typedef itk::ImageFileReader<ImageType> ImageReaderType;
    typedef itk::ImageFileWriter<ImageType> ImageWriterType;
    typedef itk::SpatialObjectReader<>      LandmarkReaderType;
    typedef LandmarkReaderType::GroupType   GroupType;
    typedef itk::SpatialObjectWriter<>      GroupWriterType;
    typedef itk::SpatialObjectReader<>      GroupReaderType;
    typedef itk::TransformFileReader        TransformReaderType;
    typedef itk::TransformFileWriter        TransformWriterType;
    typedef itk::AffineTransform<double, 3> TransformType;
#if ITK_VERSION_MAJOR < 4
    typedef itk::BSplineDeformableTransform<double, 3, 3> DeformableTransformType ;
#else
    typedef itk::BSplineTransform<double, 3, 3> DeformableTransformType ;
#endif
    typedef itk::TransformFileReader::TransformListType * TransformListType;
    
    /** Read the fixed image **/
    ImageReaderType::Pointer fixedReader = ImageReaderType::New();
    fixedReader->SetFileName(command.GetValueAsString("FixedImage").c_str());
    try
      {
      fixedReader->Update();
      }
    catch(...)
      {
      std::cerr << "ERROR: exception caught while loading fixed image."
                << std::endl;
      return 1;
      }
    imageRegistrationApp->SetFixedImage( fixedReader->GetOutput() );

    /** Read the moving image **/
    ImageReaderType::Pointer movingReader = ImageReaderType::New();
    movingReader = ImageReaderType::New();
    movingReader->SetFileName(command.GetValueAsString("MovingImage").c_str());
    try
      {
      movingReader->Update();
      }
    catch(...)
      {
      std::cerr << "ERROR: exception caught while loading moving image."
                << std::endl;
      return 1;
      }
    imageRegistrationApp->SetMovingImage( movingReader->GetOutput() );

    imageRegistrationApp->SetFixedImageRegion( 
                                fixedReader->GetOutput()
                                            ->GetLargestPossibleRegion() );

    std::cout << "Optimizer number of iterations = "
              << command.GetValueAsInt("OptIterations", "Number")
              << std::endl;
    imageRegistrationApp->SetRigidNumberOfIterations(
                           command.GetValueAsInt("OptIterations","Number") );
    imageRegistrationApp->SetAffineNumberOfIterations(
                           command.GetValueAsInt("OptIterations","Number") );
    imageRegistrationApp->SetRigidNumberOfSpatialSamples(
                           command.GetValueAsInt("MetricSamples","Number") );
    imageRegistrationApp->SetAffineNumberOfSpatialSamples(
                           command.GetValueAsInt("MetricSamples","Number") );


    if( command.GetOptionWasSet("InitMass") )
      {
      imageRegistrationApp->RegisterUsingMass();
      }
    else if( command.GetOptionWasSet("InitCenters") )
      {
      imageRegistrationApp->RegisterUsingCenters();
      }
    else if( command.GetOptionWasSet("InitMoments") )
      {
      imageRegistrationApp->RegisterUsingMoments();
      }
    else if( command.GetOptionWasSet("InitTransform") )
      {
      typedef ImageRegistrationAppType::LoadedRegTransformType LoadedTType;
      typedef ImageRegistrationAppType::DeformableTransformType LoadedDefTType;
      std::list< std::string > filenames =
                                     command.GetValueAsList("InitTransform");
      bool first = true;
      std::list< std::string >::iterator it = filenames.begin();
      while( it != filenames.end() )
        {
        TransformReaderType::Pointer transformReader = TransformReaderType::New();
        std::cout << "Loading Transform: " << it->c_str() << std::endl;
        transformReader->SetFileName( it->c_str() );
        
        //Register the transform into the TransformFactory, required for the BSplineTransform
        itk::TransformFactory<LoadedDefTType>::RegisterTransform();
        
        try
          {
          transformReader->Update();
          }
        catch( itk::ExceptionObject & excp )
          {
          std::cerr << "Error while reading the transform file" << std::endl;
          std::cerr << excp << std::endl;
          std::cerr << "[FAILED]" << std::endl;
          return EXIT_FAILURE;
          }
        
        TransformListType transforms = transformReader->GetTransformList();

        itk::TransformFileReader::TransformListType::const_iterator TransformIt = transforms->begin();
        std::cout << transforms->size() << std::endl;
        for (unsigned int i = 0 ; i < transforms->size() ; i++)
          {
          if(!strcmp((*TransformIt)->GetNameOfClass(),"AffineTransform"))
            {
            LoadedTType::Pointer affine_read = static_cast<LoadedTType*>((*TransformIt).GetPointer());
            LoadedTType::ConstPointer m_affine = affine_read.GetPointer();
            if ( first )
              {
              first = false;
              imageRegistrationApp->SetLoadedTransform(
                                    *m_affine.GetPointer());
              }
            else
              {
              imageRegistrationApp->CompositeLoadedTransform(
                                        *m_affine.GetPointer());
              }
            }

#if ITK_VERSION_MAJOR < 4
          if (!strcmp((*TransformIt)->GetNameOfClass(),"BSplineDeformableTransform"))
#else
          if (!strcmp((*TransformIt)->GetNameOfClass(),"BSplineTransform"))
#endif
            {
            LoadedDefTType::Pointer bspline_read = static_cast<LoadedDefTType*>((*TransformIt).GetPointer());
            LoadedDefTType::ConstPointer m_bspline = bspline_read.GetPointer();
            imageRegistrationApp->SetLoadedDeformableTransform(
                                    *m_bspline.GetPointer());
            }
          ++TransformIt;
          }
        ++it;
        }
      imageRegistrationApp->RegisterUsingLoadedTransform();
      }
    else if( command.GetOptionWasSet("InitInvTransform") )
      {
      typedef ImageRegistrationAppType::LoadedRegTransformType LoadedTType;
      typedef ImageRegistrationAppType::DeformableTransformType LoadedDefTType;
      std::list< std::string > filenames =
                                     command.GetValueAsList("InitInvTransform");
      bool first = true;
      std::list< std::string >::iterator it = filenames.begin();
      while( it != filenames.end() )
        {
        TransformReaderType::Pointer transformReader = TransformReaderType::New();
        std::cout << "Loading Transform: " << it->c_str() << std::endl;
        transformReader->SetFileName( it->c_str() );
        
        //Register the transform into the TransformFactory, required for the BSplineTransform
        itk::TransformFactory<LoadedDefTType>::RegisterTransform();
        
        try
          {
          transformReader->Update();
          }
        catch( itk::ExceptionObject & excp )
          {
          std::cerr << "Error while reading the transform file" << std::endl;
          std::cerr << excp << std::endl;
          std::cerr << "[FAILED]" << std::endl;
          return EXIT_FAILURE;
          }
          
        LoadedTType::Pointer invertedTransform = LoadedTType::New();
        
        TransformListType transforms = transformReader->GetTransformList();
        
        itk::TransformFileReader::TransformListType::const_iterator TransformIt = transforms->begin();
        for (unsigned int i = 0 ; i < transforms->size() ; i++)
          {
          if(!strcmp((*TransformIt)->GetNameOfClass(),"AffineTransform"))
            {
            LoadedTType::Pointer affine_read = static_cast<LoadedTType*>((*TransformIt).GetPointer());
            affine_read->GetInverse(invertedTransform);
            affine_read->SetCenter(invertedTransform->GetCenter());
            affine_read->SetMatrix(invertedTransform->GetMatrix());
            affine_read->SetOffset(invertedTransform->GetOffset());
            LoadedTType::ConstPointer m_affine = affine_read.GetPointer();
            if ( first )
              {
              first = false;
              imageRegistrationApp->SetLoadedTransform( *m_affine.GetPointer() );
              }
            else
              {
              imageRegistrationApp->CompositeLoadedTransform(
                                      *m_affine.GetPointer());
              }
            }

#if ITK_VERSION_MAJOR < 4
          if (!strcmp((*TransformIt)->GetNameOfClass(),"BSplineDeformableTransform"))
#else
          if (!strcmp((*TransformIt)->GetNameOfClass(),"BSplineTransform"))
#endif
            {
            LoadedDefTType::Pointer bspline_read = static_cast<LoadedDefTType*>((*TransformIt).GetPointer());
            LoadedDefTType::ConstPointer m_bspline = bspline_read.GetPointer();
            imageRegistrationApp->SetLoadedDeformableTransform(
                                    *m_bspline.GetPointer());
            }
          ++TransformIt;
          }        
        ++it;
        }

      imageRegistrationApp->RegisterUsingLoadedTransform();
      }
    else if( command.GetOptionWasSet("InitLandmarks") )
      {
      LandmarkType::Pointer fixedLandmarks;
      LandmarkType::Pointer movingLandmarks;
      LandmarkReaderType::Pointer fixedLandmarkReader =
                                    LandmarkReaderType::New();
      fixedLandmarkReader->SetFileName( command.GetValueAsString(
                                               "InitLandmarks",
                                               "FixedLandmarksFile").c_str() );
      fixedLandmarkReader->Update();
      GroupType::Pointer group = fixedLandmarkReader->GetGroup();
      GroupType::ChildrenListType * children = group->GetChildren();
      fixedLandmarks = dynamic_cast< LandmarkType * >
                                      ((*(children->begin())).GetPointer());
  
      LandmarkReaderType::Pointer movingLandmarkReader =
                                    LandmarkReaderType::New();
      movingLandmarkReader->SetFileName(command.GetValueAsString(
                                               "InitLandmarks",
                                               "MovingLandmarksFile").c_str() );
      movingLandmarkReader->Update();
      group = movingLandmarkReader->GetGroup();
      children = group->GetChildren();
      movingLandmarks = dynamic_cast< LandmarkType * >
                                      ((*(children->begin())).GetPointer());

      imageRegistrationApp->RegisterUsingLandmarks( 
                                    fixedLandmarks.GetPointer(),
                                    movingLandmarks.GetPointer());
      }
    else // Default if( command.GetOptionWasSet("InitNone") )
      {
      imageRegistrationApp->RegisterUsingNone();
      }

    imageRegistrationApp->SetOptimizerToOnePlusOnePlusGradient();
    if( command.GetOptionWasSet("OptOnePlusOne") )
      {
      imageRegistrationApp->SetOptimizerToOnePlusOne();
      }
    else if( command.GetOptionWasSet("OptGradient") )
      {
      imageRegistrationApp->SetOptimizerToGradient();
      }
    else if( command.GetOptionWasSet("OptOnePlusOneGradient") )
      {
      imageRegistrationApp->SetOptimizerToOnePlusOnePlusGradient();
      }

    clock_t timeInitEnd = clock();

    double finalMetricValue = 0;
    if( command.GetOptionWasSet("TfmNone") )
      {
      //if( imageRegistrationApp->RegisterUsingNone();
      }
    else if( command.GetOptionWasSet("TfmRigid") )
      {
      imageRegistrationApp->RegisterUsingRigid();
      finalMetricValue = imageRegistrationApp->GetRigidMetricValue();
      }
    else if( command.GetOptionWasSet("TfmAffine") )
      {
      imageRegistrationApp->RegisterUsingAffine();
      finalMetricValue = imageRegistrationApp->GetAffineMetricValue();
      }
    else if( command.GetOptionWasSet("TfmRigidAffine") )
      {
      imageRegistrationApp->RegisterUsingRigid();
      imageRegistrationApp->RegisterUsingAffine();
      finalMetricValue = imageRegistrationApp->GetAffineMetricValue();
      }
    
    if ( command.GetOptionWasSet("TfmDeformable") )
      {
      imageRegistrationApp->RegisterUsingDeformable();
      finalMetricValue = imageRegistrationApp->GetDeformableMetricValue();
      }

    clock_t timeRegEnd = clock();

    TransformType::MatrixType m = imageRegistrationApp->
                                             GetFinalTransform()->GetMatrix();
    TransformType::OffsetType o = imageRegistrationApp->
                                             GetFinalTransform()->GetOffset();
    for(int i=0; i<3; i++)
      {
      for(int j=0; j<3; j++)
        {
        std::cout << m[j][i] << " ";
        }
      }
    for(int i=0; i<3; i++)
      {
      std::cout << o[i] << " ";
      }
    std::cout << timeRegEnd - timeInitEnd << " ";
    std::cout << finalMetricValue << std::endl;
    
    if( command.GetOptionWasSet("SaveMovingImage") )
      {
      std::cout << "Writing registered moving image to "
                << command.GetValueAsString("SaveMovingImage", "Filename")
                << std::endl;
      ImageWriterType::Pointer imageWriter = ImageWriterType::New();
      imageWriter->SetFileName(  command.GetValueAsString("SaveMovingImage",
                                                          "Filename").c_str() );
      imageWriter->SetUseCompression(true);
      imageWriter->SetInput( 
                   imageRegistrationApp->GetFinalRegisteredMovingImage() );
      try
        {
        imageWriter->Update();
        }
      catch( itk::ExceptionObject &e )
        {
        std::cerr << e << std::endl;
        }
      }

    if( command.GetOptionWasSet("SaveTransform") )
      {
      TransformWriterType::Pointer transformWriter = TransformWriterType::New();
      transformWriter->SetFileName(command.GetValueAsString(
                                                "SaveTransform",
                                                "Filename").c_str());
      
      if (!imageRegistrationApp->GetFinalDeformableTransform())
        {
        transformWriter->SetInput(imageRegistrationApp->GetFinalTransform());
        }
      else
        {
        transformWriter->SetInput(imageRegistrationApp->GetFinalTransform());
        transformWriter->AddTransform(imageRegistrationApp->GetFinalDeformableTransform());
        }
      transformWriter->Update();
      }

    return 0;
    }

  guiMainImplementation* gui = new guiMainImplementation();
  gui->Show();
  return Fl::run();
  }
