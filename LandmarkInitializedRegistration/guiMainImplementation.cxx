#include "guiMainImplementation.h"

guiMainImplementation * ptr;
 
guiMainImplementation
::guiMainImplementation()
{
  m_FixedImageFileName = "" ;
  m_MovingImageFileName = "" ;

  m_ViewNonRegisteredImage = RGBImageType::New();
  m_ViewLandmarkRegisteredImage = RGBImageType::New();
  m_ViewRegisteredImage = RGBImageType::New();
  
  m_FixedImage = ImageType::New();
  m_ViewFixedImage = ImageType::New() ;
  
  m_MovingImage = ImageType::New();
  m_ViewMovingImage = ImageType::New() ;
  m_ViewLandmarkRegisteredMovingImage = ImageType::New() ;
  m_ViewRegisteredMovingImage = ImageType::New() ;
  
  m_App = new RegistrationAppType() ;
  
  ptr = this;
  
  this->Initialize();
}

guiMainImplementation
::~guiMainImplementation()
{
  delete m_App ;
  Quit();
}

/////////////////////////////////////////////////
// Initializations & Terminations
/////////////////////////////////////////////////
void
guiMainImplementation
::Initialize()
{
  m_Scales.resize(7);
  m_Scales.Fill(100);
  m_Scales[0] = 1;
  m_Scales[1] = 1;
  m_Scales[2] = 1;
  m_Scales[3] = 10000000; // no scale
}

void
guiMainImplementation
::Quit()
{
  tkAdvancedOptionsWindow->hide();
//  tkRegionOptionsWindow->hide();
  tkWindow->hide();
}

/////////////////////////////////////////////////
// Image File IO functions
/////////////////////////////////////////////////
void
guiMainImplementation
::SelectFixedImage()
{
  const char * filename = 
    fl_file_chooser("Choose a file for landmarks...","*.mh?","");

  if ( !filename )
    {
    return ;
    }

  std::string fileName = filename ;

  if ( m_FixedImageFileName !=  fileName )
    {
    this->HandleInputImageChange(fileName, true) ;
    }

}

void
guiMainImplementation
::SelectMovingImage()
{
  const char * filename = 
    fl_file_chooser("Choose a file for landmarks...","*.mh?","");

  if ( !filename )
    {
    return ;
    }

  std::string fileName = filename ;

  if ( m_MovingImageFileName !=  fileName )
    {
    this->HandleInputImageChange(fileName, false) ;
    }

}

void 
guiMainImplementation
::HandleInputImageChange(std::string & fileName, bool isFixedImage)
{
  ImageType::Pointer image;
  
  fl_cursor(Fl_Cursor(FL_CURSOR_WAIT), (Fl_Color) 56, (Fl_Color) 255) ;
  image = LoadImage(fileName.c_str()) ;
  
  if ( !image )
    {
    return ;
    }
  

  if ( isFixedImage )
    {
    if ( m_FixedImageFileName == fileName )
      {
      fl_cursor(Fl_Cursor(FL_CURSOR_DEFAULT), 
                (Fl_Color) 56, (Fl_Color) 255) ;
      return ;
      }
    m_FixedImageFileName = fileName ;

    tkFixedImageFile->value(m_LastLoadedImagePath.c_str());
    m_FixedImage = image.GetPointer();
    m_FixedImageLoaded = true ;
    }
  else
    {
    if ( m_MovingImageFileName == fileName )
      {
      fl_cursor(Fl_Cursor(FL_CURSOR_DEFAULT), 
                (Fl_Color) 56, (Fl_Color) 255) ;
      return ;
      }
    m_MovingImageFileName = fileName ;

    tkMovingImageFile->value(m_LastLoadedImagePath.c_str());
    m_MovingImage = image.GetPointer();
    m_MovingImageLoaded = true ;
      
    } // end of if-else
  

  if ( m_FixedImageFileName != "" && m_MovingImageFileName != "" )
    {
    this->CalculateMaximumSize() ;
      
    if ( m_FixedImageResamplingNeeded )
      {
      this->ChangeStatusDisplay("Resampling fixed image to match resolution") ;
      m_ViewFixedImage = this->ResampleUsingResolution(m_FixedImage.GetPointer()) ;
      }
    else
      {
      m_ViewFixedImage = m_FixedImage ;
      }
      
    if ( m_MovingImageResamplingNeeded )
      {
      this->ChangeStatusDisplay("Resampling moving image to match resolution") ;
      m_ViewMovingImage = 
        this->ResampleUsingResolution(m_MovingImage.GetPointer()) ;
      }
    else
      {
      m_ViewMovingImage = m_MovingImage.GetPointer() ;
      }
      
    ModifySliceViewInputImage(tkFixedImageViewer, 
                              m_ViewFixedImage.GetPointer());

    tkFixedImageLandmarkGroup->activate();
    Fl_Group* grp = tkFixedImageLandmarkGroup ;
    for(unsigned int i=0; i < grp->children(); i++)
      {
      ((Fl_Button *)grp->child(i))->clear();
      ((Fl_Button *)grp->child(i))->activate();
      }        

    tkMovingImageLandmarkGroup->activate();
    grp = tkMovingImageLandmarkGroup ;
    for(unsigned int i=0; i < grp->children(); i++)
      {
      ((Fl_Button *)grp->child(i))->clear();
      ((Fl_Button *)grp->child(i))->activate();
      }        

    m_ViewNonRegisteredImage = this->MakeRGBImage( m_ViewFixedImage,
                                                   m_ViewMovingImage) ;

    tkLandmarkRegisteredView->deactivate() ;
    tkRegisteredView->deactivate() ;
    this->SelectImageSet(0) ;
    this->SetViewAxis(2) ;
    tkViewAxisZ->setonly() ;
    tkSliceValuator->maximum(m_MaximumSize[2]-1);
    }
  fl_cursor(Fl_Cursor(FL_CURSOR_DEFAULT), (Fl_Color) 56, (Fl_Color) 255) ;
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

  return reader->GetOutput();
}

