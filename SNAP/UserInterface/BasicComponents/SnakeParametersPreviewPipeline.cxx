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

#include "FL/Fl_Gl_Window.h"
#include "GL/glu.h"

#include "LevelSetExtensionFilter.h"
#include "SignedDistanceFilter.h"
#include "SNAPLevelSetFunction.h"
#include "itkBSplineInterpolationWeightFunction.h"
#include "itkBSplineKernelFunction.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkParallelSparseFieldLevelSetImageFilter.h"
#include "itkNarrowBandLevelSetImageFilter.h"
#include "itkVTKImageExport.h"
#include "vtkCellArray.h"
#include "vtkContourFilter.h"
#include "vtkImageData.h"
#include "vtkImageImport.h"
#include "vtkPolyData.h"

using namespace std;
using namespace itk;

#ifdef SNAKE_PREVIEW_ADVANCED

class LevelSetPreviewPipeline2D
{
public:
  // Image type
  typedef itk::Image<float, 2> FloatImageType;

  // Contour type
  typedef std::vector<Vector2d> ContourType;

  // Constructor/destructor
  LevelSetPreviewPipeline2D();
  ~LevelSetPreviewPipeline2D();

  void SetSpeedImage(FloatImageType *image);
  void SetInitialLevelSetImage(FloatImageType *image);
  void SetParameters(const SnakeParameters &parameters);  
  void Update();

  // Get a contour
  ContourType &GetContour(unsigned int i);

  // Set and get number of contours
  irisSetMacro(NumberOfContours,unsigned int);
  irisGetMacro(NumberOfContours,unsigned int);

private:
  // Level set filter
  typedef LevelSetExtensionFilter<
    itk::ParallelSparseFieldLevelSetImageFilter<FloatImageType,FloatImageType> >
    LevelSetFilterType;
  //typedef LevelSetExtensionFilter<
  //  itk::NarrowBandLevelSetImageFilter<FloatImageType,FloatImageType> >
  //  LevelSetFilterType;
  typedef itk::SmartPointer<LevelSetFilterType> LevelSetFilterPointer;

  // Level set function
  typedef SNAPLevelSetFunction<FloatImageType> LevelSetFunctionType;

  // Exporter
  typedef itk::VTKImageExport<FloatImageType> ExporterType;

  // A level set function used to compute the forces
  itk::SmartPointer<LevelSetFunctionType> m_Phi;
  itk::SmartPointer<LevelSetFilterType> m_Filter;

  // VTK import/export
  itk::SmartPointer<ExporterType> m_VTKExporter;
  vtkImageImport *m_VTKImporter;
  vtkContourFilter *m_VTKContour;

  unsigned int m_NumberOfContours;

  // A collection of contours
  std::vector<ContourType> m_Contours;

  // Internal method to add a contour
  void AddContour();

  // A progress callback
  void ProgressCallback() {   }

  // Snake parameters
  SnakeParameters m_Parameters;
  SnakeParameters m_TempParameters;

  bool m_ParametersInitialized;
  bool m_ParametersChangedInUpdate;

  // Progress callback command
  typedef itk::SimpleMemberCommand<LevelSetPreviewPipeline2D> CommandType;
  itk::SmartPointer<CommandType> m_ProgressCommand;
};

