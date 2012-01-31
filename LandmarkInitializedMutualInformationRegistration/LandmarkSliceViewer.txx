/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    LandmarkSliceViewer.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef __LandmarkSliceViewer_txx
#define __LandmarkSliceViewer_txx

#include "LandmarkSliceViewer.h"
#include "itkImageRegionExclusionIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"

template<class TImagePixel>
LandmarkSliceViewer<TImagePixel>
::LandmarkSliceViewer( unsigned int x, unsigned int y,
                       unsigned int w, unsigned int h, const char * label )
  : itk::GLSliceView< TImagePixel, unsigned char >(x,y,w,h,label)
  { 
  m_Mode = Normal;
  m_Action = None;

  m_LandmarkSpatialObject = LandmarkSpatialObjectType::New();
  m_LandmarkPointList = & m_LandmarkSpatialObject->GetPoints();

  m_LandmarkChangeCallBackObject = 0;
  m_LandmarkChangeCallBack = 0;

  IndexType index;
  SizeType  sz;
  index.Fill(0);
  sz.Fill(0);
  m_RegionOfInterest.SetIndex(index);
  m_RegionOfInterest.SetSize(sz);

  this->cColorTable->SetColor(0, 0, 0, 1, "Blue");
  this->cColorTable->SetColor(1, 1, 0, 0, "Red");
  this->cColorTable->SetColor(2, 0, 1, 0, "Green");
  this->cColorTable->SetColor(3, 0.741, 0.86, 0.84, "Yellow");
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
  m_LandmarkChangeCallBackObject = ptrObject;
  m_LandmarkChangeCallBack = callBack;
  } 

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::SetInputImage( ImageType* im )
  {
    itk::GLSliceView< TImagePixel, unsigned char >::SetInputImage(im);

  typename OverlayImageType::Pointer ovly = OverlayImageType::New();
  ovly->SetRegions(im->GetLargestPossibleRegion());
  ovly->SetSpacing(im->GetSpacing());
  ovly->SetOrigin(im->GetOrigin());
  ovly->Allocate();
  ovly->FillBuffer(0);
  ovly->SetReferenceCount(2);
  
  this->SetInputOverlay(ovly);
  this->ViewOverlayData(true);
  }


template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::SetLandmarkList(LandmarkPointListType * landmarkPointList)
  {
  this->HideLandmarks();

  unsigned int sz = m_LandmarkPointList->size();
  for ( unsigned int ii = 0; ii < sz; ++ii )
    {
    this->DeleteLandmark(ii);
    }

  m_LandmarkPointList->clear();

  LandmarkPointListType::iterator iter = landmarkPointList->begin();
  LandmarkPointType landmark;
  unsigned int id = 0;
  while ( iter != landmarkPointList->end() )
    {
    landmark.SetID( id );
    landmark.SetColor( iter->GetColor() );
    landmark.SetPosition( iter->GetPosition() );
    this->SetLandmark( id, landmark );
    ++iter;
    ++id;
    }

  this->ShowLandmarks();

  m_LandmarkPointList = & m_LandmarkSpatialObject->GetPoints();
  }

template<class TImagePixel>
unsigned int
LandmarkSliceViewer<TImagePixel>
::GetNumberOfLandmarks()
  {
  return m_LandmarkPointList->size();
  }

template<class TImagePixel>
typename LandmarkSliceViewer<TImagePixel>::LandmarkPointListType &
LandmarkSliceViewer<TImagePixel>
::GetLandmarkList()
  {
  return * m_LandmarkPointList;
  }


template<class TImagePixel>
bool
LandmarkSliceViewer<TImagePixel>
::IsLandmarkAvailable(unsigned int id)
  {
  if ( id < m_LandmarkPointList->size() &&
       (*m_LandmarkPointList)[id].GetID() == (int)(id))
    {
    return true;
    }
  return false;
  }

