/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DeformableRegistration3DTimeSeriesBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/** 
  \class DeformableRegistration3DTimeSeriesBase
  \brief Class for displaying with VTK the 3D image data.

  This class is the base class for 4D Image Analysis.

*/
#ifndef __DeformableRegistration3DTimeSeriesBase_h
#define __DeformableRegistration3DTimeSeriesBase_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkStructuredPoints.h"
#include "vtkArrowSource.h"
#include "vtkMaskPoints.h"
#include "vtkGlyph3D.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkImageDataGeometryFilter.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkPointData.h"
#include "vtkCommand.h"
#include "vtkSphereSource.h"
#include "vtkGeometryFilter.h"
#include "itkPoint.h"
#include "vtkWarpVector.h"
#include "vtkThresholdPoints.h" 
#include "vtkCardinalSpline.h"
#include "vtkTubeFilter.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPlaneSource.h"
#include "vtkAxes.h"
#include "vtkImageBlend.h"  

namespace fltk{

class DeformableRegistration3DTimeSeriesBase 
{
public:
  typedef itk::Point<double,3> PointType;
  void SetImageVtk( vtkImageData * image );
  void SetImageVtkExtent( vtkImageData * image );
  void SetDeformableFieldImageVtk( vtkImageData * image );
  void SetSegmentedImageVtk( vtkImageData * image );
  void SetupCamera(vtkImageData * image);
   /**
  The SelectSlice method provides slice number of the image data to be viewed
  in the set orientation type. 
  */
  void Select3DView(vtkImageData * image );
  void Select3DFieldView(vtkImageData * image );
  void SelectSlice( int );
  void SelectAxis( int );
  void SelectFieldType(int );
  void ActivateDeformableFieldData();
  void HideDeformableFieldData();
  void ShowDeformableFieldData();
  void ActivateRawData();
  void ActivateSegmentedData();
  int GetSlice();
  int GetMaximum();
  void ShowMotionPath();
  virtual void SelectPoint( double x, double y, double z );
  virtual void  GetSelectPoint(double data[3]); 
  void SelectPoint( int x, int y);
  void SelectWindowLevel(int w,int l);
  void SetInteractor( vtkRenderWindowInteractor * interactor );
  //Motion path functions
protected:
  vtkRenderWindow            *  m_RenderWindow;
  vtkRenderer                *  m_Renderer;
  vtkRenderWindow            *  m_RenderWindow2;
  vtkRenderer                *  m_Renderer2;
private:
  //4D Image part
  vtkImageActor              *  m_Actor; 
  vtkImageMapToColors        *  m_ImageColors;
  vtkWindowLevelLookupTable  *  m_colormap;
  //4D Deformable part
  vtkImageDataGeometryFilter *  m_plane;
  vtkMaskPoints              *  m_ptMask;
  vtkThresholdPoints         *  m_ptThreshold;
  vtkArrowSource             *  m_arrow;
  vtkGlyph3D                 *  m_glyph;
  vtkGeometryFilter          *  m_glyph1;
  vtkWarpVector              *  m_warp;
  vtkPolyDataMapper          *  m_spikeMapper;
  vtkActor                   *  m_spikeActor;
  //4D Segmented Image part
  vtkImageActor              *  m_Segmented; 
  vtkWindowLevelLookupTable  *  m_Segmcolormap;
  vtkImageMapToColors        *  m_SegmImageColors;
  //General part
  vtkCamera                  *  m_Camera;
  vtkPointData               *  m_attributes;
  int                           m_extent[6];
  int                           m_axis;
  double                        m_ZoomFactor;
  int                           m_SliceNum;
  vtkCommand                 *  m_InteractorObserver;
  int                           m_field_selected;
  //double                        m_SelectPoint[3];
  vtkActor                   *  m_SphereActor;
  vtkSphereSource            *  m_Sphere;
  vtkPolyDataMapper          *  m_sphereMapper;
  vtkImageBlend              *  m_blend;
  //Motion Path part
  vtkPoints                  * inputPoints;
  vtkPolyData                * inputData;
  vtkSphereSource            * balls;
  vtkGlyph3D                 * glyphPoints;
  vtkPolyDataMapper          * glyphMapper;
  vtkActor                   * glyph;
  //Spline display
  vtkPoints                  * points;
  vtkCardinalSpline          * aSplineX;
  vtkCardinalSpline          * aSplineY;
  vtkCardinalSpline          * aSplineZ;
  vtkPolyData                * profileData;
  vtkCellArray               * lines;
  vtkTubeFilter              * profileTubes;
  vtkPolyDataMapper          * profileMapper;
  vtkActor                   * profile;
  //Axes of motion path
  vtkAxes                    * axes;
  vtkTubeFilter              * axesTubes;
  vtkPolyDataMapper          * axesMapper;
  vtkActor                   * axesActor;
  //Visualisation of the 3d Displacement Field
/*  vtkImageDataGeometryFilter *  m_3dfield;
  vtkMaskPoints              *  m_ptMask3dfield;
  vtkThresholdPoints         *  m_ptThreshold3dfield;
  vtkGlyph3D                 *  m_glyph3dfield;
  vtkPolyDataMapper          *  m_3dfieldMapper;
  vtkActor                   *  m_3dfieldActor;
  vtkPointData               *  m_3dfieldattributes;
  vtkArrowSource             *  m_3dfieldarrow;
  */
protected:
  std::vector<PointType>        m_point;
  PointType                     m_SelectedPoint;
  bool                          point_selected;
  //Variables that check if the images are loaded
  int                           m_deformable_loaded;
  int                           m_image_loaded;
  int                           m_segmented_loaded;
  int                           m_deformable_visible;
  int                           m_image_visible;
  int                           m_segmented_visible;
  int                           m_3dview_selected;
  /** Constructor and Destructor */
  DeformableRegistration3DTimeSeriesBase();
  virtual ~DeformableRegistration3DTimeSeriesBase();
};


} // end namespace fltk




#endif