LevelSetPreviewPipeline2D
::LevelSetPreviewPipeline2D()
{
  m_Phi = LevelSetFunctionType::New();


  // m_Filter = LevelSetFilterType::New();
  // m_Filter->SetSegmentationFunction(m_Phi);
  // m_Filter->SetNarrowBandTotalRadius(5);
  // m_Filter->SetNarrowBandInnerRadius(3);
  // m_Filter->SetFeatureImage(m_Phi->GetSpeedImage());  

  m_Filter = LevelSetFilterType::New();
  m_Filter->SetDifferenceFunction(m_Phi);
  m_Filter->SetNumberOfLayers(3);
  m_Filter->SetIsoSurfaceValue(0.0f);

  m_VTKExporter = itk::VTKImageExport<FloatImageType>::New();
  m_VTKExporter->SetInput(m_Filter->GetOutput());

  // Initialize the VTK Importer
  m_VTKImporter = vtkImageImport::New();

  // Pipe the importer into the exporter (that's a lot of code)
  m_VTKImporter->SetUpdateInformationCallback(
    m_VTKExporter->GetUpdateInformationCallback());
  m_VTKImporter->SetPipelineModifiedCallback(
    m_VTKExporter->GetPipelineModifiedCallback());
  m_VTKImporter->SetWholeExtentCallback(
    m_VTKExporter->GetWholeExtentCallback());
  m_VTKImporter->SetSpacingCallback(
    m_VTKExporter->GetSpacingCallback());
  m_VTKImporter->SetOriginCallback(
    m_VTKExporter->GetOriginCallback());
  m_VTKImporter->SetScalarTypeCallback(
    m_VTKExporter->GetScalarTypeCallback());
  m_VTKImporter->SetNumberOfComponentsCallback(
    m_VTKExporter->GetNumberOfComponentsCallback());
  m_VTKImporter->SetPropagateUpdateExtentCallback(
    m_VTKExporter->GetPropagateUpdateExtentCallback());
  m_VTKImporter->SetUpdateDataCallback(
    m_VTKExporter->GetUpdateDataCallback());
  m_VTKImporter->SetDataExtentCallback(
    m_VTKExporter->GetDataExtentCallback());
  m_VTKImporter->SetBufferPointerCallback(
    m_VTKExporter->GetBufferPointerCallback());  
  m_VTKImporter->SetCallbackUserData(
    m_VTKExporter->GetCallbackUserData());   

  // Create and configure the contour filter
  m_VTKContour = vtkContourFilter::New();
  m_VTKContour->SetInput(m_VTKImporter->GetOutput());    
  m_VTKContour->ReleaseDataFlagOn();
  m_VTKContour->ComputeScalarsOff();
  m_VTKContour->ComputeGradientsOff();
  m_VTKContour->UseScalarTreeOn();
  m_VTKContour->SetNumberOfContours(1);
  m_VTKContour->SetValue(0, 0.0);

  // Callback stuff
  m_ProgressCommand = CommandType::New();
  m_ProgressCommand->SetCallbackFunction(
    this, &LevelSetPreviewPipeline2D::ProgressCallback);
  //m_Filter->AddObserver(itk::ProgressEvent(),m_ProgressCommand);

  // Set contours
  m_NumberOfContours = 0;
  m_ParametersInitialized = false;
}

LevelSetPreviewPipeline2D
::~LevelSetPreviewPipeline2D()
{
  m_VTKContour->Delete();
  m_VTKImporter->Delete();
}

void
LevelSetPreviewPipeline2D
::SetSpeedImage(FloatImageType *image)
{
  if(image != m_Phi->GetSpeedImage())
    {
    // Update the speed image
    m_Phi->SetSpeedImage(image);
    m_Phi->CalculateInternalImages();
    itk::Size<2> radius = {{1,1}};
    m_Phi->Initialize(radius);

    // Touch the level set filter
    // m_Filter->SetFeatureImage(m_Phi->GetSpeedImage());    
    m_Filter->Modified();
    }    
}

void
LevelSetPreviewPipeline2D
::SetInitialLevelSetImage(FloatImageType *image)
{
  if(m_Filter->GetInput() != image)
    {
    m_Filter->SetInput(image);
    }
}

void
LevelSetPreviewPipeline2D
::SetParameters(const SnakeParameters &p)
{
  if(m_Filter->IsUpdating()) 
    {
    m_TempParameters = p;
    m_ParametersChangedInUpdate = true;
    return;
    }

  if(!m_ParametersInitialized || !(p == m_Parameters))
    {
    // Set up the level set function
    m_Phi->SetAdvectionWeight(p.GetAdvectionWeight());
    m_Phi->SetAdvectionSpeedExponent(p.GetAdvectionSpeedExponent());
    m_Phi->SetCurvatureWeight(p.GetCurvatureWeight());
    m_Phi->SetCurvatureSpeedExponent(p.GetCurvatureSpeedExponent()+1);  
    m_Phi->SetPropagationWeight(p.GetPropagationWeight());
    m_Phi->SetPropagationSpeedExponent(p.GetPropagationSpeedExponent());  
    m_Phi->SetLaplacianSmoothingWeight(p.GetLaplacianWeight());
    m_Phi->SetLaplacianSmoothingSpeedExponent(p.GetLaplacianSpeedExponent());  

    // We only need to recompute the internal images if the exponents to those
    // images have changed
    m_Phi->CalculateInternalImages();

    // Call the initialize method
    itk::Size<2> radius = {{1,1}};
    m_Phi->Initialize(radius);

    // Set the time step
    m_Phi->SetTimeStep(p.GetAutomaticTimeStep() ? 0.0 : p.GetTimeStep());

    // Remember the parameters
    m_Parameters = p;
    m_ParametersInitialized = true;

    // Update the filter
    m_Filter->Modified();
    }
}