void
guiMainImplementation
::SaveLandmarkRegisteredImage()
{
  const char * filename = fl_file_chooser("Save pre-registered image file...","*.mh?","");

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
    }
}

void
guiMainImplementation
::SaveFinalRegisteredImage()
{
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
    }
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
::CalculateMaximumSize()
{
  this->ChangeStatusDisplay("CalculateMaximumSize begins") ;

  bool identicalSpacing = true ;
  const double* fixedSpacing = m_FixedImage->GetSpacing() ;
  const double* movingSpacing = m_MovingImage->GetSpacing() ;
  bool fixedImageSpacingChanged = false ;
  bool movingImageSpacingChanged = false ;

  bool identicalSize = true ;
  ImageType::SizeType fixedSize = 
    m_FixedImage->GetLargestPossibleRegion().GetSize() ;
  ImageType::SizeType movingSize = 
    m_MovingImage->GetLargestPossibleRegion().GetSize() ;

  bool fixedImageSizeChanged = false ;
  bool movingImageSizeChanged = false ;

  double maximumExtent ;

  m_FixedImageResamplingNeeded = false ;
  m_MovingImageResamplingNeeded = false ;

  // find the minimum spacing of two images
  for ( unsigned int i = 0 ; i < ImageType::ImageDimension ; ++i )
    {
    if ( fixedSpacing[i] < movingSpacing[i] )
      {
      m_MinimumSpacing[i] = fixedSpacing[i] ;
      }
    else
      {
      m_MinimumSpacing[i] = movingSpacing[i] ;
      }

    if ( fixedSpacing[i] != m_MinimumSpacing[i] )
      {
      fixedImageSpacingChanged = true ;
      }

    if ( movingSpacing[i] != m_MinimumSpacing[i] )
      {
      movingImageSpacingChanged = true ;
      }


    if ( fixedSpacing[i] * fixedSize[i] > movingSpacing[i] * movingSize[i] )
      {
      maximumExtent = fixedSpacing[i] * fixedSize[i] ;
      }
    else
      {
      maximumExtent = movingSpacing[i] * movingSize[i] ;
      }

    m_MaximumSize[i] = (unsigned long) (maximumExtent / m_MinimumSpacing[i]) ;

    if ( m_MaximumSize[i] != fixedSize[i] )
      {
      fixedImageSizeChanged = true ;
      }

    if ( m_MaximumSize[i] != movingSize[i] )
      {
      movingImageSizeChanged = true ;
      }
    } // end of for

  if ( fixedImageSpacingChanged || fixedImageSizeChanged )
    {
    m_FixedImageResamplingNeeded = true ;
    }

  if ( movingImageSpacingChanged || movingImageSizeChanged )
    {
    m_MovingImageResamplingNeeded = true ;
    }

  this->ChangeStatusDisplay("CalculateMaximumSize ends") ;
}

