#include "guiMainImplementation.h"
#include <ITKFlFileWriter.h>
 
guiMainImplementation
::guiMainImplementation()
  {
  m_ViewAxis = 2;
  m_FixedImageFileName = "";
  m_MovingImageFileName = "";
  m_LastLoadedImagePath = "";
  m_FixedImageLoaded = false;
  m_MovingImageLoaded = false;

  m_FixedImage = ImageType::New();
  m_MovingImage = ImageType::New();
  m_LandmarkRegisteredMovingImage = ImageType::New();
  m_RegisteredMovingImage = ImageType::New();

  m_FixedLandmarkSpatialObject = LandmarkSpatialObjectType::New();
  m_MovingLandmarkSpatialObject = 
                                 LandmarkSpatialObjectType::New();
  m_LandmarkRegisteredMovingLandmarkSpatialObject = 
                                 LandmarkSpatialObjectType::New();
  m_RegisteredMovingLandmarkSpatialObject = 
                                 LandmarkSpatialObjectType::New();

  m_RigidUseLargestRegion = true;
  m_RigidUseUserRegion = false;
  m_RigidUseLandmarkRegion = false;
  m_RigidRegionScale = 1.0;

  m_AffineUseLargestRegion = true;
  m_AffineUseUserRegion = false;
  m_AffineUseLandmarkRegion = false;
  m_AffineRegionScale = 1.0;

  m_ImageRegistrationApp = ImageRegistrationAppType::New();

  m_FixedImageSize.Fill(0);
  m_MovingImageSize.Fill(0);

  }

guiMainImplementation
::~guiMainImplementation()
  {
  Quit();
  }

void
guiMainImplementation
::Quit()
  {
  tkAdvancedOptionsWindow->hide();
  tkRegionOfInterestWindow->hide();
  tkWindow->hide();
  }

/////////////////////////////////////////////////
// Image File IO functions
/////////////////////////////////////////////////
void
guiMainImplementation
::SelectFixedImage()
  {
  const char * filename = fl_file_chooser("Choose an image file...","*.mh?","");

  if ( !filename )
    {
    return;
    }

  std::string fileName = filename;

  if ( m_FixedImageFileName !=  fileName )
    {
    this->HandleInputImageChange(fileName, true);
    }
  }

void
guiMainImplementation
::SelectMovingImage()
  {
  const char * filename = 
    fl_file_chooser("Choose an image file...","*.mh?","");

  if ( !filename )
    {
    return;
    }

  std::string fileName = filename;

  if ( m_MovingImageFileName !=  fileName )
    {
    this->HandleInputImageChange(fileName, false);
    }

  }

void 
guiMainImplementation
::HandleInputImageChange(std::string & fileName, bool isFixedImage)
  {
  ImageType::Pointer image;
  
  fl_cursor(Fl_Cursor(FL_CURSOR_WAIT), (Fl_Color) 56, (Fl_Color) 255);
  image = LoadImage(fileName.c_str());

  if ( !image )
    {
    return;
    }
  

  if ( isFixedImage )
    {
    if ( m_FixedImageFileName == fileName )
      {
      fl_cursor(Fl_Cursor(FL_CURSOR_DEFAULT), 
                (Fl_Color) 56, (Fl_Color) 255);
      return;
      }
    m_FixedImageFileName = fileName;

    tkFixedImageFile->value(m_LastLoadedImagePath.c_str());
    m_FixedImage = image;
    m_FixedImageLoaded = true;
    m_FixedImageSize = image->GetLargestPossibleRegion().GetSize();
    tkFixedSliceValuator->maximum(m_FixedImageSize[2]-1);
    tkSliceValuator->maximum(m_FixedImageSize[2]-1);
    tkFixedImageViewer->SetInputImage(m_FixedImage.GetPointer());
    tkFixedImageViewer->update();
    tkFixedImageViewer->redraw();
    tkFixedImageViewer->activate();
    tkResultImageViewer->SetInputImage(m_FixedImage.GetPointer());
    tkFixedImageLandmarkGroup->activate();
    Fl_Group* grp = tkFixedImageLandmarkGroup;
    for(int i=0; i < grp->children(); i++)
      {
      ((Fl_Button *)grp->child(i))->clear();
      ((Fl_Button *)grp->child(i))->activate();
      }        
    }
  else
    {
    if ( m_MovingImageFileName == fileName )
      {
      fl_cursor(Fl_Cursor(FL_CURSOR_DEFAULT), 
                (Fl_Color) 56, (Fl_Color) 255);
      return;
      }
    m_MovingImageFileName = fileName;

    tkMovingImageFile->value(m_LastLoadedImagePath.c_str());
    m_MovingImage = image;//.GetPointer();
    m_MovingImageLoaded = true;
    m_MovingImageSize = image->GetLargestPossibleRegion().GetSize();
    tkMovingSliceValuator->maximum(m_MovingImageSize[2]-1);
    tkMovingImageViewer->SetInputImage(m_MovingImage.GetPointer());
    tkMovingImageViewer->update();
    tkMovingImageViewer->redraw();
    tkMovingImageViewer->activate();
    tkResultImageViewer->SetSecondInputImage(m_MovingImage.GetPointer());
    tkMovingImageLandmarkGroup->activate();
    for(int i=0; i < tkMovingImageLandmarkGroup->children(); i++)
      {
      ((Fl_Button *)tkMovingImageLandmarkGroup->child(i))->clear();
      ((Fl_Button *)tkMovingImageLandmarkGroup->child(i))->activate();
      }
    } // end of if-else

  tkResultImageViewer->imageMode(IMG_LOG);
  tkResultImageViewer->update();
  tkResultImageViewer->redraw();
  tkResultImageViewer->activate();

  tkLandmarkRegisteredView->deactivate();
  tkRegisteredView->deactivate();
  this->SelectImageSet(0);
  this->SetViewAxis(2);
  tkViewAxisZ->setonly();

  fl_cursor(Fl_Cursor(FL_CURSOR_DEFAULT), (Fl_Color) 56, (Fl_Color) 255);
  }

