#include "itkWin32Header.h"
#include "guiMainImplementation.h"
#include "ImageRegistrationApp.h"
#include <time.h>
#include <string>

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
            << std::endl;
  std::cout << "  -R <method#>: Registration Method" << std::endl;
  std::cout << "        0 - NONE" << std::endl;
  std::cout << "        1 - Rigid [default]" << std::endl;
  std::cout << "        2 - Affine" << std::endl;
  std::cout << "        3 - Rigid + Affine" << std::endl;
  std::cout << "  -O <method#>" << std::endl;
  std::cout << "        0 - One plus one evolutionary [default]" << std::endl;
  std::cout << "        1 - Gradient" << std::endl;
  std::cout << "        2 - Regular step gradient" << std::endl;
  std::cout << "        3 - Conjugate gradient" << std::endl;
  std::cout << "  -I <iterations> : number of iterations for the optimizer"
            << std::endl;
  std::cout << "  -S <# of samples> : number of samples for MI computation"
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
    if(argc < 3)
      {
      return usage();
      }

    int initializationMethod = 2;
    int registrationMethod = 1;
    int optimizationMethod = 0;

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

    int argNum = 1;
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
            for(unsigned int i=0; i<loadedTransformNumber; i++)
              {
              loadedTransformFilename[i] = argv[argNum++];
              }
            }
          break;
        case 'R':
          argNum++;
          registrationMethod = (int)atof(argv[argNum++]);
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
      std::cout << "ERROR: exception caught while loading fixed image."
                << std::endl;
      return 1;
      }
    fixedImage = fixedReader->GetOutput();
    std::cout << "Read fixed image: " << fixedImageFilename << std::endl;

    ImageReaderType::Pointer movingReader = ImageReaderType::New();
    movingReader = ImageReaderType::New();
    movingReader->SetFileName(movingImageFilename);
    try
      {
      movingReader->Update();
      }
    catch(...)
      {
      std::cout << "ERROR: exception caught while loading moving image."
                << std::endl;
      return 1;
      }
    movingImage = movingReader->GetOutput();
    std::cout << "Read moving image: " << movingImageFilename << std::endl;

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
      std::cout << "Read fixed landmarks: " 
                << fixedLandmarksFilename << std::endl;

      LandmarkReaderType::Pointer movingLandmarkReader =
                                  LandmarkReaderType::New();
      movingLandmarkReader->SetFileName(movingLandmarksFilename);
      movingLandmarkReader->Update();
      group = movingLandmarkReader->GetGroup();
      children = group->GetChildren();
      movingLandmarks = dynamic_cast< LandmarkType * >
                                      ((*(children->begin())).GetPointer());
      std::cout << "Read moving landmarks: " 
                << movingLandmarksFilename << std::endl;
      }

    // Register
    ImageRegistrationAppType::Pointer imageRegistrationApp =
        ImageRegistrationAppType::New();
    imageRegistrationApp->SetFixedImage( fixedImage.GetPointer() );
    imageRegistrationApp->SetMovingImage( movingImage.GetPointer() );
    imageRegistrationApp->SetMovingImageRegion( 
         movingImage->GetLargestPossibleRegion() );


    switch(optimizationMethod)
      {
      default:
        std::cout << "Error: Optimization method unknown!" << std::endl;
        break;
      case 0:
        imageRegistrationApp->SetOptimizerToOnePlusOne();
        break;
      case 1:
        imageRegistrationApp->SetOptimizerToGradient();
        break;
      case 2:
        imageRegistrationApp->SetOptimizerToRegularGradient();
        break;
      case 3:
        imageRegistrationApp->SetOptimizerToConjugateGradient();
        break;
      }

    clock_t timeStart = clock();
    switch(initializationMethod)
      {
      default:
        std::cout << "Error: Initialization method unknown!" << std::endl;
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
        for(unsigned int i=0; i<loadedTransformNumber; i++)
          {
          GroupReaderType::Pointer transformReader = GroupReaderType::New();
          transformReader->SetFileName(loadedTransformFilename[i].c_str());
          transformReader->Update();
          GroupType::Pointer group = transformReader->GetGroup();
          GroupType::TransformType::Pointer transform;
          transform = group->GetObjectToParentTransform();
          LoadedTType::Pointer loadedTransform = LoadedTType::New();
          loadedTransform->SetCenter(transform->GetCenterOfRotationComponent());
          loadedTransform->SetMatrix(transform->GetMatrix());
          loadedTransform->SetOffset(transform->GetOffset());
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

    switch(registrationMethod)
      {
      default:
        std::cout << "Error: Image registration method unknown!" << std::endl;
        break;
      case 0:
        break;
      case 1:
        imageRegistrationApp->RegisterUsingRigid();
        break;
      case 2:
        imageRegistrationApp->RegisterUsingAffine();
        break;
      case 3:
        imageRegistrationApp->RegisterUsingRigid();
        imageRegistrationApp->RegisterUsingAffine();
        break;
      }
    clock_t timeRegEnd = clock();

    std::cout << "Time for initialization = " 
              << timeInitEnd - timeStart << std::endl;
    std::cout << "Time for registration = " 
              << timeRegEnd - timeInitEnd << std::endl;
    std::cout << "Time total = " 
              << timeRegEnd - timeStart << std::endl;
    
    if(strlen(outputImageFilename)>1)
      {
      ImageWriterType::Pointer imageWriter = ImageWriterType::New();
      imageWriter->SetFileName( outputImageFilename );
      imageWriter->SetInput( 
                   imageRegistrationApp->GetFinalRegisteredMovingImage() );
      try
        {
        imageWriter->Update();
        }
      catch( itk::ExceptionObject &e )
        {
        std::cout << e << std::endl;
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
