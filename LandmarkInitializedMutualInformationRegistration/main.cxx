#include "itkWin32Header.h"
#include "guiMainImplementation.h"
#include "ImageRegistrationApp.h"
#include <time.h>
#include <string>
#include "metaCommand.h"

int usage()
  {
  std::cout << "miRegTool [options] fixedImage movingImage" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -I <method#> : Registration initialization" << std::endl;
  std::cout << "        0 - NONE" << std::endl;
  std::cout << "        1 - Image Centers" << std::endl;
  std::cout << "        2 - Centers of Mass [default]" << std::endl;
  std::cout << "        3 - Moments" << std::endl;
  std::cout << "        4 - Landmarks" << std::endl;
  std::cout << "        5 <#_Of_Tfms> <tfm1> [<tfm2> ...] - Load transform" 
            << std::endl
            << "          - if #_of_Tfms is negative, the transforms are" 
            << std::endl
            << "            inverted before being applied."
            << std::endl;
  std::cout << "  -R <method#>: Registration Method" << std::endl;
  std::cout << "        0 - NONE" << std::endl;
  std::cout << "        1 - Rigid [default]" << std::endl;
  std::cout << "        2 - Affine" << std::endl;
  std::cout << "        3 - Rigid + Affine" << std::endl;
  std::cout << "  -O <method#>" << std::endl;
  std::cout << "        0 - One plus one evolutionary" << std::endl;
  std::cout << "        1 - Powell line search" << std::endl;
  std::cout << "        2 - One plus One + Powell line search [default]" 
            << std::endl;
  std::cout << "  -i <iterations> : # of iterations for optimizer [1000]"
            << std::endl;
  std::cout << "  -s <# of samples> : # of samples for MI computation [20000]"
            << std::endl;
  std::cout << "  -L <fixedLandmarksfile> <movingLandmarksfile>" << std::endl;
  std::cout << "  -T <filename> : save registration transform" << std::endl;
  std::cout << "  -W <filename> : save registered moving image" << std::endl;
  return 1;
  }

