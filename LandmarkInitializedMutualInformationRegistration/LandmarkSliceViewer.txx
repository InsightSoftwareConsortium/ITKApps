#ifndef __LandmarkSliceViewer_txx
#define __LandmarkSliceViewer_txx

#include "LandmarkSliceViewer.h"
#include "itkImageRegionExclusionIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"

template<class TImagePixel>
LandmarkSliceViewer<TImagePixel>
::LandmarkSliceViewer( unsigned int x, unsigned int y, unsigned int w, unsigned int h, const char * label=0 )
  : Superclass(x,y,w,h,label)
{ 
  m_Mode = Normal ;
  m_Action = None ;

  m_LandmarkVector = LandmarkVectorType::New() ;
  m_LandmarkChangeCallBackObject = 0 ;
  m_LandmarkChangeCallBack = 0 ;

  typename OverlayImageType::IndexType index ;
  typename OverlayImageType::SizeType size ;
  index.Fill(0) ;
  size.Fill(0) ;
  m_RegionOfInterest.SetIndex(index) ;
  m_RegionOfInterest.SetSize(size) ;
}

/**
 *
 * Destructor
 *
 */

template<class TImagePixel>
LandmarkSliceViewer<TImagePixel>
::~LandmarkSliceViewer()
{

}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::RegisterLandmarkChangeCallBack(void* ptrObject,
                                 LandmarkChangeCallBack callBack)
{ 
  m_LandmarkChangeCallBackObject = ptrObject ;
  m_LandmarkChangeCallBack = callBack ;
} 

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::SetInputImage( ImageType* image )
{
  Superclass::SetInputImage(image) ;

  OverlayPixelType opixel ;
  opixel.Fill(0) ;
  typename OverlayImageType::Pointer overlay = OverlayImageType::New();
  overlay->SetRegions(image->GetLargestPossibleRegion());
  overlay->SetSpacing(image->GetSpacing());
  overlay->SetOrigin(image->GetOrigin());
  overlay->Allocate();
  overlay->FillBuffer(opixel);
  
  this->SetInputOverlay(overlay.GetPointer()) ;
  this->ViewOverlayData(true);
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::SetSpatialPoints(SpatialPointsType points)
{
  unsigned int size = m_Landmarks.size() ;
  for ( unsigned int i = 0 ; i < size ; ++i )
    {
    this->DeleteLandmark(i) ;
    }

  m_Landmarks.clear() ;

  SpatialPointsType::iterator iter = points.begin() ;
  unsigned int id = 0 ;
  IndexType index ;
  VisualLandmark landmark ;

  while ( iter != points.end() )
    {
    cOverlayData->TransformPhysicalPointToIndex( iter->GetPosition(), 
                                                 index ) ;
    landmark.SetIndex( index ) ;
    ColorType color ;
    for ( unsigned int i = 0 ; i < 4 ; ++i )
      {
      color[i] = (unsigned char) iter->GetColor()[i] ;
      }
    landmark.SetColor( color ) ;
    landmark.SetPoint( iter->GetPosition() ) ;
    this->DrawLandmark( landmark ) ;
    m_Landmarks[id] = landmark ;
    ++iter ;
    ++id ;
    }

//   if ( m_LandmarkChangeCallBack != 0 )
//     {
//     m_LandmarkChangeCallBack( m_LandmarkChangeCallBackObject ) ;
//     }
  
  this->update() ;
  this->redraw() ;
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::ConvertColorType( ColorType& source, 
                    SpatialObjectPointType::PixelType& target)
{
  for ( unsigned int i = 0 ; i < 4 ; ++i )
    {
    target[i] = (float) source[i] ;
    }
}


template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::ConvertColorType( SpatialObjectPointType::PixelType& source, 
                    ColorType& target)
{
  for ( unsigned int i = 0 ; i < 4 ; ++i )
    {
    target[i] = (unsigned char) source[i] ;
    }
}

template<class TImagePixel>
typename LandmarkSliceViewer<TImagePixel>::SpatialPointsType&
LandmarkSliceViewer<TImagePixel>
::GetSpatialPoints()
{
  m_SpatialPoints.clear() ;
  SpatialObjectPointType landmarkPoint ;
  PointType point ;
  ColorType color ;
  SpatialObjectPointType::PixelType soColor ;
  typename LandmarkSetType::iterator iter = m_Landmarks.begin() ;
  while ( iter != m_Landmarks.end() )
    {
    point = iter->second.GetPoint() ;
    color = iter->second.GetColor() ;
    ConvertColorType(color, soColor) ;
    landmarkPoint.SetPosition( point ) ;
    landmarkPoint.SetColor( soColor ) ;
    m_SpatialPoints.push_back( landmarkPoint ) ;
    ++iter ;
    }
  
  return m_SpatialPoints ;
}

template<class TImagePixel>
typename LandmarkSliceViewer<TImagePixel>::LandmarkVectorType*
LandmarkSliceViewer<TImagePixel>
::GetLandmarkVector()
{
  m_LandmarkVector->Reserve(m_Landmarks.size()) ;
  typename LandmarkSetType::iterator iter = m_Landmarks.begin() ;
  unsigned int id = 0 ;
  while ( iter != m_Landmarks.end() )
    {
    m_LandmarkVector->InsertElement( id, iter->second.GetPoint() ) ;
    ++id ;
    ++iter ;
    }
  return m_LandmarkVector ;
}

template<class TImagePixel>
bool
LandmarkSliceViewer<TImagePixel>
::IsLandmarkAvailable(unsigned int id)
{
  if ( m_Landmarks.find( id ) != m_Landmarks.end() )
    {
    return true ;
    }
  else
    {
    return false ;
    }
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::AddLandmark(unsigned int id, ColorType color)
{
  m_LandmarkCandidateId = id ;
  m_LandmarkCandidate.SetColor( color ) ;
  m_Mode = Selection ;
  m_Action = Add;
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::DeleteLandmark(unsigned int id)
{
  VisualLandmark landmark ;
  typename LandmarkSetType::iterator iter = m_Landmarks.find( id ) ;
  landmark = (iter)->second ;
  ColorType color ;
  color.Fill( 0 ) ;
  landmark.SetColor( color ) ;
  DrawLandmark( landmark );
  m_Landmarks.erase( id ) ;
  this->update() ;
  this->redraw() ;
}

template<class TImagePixel>
int 
LandmarkSliceViewer<TImagePixel>
::handle( int event )
{
  Superclass::handle(event);

  switch(event)
    {
    case FL_PUSH:
      if( m_Mode == Selection )
        {
        ClickPoint coord ;
        this->getClickedPoint(0, coord);
        IndexType index ;
        index[0] = (long) coord.x ;
        index[1] = (long) coord.y ;
        index[2] = (long) coord.z ;
        PointType point ;

        switch( m_Action )
          {
          case Add:
            m_LandmarkCandidate.SetIndex( index ) ;
            cOverlayData->TransformIndexToPhysicalPoint( index, point ) ;
            m_LandmarkCandidate.SetPoint( point ) ;
            m_Landmarks[m_LandmarkCandidateId] = m_LandmarkCandidate ;
            this->DrawLandmark( m_LandmarkCandidate ) ;
            if ( m_LandmarkChangeCallBack != 0 )
              {
              m_LandmarkChangeCallBack( m_LandmarkChangeCallBackObject ) ;
              }
            m_Action = None;
            this->update() ;
            this->redraw() ;
            break;
          default:
            break;
          }
        }
      break; // case FL_PUSH
      
    default:
      break;
    }

  return 1 ;
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::ShowLandmarks() 
{
  LandmarkSetType::iterator iter = m_Landmarks.begin() ;
  while ( iter != m_Landmarks.end() )
    {
    this->DrawLandmark( iter->second ) ;
    ++iter ;
    }
  this->update() ;
  this->redraw() ;
}


template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::HideLandmarks() 
{
  VisualLandmark landmark ;
  ColorType color ;
  color.Fill(0) ;
  color[3] = 255 ;
  
  LandmarkSetType::iterator iter = m_Landmarks.begin() ;
  while ( iter != m_Landmarks.end() )
    {
    landmark = iter->second ;
    landmark.SetColor(color) ;
    this->DrawLandmark(landmark) ;
    ++iter ;
    }
  this->update() ;
  this->redraw() ;
}

/**
 *
 * Draw the 'index'th landmark with the value passed as second argument.
 *
 */

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::DrawLandmark(VisualLandmark& landmark) 
{
  int    x;
  int    y;
  SizeType        size; 
  IndexType       modifiedIndex;
  
  size = cOverlayData->GetLargestPossibleRegion().GetSize();
  OverlayPixelType o_color ;
  for ( unsigned int i = 0 ; i < 4 ; i++ )
    {
    o_color[i] = landmark.GetColor()[i] ;
    }

  for( int i=-5; i<6; i++ )
    {
    x = landmark.GetIndex()[0]+i;
    
    if( x < 0 ) 
      {
      x = 0;
      }
    
    if( x >= static_cast<int>(size[0]) )
      {
      x = size[0];
      }

    modifiedIndex[0] = x;
    modifiedIndex[1] = (landmark.GetIndex())[1] ;
    modifiedIndex[2] = (landmark.GetIndex())[2];

    cOverlayData->SetPixel(modifiedIndex, o_color);
    }

  for( int i=-5; i<6; i++ )
    {

    y = (landmark.GetIndex())[1] + i;

    if( y < 0 ) 
      {
      y = 0;
      }

    if( y >= static_cast<int>(size[1]) )
      {
      y = size[1];
      }

    modifiedIndex[0] = (landmark.GetIndex())[0];
    modifiedIndex[1] = y;
    modifiedIndex[2] = (landmark.GetIndex())[2];

    cOverlayData->SetPixel(modifiedIndex, o_color);
    }
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::ApplyRegionOfInterest()
{
  m_RegionOfInterest = m_TempRegionOfInterest ;
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::FitRegion(RegionType& region)
{
  OverlayImageType::IndexType lowerBound ;
  OverlayImageType::IndexType upperBound ;

  OverlayImageType::IndexType beginIndex ;
  OverlayImageType::IndexType endIndex ;
  OverlayImageType::SizeType size ;
  OverlayImageType::RegionType largestRegion ;
  largestRegion = cOverlayData->GetLargestPossibleRegion() ;
  beginIndex = largestRegion.GetIndex() ;
  for ( unsigned int axis = 0 ; axis < 3 ; ++axis )
    {
    size[axis] = region.GetSize()[axis] ;
    endIndex[axis] = beginIndex[axis] + largestRegion.GetSize()[axis] - 1 ;
    lowerBound[axis] = region.GetIndex()[axis] ;
    upperBound[axis] = lowerBound[axis] + size[axis] - 1 ;

    if ( lowerBound[axis] < beginIndex[axis] )
      {
      size[axis] = size[axis] - (beginIndex[axis] - lowerBound[axis]) ;
      lowerBound[axis] = beginIndex[axis] ;
      }

    if ( upperBound[axis] > endIndex[axis] )
      {
      size[axis] = size[axis] - (upperBound[axis] - endIndex[axis]) ;
      upperBound[axis] = lowerBound[axis] + size[axis] - 1 ;
      }
    }
  region.SetIndex(lowerBound) ;
  region.SetSize(size) ;
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::InitializeRegionOfInterestWithLargestPossibleRegion()
{
  m_TempRegionOfInterest = cOverlayData->GetLargestPossibleRegion() ;
}

template<class TImagePixel>
typename LandmarkSliceViewer<TImagePixel>::RegionType
LandmarkSliceViewer<TImagePixel>
::ComputeLandmarkRegion(float scale)
{
  RegionType region ;

  IndexType lowerBound ;
  lowerBound.Fill(itk::NumericTraits< long >::max()) ;
  IndexType upperBound ;
  upperBound.Fill(itk::NumericTraits< long >::min()) ;

  LandmarkSetType::iterator iter = m_Landmarks.begin() ;
  IndexType index ;
  while ( iter != m_Landmarks.end() )
    {
    index = iter->second.GetIndex() ;
    for ( unsigned int axis = 0 ; axis < 3 ; ++axis )
      {
      if ( index[axis] < lowerBound[axis] )
        {
        lowerBound[axis] = index[axis] ;
        }

      if ( index[axis] > upperBound[axis] )
        {
        upperBound[axis] = index[axis] ;
        }
      }
    ++iter ;
    }

  OverlayImageType::SizeType size ;
  for ( unsigned int axis = 0 ; axis < 3 ; ++axis )
    {
    size[axis] = (upperBound[axis] - lowerBound[axis]) + 1 ;
    }

  if ( scale != 1.0 )
    {
    OverlayImageType::SizeType scaledSize ;
    for ( unsigned int axis = 0 ; axis < 3 ; ++axis )
      {
      scaledSize[axis] = (long)((float)size[axis] * scale ) ;
      lowerBound[axis] = 
        lowerBound[axis] - (long)((scaledSize[axis] - size[axis]) / 2.0) ;
      }
    region.SetIndex(lowerBound) ;
    region.SetSize(scaledSize) ;
    }
  else
    {
    region.SetIndex(lowerBound) ;
    region.SetSize(size) ;
    }
  this->FitRegion(region) ;

  return region ;
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::InitializeRegionOfInterestWithLandmarks(float scale)
{
  m_TempRegionOfInterest = this->ComputeLandmarkRegion(scale) ;
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::ResizeRegionOfInterest(unsigned int axis, int step)
{
  OverlayImageType::SizeType size ;
  size = m_TempRegionOfInterest.GetSize() ;
  size[axis] = size[axis] + step ;

  m_TempRegionOfInterest.SetSize(size) ;
  this->FitRegion(m_TempRegionOfInterest) ;
  this->DrawRegionOfInterest() ;
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::MoveRegionOfInterest(unsigned int axis, int step)
{
  OverlayImageType::IndexType index ;
  index = m_TempRegionOfInterest.GetIndex() ;
  if ( index[axis] < 0 )
    {
    index[axis] = index[axis] - step ; 
    }
  else
    {
    index[axis] = index[axis] + step ; 
    }

  m_TempRegionOfInterest.SetIndex(index) ;
  
  this->FitRegion(m_TempRegionOfInterest) ;
  this->DrawRegionOfInterest() ;
}

template<class TImagePixel>
typename LandmarkSliceViewer<TImagePixel>::RegionType
LandmarkSliceViewer<TImagePixel>
::ComputeIntersectionRegion(RegionType& A, 
                            RegionType& B)
{
  OverlayImageType::RegionType intersectionRegion ;
  OverlayImageType::SizeType intersectionRegionSize ;
  OverlayImageType::IndexType intersectionBeginIndex ;
  OverlayImageType::IndexType intersectionEndIndex ;
  
  for ( unsigned int i = 0 ; i < 3 ; ++i )
    {
    if ( A.GetIndex()[i] > B.GetIndex()[i] )
      {
      intersectionBeginIndex[i] = A.GetIndex()[i] ;
      }
    else
      {
      intersectionBeginIndex[i] = B.GetIndex()[i] ;
      }
    
    if ( (A.GetIndex()[i] + A.GetSize()[i] - 1) < 
         (B.GetIndex()[i] + B.GetSize()[i] - 1) )
      {
      intersectionEndIndex[i] = A.GetIndex()[i] + A.GetSize()[i] - 1 ;
      }
    else
      {
      intersectionEndIndex[i] = B.GetIndex()[i] + B.GetSize()[i] - 1 ;
      }
    
    intersectionRegionSize[i] =
      intersectionEndIndex[i] - intersectionBeginIndex[i] + 1 ;
    }
  intersectionRegion.SetIndex(intersectionBeginIndex) ;
  intersectionRegion.SetSize(intersectionRegionSize) ;
  return intersectionRegion ;
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::DrawExclusiveRegion(RegionType& target, 
                      RegionType& exclusion, 
                      ColorType& color)
{
  typedef itk::ImageRegionIteratorWithIndex< OverlayImageType >
    ImageIteratorType ;

  IndexType tempIndex ;
  SizeType tempSize ;
  IndexType leftoverIndex ;
  SizeType leftoverSize ;
  RegionType piece ;
  RegionType leftover = target ;
  unsigned int dim = 0 ;
  while ( dim < 3 )
    {
    tempIndex = leftover.GetIndex() ;
    tempSize = leftover.GetSize() ;
    leftoverIndex = leftover.GetIndex() ;
    leftoverSize = leftover.GetSize() ;
    if ( leftover.GetIndex()[dim] < exclusion.GetIndex()[dim] )
      {
      tempIndex[dim] = leftover.GetIndex()[dim] ;
      tempSize[dim] = exclusion.GetIndex()[dim] - leftover.GetIndex()[dim] ;
      leftoverIndex[dim] = exclusion.GetIndex()[dim] ;
      }
    else
      {
      tempIndex[dim] = 
        exclusion.GetIndex()[dim] + exclusion.GetSize()[dim] ;
      tempSize[dim] = leftover.GetSize()[dim] - exclusion.GetSize()[dim] ;
      leftoverIndex[dim] = exclusion.GetIndex()[dim] ;
      }


    piece.SetIndex(tempIndex) ;
    piece.SetSize(tempSize) ;
    
    if ( tempSize[dim] !=0 )
      {
      ImageIteratorType iter(cOverlayData, piece) ;
      while ( !iter.IsAtEnd() )
        {
        cOverlayData->SetPixel(iter.GetIndex(), color);
        ++iter ;
        }
      leftoverSize[dim] = leftover.GetSize()[dim] - tempSize[dim] ;
      leftover.SetIndex(leftoverIndex) ;
      leftover.SetSize(leftoverSize) ;
      }
    else
      {
      ++dim ;
      }
    }
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::ShowRegionOfInterest()
{
  if ( m_RegionOfInterest.GetSize()[0] != 0 ||
       m_RegionOfInterest.GetSize()[0] != 0 ||
       m_RegionOfInterest.GetSize()[0] != 0 )
    {
    m_TempRegionOfInterest = m_RegionOfInterest ;
    }
  else
    {
    if ( this->GetNumberOfLandmarks() > 3 )
      {
      this->InitializeRegionOfInterestWithLandmarks(1.0f) ;
      }
    else
      {
      this->InitializeRegionOfInterestWithLargestPossibleRegion() ;
      }
    }

  this->DrawRegionOfInterest() ;
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::HideRegionOfInterest()
{
  ColorType color ;
  typedef itk::ImageRegionIteratorWithIndex< OverlayImageType >
    ImageIteratorType ;
  ImageIteratorType c_iter(cOverlayData, m_TempPreviousRegionOfInterest) ;
  
  color.Fill(0) ;
  color[3] = 128 ;
  while ( !c_iter.IsAtEnd() )
    {
    cOverlayData->SetPixel(c_iter.GetIndex(), color);
    ++c_iter ;
    }
  this->update() ;
  this->redraw() ;
  OverlayImageType::SizeType size ;
  size.Fill(0) ;
  m_TempPreviousRegionOfInterest.SetSize(size) ;
}

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::DrawRegionOfInterest()
{
  ColorType color ;
  
  typedef itk::ImageRegionIteratorWithIndex< OverlayImageType >
    ImageIteratorType ;

  if ( m_TempPreviousRegionOfInterest.GetSize()[0] > 0 ||
       m_TempPreviousRegionOfInterest.GetSize()[0] > 0 ||
       m_TempPreviousRegionOfInterest.GetSize()[0] > 0 )
    {

    RegionType intersection = 
      this->ComputeIntersectionRegion(m_TempPreviousRegionOfInterest,
                                      m_TempRegionOfInterest) ;

    color.Fill(0) ;
    color[3] = 128 ;
    this->DrawExclusiveRegion(m_TempPreviousRegionOfInterest,
                              intersection, color ) ;
    color.Fill(0) ;
    color[0] = 255 ;
    color[3] = 128 ;
    this->DrawExclusiveRegion(m_TempRegionOfInterest, intersection, color) ;
    }
  else
    {
    ImageIteratorType c_iter(cOverlayData, m_TempRegionOfInterest) ;
    
    color.Fill(0) ;
    color[0] = 255 ;
    color[3] = 128 ;
    while ( !c_iter.IsAtEnd() )
      {
      cOverlayData->SetPixel(c_iter.GetIndex(), color);
      ++c_iter ;
      }
    }
  this->update() ;
  this->redraw() ;
  m_TempPreviousRegionOfInterest = m_TempRegionOfInterest ;
}

#endif