guiMainImplementation::ImagePointer
guiMainImplementation
::ResampleUsingResolution(ImageType* image) const
{
  ResampleImageFilterType::Pointer resampler = ResampleImageFilterType::New();

  resampler->SetInput(image);
  resampler->SetSize(m_MaximumSize);
  resampler->SetOutputOrigin(image->GetOrigin());
  resampler->SetOutputSpacing(m_MinimumSpacing);
  try
    {
    resampler->Update();
    }
  catch(itk::ExceptionObject &e)
    {
    std::cout<<e<<std::endl;
    }

  return resampler->GetOutput();
}

guiMainImplementation::RGBImagePointer
guiMainImplementation
::MakeRGBImage(ImageType * redImage, ImageType * greenImage )
{
  typedef itk::NormalizeImageFilter< ImageType, RealImageType > NormalizerType ;
  NormalizerType::Pointer redImageNormalizer = NormalizerType::New() ;
  NormalizerType::Pointer greenImageNormalizer = NormalizerType::New() ;

  redImageNormalizer->SetInput(redImage) ;
  greenImageNormalizer->SetInput(greenImage) ;
  redImageNormalizer->Update() ;
  greenImageNormalizer->Update() ;

  typedef itk::ImageRegionIterator< RealImageType > RealImageIterator ;
  RealImageIterator redImageIterator   
    (redImageNormalizer->GetOutput() , redImage->GetLargestPossibleRegion());
  RealImageIterator greenImageIterator
    (greenImageNormalizer->GetOutput() ,greenImage->GetLargestPossibleRegion());
  
  RGBImageType::Pointer rgbImage = RGBImageType::New() ;
  rgbImage->SetRegions(redImage->GetLargestPossibleRegion());
  rgbImage->SetSpacing(redImage->GetSpacing());
  rgbImage->SetOrigin(redImage->GetOrigin());
  rgbImage->Allocate() ;

  RGBImageRegionIteratorType  rgbImageIterator
    (rgbImage,rgbImage->GetLargestPossibleRegion());

  redImageIterator.GoToBegin();
  greenImageIterator.GoToBegin();
  rgbImageIterator.GoToBegin();

  for( ; !redImageIterator.IsAtEnd() ; 
       ++redImageIterator, ++greenImageIterator, ++rgbImageIterator )
    {
    rgbImageIterator.Value()[0] = redImageIterator.Value();
    rgbImageIterator.Value()[1] = greenImageIterator.Value();
    rgbImageIterator.Value()[2] = 0;
    }

  return rgbImage ;
}


void
guiMainImplementation
::ModifySliceViewInputImage( SliceViewType * sliceview, ImageType * image )
{
  sliceview->SetInputImage(image);
  sliceview->update();
  sliceview->redraw();
  sliceview->activate();
}

void
guiMainImplementation
::SetViewAxis(unsigned int axis)
{
  m_ViewAxis = axis ;
  unsigned int mid = (m_MaximumSize[axis]-1)/2 ;
  tkSliceValuator->value(mid);
  tkSliceValuator->maximum(m_MaximumSize[axis]-1);

  tkFixedImageViewer->orientation(axis) ;
  tkMovingImageViewer->orientation(axis) ;
  tkResultImageViewer->orientation(axis) ;

  tkResultImageViewer->sliceNum(mid) ;
  tkFixedImageViewer->sliceNum(mid) ;
  tkMovingImageViewer->sliceNum(mid) ;

  tkFixedImageViewer->update() ;
  tkMovingImageViewer->update() ;
  tkResultImageViewer->update() ;

  tkFixedImageViewer->redraw() ;
  tkMovingImageViewer->redraw() ;
  tkResultImageViewer->redraw() ;
}

void 
guiMainImplementation
::SelectImageSet(unsigned int i)
{
  if (i == 0)
    {
    tkResultImageViewer->SetInputImage(m_ViewNonRegisteredImage) ;
    tkMovingImageViewer->SetInputImage(m_ViewMovingImage) ;
    }
  else if ( i == 1)
    {
    tkResultImageViewer->SetInputImage(m_ViewLandmarkRegisteredImage) ;
    tkMovingImageViewer->SetInputImage(m_ViewLandmarkRegisteredMovingImage) ;
    }
  else if ( i == 2 )
    {
    tkResultImageViewer->SetInputImage(m_ViewRegisteredImage) ;
    tkMovingImageViewer->SetInputImage(m_ViewRegisteredMovingImage) ;
    }

  tkMovingImageViewer->update();
  tkMovingImageViewer->redraw();  
  tkMovingImageViewer->activate();

  tkResultImageViewer->update();
  tkResultImageViewer->redraw();  
  tkResultImageViewer->activate();
  
  this->SetViewAxis(m_ViewAxis) ;
  this->UpdateSliceNumber() ;
}

