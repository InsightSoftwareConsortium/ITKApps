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
#include "DeformableRegistration3DTimeSeriesBase.h"
#include <iostream>

#include "vtkRenderWindowInteractor.h"
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include "vtkProperty.h" 
#include "vtkInteractorStyleImage.h"
#include "InteractorObserver.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyDataMapper.h"

//Direct Volume Rendering headers
#include "vtkVolumeTextureMapper2D.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolume.h"
#include "vtkColorTransferFunction.h"
#include "vtkImageCast.h"

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
  //Slice position
  m_SliceNum=0;
  m_axis=2;
  m_field_selected=0;
  point_selected=false;
  m_ZoomFactor=10000;
  m_RenderWindow   = vtkRenderWindow::New();
  m_Renderer       = vtkRenderer::New();
  m_Renderer2       = vtkRenderer::New();

  //Initialization for 4d image part
  m_colormap=vtkWindowLevelLookupTable::New();
  m_ImageColors = vtkImageMapToColors::New();
  m_ImageColors->SetLookupTable(m_colormap);
  m_Actor=vtkImageActor::New();
  m_Actor->InterpolateOff();
   //Initialization for 4d segmented image part 
  m_SegmImageColors = vtkImageMapToColors::New();
  m_Segmcolormap=vtkWindowLevelLookupTable::New();
  m_SegmImageColors->SetLookupTable(m_Segmcolormap);
  m_Segmented=vtkImageActor::New();
  m_Segmented->InterpolateOff();
  m_blend=vtkImageBlend::New();

  //Initialization for 4d deformation field part
  m_plane=vtkImageDataGeometryFilter::New();
  m_attributes =vtkPointData::New();
  m_ptThreshold=vtkThresholdPoints::New();
  m_ptThreshold->ThresholdByUpper(1);
  m_ptMask=vtkMaskPoints::New();
  m_ptMask->SetOnRatio(5);
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
  m_Camera->SetClippingRange(100000,0.1);

   //A renderer and render window
  m_RenderWindow->AddRenderer( m_Renderer );
  
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
}


void
DeformableRegistration3DTimeSeriesBase
::SetImageVtkExtent(vtkImageData * image )
{
  image->GetExtent(m_extent );
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
  if(m_spikeActor->GetVisibility()==0)m_spikeActor->VisibilityOn();
}



void
DeformableRegistration3DTimeSeriesBase
::ActivateRawData()
{
  if(m_Actor->GetVisibility()==0)m_Actor->VisibilityOn();
  else m_Actor->VisibilityOff();
}



void
DeformableRegistration3DTimeSeriesBase
::ActivateSegmentedData()
{
  if(m_Segmented->GetVisibility()==0)m_Segmented->VisibilityOn();
  else m_Segmented->VisibilityOff();
}




void
DeformableRegistration3DTimeSeriesBase
::Select3DFieldView(vtkImageData * image)
{
  //Initialization for 4d deformation field part
  vtkThresholdPoints *m_ptThreshold3dfield=vtkThresholdPoints::New();
  vtkArrowSource *m_3dfieldarrow=vtkArrowSource::New();
  m_ptThreshold3dfield->ThresholdByUpper(1);
  vtkMaskPoints *m_ptMask3dfield=vtkMaskPoints::New();
  m_ptMask3dfield->SetOnRatio(5);
  m_ptMask3dfield->RandomModeOff();
  vtkGlyph3D *m_glyph3dfield = vtkGlyph3D::New();
  m_glyph3dfield->OrientOn();
 // m_glyph3dfield-SetScaleModeToScaleByVectorComponents();
  m_glyph3dfield->SetColorModeToColorByVector();
  //m_glyph3dfield->SetScaleFactor(8.0);
  m_glyph3dfield->SetSource(m_3dfieldarrow->GetOutput());
 vtkPolyDataMapper* m_3dfieldMapper = vtkPolyDataMapper::New();
   vtkActor* m_3dfieldActor = vtkActor::New();

  vtkPointData *m_3dfieldattributes =vtkPointData::New();
  m_3dfieldattributes=image->GetPointData();
  m_3dfieldattributes->SetVectors(m_3dfieldattributes->GetScalars());
  m_ptThreshold3dfield->SetInput(image);
  m_ptMask3dfield->SetInput(m_ptThreshold3dfield->GetOutput());
  m_glyph3dfield->SetInput(m_ptMask3dfield->GetOutput());
  m_3dfieldMapper->SetInput(m_glyph3dfield->GetOutput());

  //m_3dfieldMapper->SetScalarModeToUseCellData();
  m_3dfieldActor->SetMapper(m_3dfieldMapper);
  m_3dfieldActor->GetProperty()->SetColor(1.0,1.0,1.0);
  m_Renderer2->AddActor(m_3dfieldActor);
  m_RenderWindow2->AddRenderer(m_Renderer2); 

  m_ptThreshold3dfield->Delete();
  m_ptMask3dfield->Delete();
  m_glyph3dfield->Delete();
  m_3dfieldMapper->Delete();
  m_3dfieldActor->Delete();
  m_3dfieldarrow->Delete();

}