guiMainImplementation::ImagePointer
guiMainImplementation
::LoadImage( const char * filename )
  {
  ImageReaderType::Pointer reader;
  ImageType::Pointer image;

  reader = ImageReaderType::New();
  reader->SetFileName(filename);

  try
    {
    reader->Update();      
    m_LastLoadedImagePath = filename;     
    }
  catch( itk::ExceptionObject &e )
    {
    std::cout<<e<<std::endl;
    }
  catch(...)
    {
    std::cout<<" Unknow exception caught in LoadImage!!!"<<std::endl;
    }

  image = reader->GetOutput();

  return image;
  }

void
guiMainImplementation
::SaveLandmarkRegisteredImage()
  {
  itkFlFileWriter< ImageType >( m_LandmarkRegisteredMovingImage.GetPointer(), "Save pre-registered image file...", "*.mh?", "", 0 );
/*
  const char * filename = fl_file_chooser("Save pre-registered image file...",
                                          "*.mh?","");

  if( filename )
    {
    ImageWriterType::Pointer writer = 
      ImageWriterType::New();
    writer->SetFileName( filename );
    writer->SetInput( m_LandmarkRegisteredMovingImage.GetPointer() );
    try
      {
      writer->Update();
      }
    catch( itk::ExceptionObject &e )
      {
      std::cout<<e<<std::endl;
      }
    }*/
  }

void
guiMainImplementation
::SaveFinalRegisteredImage()
  {
  itkFlFileWriter< ImageType >( m_RegisteredMovingImage.GetPointer(), "Save registered image file...", "*.mh?", "", 0 );
/*
  const char * filename = fl_file_chooser("Save registered image file...","*.mh?","");

  if( filename )
    {
    ImageWriterType::Pointer writer = 
      ImageWriterType::New();
    writer->SetFileName( filename );
    writer->SetInput( m_RegisteredMovingImage.GetPointer() );
    try
      {
      writer->Update();
      }
    catch( itk::ExceptionObject &e )
      {
      std::cout<<e<<std::endl;
      }
    }*/
  }

/////////////////////////////////////////////////
// Image view functions
/////////////////////////////////////////////////

void
guiMainImplementation
::Show()
  { 
  tkWindow->show();
  tkResultImageViewer->show();
  tkFixedImageViewer->show();
  tkMovingImageViewer->show();

  tkResultImageViewer->update();
  tkFixedImageViewer->update();
  tkMovingImageViewer->update();

  tkResultImageViewer->deactivate();
  tkFixedImageViewer->deactivate();
  tkMovingImageViewer->deactivate();

  tkFixedImageFile->value("no image loaded...");
  tkMovingImageFile->value("no image loaded...");
  }

void
guiMainImplementation
::SetViewAxis(unsigned int axis)
  {
  m_ViewAxis = axis;

  tkMovingImageViewer->orientation(axis);
  tkFixedImageViewer->orientation(axis);
  tkResultImageViewer->orientation(axis);

  unsigned int mid = (m_FixedImageSize[axis]-1)/2;
  tkSliceValuator->value(mid);
  tkSliceValuator->maximum(m_FixedImageSize[axis]-1);
  tkFixedSliceValuator->value(mid);
  tkFixedSliceValuator->maximum(m_FixedImageSize[axis]-1);
  tkResultImageViewer->sliceNum(mid);
  tkFixedImageViewer->sliceNum(mid);

  unsigned int mMid = mid;
  if(mid >= m_MovingImageSize[axis]-1)
    {
    mMid = m_MovingImageSize[axis]-1;
    }
  tkMovingSliceValuator->value(mMid);
  tkMovingSliceValuator->maximum(m_MovingImageSize[axis]-1);
  tkMovingImageViewer->sliceNum(mMid);

  tkFixedImageViewer->update();
  tkMovingImageViewer->update();
  tkResultImageViewer->update();

  tkFixedImageViewer->redraw();
  tkMovingImageViewer->redraw();
  tkResultImageViewer->redraw();
  }

void 
guiMainImplementation
::SelectImageSet(unsigned int i)
  {
  if (i == 0)
    {
    if ( m_MovingImage != 0 )
      {
      tkResultImageViewer->SetSecondInputImage(m_MovingImage);
      }
    }
  else if ( i == 1)
    {
    if ( m_LandmarkRegisteredMovingImage != 0 )
      {
      tkResultImageViewer->SetSecondInputImage(m_LandmarkRegisteredMovingImage);
      }
    }
  else if ( i == 2 )
    {
    if ( m_RegisteredMovingImage != 0 )
      {
      tkResultImageViewer->SetSecondInputImage(m_RegisteredMovingImage);
      }
    }

  //this->ShowMovingLandmarks( i );

  tkResultImageViewer->update();
  tkResultImageViewer->redraw();  
  tkResultImageViewer->activate();  
  }