void
guiMainImplementation
::UpdateSliceNumber()
{
  if( tkFixedImageViewer->GetInputImage() )
    {
    tkFixedImageViewer->sliceNum((unsigned int)tkSliceValuator->value());
    tkFixedImageViewer->update();
    }
  
  if( tkMovingImageViewer->GetInputImage() )
    {
    tkMovingImageViewer->sliceNum((unsigned int) tkSliceValuator->value());
    tkMovingImageViewer->update();
    }
  
  if ( tkMovingImageViewer->GetInputImage() && 
       tkFixedImageViewer->GetInputImage() )
    {
    tkResultImageViewer->sliceNum((unsigned int)tkSliceValuator->value());
    tkResultImageViewer->update();
    }
}

/////////////////////////////////////////////////
// Application status functions
/////////////////////////////////////////////////

void
guiMainImplementation
::ChangeStatusDisplay(const char* message)
{
  tkStatusDisplay->value(message) ;
  Fl::check() ;
}

/////////////////////////////////////////////////
// Landmark related functions
/////////////////////////////////////////////////

void 
guiMainImplementation
::UpdateLandmark( Fl_Group* parent, unsigned int index )
{
  Fl_Group* grp = parent ;
  SliceViewType* view ;
  if (grp == tkFixedImageLandmarkGroup)
    {
    view = tkFixedImageViewer ;
    }
  else
    {
    view = tkMovingImageViewer ;
    }

  Fl_Button* button = (Fl_Button*)(grp->child(index)) ;

  if (!(button->value()))
    {
    button->value(1) ;
    view->AddLandmark(index) ;
    }
  else
    {
    if ( view->GetLandmark(index) )
      {
      view->DeleteLandmark(index) ;
      }
    button->value(0) ;
    }
}

void
guiMainImplementation
::JumpToLandmark(bool moving,unsigned int index )
{
  if( moving )
    {
    if( tkMovingImageViewer->GetLandmark(index) )
      {
      tkSliceValuator->value((*(tkMovingImageViewer->GetIndexLandmark(index)))[m_ViewAxis]);
      }
    }
  else
    {
    if( tkFixedImageViewer->GetLandmark(index) )
      {
      tkSliceValuator->value((*(tkFixedImageViewer->GetIndexLandmark(index)))[m_ViewAxis]);
      }
    }
  
  this->UpdateSliceNumber();
}

void 
guiMainImplementation
::ClearLandmarks(Fl_Group* parent)
{
  SliceViewType* view ;
  if (parent == tkFixedImageLandmarkGroup)
    {
    view = tkFixedImageViewer ;
    }
  else
    {
    view = tkMovingImageViewer ;
    }
  
  for ( unsigned int i = 0 ; i < parent->children() ; ++i )
    {
    Fl_Button* button = (Fl_Button*)(parent->child(i)) ;
  
    if ( view->GetLandmark(i) )
      {
      view->DeleteLandmark(i) ;
      }

    if ( button->value() )
      {
      button->value(0) ;
      }
    }
}

void
guiMainImplementation
::LoadLandmarks( bool moving )
{
  const char * filename = 
    fl_file_chooser("Choose a file for landmarks...","*.landmarks","");

  if( filename )
    {
    typedef itk::LandmarkSpatialObject< 3 > ObjectType ;
    typedef itk::SpatialObjectReader<> ReaderType ;
      
    ReaderType::Pointer reader = ReaderType::New() ;
    reader->SetFullFileName(filename) ;
    reader->Update() ;
    ReaderType::GroupPointer grp = reader->GetGroup() ;
    ReaderType::GroupType::ChildrenListType* children = grp->GetChildren() ;
    ObjectType::Pointer landmarks = 
      dynamic_cast< ObjectType* >(*(children->begin())) ;

    Fl_Group* tkgrp ;
    if (moving)
      {
      tkMovingImageViewer->SetLandmarks(landmarks) ;
      tkgrp = tkMovingImageLandmarkGroup ;
      }
    else
      {
      tkFixedImageViewer->SetLandmarks(landmarks) ;
      tkgrp = tkFixedImageLandmarkGroup ;
      }

    for(unsigned int i=0; i < tkgrp->children(); i++)
      {
      ((Fl_Button *)tkgrp->child(i))->value(1);
      }        

    } 
}

