/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    vtkITKMutualInformationTransform.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __vtkITKMutualInformationTransform_h
#define __vtkITKITKMutualInformationTransform_h
// .NAME vtkITKMutualInformationTransform - a linear transform specified by two images

// .SECTION Description

// MutualInformationTransform computes a transformation that will align
// the source image with the target image.  The transform can be
// initialized with a vtkLinearTransform or reset with the Identity
// method.
// The algorithm is described in the paper: Viola, P. and Wells III,
// W. (1997).  "Alignment by Maximization of Mutual Information"
// International Journal of Computer Vision, 24(2):137-154
// .SECTION see also
// vtkLinearTransform

#include "vtkLinearTransform.h"

#include "itkImageRegion.h"

class vtkImageData;

typedef itk::ImageRegion<3> ImageExtentType;

class VTK_EXPORT vtkITKMutualInformationTransform : public vtkLinearTransform
{
public:
  static vtkITKMutualInformationTransform *New();

  vtkTypeRevisionMacro(vtkITKMutualInformationTransform,vtkLinearTransform);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify the source and target images. The two images must have
  // the same scalar type.
  // Otherwise, the images can differ in scaling, resolution, etc.
  void SetSourceImage(vtkImageData *source);
  void SetTargetImage(vtkImageData *target);
  vtkGetObjectMacro(SourceImage, vtkImageData);
  vtkGetObjectMacro(TargetImage, vtkImageData);

  // Description:
  // Set the learning rate for the algorithm.
  vtkSetMacro(LearningRate, double);
  vtkGetMacro(LearningRate, double);

  // Description:
  // Set the standard deviations of the parzen window density estimators.
  vtkSetMacro(SourceStandardDeviation, double);
  vtkGetMacro(SourceStandardDeviation, double);
  vtkSetMacro(TargetStandardDeviation, double);
  vtkGetMacro(TargetStandardDeviation, double);
  
  // Description:
  // Set/Get the extent over which the metric will be computed
  void SetFixedImageExtent(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax);
  ImageExtentType GetFixedImageExtent() 
    { return FixedImageExtent; }

  // Description:
  // Set/Get the number of histogram bins
  void SetNumberOfHistogramBins(int numBins)
    { NumberOfHistogramBins = numBins; }
  int GetNumberOfHistogramBins()
    { return NumberOfHistogramBins; }

  // Description:
  // Set/Get maximum step length
  void SetMaximumStepLength(float maxStepLength)
    { MaximumStepLength = maxStepLength; }  
  float GetMaximumStepLength()
    { return MaximumStepLength; }  

  // Description:
  // Set/Get minimum step length
  void SetMinimumStepLength(float minStepLength)
    { MinimumStepLength = minStepLength; }   
  float GetMinimumStepLength()
    { return MinimumStepLength; }   

  // Description:
  // Set/Unset/Get use of Mattes Mutual Information 
  void MattesOn()
    { UseMattes = true; }
  void MattesOff()
    { UseMattes = false; }
  bool GetMattes()
    { return UseMattes; }

  // Description:
  // Set the number of iterations
  vtkSetMacro(NumberOfIterations, int);
  vtkGetMacro(NumberOfIterations, int);

  // Description:
  // Set the number of sample points for density estimation
  vtkSetMacro(NumberOfSamples, int);
  vtkGetMacro(NumberOfSamples, int);

  // Description:
  // Set the translation scale factor.
  vtkSetMacro(TranslateScale, double);
  vtkGetMacro(TranslateScale, double);

  // Description:
  // Invert the transformation.  This is done by switching the
  // source and target images.
  void Inverse();

  // Description:
  // Set the transformation to the Identity matrix.
  void Identity();

  // Description:
  // Initialize the transformation to a linear transform.
  void Initialize(vtkLinearTransform *);

  // Description:
  // Get the MTime.
  unsigned long GetMTime();

protected:
  vtkITKMutualInformationTransform();
  ~vtkITKMutualInformationTransform();

  // Update the matrix from the quaternion.
  void InternalUpdate();

  // Description:
  // Make another transform of the same type.
  vtkAbstractTransform *MakeTransform();

  // Description:
  // This method does no type checking, use DeepCopy instead.
  void InternalDeepCopy(vtkAbstractTransform *transform);

  vtkImageData *SourceImage;
  vtkImageData *TargetImage;
 
  double LearningRate;
  double SourceStandardDeviation;
  double TargetStandardDeviation;
  double TranslateScale;
  
  int NumberOfSamples;
  int NumberOfIterations;
  int NumberOfHistogramBins;   //For Mattes Mutual Information

  float MaximumStepLength;   //For Mattes Mutual Information Optimizer 
  float MinimumStepLength;   //For Mattes Mutual Information Optimizer

  bool UseMattes;

  ImageExtentType FixedImageExtent;

private:
  vtkITKMutualInformationTransform(const vtkITKMutualInformationTransform&);  // Not implemented.
  void operator=(const vtkITKMutualInformationTransform&);  // Not implemented.
};
  
#endif