void
guiMainImplementation
::UpdateSliceNumber()
  {
  if ( tkFixedImageViewer->GetInputImage() )
    {
    tkResultImageViewer->sliceNum((unsigned int)tkSliceValuator->value());
    tkResultImageViewer->update();
    }
  }

void
guiMainImplementation
::UpdateFixedSliceNumber()
  {
  if( tkFixedImageViewer->GetInputImage() )
    {
    tkFixedImageViewer->sliceNum((unsigned int)tkFixedSliceValuator->value());
    tkFixedImageViewer->update();
    }
  }

  
void
guiMainImplementation
::UpdateMovingSliceNumber()
  {
  if ( tkMovingImageViewer->GetInputImage())
    {
    tkMovingImageViewer->sliceNum((unsigned int)tkMovingSliceValuator->value());
    tkMovingImageViewer->update();
    }
  }

/////////////////////////////////////////////////
// Application status functions
/////////////////////////////////////////////////

void
guiMainImplementation
::ChangeStatusDisplay(const char* message)
  {
  tkStatusDisplay->value(message);
  std::cout << message << std::endl;
  Fl::check();
  }

/////////////////////////////////////////////////
// Landmark related functions
/////////////////////////////////////////////////

void 
guiMainImplementation
::UpdateLandmark( Fl_Group* parent, unsigned int id )
  {
  Fl_Group* grp = parent;
  SliceViewType* view;
  Fl_Button* button;
  if (grp == tkFixedImageLandmarkGroup)
    {
    view = tkFixedImageViewer;

    if ( id == 0 )
      {
      button = tkFixedImageLandmark1Button;
      }
    else if ( id == 1 )
      {
      button = tkFixedImageLandmark2Button;
      }
    else if ( id == 2 )
      {
      button = tkFixedImageLandmark3Button;
      }
    else if ( id == 3 )
      {
      button = tkFixedImageLandmark4Button;
      }
    }
  else
    {
    view = tkMovingImageViewer;
    if ( id == 0 )
      {
      button = tkMovingImageLandmark1Button;
      }
    else if ( id == 1 )
      {
      button = tkMovingImageLandmark2Button;
      }
    else if ( id == 2 )
      {
      button = tkMovingImageLandmark3Button;
      }
    else if ( id == 3 )
      {
      button = tkMovingImageLandmark4Button;
      }
    }

  if (!(view->IsLandmarkAvailable( id ) ))
    {
    button->value(1);
    view->AddLandmark(id, id);
    }
  else
    {
    view->DeleteLandmark(id);
    button->value(0);
    }
  }

void
guiMainImplementation
::JumpToLandmark(bool moving, unsigned int id )
  {
  LandmarkPointType landmark;
  IndexType index;
  if( moving )
    {
    if( tkMovingImageViewer->IsLandmarkAvailable( id ) )
      {
      tkMovingImageViewer->GetLandmark(id, landmark);
      m_MovingImage->TransformPhysicalPointToIndex(landmark.GetPosition(),
                                                   index);
      tkMovingSliceValuator->value(index[m_ViewAxis]);
      if( m_FixedImage->TransformPhysicalPointToIndex(landmark.GetPosition(),
                                                  index))
        {
        tkFixedSliceValuator->value(index[m_ViewAxis]);
        tkSliceValuator->value(index[m_ViewAxis]);
        }
      }
    }
  else
    {
    if( tkFixedImageViewer->IsLandmarkAvailable( id ) )
      {
      tkFixedImageViewer->GetLandmark(id, landmark);
      m_FixedImage->TransformPhysicalPointToIndex(landmark.GetPosition(),
                                                  index);
      tkFixedSliceValuator->value(index[m_ViewAxis]);
      tkSliceValuator->value(index[m_ViewAxis]);
      if( m_MovingImage->TransformPhysicalPointToIndex(landmark.GetPosition(),
                                                       index))
        {
        tkMovingSliceValuator->value(index[m_ViewAxis]);
        }
      }
    }
  
  this->UpdateSliceNumber();
  this->UpdateFixedSliceNumber();
  this->UpdateMovingSliceNumber();
  }

void 
guiMainImplementation
::ClearLandmarks(Fl_Group* parent)
  {
  SliceViewType* view;
  if (parent == tkFixedImageLandmarkGroup)
    {
    view = tkFixedImageViewer;
    }
  else
    {
    view = tkMovingImageViewer;
    }
  
  for ( int i = 0; i < parent->children(); ++i )
    {
    Fl_Button* button = (Fl_Button*)(parent->child(i));
  
    if ( view->IsLandmarkAvailable(i) )
      {
      view->DeleteLandmark(i);
      }

    if ( button->value() )
      {
      button->value(0);
      }
    }
  
  if ( view == tkMovingImageViewer )
    {
    m_MovingLandmarkSpatialObject->
      SetPoints( tkMovingImageViewer->GetLandmarkList() );
    }
  if ( view == tkFixedImageViewer )
    {
    m_FixedLandmarkSpatialObject->
      SetPoints( tkFixedImageViewer->GetLandmarkList() );
    }
  }