template<class TImagePixel>
bool
LandmarkSliceViewer<TImagePixel>
::GetLandmark(unsigned int id, LandmarkPointType & landmark)
  {
  if ( id < m_LandmarkPointList->size() )
    {
    landmark = (*m_LandmarkPointList)[id];
    return true;
    }
  return false;
  }

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::SetLandmark(unsigned int id, LandmarkPointType & landmark)
  {
  if((int)(id) != landmark.GetID())
    {
    std::cout << "Warning: reseting landmark id to match" << std::endl;
    landmark.SetID(id);
    }
  if(id < m_LandmarkPointList->size())
    {
    this->DeleteLandmark( id );
    (*m_LandmarkPointList)[id] = landmark;
    }
  else
    {
    for(unsigned int ii=m_LandmarkPointList->size(); ii<id; ii++)
      {
      m_LandmarkPointList->push_back(*(new LandmarkPointType));
      }
    m_LandmarkPointList->push_back(landmark);
    }
  DrawLandmark( landmark );
  }

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::AddLandmark(unsigned int id, ViewerColorType clr)
  {
  m_LandmarkCandidateId = id;
  m_LandmarkCandidate.SetColor( this->cColorTable->GetColorComponent(clr, 'r'),
                                this->cColorTable->GetColorComponent(clr, 'g'),
                                this->cColorTable->GetColorComponent(clr, 'b'), 1 );
  m_Mode = Selection;
  m_Action = Add;
  }

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::DeleteLandmark(unsigned int id)
  {
  if( id < m_LandmarkPointList->size() )
    {
    if( (*m_LandmarkPointList)[id].GetID() == (int)(id) )
      {
      LandmarkPointType::ColorType clr;
      clr.SetRed(0);
      clr.SetGreen(0);
      clr.SetBlue(0);
      (*m_LandmarkPointList)[id].SetColor( clr );
      DrawLandmark( (*m_LandmarkPointList)[id] );
      (*m_LandmarkPointList)[id].SetID( -1 );
      }
    }
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
        itk::ClickPoint coord;
        this->getClickedPoint(0, coord);
        ContinuousIndexType index;
        index[0] = (double) coord.x;
        index[1] = (double) coord.y;
        index[2] = (double) coord.z;
        PointType point;

        switch( m_Action )
          {
          case Add:
            this->cImData->TransformContinuousIndexToPhysicalPoint( index, point );
            m_LandmarkCandidate.SetPosition( point );
            m_LandmarkCandidate.SetID(m_LandmarkCandidateId);
            this->SetLandmark(m_LandmarkCandidateId, m_LandmarkCandidate);
            this->DrawLandmark( m_LandmarkCandidate );
            if ( m_LandmarkChangeCallBack != 0 )
              {
              m_LandmarkChangeCallBack( m_LandmarkChangeCallBackObject );
              }
            m_Action = None;
            break;
          default:
            break;
          }
        }
      break; // case FL_PUSH
      
    default:
      break;
    }

  return 1;
  }

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::ShowLandmarks() 
  {
  LandmarkPointListType::iterator iter = m_LandmarkPointList->begin();
  while ( iter != m_LandmarkPointList->end() )
    {
    if(iter->GetID() >= 0)
      {
      this->DrawLandmark( *iter );
      }
    ++iter;
    }
  }


template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::HideLandmarks() 
  {
  LandmarkPointType landmark;
  LandmarkPointType::ColorType clr;
  clr.SetRed( 0 );
  clr.SetGreen( 0 );
  clr.SetBlue( 0 );
  
  LandmarkPointListType::iterator iter = m_LandmarkPointList->begin();
  while ( iter != m_LandmarkPointList->end() )
    {
    if(iter->GetID() >= 0)
      {
      landmark = *iter;
      landmark.SetColor(clr);
      this->DrawLandmark(landmark);
      }
    ++iter;
    }
  }