void
DeformableRegistration3DTimeSeriesBase
::Select3DView(vtkImageData * image)
{
  // Some colourful transfer functions
  vtkImageCast *cast = vtkImageCast::New();
    cast->SetInput(image);
    cast->SetOutputScalarTypeToUnsignedShort();
  vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint(0,   0.0);
    opacityTransferFunction->AddPoint(64, 0.0);
    opacityTransferFunction->AddPoint(3000, 0.0);
    opacityTransferFunction->AddPoint(4095, 0.8);
  vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(64.0, 1.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(128.0, 0.0, 0.0, 0.2);
    colorTransferFunction->AddRGBPoint(192, 0, 1.0, 0);
    colorTransferFunction->AddRGBPoint(4095.0, 0, 0.2, 0.2);
  //The property describes how the data will look
  vtkVolumeProperty *volumeProperty=vtkVolumeProperty::New();
     volumeProperty->SetColor(colorTransferFunction);
     volumeProperty->SetScalarOpacity(opacityTransferFunction);
     volumeProperty->ShadeOff();
     volumeProperty->SetInterpolationTypeToLinear();
    

  //Uncomment this part if you want to use raycaster
  /*vtkVolumeRayCastCompositeFunction *compositeFunction = vtkVolumeRayCastCompositeFunction::New();
    compositeFunction->SetCompositeMethod(0);
  // The mapper / ray cast function know how to render the data
  vtkVolumeRayCastMapper *volumeMapper = vtkVolumeRayCastMapper::New();
    volumeMapper->SetVolumeRayCastFunction(compositeFunction);
    volumeMapper->SetInput(cast->GetOutput());
    */

  vtkVolumeTextureMapper2D *volumeMapper =vtkVolumeTextureMapper2D::New();
    volumeMapper->SetInput(cast->GetOutput());
  //volumeMapper->SetMaximumNumberOfPlanes(20);
  // The volume holds the mapper and the property and
  // can be used to position/orient the volume
  vtkVolume *volume=vtkVolume::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);
    m_Renderer->ResetCamera();
  m_Renderer2->AddVolume(volume);
  m_RenderWindow2->AddRenderer(m_Renderer2); 
  m_3dview_selected=1;
  // Clean up
  volume->Delete();
  m_Renderer->RemoveVolume( volume);
}
void
DeformableRegistration3DTimeSeriesBase
::SetImageVtk(vtkImageData * image )
{
  m_Renderer->RemoveProp( m_Actor);
  m_ImageColors->SetInput(image);

  double *range = image->GetScalarRange();
  m_colormap->SetWindow(range[1] - range[0]);
  m_colormap->SetLevel(0.5 * (range[1] + range[0]));

  m_Actor->SetInput( m_ImageColors->GetOutput());
  m_Renderer->AddProp( m_Actor );
  this->SelectAxis(m_axis);
}

void
DeformableRegistration3DTimeSeriesBase
::SetSegmentedImageVtk(vtkImageData * image )
{
  m_Renderer->RemoveProp(m_Segmented);
  double *range = image->GetScalarRange();
  m_Segmcolormap->SetWindow(range[1] - range[0]);
  m_Segmcolormap->SetLevel(0.5 * (range[1] + range[0]));
  m_SegmImageColors->SetInput(image);
  m_blend->SetInput(0,m_ImageColors->GetOutput());   
  m_blend->SetOpacity(0,0.3);
  m_blend->SetInput(1,m_SegmImageColors->GetOutput());   
  m_blend->SetOpacity(1,0.9);
  m_Segmented->SetInput(m_blend->GetOutput());
  m_Renderer->AddProp(m_Segmented);
  this->SelectAxis(m_axis);
  
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
  this->SetupCamera(m_ImageColors->GetInput());
  if(!m_image_loaded&&m_deformable_loaded)
      this->SetupCamera(m_plane->GetInput());
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
::SetInteractor( vtkRenderWindowInteractor * interactor )
{
  m_RenderWindow->SetInteractor( interactor );
  ISISImageInteractor * interactorStyle = ISISImageInteractor::New();
  interactor->SetInteractorStyle( interactorStyle );
  interactorStyle->Delete();
  interactor->AddObserver(::vtkCommand::LeftButtonPressEvent, m_InteractorObserver );
  interactor->AddObserver(::vtkCommand::MiddleButtonPressEvent, m_InteractorObserver,1);
}
void
DeformableRegistration3DTimeSeriesBase
::SelectWindowLevel(int w,int l)
{
  m_colormap->SetLevel(w);
  m_colormap->SetWindow(l);   
}
void 
DeformableRegistration3DTimeSeriesBase
::SelectPoint( int x, int y )
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
  int dimensions[3] = { 100, 100, 100 };
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
  for(int i=0;i < m_point.capacity()+1;i++)
    {
    aSplineX->AddPoint(i,-m_point[i][0]);
    aSplineY->AddPoint(i,-m_point[i][1]);
    aSplineZ->AddPoint(i,-m_point[i][2]);
    inputPoints->InsertPoint(i,-m_point[i][0],-m_point[i][1],-m_point[i][2]);
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

  for(int i=0;i < 100;i++)
    {
    float t=(m_point.capacity()+1 - 1.0 ) / (100 - 1.0 ) * i;
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
    axes->SetOrigin(-m_point[0][0],-m_point[0][1],-m_point[0][2]);
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
  
  
  //inputData->Delete();
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

} // end namespace 