void
guiMainImplementation
::LoadLandmarks( bool moving )
  {
  const char * filename = 
    fl_file_chooser("Choose a file for landmarks...","*.lnd","");

  if( filename )
    {
    typedef itk::SpatialObjectReader<> ReaderType;
      
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFullFileName(filename);
    reader->Update();
    ReaderType::GroupPointer group = reader->GetGroup();
    ReaderType::GroupType::ChildrenListType* children = group->GetChildren();
    LandmarkSpatialObjectType::Pointer landmarks = 
      dynamic_cast< LandmarkSpatialObjectType* >((*(children->begin())).GetPointer());

    if (moving)
      {
      m_MovingLandmarkSpatialObject->SetPoints( landmarks->GetPoints() );
      for(int i=0; i < tkMovingImageLandmarkGroup->children(); i++)
        {
        ((Fl_Button *)tkMovingImageLandmarkGroup->child(i))->activate();
        }
      if ( m_MovingLandmarkSpatialObject->GetPoints().size() > 0 )
        {
        tkMovingImageViewer->SetLandmarkList( & m_MovingLandmarkSpatialObject->GetPoints());
        }
      tkMovingImageLandmark1Button->value(1);
      tkMovingImageLandmark2Button->value(1);
      tkMovingImageLandmark3Button->value(1);
      tkMovingImageLandmark4Button->value(1);
      }
    else
      {
      m_FixedLandmarkSpatialObject->SetPoints( landmarks->GetPoints() );
      for(int i=0; i < tkFixedImageLandmarkGroup->children(); i++)
        {
        ((Fl_Button*)tkFixedImageLandmarkGroup->child(i))->activate();
        }   
      if ( m_FixedLandmarkSpatialObject->GetPoints().size() > 0 )
        {
        tkFixedImageViewer->SetLandmarkList( & m_FixedLandmarkSpatialObject->GetPoints());
        }
      tkFixedImageLandmark1Button->value(1);
      tkFixedImageLandmark2Button->value(1);
      tkFixedImageLandmark3Button->value(1);
      tkFixedImageLandmark4Button->value(1);
      }     
    } 
  }

void
guiMainImplementation
::SaveLandmarks( bool moving )
  {
  const char * filename = 
    fl_file_chooser("Choose a file for landmarks...","*.lnd","");

  if( !filename )
    {
    return;
    }
      
  typedef itk::LandmarkSpatialObject< 3 > ObjectType;
  typedef itk::SpatialObjectWriter< > WriterType;
  typedef itk::GroupSpatialObject< > GroupType;
  
  WriterType::Pointer writer = WriterType::New();
  writer->SetFullFileName(filename);
  ObjectType::Pointer landmarks = ObjectType::New();

  if (moving)
    {
    landmarks->SetPoints(tkMovingImageViewer->GetLandmarkList());
    }
  else
    {
    landmarks->SetPoints(tkFixedImageViewer->GetLandmarkList());
    }
  
  writer->SetInput(landmarks);
  writer->Update();
  }

void
guiMainImplementation
::TransformLandmarks(LandmarkPointListType * source,
                     LandmarkPointListType * target,
                     AffineTransformType * transform)
  {
  target->assign(source->begin(), source->end());
  LandmarkPointListType::iterator iter = target->begin();
  while ( iter != target->end() )
    {
    iter->SetPosition ( transform->TransformPoint( 
                                             iter->GetPosition()));
    ++iter;
    }
  }

/////////////////////////////////////////////////
// Region of interest related functions
/////////////////////////////////////////////////
void
guiMainImplementation
::ShowRegionOfInterestWindow()
  {
  tkMovingImageViewer->HideLandmarks();
  tkMovingImageViewer->ShowRegionOfInterest();

  tkROIViewAxisX->value(tkViewAxisX->value());
  tkROIViewAxisY->value(tkViewAxisY->value());
  tkROIViewAxisZ->value(tkViewAxisZ->value());

  tkRegionOfInterestWindow->show();
  }

void 
guiMainImplementation
::ApplyRegionOfInterest()
  {
  tkMovingImageViewer->ApplyRegionOfInterest();
  tkMovingImageViewer->HideRegionOfInterest();
  tkMovingImageViewer->ShowLandmarks();
  tkRegionOfInterestWindow->hide();
  }

void 
guiMainImplementation
::CancelRegionOfInterest()
  {
  tkMovingImageViewer->HideRegionOfInterest();
  tkMovingImageViewer->ShowLandmarks();
  tkRegionOfInterestWindow->hide();
  }

void 
guiMainImplementation
::MoveRegionOfInterest(unsigned int direction)
  {
  unsigned int axis;
  int step;
  if ( tkROIViewAxisX->value() )
    {
    if ( direction == 0 || direction == 1)
      {
      axis = 1;
      }
    else
      {
      axis = 2;
      }
    }

  if ( tkROIViewAxisY->value() )
    {
    if ( direction == 0 || direction == 1)
      {
      axis = 0;
      }
    else
      {
      axis = 2;
      }
    }

  if ( tkROIViewAxisZ->value() )
    {
    if ( direction == 0 || direction == 1)
      {
      axis = 0;
      }
    else
      {
      axis = 1;
      }
    }

  if ( direction == 1 || direction == 2 )
    {
    step = (int) tkROIStep->value();
    }
  else
    {
    step = -1 * (int) tkROIStep->value();
    }
  
  tkMovingImageViewer->MoveRegionOfInterest(axis, step);
  }