int main(int argc, char **argv)
  {
  if(argc > 1)
    {

    MetaCommand command;

    command.SetOption("RegistrInit","I",false,"Registration initialization (Default is Centers of Mass)");
    command.AddOptionField("RegistrInit","init",MetaCommand::INT,false,"2");
  
    command.SetOption("transform","tr",false,"specification of the transform");
    command.AddOptionField("transform","filename",MetaCommand::STRING,true);

    command.SetOption("RegistrMethod","R",false,"Registration Method (Default is Rigid)");
    command.AddOptionField("RegistrMethod","method",MetaCommand::INT,false,"1");
    
    command.SetOption("-O","O",false,"Default is One plus One + Powell line search");
    command.AddOptionField("-O","option",MetaCommand::INT,false,"2");
    
    command.SetOption("iterations","i",false,"# of iterations for optimizer [1000]");
    command.AddOptionField("iterations","nb",MetaCommand::INT,false,"500");

    command.SetOption("samples","s",false,"# of samples for MI computation [20000]");
    command.AddOptionField("samples","nb",MetaCommand::INT,false,"20000");

    command.SetOption("file","L",false,"<fixedLandmarksfile> <movingLandmarksfile>");
    command.AddOptionField("file","fixedLandmarksfile",MetaCommand::STRING,true);
    command.AddOptionField("file","movingLandmarksfile",MetaCommand::STRING,true);

    command.SetOption("saveTrans","T",false,"save registration transform");
    command.AddOptionField("saveTrans","filename",MetaCommand::STRING,true);

    command.SetOption("saveMove","W",false,"save registered moving image");
    command.AddOptionField("saveMove","filename",MetaCommand::STRING,true);

  // Fields are added in order
    command.AddField("fixedImage","fixedImage filename",MetaCommand::STRING,true);
    command.AddField("movingImage","movingImage filename",MetaCommand::STRING,true);
    
    if(!command.Parse(argc,argv))
      {
      return 1;
      }

 
  
//  unsigned long iterations = 500;
//    unsigned long samples = 20000;

//    int initializationMethod = 2;
//    int registrationMethod = 1;
//    int optimizationMethod = 2;

    char fixedImageFilename[255];
    char movingImageFilename[255];

    int  loadedTransformNumber = 0;
    std::string loadedTransformFilename[255];

    char outputImageFilename[255];
    outputImageFilename[0] = '\0';

    char fixedLandmarksFilename[255];
    fixedLandmarksFilename[0] = '\0';
    char movingLandmarksFilename[255];
    movingLandmarksFilename[0] = '\0';

    char outputTransformFilename[255];
    outputTransformFilename[0] = '\0';

    int initializationMethod = command.GetValueAsInt("RegistrInit","init");
    if( initializationMethod == 5 )
    {
      loadedTransformFilename[0]=command.GetValueAsString("transform","filename");
    }
    int registrationMethod = command.GetValueAsInt("RegistrMethod","method");
    int optimizationMethod = command.GetValueAsInt("-O","option");
    unsigned long iterations = command.GetValueAsInt("iterations","nb");
    unsigned long samples = command.GetValueAsInt("samples","nb");
    strcpy(fixedLandmarksFilename,command.GetValueAsString("file","fixedLandmarksfile").c_str());
    strcpy(outputTransformFilename,command.GetValueAsString("saveTrans","filename").c_str());
    strcpy(outputImageFilename,command.GetValueAsString("saveMove","filename").c_str());

    strcpy(fixedImageFilename, command.GetValueAsString("fixedImage").c_str());
    strcpy(movingImageFilename, command.GetValueAsString("movingImage").c_str());
    
/*  int argNum = 1;
    while (argNum < argc-3 && argv[argNum][0] == '-')
      {
      switch(argv[argNum][1])
        {
        case 'I':
          argNum++;
          initializationMethod = (int)atof(argv[argNum++]);
          if(initializationMethod == 5)
            {
            loadedTransformNumber = (int)atof(argv[argNum++]);
            for(int i=0; i<abs(loadedTransformNumber); i++)
              {
              loadedTransformFilename[i] = argv[argNum++];
              }
            }
          break;
        case 'R':
          argNum++;
          registrationMethod = (int)atof(argv[argNum++]);
          break;
        case 'i':
          argNum++;
          iterations = (int)atof(argv[argNum++]);
          break;
        case 's':
          argNum++;
          samples = (int)atof(argv[argNum++]);
          break;
        case 'L':
          argNum++;
          strcpy(fixedLandmarksFilename, argv[argNum++]);
          strcpy(movingLandmarksFilename, argv[argNum++]);
          break;
        case 'T':
          argNum++;
          strcpy(outputTransformFilename, argv[argNum++]);
          break;
        case 'O':
          argNum++;
          optimizationMethod = (int)atof(argv[argNum++]);
          break;
        case 'W':
          argNum++;
          strcpy(outputImageFilename, argv[argNum++]);
          break;
        default:
          return usage();
        }
      }

    if(argNum != argc-2)
      {
      return usage();
      }

    strcpy(fixedImageFilename, argv[argNum++]);
    strcpy(movingImageFilename, argv[argNum++]);
*/
    typedef itk::Image<short, 3>            ImageType;
    typedef ImageRegistrationApp<ImageType> ImageRegistrationAppType;
    typedef itk::LandmarkSpatialObject<3>   LandmarkType;
    typedef itk::ImageFileReader<ImageType> ImageReaderType;
    typedef itk::ImageFileWriter<ImageType> ImageWriterType;
    typedef itk::SpatialObjectReader<>      LandmarkReaderType;
    typedef LandmarkReaderType::GroupType   GroupType;
    typedef itk::SpatialObjectWriter<>      GroupWriterType;
    typedef itk::SpatialObjectReader<>      GroupReaderType;
    typedef itk::AffineTransform<double, 3> TransformType;

    ImageType::Pointer fixedImage;
    ImageType::Pointer movingImage;

    ImageReaderType::Pointer fixedReader = ImageReaderType::New();
    fixedReader->SetFileName(fixedImageFilename);
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
    fixedImage = fixedReader->GetOutput();

    ImageReaderType::Pointer movingReader = ImageReaderType::New();
    movingReader = ImageReaderType::New();
    movingReader->SetFileName(movingImageFilename);
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
    movingImage = movingReader->GetOutput();

    LandmarkType::Pointer fixedLandmarks;
    LandmarkType::Pointer movingLandmarks;
    if(strlen(fixedLandmarksFilename)>1)
      {
      LandmarkReaderType::Pointer fixedLandmarkReader =
                                  LandmarkReaderType::New();
      fixedLandmarkReader->SetFileName(fixedLandmarksFilename);
      fixedLandmarkReader->Update();
      GroupType::Pointer group = fixedLandmarkReader->GetGroup();
      GroupType::ChildrenListType * children = group->GetChildren();
      fixedLandmarks = dynamic_cast< LandmarkType * >
                                      ((*(children->begin())).GetPointer());

      LandmarkReaderType::Pointer movingLandmarkReader =
                                  LandmarkReaderType::New();
      movingLandmarkReader->SetFileName(movingLandmarksFilename);
      movingLandmarkReader->Update();
      group = movingLandmarkReader->GetGroup();
      children = group->GetChildren();
      movingLandmarks = dynamic_cast< LandmarkType * >
                                      ((*(children->begin())).GetPointer());
      }

    // Register
    ImageRegistrationAppType::Pointer imageRegistrationApp =
        ImageRegistrationAppType::New();
    imageRegistrationApp->SetFixedImage( fixedImage.GetPointer() );
    imageRegistrationApp->SetMovingImage( movingImage.GetPointer() );
    imageRegistrationApp->SetMovingImageRegion( 
         movingImage->GetLargestPossibleRegion() );

    imageRegistrationApp->SetRigidNumberOfIterations(iterations);
    imageRegistrationApp->SetAffineNumberOfIterations(iterations);
    imageRegistrationApp->SetRigidNumberOfSpatialSamples(samples);
    imageRegistrationApp->SetAffineNumberOfSpatialSamples(samples);


    switch(optimizationMethod)
      {
      default:
        std::cerr << "Error: Optimization method unknown!" << std::endl;
        break;
      case 0:
        imageRegistrationApp->SetOptimizerToOnePlusOne();
        break;
      case 1:
        imageRegistrationApp->SetOptimizerToGradient();
        break;
      case 2:
        imageRegistrationApp->SetOptimizerToOnePlusOnePlusGradient();
        break;
      }

    switch(initializationMethod)
      {
      default:
        std::cerr << "Error: Initialization method unknown!" << std::endl;
        break;
      case 0:
        imageRegistrationApp->RegisterUsingNone();
        break;
      case 1:
        imageRegistrationApp->RegisterUsingCenters();
        break;
      case 2:
        imageRegistrationApp->RegisterUsingMass();
        break;
      case 3:
        imageRegistrationApp->RegisterUsingMoments();
        break;
      case 4:
        if( strlen( movingLandmarksFilename ) > 1 )
          {
          imageRegistrationApp->RegisterUsingLandmarks( 
                                      fixedLandmarks.GetPointer(),
                                      movingLandmarks.GetPointer());
          }
        break;
      case 5:
        typedef ImageRegistrationAppType::LoadedRegTransformType LoadedTType;
        loadedTransformNumber = 1;
        for(int i=0; i<abs(loadedTransformNumber); i++)
          {
          GroupReaderType::Pointer transformReader = GroupReaderType::New();
          std::cout << "Loading Transform: " << loadedTransformFilename[i].c_str() << std::endl;
          transformReader->SetFileName(loadedTransformFilename[i].c_str());
          transformReader->Update();
          GroupType::Pointer group = transformReader->GetGroup();
          if(loadedTransformNumber<0)
            {
            std::cout << "Inverting" << std::endl;
            GroupType::TransformType::Pointer transform;
            transform = group->GetObjectToParentTransform();
            LoadedTType::Pointer invertedTransform = LoadedTType::New();
            transform->GetInverse(invertedTransform);
            transform->SetCenter(invertedTransform->GetCenter());
            transform->SetMatrix(invertedTransform->GetMatrix());
            transform->SetOffset(invertedTransform->GetOffset());
            }
          if(i == 0)
            {
            imageRegistrationApp->SetLoadedTransform(
                                   *(group->GetObjectToParentTransform()));
            }
          else
            {
            imageRegistrationApp->CompositeLoadedTransform(
                                     *(group->GetObjectToParentTransform()));
            }
          }
        imageRegistrationApp->RegisterUsingLoadedTransform();
        break;
      }
    clock_t timeInitEnd = clock();

    double finalMetricValue = 0;
    switch(registrationMethod)
      {
      default:
        std::cerr << "Error: Image registration method unknown!" << std::endl;
        break;
      case 0:
        break;
      case 1:
        imageRegistrationApp->RegisterUsingRigid();
        finalMetricValue = imageRegistrationApp->GetRigidMetricValue();
        break;
      case 2:
        imageRegistrationApp->RegisterUsingAffine();
        finalMetricValue = imageRegistrationApp->GetAffineMetricValue();
        break;
      case 3:
        imageRegistrationApp->RegisterUsingRigid();
        imageRegistrationApp->RegisterUsingAffine();
        finalMetricValue = imageRegistrationApp->GetAffineMetricValue();
        break;
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
    
    if(strlen(outputImageFilename)>1)
      {
      ImageWriterType::Pointer imageWriter = ImageWriterType::New();
      imageWriter->SetFileName( outputImageFilename );
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

    if( strlen( outputTransformFilename ) > 1)
      {
      GroupWriterType::Pointer transformWriter = GroupWriterType::New();
      transformWriter->SetFileName(outputTransformFilename);
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