/**
 *
 * Draw the 'index'th landmark with the value passed as second argument.
 *
 */

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::DrawLandmark(LandmarkPointType & landmark) 
  {
  int    xx;
  int    yy;
  int    zz;
  IndexType       modifiedIndex;
  IndexType       index;
  RegionType      region;
  ViewerColorType       clr;

  region = this->cOverlayData->GetLargestPossibleRegion();

  this->cImData->TransformPhysicalPointToIndex(landmark.GetPosition(), index);
  if(landmark.GetColor().GetRed() == 0 &&
     landmark.GetColor().GetGreen() == 0 &&
     landmark.GetColor().GetBlue() == 0)
    {
    clr = 0;
    }
  else
    {
    clr = this->cColorTable->GetClosestColorTableId(landmark.GetColor().GetRed(),
                                            landmark.GetColor().GetGreen(),
                                            landmark.GetColor().GetBlue()) + 1;
    }

  for( int ii=-5; ii<6; ii++ )
    {
    xx = index[0]+ii;
    
    modifiedIndex[0] = xx;
    modifiedIndex[1] = index[1];
    modifiedIndex[2] = index[2];

    if (region.IsInside(modifiedIndex))
      {
      this->cOverlayData->SetPixel(modifiedIndex, clr );
      }
    }

  for( int ii=-5; ii<6; ii++ )
    {
    yy = index[1] + ii;

    modifiedIndex[0] = index[0];
    modifiedIndex[1] = yy;
    modifiedIndex[2] = index[2];

    if (region.IsInside(modifiedIndex))
      {
      this->cOverlayData->SetPixel(modifiedIndex, clr);
      }
    }

  for( int ii=-5; ii<6; ii++ )
    {
    zz = index[2] + ii;

    modifiedIndex[0] = index[0];
    modifiedIndex[1] = index[1];
    modifiedIndex[2] = zz;

    if (region.IsInside(modifiedIndex))
      {
      this->cOverlayData->SetPixel(modifiedIndex, clr);
      }
    }

  this->update();
  this->redraw();
  }

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::ApplyRegionOfInterest()
  {
  m_RegionOfInterest = m_TempRegionOfInterest;
  }

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::FitRegion(RegionType& region)
  {
  OverlayImageType::IndexType lowerBound;
  OverlayImageType::IndexType upperBound;

  OverlayImageType::IndexType beginIndex;
  OverlayImageType::IndexType endIndex;
  OverlayImageType::SizeType sz;
  OverlayImageType::RegionType largestRegion;
  largestRegion = this->cImData->GetLargestPossibleRegion();
  beginIndex = largestRegion.GetIndex();
  for ( unsigned int axis = 0; axis < 3; ++axis )
    {
    sz[axis] = region.GetSize()[axis];
    endIndex[axis] = beginIndex[axis] + largestRegion.GetSize()[axis] - 1;
    lowerBound[axis] = region.GetIndex()[axis];
    upperBound[axis] = lowerBound[axis] + sz[axis] - 1;

    if ( lowerBound[axis] < beginIndex[axis] )
      {
      sz[axis] = sz[axis] - (beginIndex[axis] - lowerBound[axis]);
      lowerBound[axis] = beginIndex[axis];
      }

    if ( upperBound[axis] > endIndex[axis] )
      {
      sz[axis] = sz[axis] - (upperBound[axis] - endIndex[axis]);
      upperBound[axis] = lowerBound[axis] + sz[axis] - 1;
      }
    }
  region.SetIndex(lowerBound);
  region.SetSize(sz);
  }

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::InitializeRegionOfInterestWithLargestPossibleRegion()
  {
  m_TempRegionOfInterest = this->cImData->GetLargestPossibleRegion();
  }

