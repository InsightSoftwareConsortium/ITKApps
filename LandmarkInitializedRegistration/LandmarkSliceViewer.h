#ifndef __LandmarkSliceViewer_h
#define __LandmarkSliceViewer_h

#include <vector>
#include "itkRGBAPixel.h"
#include "itkExceptionObject.h"
#include "itkSpatialObjectPoint.h"
#include "itkLandmarkSpatialObject.h"
#include "LandmarkBasedRegistrator.h"
#include "GLSliceView.h"

enum Action{ NoAction, Add, Modify };


template< class TImagePixel >
class LandmarkSliceViewer
: public GLSliceView< TImagePixel,
                      itk::RGBAPixel< unsigned char > >
{

public:

  typedef LandmarkSliceViewer< TImagePixel >          Self;
  typedef GLSliceView< TImagePixel,
                       itk::RGBAPixel< unsigned char > > Superclass;

  typedef typename Superclass::SizeType SizeType ;
  typedef typename Superclass::RegionType RegionType ;
  typedef typename Superclass::ImageType ImageType ;

  typedef itk::ExceptionObject ExceptionType;

  typedef itk::RGBAPixel< unsigned char > OverlayPixelType;
  typedef itk::Image< OverlayPixelType, 3 > OverlayImageType ;
  typedef typename OverlayImageType::IndexType IndexType ;
  typedef itk::SpatialObjectPoint<3> PointType;
  typedef PointType::PixelType PointPixelType;
  typedef itk::LandmarkBasedRegistrator<>::PointType RegistrationPointType ;
  typedef itk::LandmarkBasedRegistrator<>::PointSetType 
  RegistrationPointSetType ;
  typedef std::vector<PointType> PointVectorType;
  typedef itk::LandmarkSpatialObject< 3 > LandmarkSpatialObjectType ;
  typedef std::vector<IndexType> IndexVectorType ;
  typedef std::vector<bool> BoolVectorType ;
  typedef std::list<unsigned int> ColorTableType;
  LandmarkSliceViewer(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const char * label);
  
  ~LandmarkSliceViewer();

  void SetInputImage( ImageType* image ) ;

  void SetLandmarks( LandmarkSpatialObjectType* landmarks ) ;

  LandmarkSpatialObjectType* GetLandmarks() ;

  unsigned int GetNumberOfLandmarks() ;

  RegistrationPointSetType* GetRegistrationLandmarks( ) ;
  
  PointType* GetLandmark( unsigned int index ) ;
  IndexType* GetIndexLandmark( unsigned int index ) ;

  void AddLandmark( PointType& point ) ;

  void AddLandmark( unsigned int index );
  
  void AddLandmark( OverlayPixelType color );

  void DeleteLandmark( unsigned int v_index) ;

  void ModifyLandmark( unsigned int index );

  void RemoveLandmark( unsigned int index );

  virtual int handle( int event );

protected:
  void CreateLandmark( unsigned int v_index, const IndexType& index ) ;

  PointVectorType m_Landmarks;
  LandmarkSpatialObjectType::Pointer m_LandmarkSpatialObject ;
  IndexVectorType m_LandmarkIndexes ;
  BoolVectorType m_LandmarkAvailables ;
  typename RegistrationPointSetType::Pointer m_RegistrationLandmarks ;
  bool              m_LandmarkSelectionMode;
  Action            m_Action;

  unsigned int      m_SelectedLandmarkIndex;
  OverlayPixelType         m_DefaultColor[4];
  PointPixelType         m_SelectedLandmarkColor;
  bool              m_UseCustomColor;

  void DrawPoint(const IndexType& index, PointPixelType color) ;

  unsigned int GetColorTableValue( PointType & landmark );
};

#include "LandmarkSliceViewer.txx"

#endif //__LandmarkSliceViewer_h