void 
guiMainImplementation
::ResizeRegionOfInterest(unsigned int direction)
  {
  unsigned int axis;
  int step;
  if ( tkROIViewAxisX->value() )
    {
    if ( direction == 0 || direction == 1)
      {
      axis = 1;
      }
    else
      {
      axis = 2;
      }
    }

  if ( tkROIViewAxisY->value() )
    {
    if ( direction == 0 || direction == 1)
      {
      axis = 0;
      }
    else
      {
      axis = 2;
      }
    }

  if ( tkROIViewAxisZ->value() )
    {
    if ( direction == 0 || direction == 1)
      {
      axis = 0;
      }
    else
      {
      axis = 1;
      }
    }

  if ( direction == 0 || direction == 2 )
    {
    step = -1 * (int) tkROIStep->value();
    }
  else
    {
    step = (int) tkROIStep->value();
    }
  
  tkMovingImageViewer->ResizeRegionOfInterest(axis, step);
  }

/////////////////////////////////////////////////
// Advanced option related functions
/////////////////////////////////////////////////

void
guiMainImplementation
::ShowAdvancedOptions()
  {
  tkAdvancedOptionsWindow->show();

  // set landmark advance option values
  tkLandmarkRotationScale->value(m_ImageRegistrationApp->GetLandmarkScales()[0]);
  tkLandmarkTranslationScale->value(m_ImageRegistrationApp->GetLandmarkScales()[3]);
  tkLandmarkNumberOfIterations->value(m_ImageRegistrationApp->GetLandmarkNumberOfIterations());

  // set rigid advance option values
  tkRigidRotationScale->value(m_ImageRegistrationApp->GetRigidScales()[0]);
  tkRigidTranslationScale->value(m_ImageRegistrationApp->GetRigidScales()[3]);
  tkRigidNumberOfIterations->value(m_ImageRegistrationApp->GetRigidNumberOfIterations());
  tkRigidNumberOfSpatialSamples->value
    (m_ImageRegistrationApp->GetRigidNumberOfSpatialSamples());
  
  tkRigidUseLargestRegion->value(m_RigidUseLargestRegion);
  tkRigidUseUserRegion->value(m_RigidUseUserRegion);
  tkRigidUseLandmarkRegion->value(m_RigidUseLandmarkRegion);
  tkRigidRegionScale->value(m_RigidRegionScale);

  // set affine advance option values
  tkAffineRotationScale->value(m_ImageRegistrationApp->GetAffineScales()[0]);
  tkAffineTranslationScale->value(m_ImageRegistrationApp->GetAffineScales()[3]);
  tkAffineScaleScale->value(m_ImageRegistrationApp->GetAffineScales()[6]);
  tkAffineSkewScale->value(m_ImageRegistrationApp->GetAffineScales()[9]);
  tkAffineNumberOfIterations->value(m_ImageRegistrationApp->GetAffineNumberOfIterations());
  tkAffineNumberOfSpatialSamples->value
    (m_ImageRegistrationApp->GetAffineNumberOfSpatialSamples());

  tkAffineUseLargestRegion->value(m_AffineUseLargestRegion);
  tkAffineUseUserRegion->value(m_AffineUseUserRegion);
  tkAffineUseLandmarkRegion->value(m_AffineUseLandmarkRegion);
  tkAffineRegionScale->value(m_AffineRegionScale);
  }

void
guiMainImplementation
::ApplyAdvancedOptions()
  {

  // set landmark advance option values
  ImageRegistrationAppType::RigidScalesType landmarkScales;
  landmarkScales.set_size(6);
  landmarkScales[0] = tkLandmarkRotationScale->value();
  landmarkScales[1] = tkLandmarkRotationScale->value();
  landmarkScales[2] = tkLandmarkRotationScale->value();
  landmarkScales[3] = tkLandmarkTranslationScale->value();
  landmarkScales[4] = tkLandmarkTranslationScale->value();
  landmarkScales[5] = tkLandmarkTranslationScale->value();
  m_ImageRegistrationApp->SetLandmarkScales(landmarkScales);
  m_ImageRegistrationApp->SetLandmarkNumberOfIterations(
               (unsigned int) tkLandmarkNumberOfIterations->value());

  // set rigid advance option values
  ImageRegistrationAppType::RigidScalesType rigidScales;
  rigidScales.set_size(6);
  rigidScales[0] = tkRigidRotationScale->value();
  rigidScales[1] = tkRigidRotationScale->value();
  rigidScales[2] = tkRigidRotationScale->value();
  rigidScales[3] = tkRigidTranslationScale->value();
  rigidScales[4] = tkRigidTranslationScale->value();
  rigidScales[5] = tkRigidTranslationScale->value();
  m_ImageRegistrationApp->SetRigidScales(rigidScales);
  m_ImageRegistrationApp->SetRigidNumberOfIterations(
               (unsigned int) tkRigidNumberOfIterations->value());
  m_ImageRegistrationApp->SetRigidNumberOfSpatialSamples
    ((unsigned int) tkRigidNumberOfSpatialSamples->value());

  m_RigidUseLargestRegion = tkRigidUseLargestRegion->value();
  m_RigidUseUserRegion = tkRigidUseUserRegion->value();
  m_RigidUseLandmarkRegion = tkRigidUseLandmarkRegion->value();
  m_RigidRegionScale = tkRigidRegionScale->value();

  // set affine advance option values
  ImageRegistrationAppType::AffineScalesType affineScales;
  affineScales.set_size(15);
  affineScales[0] = tkAffineRotationScale->value();
  affineScales[1] = tkAffineRotationScale->value();
  affineScales[2] = tkAffineRotationScale->value();
  affineScales[3] = tkAffineTranslationScale->value();
  affineScales[4] = tkAffineTranslationScale->value();
  affineScales[5] = tkAffineTranslationScale->value();
  affineScales[6] = tkAffineScaleScale->value();
  affineScales[7] = tkAffineScaleScale->value();
  affineScales[8] = tkAffineScaleScale->value();
  affineScales[9] = tkAffineSkewScale->value();
  affineScales[10] = tkAffineSkewScale->value();
  affineScales[11] = tkAffineSkewScale->value();
  affineScales[12] = tkAffineSkewScale->value();
  affineScales[13] = tkAffineSkewScale->value();
  affineScales[14] = tkAffineSkewScale->value();
  m_ImageRegistrationApp->SetAffineScales(affineScales);
  m_ImageRegistrationApp->SetAffineNumberOfIterations(
               (unsigned int) tkAffineNumberOfIterations->value());
  m_ImageRegistrationApp->SetAffineNumberOfSpatialSamples
    ((unsigned int) tkAffineNumberOfSpatialSamples->value());

  m_AffineUseLargestRegion = tkAffineUseLargestRegion->value();
  m_AffineUseUserRegion = tkAffineUseUserRegion->value();
  m_AffineUseLandmarkRegion = tkAffineUseLandmarkRegion->value();
  m_AffineRegionScale = tkAffineRegionScale->value();

  tkAdvancedOptionsWindow->hide();
  }