void
guiMainImplementation
::SaveLandmarks( bool moving )
{
  const char * filename = 
    fl_file_chooser("Choose a file for landmarks...","*.landmarks","");

  if( !filename )
    {
    return ;
    }
      
  typedef itk::LandmarkSpatialObject< 3 > ObjectType ;
  typedef itk::SpatialObjectWriter< > WriterType ;
  typedef itk::GroupSpatialObject< > GroupType ;
  
  WriterType::Pointer writer = WriterType::New() ;
  writer->SetFullFileName(filename) ;
  ObjectType* landmarks ;
  if (moving)
    {
    landmarks = tkMovingImageViewer->GetLandmarks() ;
    }
  else
    {
    landmarks = tkFixedImageViewer->GetLandmarks() ;
    }
  
  writer->SetInput(landmarks) ;
  writer->Update() ;
}

/////////////////////////////////////////////////
// Advanced option related functions
/////////////////////////////////////////////////

void
guiMainImplementation
::ShowAdvancedOptions()
{
  tkAdvancedOptionsWindow->show();

  // set rigid advance option values
  tkRigidRotationScaleX->value(m_App->GetRigidScales()[0]) ;
  tkRigidRotationScaleY->value(m_App->GetRigidScales()[1]) ;
  tkRigidRotationScaleZ->value(m_App->GetRigidScales()[2]) ;
  tkRigidTranslationScaleX->value(m_App->GetRigidScales()[4]) ;
  tkRigidTranslationScaleY->value(m_App->GetRigidScales()[5]) ;
  tkRigidTranslationScaleZ->value(m_App->GetRigidScales()[6]) ;
  tkRigidNumberOfIterations->value(m_App->GetRigidNumberOfIterations()) ;
  tkRigidLearningRate->value(m_App->GetRigidLearningRate()) ;
  tkRigidFixedImageStdDev->value
    (m_App->GetRigidFixedImageStandardDeviation()) ;
  tkRigidMovingImageStdDev->value
    (m_App->GetRigidMovingImageStandardDeviation()) ;
  tkRigidNumberOfSpatialSamples->value
    (m_App->GetRigidNumberOfSpatialSamples()) ;

  // set rigid advance option values
  tkAffineMatrixScale0->value(m_App->GetAffineScales()[0]) ;
  tkAffineMatrixScale1->value(m_App->GetAffineScales()[1]) ;
  tkAffineMatrixScale2->value(m_App->GetAffineScales()[2]) ;
  tkAffineMatrixScale3->value(m_App->GetAffineScales()[3]) ;
  tkAffineMatrixScale4->value(m_App->GetAffineScales()[4]) ;
  tkAffineMatrixScale5->value(m_App->GetAffineScales()[5]) ;
  tkAffineMatrixScale6->value(m_App->GetAffineScales()[6]) ;
  tkAffineMatrixScale7->value(m_App->GetAffineScales()[7]) ;
  tkAffineMatrixScale8->value(m_App->GetAffineScales()[8]) ;
  tkAffineTranslationScaleX->value(m_App->GetAffineScales()[9]) ;
  tkAffineTranslationScaleY->value(m_App->GetAffineScales()[10]) ;
  tkAffineTranslationScaleZ->value(m_App->GetAffineScales()[11]) ;
  tkAffineNumberOfIterations->value(m_App->GetAffineNumberOfIterations()) ;
  tkAffineLearningRate->value(m_App->GetAffineLearningRate()) ;
  tkAffineFixedImageStdDev->value
    (m_App->GetAffineFixedImageStandardDeviation()) ;
  tkAffineMovingImageStdDev->value
    (m_App->GetAffineMovingImageStandardDeviation()) ;
  tkAffineNumberOfSpatialSamples->value
    (m_App->GetAffineNumberOfSpatialSamples()) ;
}