void
LevelSetPreviewPipeline2D
::AddContour()
{
  // Compute the zero sets
  m_VTKExporter->UpdateLargestPossibleRegion();
  m_VTKImporter->UpdateWholeExtent();
  m_VTKContour->UpdateWholeExtent();
  
  // Get the contour
  vtkPolyData *pd = m_VTKContour->GetOutput();
  ContourType contour;
  contour.reserve(pd->GetNumberOfCells() * 2);
  for(unsigned int i=0;i<pd->GetNumberOfCells();i++)
    {
    float *pt1 = pd->GetPoint(pd->GetCell(i)->GetPointId(0));
    float *pt2 = pd->GetPoint(pd->GetCell(i)->GetPointId(1));
    contour.push_back(Vector2d(pt1[0],pt1[1]));
    contour.push_back(Vector2d(pt2[0],pt2[1]));
    }

  m_Contours.push_back(contour);
}


void
LevelSetPreviewPipeline2D
::Update()
{
  // Clear the contour
  m_Contours.clear();

  // Make sure parameters don't change in update
  m_ParametersChangedInUpdate = false;

  // Run filter for 0 iterations
  m_Filter->SetIterationsUntilPause(0);
  m_Filter->UpdateLargestPossibleRegion();
  AddContour();
  
  // Run the filter
  m_Filter->SetIterationsUntilPause(20);
  m_Filter->GetInput()->Modified();
  m_Filter->UpdateLargestPossibleRegion();
  AddContour();

  if(m_ParametersChangedInUpdate)
    SetParameters(m_TempParameters);
}

// Get a contour
LevelSetPreviewPipeline2D::ContourType &
LevelSetPreviewPipeline2D
::GetContour(unsigned int i)
{
  return m_Contours[i];
}

#endif // SNAKE_PREVIEW_ADVANCED


SnakeParametersPreviewPipeline
::SnakeParametersPreviewPipeline()
{
  // Start with a 100 interpolated points
  m_NumberOfSampledPoints = 100;

  m_ControlsModified = false;
  m_SpeedModified = false;
  m_ParametersModified = false;
  m_QuickUpdate = false;
  m_Updating = false;

#ifdef SNAKE_PREVIEW_ADVANCED
  
  // The flood fill image
  m_FloodFillImage = FloatImageType::New();

  // A filter used to fill the pipeline
  m_DistanceFilter = DistanceFilterType::New();
  m_DistanceFilter->SetInput(m_FloodFillImage);

  // Initialize the level set function
  for(unsigned int i=0;i<4;i++)
    {
    m_LevelSetPipeline[i] = new LevelSetPreviewPipeline2D;
    m_LevelSetPipeline[i]->SetInitialLevelSetImage(
      m_DistanceFilter->GetOutput());
    }

#endif // SNAKE_PREVIEW_ADVANCED
}

SnakeParametersPreviewPipeline
::~SnakeParametersPreviewPipeline()
{
#ifdef SNAKE_PREVIEW_ADVANCED
  
  // Initialize the level set function
  for(unsigned int i=0;i<4;i++)
    {
    delete m_LevelSetPipeline[i];
    }

#endif // SNAKE_PREVIEW_ADVANCED
}

