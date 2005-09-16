/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DeformableRegistration3DTimeSeriesBase.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "vtkCylinderSource.h"

#include "DeformableRegistration3DTimeSeriesBase.h"
#include <iostream>

#include "vtkRenderWindowInteractor.h"
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include "vtkProperty.h" 
#include "vtkInteractorStyleImage.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "InteractorObserver.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyDataMapper.h"

#include "vtkDataArray.h"
#include "vtkPointData.h"
//Direct Volume Rendering headers
#include "vtkVolumeTextureMapper2D.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeProperty.h"

#include "vtkImageCast.h"
#include "vtkLODProp3D.h" 
#include "vtkFiniteDifferenceGradientEstimator.h"

#include "vtkSmoothPolyDataFilter.h"
#include "vtkImageMarchingCubes.h"
//#include "vtkDecimate.h"
#include "vtkPolyDataMapper.h"

//Included files for drawing 2d primitives
#include "vtkActor2D.h"
#include "vtkFloatArray.h"
#include "vtkPolyDataMapper2D.h" 
#include "vtkCoordinate.h"
#include "vtkImageCanvasSource2D.h"
#include "vtkImageMapper.h"
#include "vtkLine.h"


#if ((VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION <=5 ) || ( VTK_MAJOR_VERSION <= 4 ))
#define HAS_VTK_IMAGE_OPACITY
#endif


