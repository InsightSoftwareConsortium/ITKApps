#ifndef __RegToolGUIImplementation_h
#define __RegToolGUIImplementation_h

#include <string>

// FLTK related includes...
#include <FL/Enumerations.H>
#include <FL/fl_file_chooser.H>

// ITK related includes....
#include <itkExceptionObject.h>
#include <itkRGBPixel.h>
#include <itkResampleImageFilter.h>
#include <itkImageRegionIterator.h>
#include <itkImageIOFactory.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include "itkSpatialObjectReader.h"
#include "itkSpatialObjectWriter.h"

#include "guiMain.h"
#include "ImageRegistrationApp.h"

class guiMainImplementation : public guiMain
{
public:
  typedef short ImagePixelType;
  typedef itk::Image<ImagePixelType,3> ImageType;
  typedef itk::Image<float,3> RealImageType;
  typedef ImageType::Pointer ImagePointer ;
  typedef unsigned char OverlayImagePixelType;

  typedef itk::RGBPixel< float > RGBPixelType;
  typedef itk::Image<RGBPixelType,3> RGBImageType;
  typedef RGBImageType::Pointer RGBImagePointer ;

  typedef ImageType::RegionType RegionType;
  typedef ImageType::IndexType IndexType;
  typedef RegionType::SizeType SizeType;
  typedef itk::Point<double,3> PointType;

  typedef itk::ImageFileReader<ImageType> ImageReaderType;
  typedef itk::ImageFileWriter<ImageType> ImageWriterType;

  typedef LandmarkSliceViewer<ImagePixelType> SliceViewType;
  typedef SliceViewType::OverlayImageType OverlayImageType ;
  typedef itk::GLColorSliceView<ImagePixelType,OverlayImagePixelType> 
  ColorSliceViewType;

  typedef itk::ResampleImageFilter<ImageType,ImageType> 
  ResampleImageFilterType;
  typedef itk::ResampleImageFilter<OverlayImageType,OverlayImageType> 
  ResampleOverlayImageFilterType;

  typedef itk::ImageRegionIterator<ImageType> ImageRegionIteratorType;
  typedef itk::ImageRegionIterator<RGBImageType> RGBImageRegionIteratorType;
  
  typedef ImageRegistrationApp< ImageType, RealImageType > 
  RegistrationAppType ;

  typedef RegistrationAppType::AffineTransformType AffineTransformType ;
  typedef AffineTransformType::OffsetType OffsetType ;
  typedef RegistrationAppType::RigidTransformType RigidTransformType ;
  typedef itk::LinearInterpolateImageFunction< ImageType, double > 
  InterpolatorType ;
  
  guiMainImplementation();
  ~guiMainImplementation();

  ////////////////////////////////////
  // Initializations & Terminations
  ////////////////////////////////////
  void Initialize();
  void Quit();

  /////////////////////////////////////////////////
  // Image File IO functions
  /////////////////////////////////////////////////
  void SelectFixedImage() ;
  void SelectMovingImage() ;
  void HandleInputImageChange(std::string & fileName, bool isFixedImage) ;
  ImagePointer LoadImage( const char * filename );
  void SaveLandmarkRegisteredImage();
  void SaveFinalRegisteredImage();

  /////////////////////////////////////////////////
  // Image view functions
  /////////////////////////////////////////////////
  void Show();
  void CalculateMaximumSize() ;
  ImagePointer ResampleUsingResolution( ImageType * image ) const;
  RGBImagePointer MakeRGBImage(ImageType * redImage, ImageType * greenImage );
  void ModifySliceViewInputImage( SliceViewType * sliceview, 
                                  ImageType * image );
  void ModifySliceViewInputImage( ColorSliceViewType * sliceview,
                                  RGBImageType * image );
  void SetViewAxis(unsigned int axis) ;
  void SelectImageSet(unsigned int i) ;
  void UpdateSliceNumber();

  /////////////////////////////////////////////////
  // Application status functions
  /////////////////////////////////////////////////
  void ChangeStatusDisplay(const char* message) ;

  /////////////////////////////////////////////////
  // Landmark related functions
  /////////////////////////////////////////////////
  void JumpToLandmark(bool moving,unsigned int index );
  void LoadLandmarks( bool moving );
  void SaveLandmarks( bool moving );
  void UpdateLandmark( Fl_Group* parent, unsigned int index );
  void ClearLandmarks(Fl_Group* parent);

  /////////////////////////////////////////////////
  // Advanced option related functions
  /////////////////////////////////////////////////
  void ShowAdvancedOptions();
  void ApplyAdvancedOptions();
  void SaveOptions() ;
  void LoadOptions() ;

  /////////////////////////////////////////////////
  // Registartion related functions
  /////////////////////////////////////////////////
  void SaveTransform() ;
  ImagePointer ResampleUsingTransform(AffineTransformType* finalTransform, 
                                      ImageType* input) ;
  void Register();

  /////////////////////////////////////////////////
  // Help related functions
  /////////////////////////////////////////////////
  void ShowHelp( const char * file );

private:
  SizeType m_MaximumSize ;
  double m_MinimumSpacing[ImageType::ImageDimension] ;
  RegistrationAppType* m_App ;
  unsigned int m_ViewAxis ;
  bool m_FixedImageResamplingNeeded ;
  bool m_MovingImageResamplingNeeded ;

  std::string m_FixedImageFileName ;
  std::string m_MovingImageFileName ;

  RGBImageType::Pointer m_ViewRegisteredImage ;
  RGBImageType::Pointer m_ViewLandmarkRegisteredImage ;
  RGBImageType::Pointer m_ViewNonRegisteredImage ;

  ImageType::Pointer m_FixedImage ;
  ImageType::Pointer m_MovingImage ;
  ImageType::Pointer m_LandmarkRegisteredMovingImage ;
  ImageType::Pointer m_RegisteredMovingImage ;
  ImageType::Pointer m_ViewFixedImage ;
  ImageType::Pointer m_ViewMovingImage ;
  ImageType::Pointer m_ViewLandmarkRegisteredMovingImage ;
  ImageType::Pointer m_ViewRegisteredMovingImage ;

  std::string m_LastLoadedImagePath;
  bool m_FixedImageLoaded;
  bool m_MovingImageLoaded;

  RegistrationAppType::LandmarkScalesType m_Scales;
};

#endif //__guiMainImplementation_h