void
SnakeParametersPreviewPipeline
::SetControlPoints(const std::vector<Vector2d> &points)
{
  if(m_ControlPoints != points)
    {  
    // Save the points
    m_ControlPoints = points;
    for(unsigned int i=0;i<m_ControlPoints.size();i++)
      m_ControlPoints[i] *= 0.25;

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
#ifdef SNAKE_PREVIEW_ADVANCED
  
  // Store the image
  for(unsigned int i=0;i<4;i++)
    m_LevelSetPipeline[i]->SetSpeedImage(image);
  
#endif // SNAKE_PREVIEW_ADVANCED

  // Update the image internally
  if(image != m_SpeedImage)
    {
    // Set the modified flag
    m_SpeedModified = true;
    m_SpeedImage = image;

    // Create a filter to compute a gradient image
    typedef GradientImageFilter<FloatImageType> GradientFilter;
    GradientFilter::Pointer filter = GradientFilter::New();

    // Set up and run the filter
    filter->SetInput(m_SpeedImage);
    m_GradientImage = filter->GetOutput();
    filter->Update();
    }
}

void
SnakeParametersPreviewPipeline
::SetSnakeParameters(const SnakeParameters &parameters)
{
  // Clean up the parameters
  SnakeParameters clean = parameters;
  clean.SetClamp(false);
  clean.SetGround(0);
  clean.SetLaplacianSpeedExponent(0);
  clean.SetLaplacianWeight(0);
  clean.SetSolver(SnakeParameters::SPARSE_FIELD_SOLVER);

  // Don't waste time on nonsense
  if(m_Parameters == clean) return;
    
  // Save the parameters
  m_Parameters = clean;
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
  // Exit if already updating
  // if(m_Updating) return;
  m_Updating = true;

  // Check what work needs to be done
  if(m_ControlsModified)
    {
    UpdateContour();
    }
  if(!m_QuickUpdate)
    {
    if(m_ControlsModified)
      {
      UpdateLevelSet(context);
      }
    if(m_ParametersModified || m_ControlsModified)
      {
      UpdateForces();
      m_ParametersModified = false;
      }
    }

  // Clear the modified flags
  m_ControlsModified = false;
  m_Updating = false;
}

void 
SnakeParametersPreviewPipeline
::UpdateContour()
{
  // Create a b-spline object
  typedef itk::BSplineInterpolationWeightFunction<double,1,3> FunctionType;
  FunctionType::Pointer function = FunctionType::New();

  // Used to compute spline derivatives
  itk::BSplineKernelFunction<3>::Pointer kf3 = itk::BSplineKernelFunction<3>::New();
  itk::BSplineKernelFunction<2>::Pointer kf2 = itk::BSplineKernelFunction<2>::New();
  itk::BSplineKernelFunction<2>::Pointer kf1 = itk::BSplineKernelFunction<2>::New();

  // Initialize the sampled point array
  m_SampledPoints.clear();
  m_SampledPoints.reserve(m_NumberOfSampledPoints);
  
  int uMax = m_ControlPoints.size() - 3; 
  for(double t = 0; t < 1.0; t += 0.005)
    {
    double s = t * uMax;
    /*
    position[0] = s; 
    function->Evaluate(position,weights,startIndex);

    Vector2d x(0.0f,0.0f);
    Vector2d xu(0.0f,0.0f);
    for(unsigned int j=0;j<weights.size();j++)
      {
      int idx = (startIndex[0] + j) % uMax;      
      x += weights[j] * m_ControlPoints[idx];
      }*/
    
    // The starting index
    // int si = ((int)(t * uMax)) - 1;
    int sidx = (int) floor(s - 1);
    double u = s - sidx;

    // Compute the position and derivatives of the b-spline
    Vector2d x(0.0f,0.0f);
    Vector2d xu(0.0f,0.0f);
    Vector2d xuu(0.0f,0.0f);
    for(int k=0; k < 4; k++)
      {      
      double w = kf3->Evaluate(u);
      double wu = kf2->Evaluate(u+0.5) - kf2->Evaluate(u-0.5);
      double wuu = kf1->Evaluate(u+1) + kf1->Evaluate(u-1) - 2 * kf1->Evaluate(u);
      u-=1.0;

      int idx = (uMax + sidx + k) % uMax;
      x += w * m_ControlPoints[idx];
      xu += wu * m_ControlPoints[idx];
      xuu += wuu * m_ControlPoints[idx];
      }

    // Create and save the point
    SampledPoint pt;
    pt.x = x;
    pt.t = t;
    xu.normalize();
    pt.n = Vector2d(-xu[1],xu[0]);    
    pt.PropagationForce = pt.CurvatureForce = pt.AdvectionForce = 0.0;
    pt.kappa 
      = (xu[0] * xuu[1] - xu[1] * xuu[0]) / pow(xu[0]*xu[0] + xu[1]*xu[1],1.5);

    m_SampledPoints.push_back(pt);
    }
}

void
SnakeParametersPreviewPipeline
::UpdateLevelSetFunction()
{
}

#ifdef SNAKE_PREVIEW_ADVANCED
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
}
#else
void
SnakeParametersPreviewPipeline
::UpdateLevelSet(Fl_Gl_Window *irisNotUsed(context))
{
}
#endif // SNAKE_PREVIEW_ADVANCED