#define radiusline 3 //This sets the point of the canvas line
namespace fltk {
DeformableRegistration3DTimeSeriesBase
::DeformableRegistration3DTimeSeriesBase()
{
  //No data loaded
  m_deformable_loaded=0;
  m_image_loaded=0;
  m_segmented_loaded=0;
  m_deformable_visible=1;
  m_image_visible=1;
  m_segmented_visible=1;
  m_3dview_selected=0;
  //Slice position
  m_SliceNum=0;
  m_axis=2;
  m_field_selected=0;
  point_selected=false;
  m_ZoomFactor=10000;
  m_RenderWindow   = vtkRenderWindow::New();
  m_Renderer       = vtkRenderer::New();
  m_Renderer2       = vtkRenderer::New();
  //Initialization for the 4d volume part
  m_volume=vtkVolume::New();
  m_volumeSegmented=vtkVolume::New();
  //Initialization for 4d image part
  m_colormap=vtkWindowLevelLookupTable::New();
  m_ImageColors = vtkImageMapToColors::New();
  m_ImageColors->SetLookupTable(m_colormap);
  m_Actor=vtkImageActor::New();
  m_Actor->InterpolateOff();
  m_Renderer->AddViewProp( m_Actor );
  //Initialization of 3d volume for 4d image part
  m_Renderer->AddVolume(m_volume);
   //Initialization for 4d segmented image part 
  m_SegmImageColors = vtkImageMapToColors::New();
  m_Segmcolormap=vtkWindowLevelLookupTable::New();
  m_SegmImageColors->SetLookupTable(m_Segmcolormap);
  m_Segmented=vtkImageActor::New();
  m_Segmented->InterpolateOff();
  m_Segmented->SetOpacity(0.3);
  m_blend=vtkImageBlend::New();

  //Initialization for 4d deformation field part
  m_plane=vtkImageDataGeometryFilter::New();
  m_attributes =vtkPointData::New();
  m_ptThreshold=vtkThresholdPoints::New();
  m_ptThreshold->ThresholdByUpper(1);
  m_ptMask=vtkMaskPoints::New();
  m_ptMask->SetOnRatio(150);
  m_ptMask->RandomModeOff();
  m_arrow=vtkArrowSource::New();
  m_glyph = vtkGlyph3D::New();
  m_warp=vtkWarpVector::New();
  m_glyph1= vtkGeometryFilter::New();

  m_glyph->OrientOn();
  //m_glyph->ScalingOn();
  m_glyph->SetInput(m_ptMask->GetOutput());
 // m_glyph-SetScaleModeToScaleByVectorComponents();
  m_glyph->SetColorModeToColorByVector();
  //m_glyph->SetScaleFactor(8.0);
  m_glyph->SetSource(m_arrow->GetOutput());

  m_spikeMapper = vtkPolyDataMapper::New();
  m_spikeActor = vtkActor::New();
  
   //Motion path
  m_RenderWindow2   =vtkRenderWindow::New();

  //Interactor
  InteractorObserver * observer = InteractorObserver::New();
  observer->SetImageSliceViewer( this );
  m_InteractorObserver = observer;
  //observer->Delete();
  
  //Selected point initialization 
  m_Sphere      = vtkSphereSource::New();
  m_SphereActor = vtkActor::New();  
  m_sphereMapper = vtkPolyDataMapper::New();
  m_sphereMapper->SetInput( m_Sphere->GetOutput() );  
  m_SphereActor->SetMapper(m_sphereMapper );
  m_SphereActor->VisibilityOff();
  m_Renderer->AddActor( m_SphereActor );

  //Camera settings
  m_Camera = m_Renderer->GetActiveCamera();
  m_Camera->ParallelProjectionOn();

  //Camera ClippingRange
  m_Camera->SetClippingRange(1000000,0.1);

   //A renderer and render window
  m_RenderWindow->AddRenderer( m_Renderer );
  //Opacity initial value for the segmented and initial volume
  m_SegmentedVolumeOpacity=0.9;
  //The Aurora Tracker Needle Initialization-Functionality disabled
  //Volume renderer transformation functions
  OpacityTransferFunction = vtkPiecewiseFunction::New();

  ColorTransferFunction = vtkColorTransferFunction::New();
  //Manual segmentation boolean variable
  m_polyline_draw=0;
  m_manual_segmentation_selected=0;
  lastx=-1;firstx=0;
  lasty=-1;firsty=0;
  CanvasLineActors = vtkActorCollection::New();
  //Set 'on' the visibility variables
  m_image_visible=1;
  m_segmented_visible=1;
  m_deformable_visible=1;
}

DeformableRegistration3DTimeSeriesBase
::~DeformableRegistration3DTimeSeriesBase()
{
  // Delete only VTK objects
  //m_ActorList->RemoveItem (m_SphereActor);
  //m_ActorList->RemoveItem (m_spikeActor);
  //m_ActorList->Delete();
  if( m_Sphere )
  {
    m_Sphere->Delete();
  }   
  if( m_SphereActor )
  {
    m_SphereActor->Delete();
  } 
  if( m_sphereMapper )
  {
    m_sphereMapper->Delete();
  } 
  if(m_Camera )
  {
    m_Camera->Delete();
  }
  m_colormap->Delete();
  m_ImageColors->Delete();
  m_SegmImageColors->Delete();
  m_Actor->Delete();
  m_volume->Delete();
  m_plane->Delete();
  m_Segmented->Delete();
  m_ptMask->Delete();
  m_arrow->Delete();
  m_glyph->Delete();
  m_spikeMapper->Delete();
  m_spikeActor->Delete();
  //Motion path
  if( m_RenderWindow2 )
  {
    m_RenderWindow2->Delete();
  }
  if( m_Renderer2 )
  {
    m_Renderer2->Delete();
  }
  if( m_RenderWindow )
  {
    m_RenderWindow->Delete();
  }
  if( m_Renderer )
  {
    m_Renderer->Delete();
  }
  if( m_InteractorObserver )
  {
    m_InteractorObserver->Delete();
  }
  ColorTransferFunction->Delete();
  OpacityTransferFunction->Delete();
}


void
DeformableRegistration3DTimeSeriesBase
::SetImageVtkExtent(vtkImageData * image )
{
  image->GetExtent(m_extent );
  image->GetSpacing(m_spacing);
  image->GetOrigin( m_origin);
  image->GetDimensions(m_dimensions);
}


void
DeformableRegistration3DTimeSeriesBase
::ActivateDeformableFieldData()
{
  if(!m_deformable_visible)
  {
    m_deformable_visible=1;
    m_spikeActor->VisibilityOn();
  }else
  {
    m_deformable_visible=0;
    m_spikeActor->VisibilityOff();
  }
}



void
DeformableRegistration3DTimeSeriesBase
::HideDeformableFieldData()
{
  if(m_spikeActor->GetVisibility()==1)m_spikeActor->VisibilityOff();
}

void
DeformableRegistration3DTimeSeriesBase
::ShowDeformableFieldData()
{
  m_deformable_visible = !m_deformable_visible;
  if(m_spikeActor->GetVisibility()==0)
    {
    m_spikeActor->VisibilityOn();
    }
}

void
DeformableRegistration3DTimeSeriesBase
::ActivateRawData()
{
  m_image_visible=!m_image_visible;
  if( m_3dview_selected )
    {
    if( m_volume->GetVisibility() == 0 )
      {
      m_volume->VisibilityOn();
      }
    else 
      {
      m_volume->VisibilityOff();
      }
    }
  else
    {
    if( m_Actor->GetVisibility() == 0 )
      {
      m_Actor->VisibilityOn();
      }
    else 
      {
      m_Actor->VisibilityOff();
      }
    }
}



void
DeformableRegistration3DTimeSeriesBase
::ActivateSegmentedData()
{
  m_segmented_visible=!m_segmented_visible;
  if( m_3dview_selected )
    {
    if( m_volumeSegmented->GetVisibility() == 0 )
      {
      m_volumeSegmented->VisibilityOn();
      }
    else
      {
      m_volumeSegmented->VisibilityOff();
      }
    }
  else
    {
    if( m_Segmented->GetVisibility() == 0 )
      {
      m_Segmented->VisibilityOn();
      }
    else 
      {
      m_Segmented->VisibilityOff();
      }
    }
}

void
DeformableRegistration3DTimeSeriesBase
::Select3DFieldView(vtkImageData * image)
{
 if( image )
    {
    m_plane->SetExtent(m_extent );//Expand the extent to the full 3d image
    m_plane->SetInput(image);
    m_attributes=image->GetPointData();
    m_attributes->SetVectors(m_attributes->GetScalars());

    SelectFieldType(m_field_selected);   

    m_spikeMapper->SetScalarModeToUseCellData();
    m_spikeActor->SetMapper(m_spikeMapper);
    m_spikeActor->GetProperty()->SetRepresentationToWireframe();
    m_spikeActor->GetProperty()->SetColor(1.0,1.0,1.0);
    }
}

void
DeformableRegistration3DTimeSeriesBase
::Select3DSurfaceViewImage(vtkImageData * image)
{

  if(!m_3dview_selected)
    {
    m_Renderer->RemoveViewProp( m_Actor);
    }
  if(m_3dview_selected)
    {
    m_Renderer->RemoveActor(m_SurfaceExtractor);
    m_SurfaceExtractor->Delete();
    }
  vtkImageMarchingCubes *marcher = vtkImageMarchingCubes::New();
    marcher->SetInput(image);
    marcher->SetValue(0, 1000);
    marcher->Update();
  /*
  vtkDecimate *decimator = vtkDecimate::New();
    decimator->SetInput(marcher->GetOutput());
    decimator->SetTargetReduction(100);
    decimator->SetMaximumIterations(4);
    decimator->SetInitialError(0.01);
    decimator->SetErrorIncrement(0.01);
    decimator->SetPreserveTopology(1);
    decimator->Update();
  */
  vtkSmoothPolyDataFilter* smoother = vtkSmoothPolyDataFilter::New();
    smoother->SetInput(marcher->GetOutput());
    smoother->SetNumberOfIterations(5);
    smoother->SetFeatureAngle(60);
    smoother->SetRelaxationFactor(0.05);
    smoother->FeatureEdgeSmoothingOff();
  // Save the mesh in an ASCII file
  /*
  char *meshFname =  fl_file_chooser("Choose VTK Mesh File", "*.msh*", "d:/datanpr");
  vtkPolyDataWriter *vtkwriter = vtkPolyDataWriter::New();
    vtkwriter->SetFileName(meshFname);
    vtkwriter->SetInput(smoother->GetOutput());
    vtkwriter->SetFileTypeToASCII();
    vtkwriter->Update();
  */
  // render 3D model
  vtkPolyDataMapper* isoMapper = vtkPolyDataMapper::New();
    isoMapper->SetInput(marcher->GetOutput());
    isoMapper->ScalarVisibilityOff();

  
  m_SurfaceExtractor = vtkActor::New();
  m_SurfaceExtractor->SetMapper(isoMapper);
  m_SurfaceExtractor->GetProperty()->SetDiffuseColor(1,1,0.9412);

  m_Renderer->AddActor(m_SurfaceExtractor);
  //3d is selected later in 3d view segmented if segmented object is loaded
  if(!m_segmented_loaded)m_3dview_selected=1;
}
void
DeformableRegistration3DTimeSeriesBase
::Select3DViewImage(vtkImageData * image)
{
  if(!m_3dview_selected)
    {
    if(m_image_visible)
      {
      m_Actor->VisibilityOff();
      m_volume->VisibilityOn();
      }
    }
  vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput(image);
    cast->SetOutputScalarTypeToUnsignedShort();
  double Range[2];
  image->GetScalarRange(Range);
  vtkFloatingPointType window = Range[1] - Range[0];
  vtkColorTransferFunction* GrayTransferFunction = vtkColorTransferFunction::New();;

  vtkPiecewiseFunction* GradientTransferFunction = vtkPiecewiseFunction::New();
/*
    ColorTransferFunction->AddRGBPoint(Range[0] + window * 0.000, 0.00,  0.00,  0.00);//0.25
    ColorTransferFunction->AddRGBPoint(Range[0] + window * 0.200, 1.00,  1.00,  0.62);//1
    //ColorTransferFunction->AddRGBPoint(Range[0] + window * 0.400, 1.00,  1.00,  0.00);//1,1
    //ColorTransferFunction->AddRGBPoint(Range[0] + window * 0.600, 0.00,  1.00,  0.00);
    ColorTransferFunction->AddRGBPoint(Range[0] + window * 0.800, 0.98,  1.0,  0.93);//0,1,1
    ColorTransferFunction->AddRGBPoint(Range[0] + window * 1.000, 0.00,  0.00,  0.00);//0,0,1
*/
  GrayTransferFunction->SetColorSpaceToHSV();

  vtkFloatingPointType a;

  for (int n=0; n<10; n++)
    {
    a = n / 9.0;
    GrayTransferFunction->AddHSVPoint(Range[0]+ window * a*a, 0, 0, a);
    //OpacityTransferFunction->AddPoint(Range[0] + window * a, a*a*0.67);
    //GradientTransferFunction->AddPoint(pow(10.0, 2.5 * a), a);
    }
  //The property describes how the data will look
  vtkVolumeProperty *volumeProperty=vtkVolumeProperty::New();
    volumeProperty->SetColor(ColorTransferFunction);
    volumeProperty->SetScalarOpacity(OpacityTransferFunction);
    volumeProperty->SetGradientOpacity(GradientTransferFunction);
    volumeProperty->ShadeOff();
    volumeProperty->SetAmbient(0.1);
    volumeProperty->SetDiffuse(0.8);
    volumeProperty->SetSpecular(0.6);
    volumeProperty->SetSpecularPower(32.0);
    volumeProperty->SetInterpolationTypeToLinear();

  //Uncomment this part if you want to use raycaster
    /*
  vtkVolumeRayCastCompositeFunction *compositeFunction = vtkVolumeRayCastCompositeFunction::New();
     compositeFunction->SetCompositeMethodToInterpolateFirst();
  // The mapper / ray cast function know how to render the data
  vtkVolumeRayCastMapper *volumeMapper = vtkVolumeRayCastMapper::New();
    volumeMapper->SetVolumeRayCastFunction(compositeFunction);
    volumeMapper->SetInput(cast->GetOutput());
  */
  vtkVolumeTextureMapper2D *volumeMapper =vtkVolumeTextureMapper2D::New();
    volumeMapper->SetInput(cast->GetOutput());
    volumeMapper->SetMaximumNumberOfPlanes(100);
  /*
    vtkLODProp3D * m_volume=vtkLODProp3D::New();
    m_volume->AddLOD(volumeMapper,volumeProperty,0.3);
    m_Renderer->AddProp(m_volume);
  */
  m_volume->SetMapper(volumeMapper);
  m_volume->SetProperty(volumeProperty);
  
  vtkFiniteDifferenceGradientEstimator* gradientEstimator =vtkFiniteDifferenceGradientEstimator::New();
  // Create a ray function - this is a compositing ray function
  volumeMapper->SetGradientEstimator(gradientEstimator);
//  volumeMapper->AutoAdjustSampleDistancesOn();
//  volumeMapper->IntermixIntersectingGeometryOn();
  gradientEstimator->Delete();

  volumeMapper->Delete();
  volumeProperty->Delete();
  GrayTransferFunction->Delete();
  GradientTransferFunction->Delete();
  cast->Delete();
  //3d is selected later in 3d view segmented if segmented object is loaded
  if(!m_segmented_loaded)
    {
    m_3dview_selected=1;
    }
}
void
DeformableRegistration3DTimeSeriesBase
::Select3DSurfaceViewSegmented(vtkImageData * image)
/*this must be redone*/
{
  if(!m_3dview_selected)
    {
    if(m_Segmented)m_Renderer->RemoveViewProp( m_Segmented);
    }
  if(m_3dview_selected)
    m_Renderer->RemoveActor(m_SurfaceExtractor);
  
  vtkImageMarchingCubes * marcher = vtkImageMarchingCubes::New();
    marcher->SetInput(image);
    marcher->SetValue(1,1.0);
    marcher->Update();
    std::cout << "Marching Cube finished...." << std::endl;

  vtkSmoothPolyDataFilter* smoother = vtkSmoothPolyDataFilter::New();
    smoother->SetInput(marcher->GetOutput());
    smoother->SetNumberOfIterations(5);
    smoother->SetFeatureAngle(60);
    smoother->SetRelaxationFactor(0.05);
    smoother->FeatureEdgeSmoothingOff();
    std::cout << "VTK Smoothing mesh finished...." << std::endl;

  // render 3D model
  vtkPolyDataMapper* isoMapper = vtkPolyDataMapper::New();
    isoMapper->SetInput(smoother->GetOutput());
    isoMapper->ScalarVisibilityOn();

  if(m_3dview_selected)
    {
    m_Renderer->RemoveActor(m_SurfaceExtractor);
    m_SurfaceExtractor->Delete();
    }

  m_SurfaceExtractor = vtkActor::New();
  m_SurfaceExtractor->SetMapper(isoMapper);
  m_SurfaceExtractor->GetProperty()->SetDiffuseColor(1,1,0.9412);

  m_Renderer->AddActor(m_SurfaceExtractor);
  m_3dview_selected=1;
  isoMapper->Delete();
  smoother->Delete();
  marcher->Delete();
}

void
DeformableRegistration3DTimeSeriesBase
::Select3DViewSegmented(vtkImageData * image)
{
  if(!m_3dview_selected)
    {
    if(m_segmented_visible)
      {
      m_Segmented->VisibilityOff();
      m_volumeSegmented->VisibilityOn();
      }
    }
  vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput(image);
    cast->SetOutputScalarTypeToUnsignedShort();
  double Range[2];
  image->GetScalarRange(Range);
  vtkFloatingPointType window = Range[1] - Range[0];
  vtkColorTransferFunction* GrayTransferFunction = vtkColorTransferFunction::New();;

  vtkPiecewiseFunction* GradientTransferFunction = vtkPiecewiseFunction::New();;

  vtkPiecewiseFunction *OpacityTransferFunction = vtkPiecewiseFunction::New();

  vtkColorTransferFunction *ColorTransferFunction = vtkColorTransferFunction::New();
    ColorTransferFunction->AddRGBPoint(Range[0] + window * 0.000, 0.00,  0.00,  1.00);//0.25
    ColorTransferFunction->AddRGBPoint(Range[0] + window * 0.200, 1.00,  1.00,  0.62);//1
    ColorTransferFunction->AddRGBPoint(Range[0] + window * 0.400, 1.00,  1.00,  0.00);//1,1
    ColorTransferFunction->AddRGBPoint(Range[0] + window * 0.600, 0.00,  1.00,  0.00);
    ColorTransferFunction->AddRGBPoint(Range[0] + window * 0.800, 0.98,  1.0,  0.93);//0,1,1
    ColorTransferFunction->AddRGBPoint(Range[0] + window * 1.000, 0.00,  0.00,  1.00);//0,0,1

  GrayTransferFunction->SetColorSpaceToHSV();

  vtkFloatingPointType a;

  for (int n=0; n<10; n++)
    {
    a = n / 9.0;
    GrayTransferFunction->AddHSVPoint(Range[0]+ window * a*a, 0, 0, a);
    OpacityTransferFunction->AddPoint(Range[0] + window * a, a*a*0.67);
   // GradientTransferFunction->AddPoint(pow(10.0, 2.5 * a), a);
    }
  //The property describes how the data will look
    vtkVolumeProperty *volumeProperty=vtkVolumeProperty::New();
    volumeProperty->SetColor(ColorTransferFunction);
    volumeProperty->SetScalarOpacity(OpacityTransferFunction);
    volumeProperty->SetGradientOpacity(GradientTransferFunction);
    volumeProperty->ShadeOff();
    volumeProperty->SetAmbient(0.1);
    volumeProperty->SetDiffuse(0.8);
    volumeProperty->SetSpecular(0.6);
    volumeProperty->SetSpecularPower(32.0);
    volumeProperty->SetInterpolationTypeToLinear();

  vtkVolumeTextureMapper2D *volumeMapper =vtkVolumeTextureMapper2D::New();
    volumeMapper->SetInput(cast->GetOutput());
  m_volumeSegmented->SetMapper(volumeMapper);
  m_volumeSegmented->SetProperty(volumeProperty);
  m_Renderer->AddVolume(m_volumeSegmented);
  vtkFiniteDifferenceGradientEstimator* gradientEstimator =vtkFiniteDifferenceGradientEstimator::New();

  volumeMapper->SetGradientEstimator(gradientEstimator);

  gradientEstimator->Delete();

  m_3dview_selected=1;
  volumeMapper->Delete();
  volumeProperty->Delete();
  ColorTransferFunction->Delete();
  OpacityTransferFunction->Delete();
  GrayTransferFunction->Delete();
  GradientTransferFunction->Delete();
  cast->Delete();
}

void 
DeformableRegistration3DTimeSeriesBase
::SetColorTransferFunctionPoint(double x,double r,double g,double b)
{
  ColorTransferFunction->AddRGBPoint(x,r,g,b);
}

void 
DeformableRegistration3DTimeSeriesBase
::SetOpacityTransferFunctionPoint(double x,double value)
{
  OpacityTransferFunction->AddPoint(x,value);
}
void 
DeformableRegistration3DTimeSeriesBase
::RemoveColorTransferFunctionPoint(double x)
{
  ColorTransferFunction->RemovePoint(x);
}

void 
DeformableRegistration3DTimeSeriesBase
::RemoveOpacityTransferFunctionPoint(double x)
{
  OpacityTransferFunction->RemovePoint(x);
}

void
DeformableRegistration3DTimeSeriesBase
::SetImageVtk(vtkImageData * image )
{
  if(m_Actor)
    {
      //m_Renderer->RemoveViewProp( m_Actor);
    }
  m_ImageColors->SetInput(image);
  //Window/Level initialization
  double *range = image->GetScalarRange();
  m_Window=range[1] - range[0];
  m_Level=0.5 * (range[1] + range[0]);
  m_colormap->SetWindow(m_Window);
  m_colormap->SetLevel(m_Level);

  m_Actor->SetInput( m_ImageColors->GetOutput());

  this->SelectAxis(m_axis);
}
void
DeformableRegistration3DTimeSeriesBase
::SetWindowLevel(int WindowLevel,int Number )
{
  if(Number==0)
    {
    m_Window=WindowLevel;
    m_colormap->SetWindow(m_Window);
    }
  if(Number==1)
    {
    m_Level=WindowLevel;
    m_colormap->SetLevel(m_Level);
    }
}
int* 
DeformableRegistration3DTimeSeriesBase
::GetWindowLevel(void )
{
  WindowLevel[0]=m_Window;
  WindowLevel[1]=m_Level;
  return WindowLevel;
}
void
DeformableRegistration3DTimeSeriesBase
::SetSegmentedImageVtk(vtkImageData * image )
{
  if(m_Segmented)
    {
      m_Renderer->RemoveViewProp(m_Segmented);
    }
  double *range = image->GetScalarRange();
  m_Segmcolormap->SetWindow(range[1] - range[0]);
  m_Segmcolormap->SetLevel(0.5 * (range[1] + range[0]));
  m_SegmImageColors->SetInput(image);
  

#ifdef HAS_VTK_IMAGE_OPACITY
#else
  m_blend->SetInput(0,m_ImageColors->GetOutput());   
  m_blend->SetInput(1,m_SegmImageColors->GetOutput());   
  m_Segmented->SetInput(m_blend->GetOutput());
#endif

  m_Segmented->SetInput(m_SegmImageColors->GetOutput());
  m_Renderer->AddViewProp(m_Segmented);

  this->SelectAxis(m_axis);
  
}
bool
DeformableRegistration3DTimeSeriesBase
::SetSegmentedVolumeOpacity( const double value )
{
  if ((value>=0.0f) && (value<=1.0f))
    {
    m_SegmentedVolumeOpacity = value;

    m_Segmented->SetOpacity(1.0 - m_SegmentedVolumeOpacity);
    m_Actor->SetOpacity(m_SegmentedVolumeOpacity);
    /*
    m_blend->SetOpacity(0, 1.0 - m_SegmentedVolumeOpacity );
    m_blend->SetOpacity(1, m_SegmentedVolumeOpacity );
    m_blend->Update();
    */
    return true;
    }
  return false;
}

void
DeformableRegistration3DTimeSeriesBase
::SelectFieldType(int i)
{
  m_Renderer->RemoveActor(m_spikeActor);
  switch(i)
  {
    case 0:
    {
      m_ptThreshold->SetInput(m_plane->GetOutput());
      m_ptMask->SetInput(m_ptThreshold->GetOutput());
      m_spikeMapper->SetInput(m_glyph->GetOutput());
      m_spikeActor->GetProperty()->SetRepresentationToSurface();
    break;
    }
    case 1:
    {
      m_warp->SetInput(m_plane->GetOutput());
      m_glyph1->SetInput(m_warp->GetOutput());
      m_spikeMapper->SetInput(m_glyph1->GetOutput());
      m_spikeActor->GetProperty()->SetRepresentationToWireframe();
    break;
    }
  }
  m_Renderer->AddActor(m_spikeActor);
  m_field_selected=i;
}
void
DeformableRegistration3DTimeSeriesBase
::SetDeformableFieldImageVtk(vtkImageData * image )
{
  if(image)
    {
    m_plane->SetInput(image);
    m_attributes=image->GetPointData();
    m_attributes->SetVectors(m_attributes->GetScalars());

    SelectFieldType(m_field_selected);   

    m_spikeMapper->SetScalarModeToUseCellData();
    m_spikeActor->SetMapper(m_spikeMapper);
    m_spikeActor->GetProperty()->SetRepresentationToWireframe();
    m_spikeActor->GetProperty()->SetColor(1.0,1.0,1.0);
   
    this->SelectAxis(m_axis);
    }
}
void 
DeformableRegistration3DTimeSeriesBase
::SelectAxis( int axis)
{
  if(m_3dview_selected)//If the user switched from 3d to slice mode
    {
    if(m_image_visible)
      {
      m_volume->VisibilityOff();
      }

    if(m_segmented_visible)
      {
      m_volumeSegmented->VisibilityOff();
      }

    if( m_image_visible )
      { 
      m_Actor->VisibilityOn();
      }

    if( m_segmented_visible )
      {
      m_Segmented->VisibilityOn();
      }

    m_3dview_selected=0;
    //Camera settings
    m_Camera = m_Renderer->GetActiveCamera();//Set the camera parameters again
    m_Camera->ParallelProjectionOn();
    //Camera ClippingRange
    m_Camera->SetClippingRange(1000000,0.1);
    /*
    m_blend->SetInput(0,m_ImageColors->GetOutput());
    m_blend->SetInput(1,m_SegmImageColors->GetOutput());
    */
    }
  m_axis=axis;
  this->SelectSlice(m_SliceNum);
}
void DeformableRegistration3DTimeSeriesBase
::SelectSlice( int slice )
{
  int   extent[6];
  for(int i=0;i<6;i++)
    extent[i]=m_extent[i];
  switch( m_axis)
  {
    case 0:
      if ((slice>=extent[0]) && (slice<=extent[1]))
      {     
          extent[0] = slice;
          extent[1] = slice;
          m_SliceNum = slice;
      }else {extent[0]=extent[1];m_SliceNum =0;}
    break;
    case 1:
      if ((slice>=extent[2]) && (slice<=extent[3]))
      {
        extent[2] = slice;
        extent[3] = slice;
        m_SliceNum = slice;
      }else {extent[2]=extent[3];m_SliceNum =0;}
    break;
    case 2:
      if ((slice>=extent[4]) && (slice<=extent[5]))
      {
        extent[4] = slice;
        extent[5] = slice;
        m_SliceNum = slice;
      }else {extent[4]=extent[5];m_SliceNum =0;}
      break;
  }
  m_Actor->SetDisplayExtent( extent );
  m_plane->SetExtent( extent );
  m_Segmented->SetDisplayExtent( extent );
  this->SetupCamera((vtkImageData *)m_ImageColors->GetInput());
  if(!m_image_loaded&&m_deformable_loaded)
      this->SetupCamera((vtkImageData *)m_plane->GetInput());
}
void
DeformableRegistration3DTimeSeriesBase
::SetupCamera(vtkImageData * image)
{

  if ( !image )
    {
    return;
    }
  vtkFloatingPointType spacing[3];
  vtkFloatingPointType origin[3];
  int   dimensions[3];

  image->GetSpacing(spacing);
  image->GetOrigin(origin);
  image->GetDimensions(dimensions);

  double focalPoint[3];
  double position[3];

  for ( unsigned int cc = 0; cc < 3; cc++)
    {
    focalPoint[cc] = origin[cc] + ( spacing[cc] * dimensions[cc] ) / 2.0;
    position[cc]   = focalPoint[cc];
    }

  int idx = 0;
  switch( m_axis )
    {
  case 0:
      {
      //origin[0]+=m_SliceNum;
      idx = 0;
      m_Camera->SetViewUp (     0,  0,  1 );
      break;
      }
  case 1:
      {
      //origin[1]+=m_SliceNum;
      idx = 1;
      m_Camera->SetViewUp (     0,  0,  1 );
      break;
      }
  case 2:
      {
      //origin[2]+=m_SliceNum;
      idx = 2;
      m_Camera->SetViewUp (     0,  -1,  0 );
      break;
      }
    }

  const double distanceToFocalPoint = -1000;
  position[idx] += distanceToFocalPoint;

  m_Camera->SetPosition (   position );
  m_Camera->SetFocalPoint ( focalPoint );

#define myMAX(x,y) (((x)>(y))?(x):(y))  

   int d1 = (idx + 1) % 3;
   int d2 = (idx + 2) % 3;
 
  double max = myMAX( 
    spacing[d1] * dimensions[d1],
    spacing[d2] * dimensions[d2]);


  m_Camera->SetParallelScale( max / 3   );
}

int
DeformableRegistration3DTimeSeriesBase
::GetSlice()
{
 return m_SliceNum;
}
int random_range(int lowest_number, int highest_number)
{
  int range = highest_number - lowest_number + 1;
  return lowest_number + int(range * rand()/(RAND_MAX + 1.0));
}
int
DeformableRegistration3DTimeSeriesBase
::GetMaximum()
{
  //std::cout << "Max:" <<m_extent[2*m_axis+1]-m_extent[2*m_axis]<< std::endl;
  return m_extent[2*m_axis+1]-m_extent[2*m_axis];
}



void DeformableRegistration3DTimeSeriesBase
::SetBaseInteractor( vtkRenderWindowInteractor * interactor ,int i)
{
  switch(i)
    {
    case 0:
      {
      ISISImageInteractor * interactorStyle = ISISImageInteractor::New();
      interactor->SetInteractorStyle( interactorStyle );
      interactorStyle->Delete();
      break;
      }
    case 1:
      {
      vtkInteractorStyleTrackballCamera  * interactorStyle = vtkInteractorStyleTrackballCamera::New();
      interactor->SetInteractorStyle( interactorStyle );
      //interactor->SetDesiredUpdateRate(2.0);
      interactorStyle->Delete();
      break;
      }
    }
  m_RenderWindow->SetInteractor( interactor );
  interactor->AddObserver(::vtkCommand::LeftButtonPressEvent, m_InteractorObserver );
  interactor->AddObserver(::vtkCommand::RightButtonPressEvent, m_InteractorObserver );
  interactor->AddObserver(::vtkCommand::MiddleButtonPressEvent, m_InteractorObserver,1);
}

void 
DeformableRegistration3DTimeSeriesBase
::SelectPoint( int x, int y,int mousebutton )
{
  int* winsize = m_RenderWindow->GetSize();
  y = winsize[1] - y;
  const double z = m_SliceNum ;
  // Convert display point to world point
  double wpoint[4];
  m_Renderer->SetDisplayPoint( x, y, z);
  m_Renderer->DisplayToWorld();
  m_Renderer->GetWorldPoint( wpoint );
  itk::Point<float,3> point;
  double spacing[3]={1,1,1};
  double origin[3] ={0,0,0};
  int dimensions[3] = { 320, 240, 28 };
 if ( m_Actor->GetInput() )
  {
    m_Actor->GetInput()->GetSpacing(spacing);
    m_Actor->GetInput()->GetOrigin(origin);
    m_Actor->GetInput()->GetDimensions(dimensions);
   }
   
  int idx = 0;
  switch( m_axis )
    {
    case 0:
    {
      idx = 0;
      break;
    }
    case 1:
    {
      idx = 1;
      break;
    }
    case 2:
    {
      idx = 2;
      break;
    }
  }
  float realz = m_SliceNum * spacing[idx] + origin[idx];
  wpoint[idx] = realz;
  // At this point we have 3D position in the variable wpoint
  if(m_polyline_draw&&(mousebutton==0))
    {
    double temp[2];
    if((lastx!=-1)&&(lasty!=-1))
      {
      temp[0]=wpoint[0];
      temp[1]=wpoint[1];
      m_rasterize_polygon.push_back(temp);//Add the point to the polygon data
      this->Draw2DCanvasLine(lastx,lasty,wpoint[0],wpoint[1]);
      }
    else
      {
      temp[0]=firstx=wpoint[0];
      temp[1]=firsty=wpoint[1];
      m_rasterize_polygon.push_back(temp);//This save the first point clicked
      }
    lastx=wpoint[0];
    lasty=wpoint[1];
    }
  if (m_polyline_draw&&(mousebutton==1))
    {
    m_polyline_draw=0;
    this->Draw2DCanvasLine(firstx,firsty,lastx,lasty);
    //Reinitialize the drawing procedure
    lastx=-1;firstx=0;
    lasty=-1;firsty=0;
    }
  this->SelectPoint(wpoint[0], wpoint[1], wpoint[2]);
}
void
DeformableRegistration3DTimeSeriesBase
::SelectPoint( double x, double y, double z )
{
  point_selected=true;
  if (!m_Actor->GetInput()) 
  {
    return;     // return, if no image is loaded yet.
  }
  m_SelectedPoint[0] = x;
  m_SelectedPoint[1] = y;
  m_SelectedPoint[2] = z;
  int dimensions[3] = { 100, 100, 100 };
  m_Sphere->SetRadius(0.8);
  m_SphereActor->SetPosition( x, y, z );
  m_SphereActor->VisibilityOn();
}

void 
DeformableRegistration3DTimeSeriesBase
::GetSelectPoint(double data[3])
{
  for(int i=0; i<3; i++)
  {
    data[i] = m_SelectedPoint[i];
  }
}
void 
DeformableRegistration3DTimeSeriesBase
::ShowMotionPath()
{
  aSplineX=vtkCardinalSpline::New();
  aSplineY=vtkCardinalSpline::New();
  aSplineZ=vtkCardinalSpline::New();

  inputPoints=vtkPoints::New();
  //Insert the points from the motion path in a spline and in an array of point
  for(int i=0;i < m_point.capacity();i++)
    {
    aSplineX->AddPoint(i,m_point[i][0]);
    aSplineY->AddPoint(i,m_point[i][1]);
    aSplineZ->AddPoint(i,m_point[i][2]);
    inputPoints->InsertPoint(i,m_point[i][0],m_point[i][1],m_point[i][2]);
    }
  
  //Spheres along the spline
  inputData=vtkPolyData::New();
  inputData->SetPoints(inputPoints);

  balls=vtkSphereSource::New();
  balls->SetRadius(0.3);
  balls->SetPhiResolution(10);
  balls->SetThetaResolution(10);

  glyphPoints=vtkGlyph3D::New();
  glyphPoints->SetInput(inputData);
  glyphPoints->SetSource(balls->GetOutput());

  glyphMapper=vtkPolyDataMapper::New();
  glyphMapper->SetInput(glyphPoints->GetOutput());

  glyph=vtkActor::New();
  glyph->SetMapper(glyphMapper);
  
  //Spline display
  points=vtkPoints::New();
  profileData=vtkPolyData::New();
//m_point.size()
  for(int i=0;i < 100;i++)
    {
    float t=(m_point.capacity() - 1.0 ) / (100 - 1.0 ) * i;
      points->InsertPoint(i,aSplineX->Evaluate(t),aSplineY->Evaluate(t),aSplineZ->Evaluate(t));
    }
  lines=vtkCellArray::New();
  lines->InsertNextCell(100);
  for(int i=0;i < 100;i++){
    lines->InsertCellPoint(i);
  }
  profileData->SetPoints(points);
  profileData->SetLines(lines);


  profileTubes=vtkTubeFilter::New();
  profileTubes->SetNumberOfSides(8);
  profileTubes->SetInput(profileData);
  profileTubes->SetRadius(0.1);

  profileMapper=vtkPolyDataMapper::New();
  profileMapper->SetInput(profileTubes->GetOutput());
  profile=vtkActor::New();
  profile->SetMapper(profileMapper);
  
  //Axes dislpay:x axes is red ,y is yellow and z is green
  axes = vtkAxes::New();
    axes->SetOrigin(m_point[0][0],m_point[0][1],m_point[0][2]);
    axes->SetScaleFactor(1);
  axesTubes = vtkTubeFilter::New();
    axesTubes->SetInput(axes->GetOutput());
    axesTubes->SetRadius(0.1);//axes->GetScaleFactor()/15.0);
    axesTubes->SetNumberOfSides(6);
  axesMapper = vtkPolyDataMapper::New();
    axesMapper->SetInput(axesTubes->GetOutput());
  axesActor = vtkActor::New();
    axesActor->SetMapper(axesMapper);
    
  m_Renderer2->AddActor(axesActor);
  m_Renderer2->AddActor(glyph);
  m_Renderer2->AddActor(profile);
  m_RenderWindow2->AddRenderer(m_Renderer2); 
  
  
  m_point.clear();
  profileTubes->Delete();
  profileMapper->Delete();
  profile->Delete();
  balls->Delete();
  aSplineX->Delete();
  aSplineY->Delete();
  aSplineZ->Delete();
  glyphPoints->Delete();
  glyphMapper->Delete();
  glyph->Delete();
  axesActor->Delete();
  axesTubes->Delete();
  axes->Delete();
}

void 
DeformableRegistration3DTimeSeriesBase
::HistogramComputation(vtkImageData *imagedata)
{
  float lowerbound=(float)imagedata->GetScalarRange()[0];
  float upperbound=(float)imagedata->GetScalarRange()[1];
  int* data_dimensions=imagedata->GetDimensions();
  int numbins=(int)(upperbound-lowerbound)+1;
  for(int i=0;i<numbins;i++)
    {
    histogram[i]=0.0f;
    }
  for(int k=0;k<data_dimensions[2];k++)
    for(int j=0;j<data_dimensions[1];j++)
      for(int i=0;i<data_dimensions[0];i++)
        {
        vtkPointData *point_data=imagedata->GetPointData();
        vtkDataArray *scalars=point_data->GetScalars();
        int ijk[3];
        ijk[0]=i;ijk[1]=j;ijk[2]=k;
        int pointId=imagedata->ComputePointId(ijk);
        int value=scalars->GetTuple1(pointId);
        histogram[value]+=1;
        }
}

void
DeformableRegistration3DTimeSeriesBase
::Draw2DPolygon()
{
  //This is not finished simply it display a polygon at some coordinates
  //Is obsolette,use instead vtkImageCanvasSource2D
  int i;
  static float x[3][3]={{0,0,0}, {1,0,0}, {1,1,0}};
  static vtkIdType pts[1][3]={{0,1,2}};
  
  // We'll create the building blocks of polydata including data attributes.
  vtkPolyData *polyline = vtkPolyData::New();
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *polys = vtkCellArray::New();
  vtkFloatArray *scalars = vtkFloatArray::New();

  // Load the point, cell, and data attributes.
  for (i=0; i<1; i++) scalars->InsertTuple1(i,13);
  for (i=0; i<3; i++) points->InsertPoint(i,x[i]);
  for (i=0; i<1; i++)
    {
   
    polys->InsertNextCell(3,pts[i]);
    }
  // We now assign the pieces to the vtkPolyData.
  polyline->SetPoints(points);
  points->Delete();
  polyline->SetLines(polys);
  polys->Delete();
  polyline->GetPointData()->SetScalars(scalars);


  vtkCoordinate *coords = vtkCoordinate::New();
     coords->SetCoordinateSystemToView();

  vtkPolyDataMapper2D  *mapper = vtkPolyDataMapper2D::New();
  mapper->SetInput(polyline); // 3D with z=0
     mapper->SetTransformCoordinate(coords);

  vtkActor2D  *actor = vtkActor2D::New();
     actor->SetMapper(mapper);

     m_Renderer->AddActor2D(actor);
}

void
DeformableRegistration3DTimeSeriesBase
::Draw2DCanvasLine(double x1,double y1,double x2,double y2)
{
  int i;
  static double x[2][3];
  x[0][0]=x1;
  x[0][1]=y1;
  x[0][2]=0;
  x[1][0]=x2;
  x[1][1]=y2;
  x[1][2]=0;
  static vtkIdType pts[1][2]={{0,1}};
  
  // We'll create the building blocks of polydata including data attributes.
  vtkPolyData *polyline = vtkPolyData::New();
  vtkPoints *points = vtkPoints::New();
  vtkCellArray *polys = vtkCellArray::New();
  vtkFloatArray *scalars = vtkFloatArray::New();

  // Load the point, cell, and data attributes.
  for (i=0; i<2; i++) scalars->InsertTuple1(i,13);
  for (i=0; i<2; i++) points->InsertPoint(i,x[i]);
  for (i=0; i<1; i++)
    {
    polys->InsertNextCell(2,pts[i]);
    }
  
  // We now assign the pieces to the vtkPolyData.
  polyline->SetPoints(points);
  points->Delete();
  polyline->SetLines(polys);
  polys->Delete();
  polyline->GetPointData()->SetScalars(scalars);

  
  profileTubes=vtkTubeFilter::New();
  profileTubes->SetNumberOfSides(8);
  profileTubes->SetInput(polyline);
  profileTubes->SetRadius(0.9);

  profileMapper=vtkPolyDataMapper::New();
  profileMapper->SetInput(profileTubes->GetOutput());
  profile=vtkActor::New();
  profile->SetMapper(profileMapper);
 
  m_Renderer->AddActor(profile);
  CanvasLineActors->AddItem(profile);

  profileTubes->Delete();
  profileMapper->Delete();
  profile->Delete();
}
void
DeformableRegistration3DTimeSeriesBase
::Delete2DCanvasLine()
{
  CanvasLineActors->InitTraversal();
  for(int i=0;i<CanvasLineActors->GetNumberOfItems();i++)
    {
    m_Renderer->RemoveActor(CanvasLineActors->GetNextItem());
    }
  CanvasLineActors->RemoveAllItems();
}

} // end namespace 


