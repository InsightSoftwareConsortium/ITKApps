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

#include "IRISTypes.h"
#include "itkImage.h"
#include "SnakeParameters.h"
#include <vector>

template<class TInputImage, class TOutputImage> class SignedDistanceFilter;
template<class TInputImage> class SNAPLevelSetFunction;
class Fl_Gl_Window;

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

  // A structure representing a point in the level set image for which the 
  // evolution forces have been computed
  struct PointInfo {
    IndexType index;              // The index into the image
    Vector2d normal;              // I.e., the normalized gradient of phi.
    double CurvatureForce;        // Curvature force acting on the point
    double PropagationForce;      // Propagation force acting on the point
    double AdvectionForce;        // Advection force acting on the point
    double UnitCurvatureForce;    // Unit curvature force (unscaled by parameters)
    double UnitAdvectionForce;    // Unit advection force (unscaled by parameters)
    double UnitPropagationForce;  // Unit propagation force (unscaled by parameters)
  };

  // Various list types
  typedef std::vector<Vector2d> ControlPointList;
  typedef std::vector<Vector2d> SampledPointList;  
  typedef std::vector<PointInfo> ImagePointList;

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

  /** Get the speed image */
  irisGetMacro(SpeedImage,FloatImageType *);
  
  /** Get a reference to the control points of the interface curve */
  irisGetMacro(ControlPoints,const ControlPointList &);

  /** Get a list of densely interpolated points on the curve (for drawing) */
  irisGetMacro(SampledPoints,const SampledPointList &);

  /** Get a list of points with forces computed */
  irisGetMacro(ImagePoints, const ImagePointList &);

private:
      
  // Filter types used in the pipeline
  typedef SignedDistanceFilter<FloatImageType,FloatImageType> DistanceFilterType;      
  typedef SNAPLevelSetFunction<FloatImageType> LevelSetFunctionType;

  /** The speed image */
  itk::SmartPointer<FloatImageType> m_SpeedImage;
  
  /** An image containing the binary image of the spline inside */
  itk::SmartPointer<FloatImageType> m_FloodFillImage;

  /** A set of snake parameters */
  SnakeParameters m_Parameters;
    
  /** A list of control points */
  ControlPointList m_ControlPoints;
  
  /** Number of points to sample from the curve */
  unsigned int m_NumberOfSampledPoints;

  /** A list of sampled points */
  SampledPointList m_SampledPoints;

  /** A list of points with forces computed on the image */
  ImagePointList m_ImagePoints;
 
  // A filter used to fill the pipeline
  itk::SmartPointer<DistanceFilterType> m_DistanceFilter;

  // A level set function used to compute the forces
  itk::SmartPointer<LevelSetFunctionType> m_LevelSetFunction;
  
  // Flags indicating which part of the pipeline should be refreshed
  bool m_ControlsModified;
  bool m_SpeedModified;
  bool m_ParametersModified;
  bool m_QuickUpdate;
    
  // Internal components of the Update method
  void UpdateLevelSetFunction();
  void UpdateContour();
  void UpdateLevelSet(Fl_Gl_Window *context);
  void UpdateForces();
  void Update();

  // A method to scan-convert a polygon
  void ScanConvertSpline(Fl_Gl_Window *context);  
};


#endif // __SnakeParametersPreviewPipeline_h_