void
guiMainImplementation
::SaveOptions()
  {
  const char * filename = 
    fl_file_chooser("Choose a file for options","*.ops","");

  std::map< std::string, double > options;

  if( !filename )
    {
    std::cout << "ERROR: cannot find the file." << std::endl;
    return;
    }
  
  std::ofstream outputFile(filename);
  if ( !outputFile )
    {
    std::cout << "ERROR: cannot open the option file" << std::endl;
    }
  
  // landmark
  ImageRegistrationAppType::RigidScalesType landmarkScales = 
    m_ImageRegistrationApp->GetLandmarkScales();
  for ( unsigned int i = 0; i < landmarkScales.Size(); ++i )
    {
    outputFile << "LandmarkScales_" << i << " " 
               << landmarkScales[i] << std::endl;
    }
  
  outputFile << "LandmarkNumberOfIterations " 
             << m_ImageRegistrationApp->GetLandmarkNumberOfIterations() << std::endl;

  // rigid
  ImageRegistrationAppType::RigidScalesType rigidScales = 
    m_ImageRegistrationApp->GetRigidScales();
  for ( unsigned int i = 0; i < rigidScales.Size(); ++i )
    {
    outputFile << "RigidScales_" << i << " " 
               << rigidScales[i] << std::endl;
    }
  
  outputFile << "RigidNumberOfIterations " 
             << m_ImageRegistrationApp->GetRigidNumberOfIterations() << std::endl;
  
  outputFile << "RigidNumberOfSpatialSamples " 
             << m_ImageRegistrationApp->GetRigidNumberOfSpatialSamples() << std::endl;
  
  outputFile << "RigidUseLargestRegion " << m_RigidUseLargestRegion 
             << std::endl;
  outputFile << "RigidUseUserRegion " << m_RigidUseUserRegion 
             << std::endl;
  outputFile << "RigidUserLandmarkRegion " << m_RigidUseLandmarkRegion
             << std::endl;
  outputFile << "RigidRegionScale " << m_RigidRegionScale 
             << std::endl;

  // set affine advance option values
  ImageRegistrationAppType::AffineScalesType affineScales = 
    m_ImageRegistrationApp->GetAffineScales();
  for ( unsigned int i = 0; i < affineScales.Size(); ++i )
    {
    outputFile << "AffineScales_" << i << " " 
               << affineScales[i] << std::endl;
    }
  
  outputFile << "AffineNumberOfIterations " 
             << m_ImageRegistrationApp->GetAffineNumberOfIterations() << std::endl;
  
  outputFile << "AffineNumberOfSpatialSamples " 
             << m_ImageRegistrationApp->GetAffineNumberOfSpatialSamples() << std::endl;

  outputFile << "AffineUseLargestRegion " << m_AffineUseLargestRegion 
             << std::endl;
  outputFile << "AffineUseUserRegion " << m_AffineUseUserRegion 
             << std::endl;
  outputFile << "AffineUseLandmarkRegion " << m_AffineUseLandmarkRegion
             << std::endl;
  outputFile << "AffineRegionScale " << m_AffineRegionScale 
             << std::endl;
  }

