/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SnakeParametersPreviewPipeline.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __SnakeParametersPreviewPipeline_h_
#define __SnakeParametersPreviewPipeline_h_

#include "SNAPCommonUI.h"
#include "itkImage.h"
#include "itkCovariantVector.h"
#include "SnakeParameters.h"

template<class TInputImage, class TOutputImage> class SignedDistanceFilter;
template<class TInputImage> class SNAPLevelSetFunction;
template<class TFilter> class LevelSetExtensionFilter;
class Fl_Gl_Window;

class vtkImageImport;
class vtkContourFilter;
class LevelSetPreviewPipeline2D;

namespace itk {
  template<class TInputImage, class TOutputImage> 
    class SparseFieldLevelSetImageFilter;
  template<class TInputImage> class VTKImageExport;
};

/** 
 * \class SnakeParametersPreviewPipeline
 * \brief A pipeline used to preview snake parameters.
 * 
 * Given a set of control points an image, and some snake parameters, this
 * class computes a b-spline curve based on those control points, creates a
 * level set embedding of the curve, and computes various level set evolution
 * forces acting on the curve.
 */
class SnakeParametersPreviewPipeline
{
public:
  SnakeParametersPreviewPipeline();
  virtual ~SnakeParametersPreviewPipeline();

  // Images used by this class (internally and externally)
  typedef itk::Image<unsigned char, 2> CharImageType;
  typedef itk::Image<float, 2> FloatImageType;

  // Index type used to refer to pixels
  typedef FloatImageType::IndexType IndexType;

  // Force types
  enum ForceType {CURVATURE=0, ADVECTION, PROPAGATION, TOTAL };
  
  // A sample from the curve
  struct SampledPoint {
    // The geometry of the point
    double t;
    Vector2d x;
    Vector2d n;
    double kappa;
    
    // The forces acting on the point
    double PropagationForce;
    double CurvatureForce;
    double AdvectionForce;
    
  };

  // Various list types
  typedef std::vector<Vector2d> ControlPointList;
  typedef std::vector<Vector2d> LevelSetContourType;
  typedef std::vector<SampledPoint> SampledPointList;  

  /** Set the speed image */
  void SetSpeedImage(FloatImageType *image);

  /** Set the snake parameters */
  void SetSnakeParameters(const SnakeParameters &parameters);

  /** Set the control points of the interface curve */
  void SetControlPoints(const ControlPointList &points);

  /** Change just one control point, with an option of changing it
   * quickly, ie, not recomputing the level set, only the curve */
  void ChangeControlPoint(unsigned int index, const Vector2d &point,
    bool quickUpdate);

  /** Set the number of points sampled for display of the curve */
  void SetNumberOfSampledPoints(unsigned int number);
  
  /** Update the internals of the pipeline and compute the curve and the
   * force points.  This method requires a GL context because it relies on
   * GL tesselation code for generating an image from the curve */
  void Update(Fl_Gl_Window *context);

#ifdef SNAKE_PREVIEW_ADVANCED

  /** Get the contours associated with a particular level set */
  const LevelSetContourType &GetLevelSetContour(
    ForceType force, unsigned int level);

#endif

  /** Get the speed image */
  irisGetMacro(SpeedImage,FloatImageType *);
  
  /** Get a reference to the control points of the interface curve */
  irisGetMacro(ControlPoints,const ControlPointList &);

  /** Get a list of densely interpolated points on the curve (for drawing) */
  irisGetMacro(SampledPoints,const SampledPointList &);

private:
      
  /** The speed image */
  itk::SmartPointer<FloatImageType> m_SpeedImage;

  // Gradient image used by this component
  typedef itk::CovariantVector<float,2> VectorType;
  typedef itk::Image<VectorType,2> VectorImageType;
  typedef itk::SmartPointer<VectorImageType> VectorImagePointer;
  
  /** The grandient of the speed image */
  VectorImagePointer m_GradientImage;
  
  /** A set of snake parameters */
  SnakeParameters m_Parameters;
    
  /** A list of control points */
  ControlPointList m_ControlPoints;
  
  /** Number of points to sample from the curve */
  unsigned int m_NumberOfSampledPoints;

  /** A list of sampled points */
  SampledPointList m_SampledPoints;

  // Flags indicating which part of the pipeline should be refreshed
  bool m_ControlsModified;
  bool m_SpeedModified;
  bool m_ParametersModified;
  bool m_QuickUpdate;
  bool m_Updating;
    
  // Internal components of the Update method
  void UpdateLevelSetFunction();
  void UpdateContour();
  void UpdateLevelSet(Fl_Gl_Window *context);
  void UpdateForces();
  void Update();


#ifdef SNAKE_PREVIEW_ADVANCED
  // Filter types used in the pipeline
  typedef SignedDistanceFilter<FloatImageType,FloatImageType> DistanceFilterType;      
  typedef SNAPLevelSetFunction<FloatImageType> LevelSetFunctionType;

  /** An image containing the binary image of the spline inside */
  itk::SmartPointer<FloatImageType> m_FloodFillImage;

  // A filter used to fill the pipeline
  itk::SmartPointer<DistanceFilterType> m_DistanceFilter;

  // The mini leve set pipelines
  LevelSetPreviewPipeline2D *m_LevelSetPipeline[4];

  // A method to scan-convert a polygon
  void ScanConvertSpline(Fl_Gl_Window *context);  
#endif // SNAKE_PREVIEW_ADVANCED

};


#endif // __SnakeParametersPreviewPipeline_h_
