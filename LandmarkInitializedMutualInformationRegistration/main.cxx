#include "itkWin32Header.h"
#include "guiMainImplementation.h"
#include "ImageRegistrationApp.h"
#include <time.h>
#include <string>
#include "metaCommand.h"

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
    typedef itk::AffineTransform<double, 3> TransformType;

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
    imageRegistrationApp->SetMovingImageRegion( 
                                movingReader->GetOutput()
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
      std::list< std::string > filenames =
                                     command.GetValueAsList("InitTransform");
      bool first = true;
      std::list< std::string >::iterator it = filenames.begin();
      while( it != filenames.end() )
        {
        GroupReaderType::Pointer transformReader = GroupReaderType::New();
        std::cout << "Loading Transform: " << it->c_str() << std::endl;
        transformReader->SetFileName( it->c_str() );
        transformReader->Update();
        GroupType::Pointer group = transformReader->GetGroup();
        if( first )
          {
          first = false;
          imageRegistrationApp->SetLoadedTransform(
                                 *(group->GetObjectToParentTransform()));
          }
        else
          {
          imageRegistrationApp->CompositeLoadedTransform(
                                   *(group->GetObjectToParentTransform()));
          }
        ++it;
        }
      imageRegistrationApp->RegisterUsingLoadedTransform();
      }
    else if( command.GetOptionWasSet("InitInvTransform") )
      {
      typedef ImageRegistrationAppType::LoadedRegTransformType LoadedTType;
      std::list< std::string > filenames =
                                     command.GetValueAsList("InitInvTransform");
      bool first = true;
      std::list< std::string >::iterator it = filenames.begin();
      while( it != filenames.end() )
        {
        GroupReaderType::Pointer transformReader = GroupReaderType::New();
        std::cout << "Loading Transform: " << it->c_str() << std::endl;
        transformReader->SetFileName( it->c_str() );
        transformReader->Update();
        GroupType::Pointer group = transformReader->GetGroup();
        std::cout << "Inverting" << std::endl;
        GroupType::TransformType::Pointer transform;
        transform = group->GetObjectToParentTransform();
        LoadedTType::Pointer invertedTransform = LoadedTType::New();
        transform->GetInverse(invertedTransform);
        transform->SetCenter(invertedTransform->GetCenter());
        transform->SetMatrix(invertedTransform->GetMatrix());
        transform->SetOffset(invertedTransform->GetOffset());
        if( first )
          {
          first = false;
          imageRegistrationApp->SetLoadedTransform(
                                 *(group->GetObjectToParentTransform()));
          }
        else
          {
          imageRegistrationApp->CompositeLoadedTransform(
                                   *(group->GetObjectToParentTransform()));
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
      GroupWriterType::Pointer transformWriter = GroupWriterType::New();
      transformWriter->SetFileName( command.GetValueAsString(
                                                "SaveTransform",
                                                "Filename").c_str() );
      GroupType::Pointer group = GroupType::New();
      itk::SpatialObject<3>::TransformType::Pointer transform =
          itk::SpatialObject<3>::TransformType::New();
      transform->SetCenter(imageRegistrationApp
                            ->GetFinalTransform()->GetCenter());
      transform->SetMatrix(imageRegistrationApp
                            ->GetFinalTransform()->GetMatrix());
      transform->SetOffset(imageRegistrationApp
                            ->GetFinalTransform()->GetOffset());
      group->SetObjectToParentTransform( transform.GetPointer() );
      transformWriter->SetInput( group );
      transformWriter->Update();
      }

    return 0;
    }

  guiMainImplementation* gui = new guiMainImplementation();
  gui->Show();
  return Fl::run();
  }
