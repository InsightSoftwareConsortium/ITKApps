#ifndef __LandmarkSliceViewer_h
#define __LandmarkSliceViewer_h

#include <vector>
#include <map>
#include "itkRGBAPixel.h"
#include "itkExceptionObject.h"
#include "itkSpatialObjectPoint.h"
#include "itkLandmarkSpatialObject.h"
#include "LandmarkBasedRegistrator.h"
#include "GLSliceView.h"

class VisualLandmark
{
public:
  typedef itk::Index< 3 > IndexType ;
  typedef itk::Point< double, 3 > PointType ;
  typedef itk::RGBAPixel< unsigned char > ColorType ;
  
  VisualLandmark() 
  {
    this->Initialize() ;
  }
  
  void Initialize()
  {
    m_IndexFilled = false ;
    m_Index.Fill( 0 ) ;
    m_PointFilled = false ;
    m_Point.Fill( 0.0 ) ;
    m_ColorFilled = false ;
    m_Color.Fill( 0 ) ;
  }

  bool IsComplete()
  {
    if ( m_IndexFilled && m_PointFilled && m_ColorFilled )
      {
      return true ;
      }
    else
      {
      return false ;
      }
  }
    
  void SetIndex(IndexType& index)
  { m_Index = index ; m_IndexFilled = true ; }

  const IndexType& GetIndex() const
  { return m_Index ; }

  bool IndexFilled()
  { return m_IndexFilled ; }

  void SetPoint(PointType point)
  { m_Point = point ; m_PointFilled = true ; }

  const PointType& GetPoint() const
  { return m_Point ; }

  bool PointFilled() const
  { return m_PointFilled ; }

  void SetColor(ColorType& color)
  { m_Color = color ; m_ColorFilled = true ; }

  const ColorType& GetColor() const
  { return m_Color ; }

  bool ColorFilled() const 
  { return m_ColorFilled ; }

private:
  IndexType m_Index ;
  PointType m_Point ;
  ColorType m_Color ;
  bool m_IndexFilled ;
  bool m_PointFilled ;
  bool m_ColorFilled ;
} ;

template< class TImagePixel >
class LandmarkSliceViewer
  : public GLSliceView< TImagePixel,
                        itk::RGBAPixel< unsigned char > >
{

public:
  // constructor
  LandmarkSliceViewer(unsigned int x, unsigned int y, 
                      unsigned int w, unsigned int h, const char * label);

  // destructor
  ~LandmarkSliceViewer();

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

  typedef itk::Point< double, 3 > PointType ;
  typedef itk::RGBAPixel< unsigned char > ColorType ;
  
  typedef std::map< unsigned int, VisualLandmark > LandmarkSetType ;
  
  typedef itk::VectorContainer<int,PointType > LandmarkVectorType ;
  typedef LandmarkVectorType::Pointer LandmarkVectorPointer ;

  typedef itk::SpatialObjectPoint< 3 > SpatialObjectPointType ;
  typedef std::list< SpatialObjectPointType > SpatialPointsType ;

  typedef std::list<unsigned int> ColorTableType;

  void SetInputImage(ImageType* image) ;

  void ResizeLandmarkContainer(unsigned int size) ;

  unsigned int GetNumberOfLandmarks()
  { return m_Landmarks.size() ; }

  LandmarkSetType& GetLandmarkSet() ;
  
  void SetSpatialPoints(SpatialPointsType points) ;
  SpatialPointsType& GetSpatialPoints() ;
  
  // returns the vector of registration landmarks that includes only
  // the valid landmarks. 
  LandmarkVectorType* GetLandmarkVector() ;
  
  bool IsLandmarkAvailable(unsigned int id) ;

  bool GetLandmark(unsigned int id, VisualLandmark& landmark)
  { 
    LandmarkSetType::iterator iter = m_Landmarks.find( id ) ;
    if ( iter != m_Landmarks.end() )
      {
      landmark = iter->second ; 
      return true ;
      }
    return false ;
  } 

  void AddLandmark(unsigned int id, ColorType color);
  
  void DeleteLandmark(unsigned int id) ;

  virtual int handle(int event);

  void ConvertColorType( SpatialObjectPointType::PixelType& source, 
                         ColorType& target) ;
  void ConvertColorType( ColorType& source, 
                         SpatialObjectPointType::PixelType& target) ;

  typedef void (*LandmarkChangeCallBack)(void* ptrObject) ;
  void RegisterLandmarkChangeCallBack(void* ptrObject,
                                      LandmarkChangeCallBack callBack) ;

  void ShowLandmarks() ;

  void HideLandmarks() ;

  // Region of interest related functions
  void ShowRegionOfInterest() ;

  RegionType ComputeLandmarkRegion(float scale ) ;

  void InitializeRegionOfInterestWithLandmarks(float scale) ;

  void InitializeRegionOfInterestWithLargestPossibleRegion() ;

  void ResizeRegionOfInterest(unsigned int axis, int step) ;

  void MoveRegionOfInterest(unsigned int axis, int step) ;

  RegionType ComputeIntersectionRegion(RegionType& A, RegionType& B) ;

  void DrawExclusiveRegion(RegionType& target, 
                           RegionType& exclusion, 
                           ColorType& color) ;

  void DrawRegionOfInterest() ;

  void HideRegionOfInterest() ;

  void FitRegion(RegionType& region) ;

  void ApplyRegionOfInterest() ;
  
  bool IsRegionOfInterestAvailable()
  {
    for ( unsigned int i = 0 ; i < 3 ; ++i )
      {
      if ( m_RegionOfInterest.GetSize()[i] > 0 )
        {
        return true ;
        }
      }
    return false ;
  }

  RegionType& GetRegionOfInterest() 
  { return m_RegionOfInterest ; }

protected:
  void DrawLandmark(VisualLandmark& landmark) ;

  LandmarkSetType m_Landmarks;
  LandmarkVectorPointer m_LandmarkVector ;
  SpatialPointsType m_SpatialPoints ;

  enum Action{ None, Add };

  enum Mode{ Normal, Selection };

  Mode m_Mode; 
  Action m_Action;

  unsigned int m_LandmarkCandidateId ;
  VisualLandmark m_LandmarkCandidate ;

  void* m_LandmarkChangeCallBackObject ;
  LandmarkChangeCallBack m_LandmarkChangeCallBack ;

  RegionType m_RegionOfInterest ;
  RegionType m_TempRegionOfInterest ;
  RegionType m_TempPreviousRegionOfInterest ;
  
  typedef std::list< IndexType > IndexListType ;
  IndexListType m_TempRemovalIndexes ;
  IndexListType m_TempAdditionIndexes ;
};

#include "LandmarkSliceViewer.txx"

#endif //__LandmarkSliceViewer_h
