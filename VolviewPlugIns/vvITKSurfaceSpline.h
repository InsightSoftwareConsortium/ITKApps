/** Generic interface for protocol communication between an ITK filter
    and the VolView Plugin Interface */

#ifndef _itkVVSurfaceSpline_h
#define _itkVVSurfaceSpline_h

#include "vtkVVPluginAPI.h"

#include <string.h>
#include <stdlib.h>
#include <fstream>

#include "vvITKFilterModuleBase.h"

#include "itkImportImageFilter.h"

#include "itkThinPlateSplineKernelTransform.h"

namespace VolView 
{

namespace PlugIn
{

template <class TInputPixelType >
class SurfaceSpline : public FilterModuleBase {

public:

   // Pixel type of the input buffer
  typedef TInputPixelType                InputPixelType;

  itkStaticConstMacro( Dimension, unsigned int, 3 );

  typedef itk::Image< InputPixelType,  Dimension >  InputImageType;

  // Instantiate the ImportImageFilter
  // This filter is used for building an ITK image using 
  // the data passed in a buffer.
  typedef itk::ImportImageFilter< InputPixelType, 
                                  Dimension       > ImportFilterType;

  typedef typename ImportFilterType::RegionType    RegionType;
  typedef typename RegionType::SizeType            SizeType;
  typedef typename RegionType::IndexType           IndexType;

  typedef float                                    CoordinateRepresentationType;
  typedef itk::Point< CoordinateRepresentationType,
                                         Dimension 
                                             >     PointType;
  typedef std::vector< PointType >                 PointListType;
  typedef typename PointListType::iterator         PointIterator;
  typedef typename PointListType::const_iterator   ConstPointIterator;

  typedef itk::ThinPlateSplineKernelTransform< 
                            CoordinateRepresentationType,
                            Dimension >            SplineType;                
  typedef typename SplineType::Pointer             SplinePointer;

  typedef typename SplineType::PointSetType        PointSetType;
  typedef typename PointSetType::Pointer           PointSetPointer;

  typedef typename PointSetType::PointsContainer   LandmarkContainer;
  typedef typename LandmarkContainer::Pointer      LandmarkContainerPointer;
  typedef typename LandmarkContainer::Iterator     LandmarkIterator;
  typedef typename PointSetType::PointType         LandmarkType;


  //  Set the number of points along the rows
  void SetNumberOfPointsAlongRows( unsigned int );

  //  Set the number of points along the columns
  void SetNumberOfPointsAlongColumns( unsigned int );
     

public:
    SurfaceSpline();
   ~SurfaceSpline();

    void ProcessData( const vtkVVProcessDataStruct * pds );
    void PostProcessData( const vtkVVProcessDataStruct * pds );

private:
    typename ImportFilterType::Pointer              m_ImportFilter;

    unsigned int                                    m_SidePointsCol;
    unsigned int                                    m_SidePointsRow;

    SplinePointer                                   m_Spline;

    PointListType                                   m_SourcePoints;
    PointListType                                   m_TargetPoints;

    PointSetPointer                                 m_SourceLandmarks;
    PointSetPointer                                 m_TargetLandmarks;

    std::ofstream  ofs;


};

} // end of namespace PlugIn

} // end of namespace Volview

#endif
