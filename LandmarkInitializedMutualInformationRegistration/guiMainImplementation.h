#ifndef __RegToolGUIMAINImplementation_h
#define __RegToolGUIMAINImplementation_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#include <string>

// FLTK related includes...
#include <FL/Enumerations.H>
#include <FL/fl_file_chooser.H>

// ITK related includes....
#include <itkAffineTransform.h>
#include <itkExceptionObject.h>
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
    typedef short                         ImagePixelType;
    typedef itk::Image<ImagePixelType,3>  ImageType;
    typedef ImageType::Pointer            ImagePointer ;

    typedef ImageType::RegionType         RegionType;
    typedef ImageType::IndexType          IndexType;
    typedef RegionType::SizeType          SizeType;
    typedef itk::Point<double,3>          PointType;

    typedef itk::ImageFileReader<ImageType>     ImageReaderType;
    typedef itk::ImageFileWriter<ImageType>     ImageWriterType;

    typedef LandmarkSliceViewer<ImagePixelType> SliceViewType;
    typedef SliceViewType::LandmarkSpatialObjectType 
                                                LandmarkSpatialObjectType ;
    typedef SliceViewType::LandmarkPointType        
                                                LandmarkPointType ;
    typedef SliceViewType::LandmarkPointListType    
                                                LandmarkPointListType ;

    typedef SliceViewType::OverlayImageType       OverlayImageType ;
    typedef SliceViewType::OverlayPixelType       OverlayPixelType ;
    typedef itk::GLTwoImageSliceView<ImagePixelType,OverlayPixelType> 
                                                  TwoImageSliceViewType;

    typedef itk::ImageRegistrationApp< ImageType >   ImageRegistrationAppType ;

    typedef itk::AffineTransform<double, 3>     AffineTransformType ;
    typedef itk::LinearInterpolateImageFunction< ImageType, double > 
                                                InterpolatorType ;
    
    guiMainImplementation();
    virtual ~guiMainImplementation();

    ////////////////////////////////////
    // Initializations & Terminations
    ////////////////////////////////////
    void Quit();

    /////////////////////////////////////////////////
    // Image File IO functions
    /////////////////////////////////////////////////
    void SelectFixedImage() ;
    void SelectMovingImage() ;
    void HandleInputImageChange(std::string & fileName, bool isFixedImage) ;
    ImagePointer LoadImage( const char * filename );
    void SaveFinalRegisteredImage();

    /////////////////////////////////////////////////
    // Image view functions
    /////////////////////////////////////////////////
    void Show();
    void SetViewAxis(unsigned int axis) ;
    void SelectImageSet(unsigned int i) ;
    void UpdateSliceNumber();
    void UpdateFixedSliceNumber();
    void UpdateMovingSliceNumber();

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
    void TransformLandmarks(LandmarkPointListType * source,
                            LandmarkPointListType * target,
                            AffineTransformType * transform) ;
    void UpdateMovingImageSpacing();

    /////////////////////////////////////////////////
    // Region of interest related functions
    /////////////////////////////////////////////////
    void ShowRegionOfInterestWindow() ;
    void ApplyRegionOfInterest() ;
    void CancelRegionOfInterest() ;
    void MoveRegionOfInterest(unsigned int direction) ;
    void ResizeRegionOfInterest(unsigned int direction) ;

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

    void Register();

    /////////////////////////////////////////////////
    // Help related functions
    /////////////////////////////////////////////////
    void ShowHelp( const char * file );

  private:
    ImageRegistrationAppType::Pointer m_ImageRegistrationApp ;

    unsigned int m_ViewAxis ;

    SizeType m_FixedImageSize ;
    SizeType m_MovingImageSize ;

    std::string m_FixedImageFileName ;
    std::string m_MovingImageFileName ;

    LandmarkSpatialObjectType::Pointer 
                              m_FixedLandmarkSpatialObject ;
    LandmarkSpatialObjectType::Pointer 
                              m_MovingLandmarkSpatialObject ;
    LandmarkSpatialObjectType::Pointer 
                              m_InitializedMovingLandmarkSpatialObject ;
    LandmarkSpatialObjectType::Pointer 
                              m_RegisteredMovingLandmarkSpatialObject ;

    ImageType::Pointer m_FixedImage ;
    ImageType::Pointer m_MovingImage ;
    ImageType::Pointer m_InitializedMovingImage ;
    ImageType::Pointer m_RegisteredMovingImage ;

    std::string m_LastLoadedImagePath;
    bool m_FixedImageLoaded;
    bool m_MovingImageLoaded;

    bool m_RegionUseLargestRegion ;
    bool m_RegionUseUserRegion ;
    bool m_RegionUseLandmarkRegion ;
    double m_RegionScale ;

  };

#endif //__guiMainImplementation_h