void
guiMainImplementation
::ApplyAdvancedOptions()
{
  // set rigid advance option values
  RegistrationAppType::RigidScalesType rigidScales = m_App->GetRigidScales() ;
  rigidScales[0] = tkRigidRotationScaleX->value() ;
  rigidScales[1] = tkRigidRotationScaleY->value() ;
  rigidScales[2] = tkRigidRotationScaleZ->value() ;
  rigidScales[4] = tkRigidTranslationScaleX->value() ;
  rigidScales[5] = tkRigidTranslationScaleY->value() ;
  rigidScales[6] = tkRigidTranslationScaleZ->value() ;
  m_App->SetRigidScales(rigidScales) ;
  m_App->SetRigidNumberOfIterations((unsigned int) tkRigidNumberOfIterations->value()) ;
  m_App->SetRigidLearningRate(tkRigidLearningRate->value()) ;
  m_App->SetRigidFixedImageStandardDeviation
    (tkRigidFixedImageStdDev->value()) ;
  m_App->SetRigidMovingImageStandardDeviation
    (tkRigidMovingImageStdDev->value()) ;
  m_App->SetRigidNumberOfSpatialSamples
    ((unsigned int) tkRigidNumberOfSpatialSamples->value()) ;

  // set affine advance option values
  RegistrationAppType::AffineScalesType affineScales = 
    m_App->GetAffineScales() ;
  affineScales[0] = tkAffineMatrixScale0->value() ;
  affineScales[1] = tkAffineMatrixScale1->value() ;
  affineScales[2] = tkAffineMatrixScale2->value() ;
  affineScales[3] = tkAffineMatrixScale3->value() ;
  affineScales[4] = tkAffineMatrixScale4->value() ;
  affineScales[5] = tkAffineMatrixScale5->value() ;
  affineScales[6] = tkAffineMatrixScale6->value() ;
  affineScales[7] = tkAffineMatrixScale7->value() ;
  affineScales[8] = tkAffineMatrixScale8->value() ;

  affineScales[9] = tkAffineTranslationScaleX->value() ;
  affineScales[10] = tkAffineTranslationScaleY->value() ;
  affineScales[11] = tkAffineTranslationScaleZ->value() ;
  m_App->SetAffineScales(affineScales) ;
  m_App->SetAffineNumberOfIterations((unsigned int) tkAffineNumberOfIterations->value()) ;
  m_App->SetAffineLearningRate(tkAffineLearningRate->value()) ;
  m_App->SetAffineFixedImageStandardDeviation
    (tkAffineFixedImageStdDev->value()) ;
  m_App->SetAffineMovingImageStandardDeviation
    (tkAffineMovingImageStdDev->value()) ;
  m_App->SetAffineNumberOfSpatialSamples
    ((unsigned int) tkAffineNumberOfSpatialSamples->value()) ;

  tkAdvancedOptionsWindow->hide();
}

void
guiMainImplementation
::SaveOptions()
{
  const char * filename = 
    fl_file_chooser("Choose a file for options","*.options","");

  std::map< std::string, double > options ;

  if( !filename )
    {
    std::cout << "ERROR: cannot find the file." << std::endl ;
    return ;
    }
  
  std::ofstream outputFile(filename) ;
  if ( !outputFile )
    {
    std::cout << "ERROR: cannot open the option file" << std::endl ;
    }
  
  
  RegistrationAppType::RigidScalesType rigidScales = 
    m_App->GetRigidScales() ;
  for ( unsigned int i = 0 ; i < rigidScales.Size() ; ++i )
    {
    outputFile << "RigidScales_" << i << " " 
               << rigidScales[i] << std::endl ;
    }
  
  outputFile << "RigidNumberOfIterations " 
             << m_App->GetRigidNumberOfIterations() << std::endl ;
  
  outputFile << "RigidLearningRate " 
             << m_App->GetRigidLearningRate() << std::endl ;
  outputFile << "RigidFixedImageStandardDeviation " 
             << m_App->GetRigidFixedImageStandardDeviation() << std::endl ;
  outputFile << "RigidMovingImageStandardDeviation " 
             << m_App->GetRigidMovingImageStandardDeviation() 
             << std::endl ;
  outputFile << "RigidNumberOfSpatialSamples " 
             << m_App->GetRigidNumberOfSpatialSamples() << std::endl ;
  
  // set affine advance option values
  RegistrationAppType::AffineScalesType affineScales = 
    m_App->GetAffineScales() ;
  for ( unsigned int i = 0 ; i < affineScales.Size() ; ++i )
    {
    outputFile << "AffineScales_" << i << " " 
               << affineScales[i] << std::endl ;
    }
  
  outputFile << "AffineNumberOfIterations " 
             << m_App->GetAffineNumberOfIterations() << std::endl ;
  
  outputFile << "AffineLearningRate " 
             << m_App->GetAffineLearningRate() << std::endl ;
  outputFile << "AffineFixedImageStandardDeviation " 
             << m_App->GetAffineFixedImageStandardDeviation() 
             << std::endl ;
  outputFile << "AffineMovingImageStandardDeviation " 
             << m_App->GetAffineMovingImageStandardDeviation() 
             << std::endl ;
  outputFile << "AffineNumberOfSpatialSamples " 
             << m_App->GetAffineNumberOfSpatialSamples() << std::endl ;
}