void
SnakeParametersPreviewPipeline
::UpdateForces()
{
#ifdef SNAKE_PREVIEW_ADVANCED
  // Compute the sparse field filters
  for(unsigned int i=0; i < 4; i++)
    {
    // Make a copy of the parameters
    SnakeParameters special = m_Parameters;

    // Set some parameters to zero
    if(i != CURVATURE && i != TOTAL)
      {
      special.SetCurvatureSpeedExponent(-1);
      special.SetCurvatureWeight(0);
      }
    if(i != ADVECTION && i != TOTAL)
      {
      special.SetAdvectionSpeedExponent(0);
      special.SetAdvectionWeight(0);
      }
    if(i != PROPAGATION && i != TOTAL)
      {
      special.SetPropagationSpeedExponent(0);
      special.SetPropagationWeight(0);
      }

    // Pass parameters to the pipeline
    m_LevelSetPipeline[i]->SetParameters(special);    
    m_LevelSetPipeline[i]->Update();
    }

#else 

  // Image interpolator types
  typedef LinearInterpolateImageFunction<
    FloatImageType,double> LerpType;
  typedef VectorLinearInterpolateImageFunction<
    VectorImageType,double> VectorLerpType;
  
  // Create the speed image interpolator
  LerpType::Pointer sLerp = LerpType::New();
  sLerp->SetInputImage(m_SpeedImage);

  // Create the gradient image interpolator
  VectorLerpType::Pointer gLerp = VectorLerpType::New();
  gLerp->SetInputImage(m_GradientImage);
  
  // Compute the geometry of each point
  for(unsigned int i = 0; i < m_SampledPoints.size(); i++)
    {
    // A reference so we can access the point in shorthand
    SampledPoint &p = m_SampledPoints[i];
    
    // We're done computing the geometric properties of the curve.  Now, let's
    // compute the image-related quantities.  First, convert the point to image
    // coordinates
    LerpType::ContinuousIndexType idx;
    idx[0] = p.x[0];
    idx[1] = p.x[1];

    // Get the value of the g function
    double g = sLerp->EvaluateAtContinuousIndex(idx);

    // Get the value of the gradient
    VectorLerpType::OutputType gradG = gLerp->EvaluateAtContinuousIndex(idx);
    
    // Compute the propagation force component of the curve evolution
    p.PropagationForce = m_Parameters.GetPropagationWeight() 
      * pow(g,m_Parameters.GetPropagationSpeedExponent());

    // Compute the curvature force component of the curve evolution
    p.CurvatureForce = m_Parameters.GetCurvatureWeight() * p.kappa
      * pow(g,m_Parameters.GetCurvatureSpeedExponent()+1);

    // Compute the advection force component of the curve evolution
    p.AdvectionForce = m_Parameters.GetAdvectionWeight()
      * (p.n[0] * gradG[0] + p.n[1] * gradG[1])
      * pow(g,m_Parameters.GetAdvectionSpeedExponent());    
    }

#endif // SNAKE_PREVIEW_ADVANCED
}

#ifdef SNAKE_PREVIEW_ADVANCED

const SnakeParametersPreviewPipeline::LevelSetContourType &
SnakeParametersPreviewPipeline
::GetLevelSetContour(ForceType force, unsigned int level)
{
  return m_LevelSetPipeline[(unsigned int)force]->GetContour(level);
  return LevelSetContourType();
}

#endif // SNAKE_PREVIEW_ADVANCED

#if defined(WIN32) && !defined(__CYGWIN__)
typedef void (CALLBACK *TessCallback)();
extern void CALLBACK BeginCallback(GLenum);
extern void CALLBACK EndCallback();
extern void CALLBACK ErrorCallback(GLenum);
extern void CALLBACK CombineCallback(GLdouble[3], GLdouble **,GLfloat *,GLdouble **) ;
#else
#if defined(__CYGWIN__)
typedef void (__stdcall *TessCallback)();
extern void __stdcall BeginCallback(GLenum);
extern void __stdcall EndCallback();
extern void __stdcall ErrorCallback(GLenum);
extern void __stdcall CombineCallback(GLdouble[3], GLdouble **,GLfloat *,GLdouble **) ;
#else
typedef void (*TessCallback)();
extern void BeginCallback(GLenum);
extern void EndCallback();
extern void ErrorCallback(GLenum);
extern void CombineCallback(GLdouble*, GLdouble **,GLfloat *,GLdouble **) ;
#endif
#endif

#ifdef SNAKE_PREVIEW_ADVANCED

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
    v[i][0] = m_SampledPoints[i].x[0];
    v[i][1] = m_SampledPoints[i].x[1];
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

#endif // SNAKE_PREVIEW_ADVANCED

