/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SnakeParametersPreviewPipeline.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "SnakeParametersPreviewPipeline.h"
#include "SignedDistanceFilter.h"
#include "SNAPLevelSetFunction.h"
#include "itkBSplineInterpolationWeightFunction.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "FL/Fl_Gl_Window.h"
#include "GL/glu.h"

using namespace std;
using namespace itk;

SnakeParametersPreviewPipeline
::SnakeParametersPreviewPipeline()
{
  // Start with a 100 interpolated points
  m_NumberOfSampledPoints = 100;

  // The flood fill image
  m_FloodFillImage = FloatImageType::New();

  // A filter used to fill the pipeline
  m_DistanceFilter = DistanceFilterType::New();
  m_DistanceFilter->SetInput(m_FloodFillImage);

  m_ControlsModified = false;
  m_SpeedModified = false;
  m_ParametersModified = false;
  m_QuickUpdate = false;

  // Initialize the level set function
  m_LevelSetFunction = LevelSetFunctionType::New();  
}

SnakeParametersPreviewPipeline
::~SnakeParametersPreviewPipeline()
{
}

void
SnakeParametersPreviewPipeline
::SetControlPoints(const std::vector<Vector2d> &points)
{
  if(m_ControlPoints != points)
    {  
    // Save the points
    m_ControlPoints = points;

    // Set the flags
    m_ControlsModified = true;
    m_QuickUpdate = false;
    }
}

void 
SnakeParametersPreviewPipeline
::ChangeControlPoint(
  unsigned int index, const Vector2d &point, bool quickUpdate)
{
  // Update the point
  assert(index < m_ControlPoints.size());

  // Update the point
  m_ControlPoints[index] = point;

  // Set the flags
  m_ControlsModified = true;

  // Set the update flag
  m_QuickUpdate = quickUpdate;
}

void
SnakeParametersPreviewPipeline
::SetSpeedImage(FloatImageType *image)
{
  if(image != m_SpeedImage)
    {
    // Store the image
    m_SpeedImage = image;

    // Set the modified flag
    m_SpeedModified = true;
    }
}

void
SnakeParametersPreviewPipeline
::SetSnakeParameters(const SnakeParameters &parameters)
{
  // Don't waste time on nonsense
  if(m_Parameters == parameters) return;
  
  // Check if the update concerns the speed images
  m_SpeedModified  = (
    (m_Parameters.GetAdvectionSpeedExponent() != 
     parameters.GetAdvectionSpeedExponent()) ||
    (m_Parameters.GetPropagationSpeedExponent() != 
     parameters.GetPropagationSpeedExponent()) ||
    (m_Parameters.GetCurvatureSpeedExponent() != 
     parameters.GetCurvatureSpeedExponent()));

  // Store the parameters
  m_Parameters = parameters;
  m_ParametersModified = true;
}

void 
SnakeParametersPreviewPipeline
::SetNumberOfSampledPoints(unsigned int number)
{
  if(number!=m_NumberOfSampledPoints)
    {
    m_NumberOfSampledPoints = number;
    m_ControlsModified = true;
    }
}

void
SnakeParametersPreviewPipeline
::Update(Fl_Gl_Window *context)
{
  // Check what work needs to be done
  if(m_SpeedModified)
    {
    UpdateLevelSetFunction();
    }
  if(m_ControlsModified)
    {
    UpdateContour();
    }
  if((m_SpeedModified || m_ControlsModified) && !m_QuickUpdate)
    {
    UpdateLevelSet(context);
    }
  if(m_ParametersModified || 
     ((m_SpeedModified || m_ControlsModified) && !m_QuickUpdate))
    {
    UpdateForces();
    }

  // Clear the modified flags
  m_SpeedModified = m_ControlsModified = m_ParametersModified = false;
}

void 
SnakeParametersPreviewPipeline
::UpdateContour()
{
  // Create a b-spline object
  typedef itk::BSplineInterpolationWeightFunction<double,1,3> FunctionType;
  FunctionType::Pointer function = FunctionType::New();

  // Initialize variables for the interpolation
  FunctionType::ContinuousIndexType position;
  FunctionType::IndexType startIndex;
  FunctionType::WeightsType weights(function->GetNumberOfWeights());;

  // Initialize the sampled point array
  m_SampledPoints.clear();
  m_SampledPoints.reserve(m_NumberOfSampledPoints);
  
  int uMax = m_ControlPoints.size() - 3; 
  for(double t = 0; t < 1.0; t += 0.005)
    {
    position[0] = t * uMax;
    function->Evaluate(position,weights,startIndex);

    Vector2d x(0.0f,0.0f);
    for(unsigned int j=0;j<weights.size();j++)
      {
      int idx = (startIndex[0] + j) % uMax;      
      x += weights[j] * m_ControlPoints[idx];
      }
    m_SampledPoints.push_back(x);
    }
}