void
guiMainImplementation
::LoadOptions()
  {
  const char * filename = 
    fl_file_chooser("Choose a file for options","*.ops","");

  if( !filename )
    {
    std::cout << "ERROR: cannot find the file" << std::endl;
    return;
    }

  std::ifstream inputFile(filename);
  if ( !inputFile )
    {
    std::cout << "ERROR: cannot open the option file" << std::endl;
    }


  typedef std::map< std::string, double > OptionsMapType;
  OptionsMapType options;

  std::string key = "";
  double value;
  // parsing option file
  while ( !inputFile.eof() )
    {
    inputFile >> key;
    inputFile >> value;
    options[key] = value;
    }

  // set options 

  // landmark
  ImageRegistrationAppType::RigidScalesType landmarkScales = 
    m_ImageRegistrationApp->GetLandmarkScales();
  for ( unsigned int i = 0; i < landmarkScales.Size(); ++i )
    {
    itk::OStringStream temp;
    temp <<  "LandmarkScales_" << i;
    landmarkScales[i] = options[temp.str()];
    }
  m_ImageRegistrationApp->SetLandmarkScales(landmarkScales);
  m_ImageRegistrationApp->SetLandmarkNumberOfIterations((unsigned int)options["LandmarkNumberOfIterations"]);

  // rigid
  ImageRegistrationAppType::RigidScalesType rigidScales = m_ImageRegistrationApp->GetRigidScales();
  for ( unsigned int i = 0; i < rigidScales.Size(); ++i )
    {
    itk::OStringStream temp;
    temp <<  "RigidScales_" << i;
    rigidScales[i] = options[temp.str()];
    }
  m_ImageRegistrationApp->SetRigidScales(rigidScales);
  m_ImageRegistrationApp->SetRigidNumberOfIterations((unsigned int)options["RigidNumberOfIterations"]);
  m_ImageRegistrationApp->SetRigidNumberOfSpatialSamples((unsigned int)options["RigidNumberOfSpatialSamples"]);

  m_RigidUseLargestRegion = (bool) options["RigidUseLargestRegion"];
  m_RigidUseUserRegion = (bool) options["RigidUseUserRegion"];
  m_RigidUseLandmarkRegion = (bool) options["RigidUseLandmarkRegion"];
  m_RigidRegionScale = options["RigidRegionScale"];
  
  // affine
  ImageRegistrationAppType::AffineScalesType affineScales = 
    m_ImageRegistrationApp->GetAffineScales();
  for ( unsigned int i = 0; i < affineScales.Size(); ++i )
    {
    itk::OStringStream temp;
    temp << "AffineScales_" << i;
    affineScales[i] = options[temp.str()];
    }
  m_ImageRegistrationApp->SetAffineScales(affineScales);
  m_ImageRegistrationApp->SetAffineNumberOfIterations((unsigned int)options["AffineNumberOfIterations"]);
  m_ImageRegistrationApp->SetAffineNumberOfSpatialSamples((unsigned int)options["AffineNumberOfSpatialSamples"]);

  m_AffineUseLargestRegion = (bool) options["AffineUseLargestRegion"];
  m_AffineUseUserRegion = (bool) options["AffineUseUserRegion"];
  m_AffineUseLandmarkRegion = (bool) options["AffineUseLandmarkRegion"];
  m_AffineRegionScale = options["AffineRegionScale"];
  }

/////////////////////////////////////////////////
// Registartion related functions
/////////////////////////////////////////////////

void
guiMainImplementation
::SaveTransform()
  {
  const char * filename = 
    fl_file_chooser("Choose a file for transform...","*.tfm","");

  if( filename )
    {
    typedef itk::SpatialObjectWriter< > WriterType;
    typedef itk::GroupSpatialObject< > GroupType;
    typedef itk::FixedCenterOfRotationAffineTransform< double, 3 > 
      TransformType;

    WriterType::Pointer writer = WriterType::New();
    writer->SetFullFileName(filename);
      
    GroupType::Pointer grp = GroupType::New();
    TransformType::Pointer transform = TransformType::New();
    transform->SetParameters( m_ImageRegistrationApp->GetFinalTransform()->GetParameters() );
    grp->SetObjectToParentTransform( transform.GetPointer() );
    writer->SetInput( grp );
    writer->Update();
    }
  }

guiMainImplementation::ImagePointer
guiMainImplementation
::ResampleUsingTransform(AffineTransformType * finalTransform, 
                         ImageType* input, ImageType* output)
  {
  // resample the moving image 
  InterpolatorType::Pointer interpolator = 
    InterpolatorType::New();
  interpolator->SetInputImage(input);
  
  ResampleImageFilterType::Pointer resample = 
    ResampleImageFilterType::New();
  resample->SetInput(input);
  resample->SetInterpolator(interpolator.GetPointer());
  resample->SetSize(output->GetLargestPossibleRegion().GetSize());
  resample->SetOutputOrigin(output->GetOrigin());
  resample->SetOutputSpacing(output->GetSpacing());
  resample->SetTransform(finalTransform->Inverse());
  resample->Update();
  
  return resample->GetOutput();

  }

