#include "itkWin32Header.h"
#include "guiMainImplementation.h"
#include "ImageRegistrationApp.h"

int usage()
  {
  std::cout 
    << "limir [options] fixedImage movingImage outputResampledMovingImage" 
    << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -A : perform affine registration" << std::endl;
  std::cout << "  -L fixedLandmarks movingLandmarks" << std::endl;
  std::cout << "  -R <x y z> <x y z>: use ROI" << std::endl;
  std::cout << "  -l <filename> : save landmark registered image" << std::endl;
  std::cout << "  -r <filename> : save rigid registered image" << std::endl;
  std::cout << "  -1 <filename> : save landmark transform" << std::endl;
  std::cout << "  -2 <filename> : save rigid transform" << std::endl;
  return 1;
  }

int main(int argc, char **argv)
  {
  if(argc > 1)
    {
    if(argc < 4)
      {
      return usage();
      }

    bool affineRegistration = false;

    char fixedImageFilename[255];
    char movingImageFilename[255];
    char outputImageFilename[255];

    char fixedLandmarksFilename[255];
    fixedLandmarksFilename[0] = '\0';
    char movingLandmarksFilename[255];
    movingLandmarksFilename[0] = '\0';

    char outputLandmarksImageFilename[255];
    outputLandmarksImageFilename[0] = '\0';

    char outputRigidImageFilename[255];
    outputRigidImageFilename[0] = '\0';

    char outputLandmarksTransformFilename[255];
    outputLandmarksTransformFilename[0] = '\0';

    char outputRigidTransformFilename[255];
    outputRigidTransformFilename[0] = '\0';

    char outputTransformFilename[255];
    outputTransformFilename[0] = '\0';


    int argNum = 1;
    while (argNum < argc-3 && argv[argNum][0] == '-')
      {
      switch(argv[argNum][1])
        {
        case 'A':
          argNum++;
          affineRegistration = true;
          break;
        case 'L':
          argNum++;
          strcpy(fixedLandmarksFilename, argv[argNum++]);
          strcpy(movingLandmarksFilename, argv[argNum++]);
          break;
        case 'l':
          argNum++;
          strcpy(outputLandmarksImageFilename, argv[argNum++]);
          break;
        case 'r':
          argNum++;
          strcpy(outputRigidImageFilename, argv[argNum++]);
          break;
        case '1':
          argNum++;
          strcpy(outputLandmarksTransformFilename, argv[argNum++]);
          break;
        case '2':
          argNum++;
          strcpy(outputRigidTransformFilename, argv[argNum++]);
          break;
        default:
          return usage();
        }
      }

    if(argNum != argc-3)
      {
      return usage();
      }

    strcpy(fixedImageFilename, argv[argNum++]);
    strcpy(movingImageFilename, argv[argNum++]);
    strcpy(outputImageFilename, argv[argNum++]);

    typedef itk::Image<short, 3>            ImageType;
    typedef ImageRegistrationApp<ImageType> ImageRegistrationAppType;
    typedef itk::LandmarkSpatialObject<3>   LandmarkType;
    typedef itk::ImageFileReader<ImageType> ImageReaderType;
    typedef itk::ImageFileWriter<ImageType> ImageWriterType;
    typedef itk::SpatialObjectReader<>      LandmarkReaderType;
    typedef LandmarkReaderType::GroupType   GroupType;
    typedef itk::SpatialObjectWriter<>      LandmarkWriterType;
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
      fixedLandmarkReader->SetFullFileName(fixedLandmarksFilename);
      fixedLandmarkReader->Update();
      GroupType::Pointer group = fixedLandmarkReader->GetGroup();
      GroupType::ChildrenListType * children = group->GetChildren();
      fixedLandmarks = dynamic_cast< LandmarkType * >
                                      ((*(children->begin())).GetPointer());
      std::cout << "Read fixed landmarks: " 
                << fixedLandmarksFilename << std::endl;

      LandmarkReaderType::Pointer movingLandmarkReader =
                                  LandmarkReaderType::New();
      movingLandmarkReader->SetFullFileName(movingLandmarksFilename);
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

    if( strlen( movingLandmarksFilename ) > 1 )
      {
      imageRegistrationApp->RegisterUsingLandmarks( fixedLandmarks.GetPointer(),
                                                  movingLandmarks.GetPointer());
      }

    imageRegistrationApp->SetMovingImageRegion( 
        movingImage->GetLargestPossibleRegion() );

    imageRegistrationApp->RegisterUsingRigidMethod();

    if( affineRegistration )
      {
      imageRegistrationApp->RegisterUsingAffineMethod();
      }

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

    if( strlen( outputLandmarksImageFilename ) > 1)
      {
      }

    if( strlen( outputLandmarksTransformFilename ) > 1)
      {
      LandmarkWriterType::Pointer landmarkWriter = LandmarkWriterType::New();
      landmarkWriter->SetFullFileName(outputLandmarksTransformFilename);
      GroupType::Pointer group = GroupType::New();
      itk::SpatialObject<3>::TransformType::Pointer transform =
          itk::SpatialObject<3>::TransformType::New();
      transform->SetMatrix(imageRegistrationApp
                             ->GetLandmarkAffineTransform()->GetMatrix());
      transform->SetOffset(imageRegistrationApp
                            ->GetLandmarkAffineTransform()->GetOffset());
      group->SetObjectToParentTransform( transform.GetPointer() );
      landmarkWriter->SetInput( group );
      landmarkWriter->Update();
      }
    if( strlen( outputRigidTransformFilename ) > 1)
      {
      LandmarkWriterType::Pointer landmarkWriter = LandmarkWriterType::New();
      landmarkWriter->SetFullFileName(outputRigidTransformFilename);
      GroupType::Pointer group = GroupType::New();
      itk::SpatialObject<3>::TransformType::Pointer transform =
          itk::SpatialObject<3>::TransformType::New();
      transform->SetMatrix(imageRegistrationApp
                            ->GetRigidAffineTransform()->GetMatrix());
      transform->SetOffset(imageRegistrationApp
                            ->GetRigidAffineTransform()->GetOffset());
      group->SetObjectToParentTransform( transform.GetPointer() );
      landmarkWriter->SetInput( group );
      landmarkWriter->Update();
      }

    return 0;
    }

  guiMainImplementation* gui = new guiMainImplementation();
  gui->Show();
  return Fl::run();
  }