template<class TImagePixel>
typename LandmarkSliceViewer<TImagePixel>::RegionType
LandmarkSliceViewer<TImagePixel>
::ComputeLandmarkRegion(float scale)
  {
  RegionType region;

  IndexType index;
  if(m_LandmarkPointList->size() == 0)
    {
    index.Fill(0);
    region.SetIndex(index);
    OverlayImageType::SizeType sz;
    for ( unsigned int axis = 0; axis < 3; ++axis )
      {
      sz[axis] = 0;
      }
    region.SetSize(sz);

    return region;
    }
  else
    {
    IndexType lowerBound;
    lowerBound.Fill(itk::NumericTraits< long >::max());
    IndexType upperBound;
    upperBound.Fill(itk::NumericTraits< long >::min());

    LandmarkPointListType::iterator iter = m_LandmarkPointList->begin();
    while ( iter != m_LandmarkPointList->end() )
      {
      this->cImData->TransformPhysicalPointToIndex(iter->GetPosition(), index);
      for ( unsigned int axis = 0; axis < 3; ++axis )
        {
        if ( index[axis] < lowerBound[axis] )
          {
          lowerBound[axis] = index[axis];
          }

        if ( index[axis] > upperBound[axis] )
          {
          upperBound[axis] = index[axis];
          }
        }
      ++iter;
      }

    OverlayImageType::SizeType sz;
    for ( unsigned int axis = 0; axis < 3; ++axis )
      {
      sz[axis] = (upperBound[axis] - lowerBound[axis]) + 1;
      }

    if ( scale != 1.0 )
      {
      OverlayImageType::SizeType scaledSize;
      for ( unsigned int axis = 0; axis < 3; ++axis )
        {
        scaledSize[axis] = (long)((float)sz[axis] * scale );
        lowerBound[axis] = 
          lowerBound[axis] - (long)((scaledSize[axis] - sz[axis]) / 2.0);
        }
      region.SetIndex(lowerBound);
      region.SetSize(scaledSize);
      }
    else
      {
      region.SetIndex(lowerBound);
      region.SetSize(sz);
      }
    this->FitRegion(region);
    }

  return region;
  }

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::InitializeRegionOfInterestWithLandmarks(float scale)
  {
  if(m_LandmarkPointList->size() > 0)
    {
    m_TempRegionOfInterest = this->ComputeLandmarkRegion(scale);
    }
  }

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::ResizeRegionOfInterest(unsigned int axis, int step)
  {
  OverlayImageType::SizeType sz;
  sz = m_TempRegionOfInterest.GetSize();
  sz[axis] = sz[axis] + step;

  m_TempRegionOfInterest.SetSize(sz);
  this->FitRegion(m_TempRegionOfInterest);
  this->DrawRegionOfInterest();
  }

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::MoveRegionOfInterest(unsigned int axis, int step)
  {
  OverlayImageType::IndexType index;
  index = m_TempRegionOfInterest.GetIndex();
  if ( index[axis] < 0 )
    {
    index[axis] = index[axis] - step; 
    }
  else
    {
    index[axis] = index[axis] + step; 
    }

  m_TempRegionOfInterest.SetIndex(index);
  
  this->FitRegion(m_TempRegionOfInterest);
  this->DrawRegionOfInterest();
  }