void
SnakeParametersPreviewPipeline
::UpdateLevelSetFunction()
{
  // Pass the new image to the level set function
  m_LevelSetFunction->SetSpeedImage(m_SpeedImage);

  // Reset all weights to 1 in the function
  m_LevelSetFunction->SetCurvatureWeight(1.0f);
  m_LevelSetFunction->SetAdvectionWeight(1.0f);
  m_LevelSetFunction->SetPropagationWeight(1.0f);

  // Set the heavy parameters of the level set function
  m_LevelSetFunction->SetAdvectionSpeedExponent(
    m_Parameters.GetAdvectionSpeedExponent());

  m_LevelSetFunction->SetCurvatureSpeedExponent(
    m_Parameters.GetCurvatureSpeedExponent()+1);  

  m_LevelSetFunction->SetPropagationSpeedExponent(
    m_Parameters.GetPropagationSpeedExponent());  

  // Compute the internal images
  m_LevelSetFunction->CalculateInternalImages();
  
  // Call the initialize method
  Size<2> radius = {{1,1}};
  m_LevelSetFunction->Initialize(radius);
}

void
SnakeParametersPreviewPipeline
::UpdateLevelSet(Fl_Gl_Window *context)
{
  // Allocate the flood fill image  
  m_FloodFillImage->SetRegions(m_SpeedImage->GetBufferedRegion());
  m_FloodFillImage->Allocate();
  ScanConvertSpline(context);

  // Compute the distance image
  m_DistanceFilter->Update();

  // Define some objects from the level set function world
  typedef itk::LevelSetFunction<FloatImageType> FunctionType;
  typedef FunctionType::NeighborhoodType NeighborhoodType;
  typedef FunctionType::GlobalDataStruct GlobalDataStruct;

  // Create a neighborhood operator for parsing the image
  Size<2> radius = {{1,1}};
  NeighborhoodType it(radius,m_DistanceFilter->GetOutput(),
                       m_DistanceFilter->GetOutput()->GetBufferedRegion());

  // Clear the array of image points
  m_ImagePoints.clear();

  // Reset all weights to 0 in the function
  m_LevelSetFunction->SetCurvatureWeight(0.0f);
  m_LevelSetFunction->SetAdvectionWeight(0.0f);
  m_LevelSetFunction->SetPropagationWeight(0.0f);

  // Go through all points in the image
  while(!it.IsAtEnd())
    {
    if (vnl_math_abs(it.GetCenterPixel()) <= 1.5 && it.GetCenterPixel() > 0.0)
      {
      // Create a point object
      PointInfo point;

      // Create a global data struct for getting back the normal, etc
      GlobalDataStruct gd;

      // Store the index (position of the point)
      point.index = it.GetIndex();

      // Compute the curvature force
      m_LevelSetFunction->SetCurvatureWeight(1.0f);
      point.UnitCurvatureForce = -m_LevelSetFunction->ComputeUpdate(it,&gd);
      m_LevelSetFunction->SetCurvatureWeight(0.0f);

      // Compute the propagation force
      m_LevelSetFunction->SetPropagationWeight(1.0f);
      point.UnitPropagationForce = -m_LevelSetFunction->ComputeUpdate(it,&gd);
      m_LevelSetFunction->SetPropagationWeight(0.0f);

      // Compute the advection force
      m_LevelSetFunction->SetAdvectionWeight(1.0f);
      point.UnitAdvectionForce = -m_LevelSetFunction->ComputeUpdate(it,&gd);
      m_LevelSetFunction->SetAdvectionWeight(0.0f);

      // Compute the gradient
      point.normal[0] = 
        vnl_math_max(gd.m_dx_backward[0], 0.0f) + 
        vnl_math_min(gd.m_dx_forward[0], 0.0f);
      point.normal[1] = 
        vnl_math_max(gd.m_dx_backward[1], 0.0f) + 
        vnl_math_min(gd.m_dx_forward[1], 0.0f);

      // point.normal = gd.m_dx;

      // Normalize the point
      if(point.normal.squared_magnitude() > 0.0)
        point.normal.normalize();

      // Save the point in the list
      m_ImagePoints.push_back(point);
      }
    ++it;
    }
}