void
guiMainImplementation
::Register()
  {
  this->ChangeStatusDisplay("Registration begins.");
  fl_cursor(Fl_Cursor(FL_CURSOR_WAIT), (Fl_Color) 56, (Fl_Color) 255);
  
  m_ImageRegistrationApp->SetFixedImage(m_FixedImage.GetPointer());
  m_ImageRegistrationApp->SetMovingImage(m_MovingImage.GetPointer());
  typedef itk::VectorContainer<int, Point< double, 3 > > PointSetType;
  PointSetType::Pointer fixedLandmarks = PointSetType::New();
  PointSetType::Pointer movingLandmarks = PointSetType::New();

  m_FixedLandmarkSpatialObject->SetPoints( tkFixedImageViewer->GetLandmarkList() );
  m_MovingLandmarkSpatialObject->SetPoints( tkMovingImageViewer->GetLandmarkList() );
  if( m_FixedLandmarkSpatialObject->GetPoints().size() == 4 &&
      m_MovingLandmarkSpatialObject->GetPoints().size() == 4)
    {
    this->ChangeStatusDisplay("Register using landmarks");

    fixedLandmarks->Reserve
                    (m_FixedLandmarkSpatialObject->GetPoints().size());
    movingLandmarks->Reserve
                     (m_MovingLandmarkSpatialObject->GetPoints().size());
    LandmarkPointListType::iterator fixedIter = 
                           m_FixedLandmarkSpatialObject->GetPoints().begin();
    LandmarkPointListType::iterator movingIter = 
                           m_MovingLandmarkSpatialObject->GetPoints().begin();
    unsigned int id = 0;
    while ( movingIter != m_MovingLandmarkSpatialObject->GetPoints().end() )
      {
      fixedLandmarks->InsertElement(id, (*fixedIter).GetPosition());
      movingLandmarks->InsertElement(id, (*movingIter).GetPosition());
      ++fixedIter;
      ++movingIter;
      ++id;
      }
    m_ImageRegistrationApp->RegisterUsingLandmarks ( fixedLandmarks.GetPointer(),
                                    movingLandmarks.GetPointer() );

    this->ChangeStatusDisplay("Transforming the landmarks...");

    this->TransformLandmarks( 
          & m_MovingLandmarkSpatialObject->GetPoints(),
          & m_LandmarkRegisteredMovingLandmarkSpatialObject->GetPoints(),
          m_ImageRegistrationApp->GetLandmarkAffineTransform() );
    this->ChangeStatusDisplay("Resampling registered moving image...");
    m_LandmarkRegisteredMovingImage = 
      this->ResampleUsingTransform(m_ImageRegistrationApp->GetLandmarkAffineTransform(), 
                                   m_MovingImage.GetPointer(), m_FixedImage.GetPointer());
    
    tkLandmarkRegisteredView->activate();
    }
  else
    {
    tkLandmarkRegisteredView->deactivate();
    }
  
  this->ChangeStatusDisplay("Registering using the rigid method...");

  RegionType region;
  region = m_MovingImage->GetLargestPossibleRegion();

  if ( tkRigidUseUserRegion->value() == 1
        && tkMovingImageViewer->IsRegionOfInterestAvailable() )
    {
    std::cout << "Rigid registration using user roi." << std::endl;
    region = tkMovingImageViewer->GetRegionOfInterest();
    }
  else if ( tkRigidUseLandmarkRegion->value() == 1 
            && tkMovingImageViewer->GetNumberOfLandmarks() == 4 
            && tkFixedImageViewer->GetNumberOfLandmarks() == 4)
    {
    std::cout << "Rigid registration using landmark roi." << std::endl;
    region = tkMovingImageViewer->ComputeLandmarkRegion
      (tkRigidRegionScale->value());
    }

    m_ImageRegistrationApp->SetFixedImageRegion(region); // actually moving image region
    m_ImageRegistrationApp->RegisterUsingRigidMethod();

  if( tkRegistrationMethodChoice->value() > 0 ) // also do affine
    {
    this->ChangeStatusDisplay("Registering using the affine method...");
    if ( tkAffineUseUserRegion->value() == 1
          && tkMovingImageViewer->IsRegionOfInterestAvailable() )
      {
      std::cout << "Affine registration using user roi." << std::endl;
      region = tkMovingImageViewer->GetRegionOfInterest();
      }
    else if ( tkAffineUseLandmarkRegion->value() == 1 
              && tkFixedImageViewer->GetNumberOfLandmarks() == 4 
              && tkMovingImageViewer->GetNumberOfLandmarks() == 4)
      {
      std::cout << "Affine registration using landmark roi." << std::endl;
      region = tkMovingImageViewer->ComputeLandmarkRegion
        (tkAffineRegionScale->value());
      }
    m_ImageRegistrationApp->SetFixedImageRegion(region); // actually moving image region
    m_ImageRegistrationApp->RegisterUsingAffineMethod();
    }
  
  this->TransformLandmarks( & m_MovingLandmarkSpatialObject->GetPoints(),
                        & m_RegisteredMovingLandmarkSpatialObject->GetPoints(),
                        m_ImageRegistrationApp->GetFinalTransform() );
  
  this->ChangeStatusDisplay("Resampling the moving image...");
  m_RegisteredMovingImage = 
    this->ResampleUsingTransform(m_ImageRegistrationApp->GetFinalTransform(), 
                                 m_MovingImage.GetPointer(), m_FixedImage.GetPointer());

  tkRegisteredView->activate();
  
  tkDisplayMovingImageChoice->value(2);
  this->SelectImageSet(2);
  
  fl_cursor(Fl_Cursor(FL_CURSOR_DEFAULT), (Fl_Color) 56, (Fl_Color) 255);
  this->ChangeStatusDisplay("Registration done");
  }

/////////////////////////////////////////////////
// Help related functions
/////////////////////////////////////////////////

void
guiMainImplementation
::ShowHelp( const char * file )
  {
  tkHelpWindow->show();
  tkHelpView->load(file);
  }
 