template<class TImagePixel>
typename LandmarkSliceViewer<TImagePixel>::RegionType
LandmarkSliceViewer<TImagePixel>
::ComputeIntersectionRegion(RegionType& A, 
                            RegionType& B)
  {
  OverlayImageType::RegionType intersectionRegion;
  OverlayImageType::SizeType intersectionRegionSize;
  OverlayImageType::IndexType intersectionBeginIndex;
  OverlayImageType::IndexType intersectionEndIndex;
  
  for ( unsigned int ii = 0; ii < 3; ++ii )
    {
    if ( A.GetIndex()[ii] > B.GetIndex()[ii] )
      {
      intersectionBeginIndex[ii] = A.GetIndex()[ii];
      }
    else
      {
      intersectionBeginIndex[ii] = B.GetIndex()[ii];
      }
    
    if ( (A.GetIndex()[ii] + A.GetSize()[ii] - 1) <
         (B.GetIndex()[ii] + B.GetSize()[ii] - 1) )
      {
      intersectionEndIndex[ii] = A.GetIndex()[ii] + A.GetSize()[ii] - 1;
      }
    else
      {
      intersectionEndIndex[ii] = B.GetIndex()[ii] + B.GetSize()[ii] - 1;
      }
    
    intersectionRegionSize[ii] =
      intersectionEndIndex[ii] - intersectionBeginIndex[ii] + 1;
    }
  intersectionRegion.SetIndex(intersectionBeginIndex);
  intersectionRegion.SetSize(intersectionRegionSize);
  return intersectionRegion;
  }

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::DrawExclusiveRegion(RegionType& target, 
                      RegionType& exclusion, 
                      ViewerColorType& clr)
  {
  typedef itk::ImageRegionIteratorWithIndex< OverlayImageType >
    ImageIteratorType;

  IndexType tempIndex;
  SizeType tempSize;
  IndexType leftoverIndex;
  SizeType leftoverSize;
  RegionType piece;
  RegionType leftover = target;
  unsigned int dim = 0;
  while ( dim < 3 )
    {
    tempIndex = leftover.GetIndex();
    tempSize = leftover.GetSize();
    leftoverIndex = leftover.GetIndex();
    leftoverSize = leftover.GetSize();
    if ( leftover.GetIndex()[dim] < exclusion.GetIndex()[dim] )
      {
      tempIndex[dim] = leftover.GetIndex()[dim];
      tempSize[dim] = exclusion.GetIndex()[dim] - leftover.GetIndex()[dim];
      leftoverIndex[dim] = exclusion.GetIndex()[dim];
      }
    else
      {
      tempIndex[dim] = 
        exclusion.GetIndex()[dim] + exclusion.GetSize()[dim];
      tempSize[dim] = leftover.GetSize()[dim] - exclusion.GetSize()[dim];
      leftoverIndex[dim] = exclusion.GetIndex()[dim];
      }


    piece.SetIndex(tempIndex);
    piece.SetSize(tempSize);
    
    if ( tempSize[dim] !=0 )
      {
      ImageIteratorType iter(this->cOverlayData, piece);
      iter.GoToBegin();
      while ( !iter.IsAtEnd() )
        {
        this->cOverlayData->SetPixel(iter.GetIndex(), clr);
        ++iter;
        }
      leftoverSize[dim] = leftover.GetSize()[dim] - tempSize[dim];
      leftover.SetIndex(leftoverIndex);
      leftover.SetSize(leftoverSize);
      }
    else
      {
      ++dim;
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
    m_TempRegionOfInterest = m_RegionOfInterest;
    }
  else
    {
    if ( this->GetNumberOfLandmarks() > 3 )
      {
      this->InitializeRegionOfInterestWithLandmarks(1.0f);
      }
    else
      {
      this->InitializeRegionOfInterestWithLargestPossibleRegion();
      }
    }

  this->DrawRegionOfInterest();
  }

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::HideRegionOfInterest()
  {
  typedef itk::ImageRegionIteratorWithIndex< OverlayImageType >
    ImageIteratorType;
  ImageIteratorType c_iter(this->cOverlayData, m_TempPreviousRegionOfInterest);
  
  while ( !c_iter.IsAtEnd() )
    {
    this->cOverlayData->SetPixel(c_iter.GetIndex(), 0);
    ++c_iter;
    }
  this->update();
  this->redraw();
  OverlayImageType::SizeType sz;
  sz.Fill(0);
  m_TempPreviousRegionOfInterest.SetSize(sz);
  }

template<class TImagePixel>
void
LandmarkSliceViewer<TImagePixel>
::DrawRegionOfInterest()
  {
  ViewerColorType clr;
  
  typedef itk::ImageRegionIteratorWithIndex< OverlayImageType >
    ImageIteratorType;

  if ( m_TempPreviousRegionOfInterest.GetSize()[0] > 0 ||
       m_TempPreviousRegionOfInterest.GetSize()[1] > 0 ||
       m_TempPreviousRegionOfInterest.GetSize()[2] > 0 )
    {

    RegionType intersection = 
      this->ComputeIntersectionRegion(m_TempPreviousRegionOfInterest,
                                      m_TempRegionOfInterest);

    clr = 0;
    this->DrawExclusiveRegion(m_TempPreviousRegionOfInterest,
                              intersection, clr );
    clr = 1;
    this->DrawExclusiveRegion(m_TempRegionOfInterest, intersection, clr);
    }
  else
    {
    ImageIteratorType c_iter(this->cOverlayData, m_TempRegionOfInterest);
    
    clr = 1;
    while ( !c_iter.IsAtEnd() )
      {
      this->cOverlayData->SetPixel(c_iter.GetIndex(), clr);
      ++c_iter;
      }
    }
  this->update();
  this->redraw();
  m_TempPreviousRegionOfInterest = m_TempRegionOfInterest;
  }

#endif