void
SnakeParametersPreviewPipeline
::UpdateForces()
{
  // Iterate over the selected points
  for(ImagePointList::iterator it = m_ImagePoints.begin();
      it!=m_ImagePoints.end();++it)
    {
    it->CurvatureForce = 
      it->UnitCurvatureForce * m_Parameters.GetCurvatureWeight();
    
    it->AdvectionForce = 
      it->UnitAdvectionForce * m_Parameters.GetAdvectionWeight();
    
    it->PropagationForce = 
      it->UnitPropagationForce * m_Parameters.GetPropagationWeight();
    }
}


#ifdef WIN32
typedef void (CALLBACK *TessCallback)();
extern void CALLBACK BeginCallback(GLenum);
extern void CALLBACK EndCallback();
extern void CALLBACK ErrorCallback(GLenum);
extern void CALLBACK CombineCallback(GLdouble[3], GLdouble **,GLfloat *,GLdouble **) ;
#else
typedef void (*TessCallback)();
extern void BeginCallback(GLenum);
extern void EndCallback();
extern void ErrorCallback(GLenum);
extern void CombineCallback(GLdouble*, GLdouble **,GLfloat *,GLdouble **) ;
#endif



void 
SnakeParametersPreviewPipeline
::ScanConvertSpline(Fl_Gl_Window *context)
{
  unsigned int i;

  // Make current context
  context->make_current();

  // Get the image size
  CharImageType::SizeType size = 
    m_FloodFillImage->GetLargestPossibleRegion().GetSize();
  
  // Push the GL attributes
  glPushAttrib(GL_COLOR_BUFFER_BIT | GL_VIEWPORT_BIT | 
    GL_PIXEL_MODE_BIT | GL_TRANSFORM_BIT);

  glDrawBuffer(GL_AUX0);
  glReadBuffer(GL_AUX0);

  glDisable(GL_LIGHTING);

  // Draw polygon into back buffer - back buffer should get redrawn
  // anyway before it gets swapped to the screen.
  glClearColor(0.0,0.0,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Create a new projection matrix.  Why do this?  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0,1.0,0,1.0);

  // New model matrix
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glScaled(1.0 / size[0],1.0 / size[1],1.0);

  // Set up the viewport
  glViewport(0,0,size[0],size[1]);


  // Paint in black
  glColor3d(1,1,1);
 
  // GLU tesselator draws the poly 
  GLUtesselatorObj *tesselator = gluNewTess();
  gluTessCallback(tesselator,(GLenum) GLU_TESS_VERTEX, (TessCallback) glVertex3dv);
  gluTessCallback(tesselator,(GLenum) GLU_TESS_BEGIN, (TessCallback) BeginCallback);
  gluTessCallback(tesselator,(GLenum) GLU_TESS_END, (TessCallback) EndCallback);
  gluTessCallback(tesselator,(GLenum) GLU_TESS_ERROR, (TessCallback) ErrorCallback);     
  gluTessCallback(tesselator,(GLenum) GLU_TESS_COMBINE, (TessCallback) CombineCallback);
  gluTessNormal(tesselator,0,0,1);

  gluTessBeginPolygon(tesselator,NULL);
  gluTessBeginContour(tesselator);
  
  double (*v)[3] = new double[m_SampledPoints.size()][3];
  for (i = 0; i < m_SampledPoints.size(); i++)
    {
    v[i][0] = m_SampledPoints[i][0];
    v[i][1] = m_SampledPoints[i][1];
    v[i][2] = 0.0;
    }

  for(i = 0; i < m_SampledPoints.size(); i++)
    gluTessVertex(tesselator,v[i],v[i]);
  
  gluTessEndContour(tesselator);
  gluTessEndPolygon(tesselator);
  
  gluDeleteTess(tesselator);

  glPopMatrix();
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glFinish();
  glReadPixels(0,0,size[0],size[1],GL_LUMINANCE,GL_FLOAT,m_FloodFillImage->GetBufferPointer());

  glPopAttrib();
  m_FloodFillImage->Modified();
}