void
guiMainImplementation
::LoadOptions()
{
  const char * filename = 
    fl_file_chooser("Choose a file for options","*.options","");

  if( !filename )
    {
    std::cout << "ERROR: cannot find the file" << std::endl ;
    return ;
    }

  std::ifstream inputFile(filename) ;
  if ( !inputFile )
    {
    std::cout << "ERROR: cannot open the option file" << std::endl ;
    }


  typedef std::map< std::string, double > OptionsMapType ;
  OptionsMapType options ;

  std::string key = "" ;
  double value ;
  // parsing option file
  while ( !inputFile.eof() )
    {
    inputFile >> key ;
    inputFile >> value ;
    options[key] = value ;
    }

  // set options 
  RegistrationAppType::RigidScalesType rigidScales = m_App->GetRigidScales();
  for ( unsigned int i = 0 ; i < rigidScales.Size() ; ++i )
    {
    std::ostringstream temp ;
    temp <<  "RigidScales_" << i ;
    rigidScales[i] = options[temp.str()] ;
    }
  m_App->SetRigidScales(rigidScales) ;
  m_App->SetRigidNumberOfIterations((unsigned int)options["RigidNumberOfIterations"]) ;
  m_App->SetRigidLearningRate(options["RigidLearningRate"]) ;
  m_App->SetRigidFixedImageStandardDeviation(options["RigidFixedImageStandardDeviation"]) ;
  m_App->SetRigidMovingImageStandardDeviation(options["RigidMovingImageStandardDeviation"]) ; 
  m_App->SetRigidNumberOfSpatialSamples((unsigned int)options["RigidNumberOfSpatialSamples"]) ;

  RegistrationAppType::AffineScalesType affineScales = 
    m_App->GetAffineScales() ;
  for ( unsigned int i = 0 ; i < affineScales.Size() ; ++i )
    {
    std::ostringstream temp ;
    temp << "AffineScales_" << i ;
    affineScales[i] = options[temp.str()] ;
    }
  m_App->SetAffineScales(affineScales) ;
  m_App->SetAffineNumberOfIterations((unsigned int)options["AffineNumberOfIterations"]) ;
  m_App->SetAffineLearningRate(options["AffineLearningRate"]) ;
  m_App->SetAffineFixedImageStandardDeviation(options["AffineFixedImageStandardDeviation"]) ;
  m_App->SetAffineMovingImageStandardDeviation(options["AffineMovingImageStandardDeviation"]) ; 
  m_App->SetAffineNumberOfSpatialSamples((unsigned int)options["AffineNumberOfSpatialSamples"]) ;
}

/////////////////////////////////////////////////
// Registartion related functions
/////////////////////////////////////////////////

void
guiMainImplementation
::SaveTransform()
{
  const char * filename = 
    fl_file_chooser("Choose a file for transform...","*.transform","");

  if( filename )
    {
    typedef itk::SpatialObjectWriter< > WriterType ;
    typedef itk::GroupSpatialObject< > GroupType ;
    typedef itk::FixedCenterOfRotationAffineTransform< double, 3 > 
      TransformType ;

    WriterType::Pointer writer = WriterType::New() ;
    writer->SetFullFileName(filename) ;
      
    GroupType::Pointer grp = GroupType::New() ;
    TransformType::Pointer transform = TransformType::New() ;
    transform->SetParameters( m_App->GetAffineTransform()->GetParameters() ) ;
    grp->SetObjectToParentTransform( transform.GetPointer() ) ;
    writer->SetInput( grp ) ;
    writer->Update() ;
    }
  
}

