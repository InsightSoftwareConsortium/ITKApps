/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    LandmarkSliceViewer.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __LandmarkSliceViewer_h
#define __LandmarkSliceViewer_h

#include <vector>
#include <map>
#include "itkExceptionObject.h"
#include "itkSpatialObjectPoint.h"
#include "itkLandmarkSpatialObject.h"
#include "LandmarkRegistrator.h"
#include "GLSliceView.h"

template< class TImagePixel >
class LandmarkSliceViewer : public GLSliceView< TImagePixel,
                                                unsigned char >
  {
  public:

    LandmarkSliceViewer(unsigned int x, unsigned int y, 
                        unsigned int w, unsigned int h, const char * label=0);
  
    ~LandmarkSliceViewer();
  
    typedef LandmarkSliceViewer< TImagePixel >          Self;
    typedef GLSliceView< TImagePixel, unsigned char >   Superclass;
  
    typedef unsigned char  ViewerColorType;

    typedef typename Superclass::SizeType     SizeType;
    typedef typename Superclass::RegionType   RegionType;
    typedef typename Superclass::ImageType    ImageType;
  
    typedef itk::ExceptionObject              ExceptionType;
  
    typedef unsigned char                          OverlayPixelType;
    typedef itk::Image< OverlayPixelType, 3 >      OverlayImageType;
  
    typedef typename ImageType::IndexType          IndexType;
    typedef itk::ContinuousIndex< double, 3 >      ContinuousIndexType;
    typedef itk::Point< double, 3 >                PointType;
    
    typedef itk::LandmarkSpatialObject< 3 >         LandmarkSpatialObjectType;
    typedef LandmarkSpatialObjectType::PointListType  LandmarkPointListType;
    typedef LandmarkSpatialObjectType::LandmarkPointType LandmarkPointType;
    
    void SetInputImage(ImageType* image);
  
    // Landmark functions
    unsigned int GetNumberOfLandmarks();
    //itkSetObjectMacro(LandmarkSpatialObject, LandmarkSpatialObjectType);
    const LandmarkSpatialObjectType * GetLandmarkSpatialObject(void)
      {
      return m_LandmarkSpatialObject.GetPointer();
      }

    LandmarkPointListType & GetLandmarkList(void);
    void SetLandmarkList(LandmarkPointListType * landmarkPointList);
    bool IsLandmarkAvailable(unsigned int id);
    bool GetLandmark(unsigned int id, LandmarkPointType & landmark);
    void SetLandmark(unsigned int id, LandmarkPointType & landmark);
    void AddLandmark(unsigned int id, ViewerColorType color);
    void DeleteLandmark(unsigned int id);
    void ShowLandmarks();
    void HideLandmarks();
    typedef void (*LandmarkChangeCallBack)(void* ptrObject);
    void RegisterLandmarkChangeCallBack(void* ptrObject,
                                        LandmarkChangeCallBack callBack);
  
  
    virtual int handle(int event);
  
    // Region of interest related functions
    void ShowRegionOfInterest();
  
    RegionType ComputeLandmarkRegion(float scale );
  
    void InitializeRegionOfInterestWithLandmarks(float scale);
  
    void InitializeRegionOfInterestWithLargestPossibleRegion();
  
    void ResizeRegionOfInterest(unsigned int axis, int step);
  
    void MoveRegionOfInterest(unsigned int axis, int step);
  
    RegionType ComputeIntersectionRegion(RegionType& A, RegionType& B);
  
    void DrawExclusiveRegion(RegionType& target, 
                             RegionType& exclusion, 
                             ViewerColorType& color);
  
    void DrawRegionOfInterest();
  
    void HideRegionOfInterest();
  
    void FitRegion(RegionType& region);
  
    void ApplyRegionOfInterest();
    
    bool IsRegionOfInterestAvailable()
      {
      for ( unsigned int i = 0; i < 3; ++i )
        {
        if ( m_RegionOfInterest.GetSize()[i] > 0 )
          {
          return true;
          }
        }
      return false;
      }
  
    RegionType& GetRegionOfInterest() 
      { return m_RegionOfInterest; }
  
  protected:
  
    void DrawLandmark(LandmarkPointType & landmark);
  
    LandmarkSpatialObjectType::Pointer     m_LandmarkSpatialObject;
    LandmarkPointListType *                m_LandmarkPointList;
  
    enum Action{ None, Add };
  
    enum Mode{ Normal, Selection };
  
    Mode                   m_Mode; 
    Action                 m_Action;
  
    unsigned int           m_LandmarkCandidateId;
    LandmarkPointType      m_LandmarkCandidate;
  
    void *                 m_LandmarkChangeCallBackObject;
    LandmarkChangeCallBack m_LandmarkChangeCallBack;
  
    RegionType             m_RegionOfInterest;
    RegionType             m_TempRegionOfInterest;
    RegionType             m_TempPreviousRegionOfInterest;
    
    typedef std::list< IndexType >      IndexListType;
    IndexListType          m_TempRemovalIndexes;
    IndexListType          m_TempAdditionIndexes;
  };
  
#include "LandmarkSliceViewer.txx"
  
#endif //__LandmarkSliceViewer_h