guiMainImplementation::ImagePointer
guiMainImplementation
::ResampleUsingTransform(AffineTransformType* finalTransform, 
                         ImageType* input)
{
  // resample the moving image 
  InterpolatorType::Pointer interpolator = 
    InterpolatorType::New();
  interpolator->SetInputImage(input);
  
  ResampleImageFilterType::Pointer resample = 
    ResampleImageFilterType::New();
  resample->SetInput(input);
  resample->SetInterpolator(interpolator.GetPointer());
  resample->SetSize(input->GetLargestPossibleRegion().GetSize());
  resample->SetOutputOrigin(input->GetOrigin());
  resample->SetOutputSpacing(input->GetSpacing());
  resample->SetTransform(finalTransform);
  resample->Update();
  
  return resample->GetOutput() ;

}

void
guiMainImplementation
::Register()
{
  this->ChangeStatusDisplay("Registration begins.") ;
  fl_cursor(Fl_Cursor(FL_CURSOR_WAIT), (Fl_Color) 56, (Fl_Color) 255) ;
  m_App->SetFixedImage(m_FixedImage.GetPointer()) ;
  m_App->SetMovingImage(m_MovingImage.GetPointer()) ;

  if( tkFixedImageViewer->GetNumberOfLandmarks() == 4 &&
      tkMovingImageViewer->GetNumberOfLandmarks() == 4)
    {
    this->ChangeStatusDisplay("Register using landmarks") ;
    m_App->RegisterUsingLandmarks
      (tkFixedImageViewer->GetRegistrationLandmarks(),
       tkMovingImageViewer->GetRegistrationLandmarks(),
       m_Scales,
       1000) ;
      
    this->ChangeStatusDisplay("Resampling registered moving image...") ;
    m_LandmarkRegisteredMovingImage = 
      this->ResampleUsingTransform(m_App->GetLandmarkTransform(), 
                                   m_MovingImage.GetPointer()) ;
    
    if ( m_MovingImageResamplingNeeded )
      {
      this->ChangeStatusDisplay("Resampling registered moving image to match the resolution...") ;
      m_ViewLandmarkRegisteredMovingImage = 
        this->ResampleUsingResolution(m_LandmarkRegisteredMovingImage.GetPointer()) ;
      }
    else
      {
      m_ViewLandmarkRegisteredMovingImage = m_LandmarkRegisteredMovingImage.GetPointer() ;
      }
    
    this->ChangeStatusDisplay("Generating RGB image using the landmark registered image...") ;
    m_ViewLandmarkRegisteredImage = 
      this->MakeRGBImage(m_ViewFixedImage.GetPointer(),
                         m_ViewLandmarkRegisteredMovingImage.GetPointer()) ;
    tkLandmarkRegisteredView->activate() ;
    }
  
  switch( tkRegistrationMethodChoice->value() )
    {
    case 0: // rigid 
    {
    this->ChangeStatusDisplay("Registering using the rigid method...") ;
    m_App->NormalizeAndCenter() ;
    m_App->RegisterUsingRigidMethod() ;
    break ;
    }
    case 1: // affine
    {
    this->ChangeStatusDisplay("Registering using the affine method...") ;
    m_App->NormalizeAndCenter() ;
    m_App->RegisterUsingAffineMethod() ;
    break ;
    //     case 2: // deformable
    //       m_App->RegisterUsingDeformableMethod() ;
    //       break ;
    }
    default:
      break ;
    }
  
  this->ChangeStatusDisplay("Resampling the moving image...") ;
  m_RegisteredMovingImage = 
    this->ResampleUsingTransform(m_App->GetAffineTransform(), 
                                 m_MovingImage.GetPointer()) ;

  if ( m_MovingImageResamplingNeeded )
    {
    this->ChangeStatusDisplay("Resampling the registered moving image to match the resolution") ;
    m_ViewRegisteredMovingImage = 
      this->ResampleUsingResolution(m_RegisteredMovingImage.GetPointer()) ;
    }
  else
    {
    m_ViewRegisteredMovingImage = m_RegisteredMovingImage ;
    }
  
  this->ChangeStatusDisplay("Generating the RGB image...") ;
  m_ViewRegisteredImage = 
    this->MakeRGBImage(m_ViewFixedImage.GetPointer(),
                       m_ViewRegisteredMovingImage.GetPointer()) ;
  tkRegisteredView->activate() ;
  
  tkDisplayMovingImageChoice->value(2) ;
  this->SelectImageSet(2) ;
  
  fl_cursor(Fl_Cursor(FL_CURSOR_DEFAULT), (Fl_Color) 56, (Fl_Color) 255) ;
  this->ChangeStatusDisplay("Registration done") ;
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
 


