/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SnakeTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "IRISTypes.h"
#include "IRISVectorTypesToITKConversion.h"
#include "SNAPLevelSetFunction.h"
#include "EdgePreprocessingImageFilter.h"
#include "GreyImageWrapperImplementation.h"
#include "SpeedImageWrapperImplementation.h"
#include "IRISApplication.h"
#include "IRISImageData.h"
#include "SNAPImageData.h"
#include "GlobalState.h"

#include "itkImageFileWriter.h"
#include "itkPNGImageIO.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkDenseFiniteDifferenceImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkSparseFieldLevelSetImageFilter.h"
#include "itkSubtractImageFilter.h"

#include <cassert>
#include <cstdio>

using namespace itk;

/**
 * Utility method: write a slice to a PNG file
 */
template <class TPixel>
void WriteSliceToFile(Image<TPixel,2> *slice, const char *fname = "out.png")
{
  // ShiftScale slice to uchar
  typedef RescaleIntensityImageFilter<
    Image<TPixel,2>,Image<unsigned char,2> > RescaleIntensityFilterType;
  typename RescaleIntensityFilterType::Pointer caster
    = RescaleIntensityFilterType::New();
  caster->SetInput(slice);
  caster->SetOutputMaximum(255);
  caster->SetOutputMinimum(0);

  // Write the slice out
  typedef ImageFileWriter<Image<unsigned char,2> > WriterType;  
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(caster->GetOutput());
  writer->SetFileName(fname);
  writer->Update();
}

template <class TImage>
void WriteVolumeToFile(TImage *volume, const char *fname = "out.png")
{
  // Write the slice out
  typedef ImageFileWriter<TImage> WriterType;  
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetInput(volume);
  writer->SetFileName(fname);
  writer->Update();
}



#include <list>

template <class TInputImage, class TOutputImage>
class SimpleDenseFiniteDifferenceImageFilter
  : public DenseFiniteDifferenceImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef SimpleDenseFiniteDifferenceImageFilter Self;
  typedef DenseFiniteDifferenceImageFilter<TInputImage, TOutputImage>
   Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Run-time type information. */
  itkTypeMacro(SimpleDenseFiniteDifferenceImageFilter,
               DenseFiniteDifferenceImageFilter);

  /** Capture information from the superclass. */
  typedef typename Superclass::InputImageType   InputImageType;
  typedef typename Superclass::OutputImageType  OutputImageType;
  typedef typename Superclass::UpdateBufferType UpdateBufferType;

  /** Dimensionality of input and output data is assumed to be the same.
   * It is inherited from the superclass. */
  itkStaticConstMacro(ImageDimension, unsigned int,Superclass::ImageDimension);

  itkNewMacro(SimpleDenseFiniteDifferenceImageFilter);

  /** The pixel type of the output image will be used in computations.
   * Inherited from the superclass. */
  typedef typename Superclass::PixelType PixelType;
  typedef typename Superclass::TimeStepType TimeStepType;

  /** Set/Get the number of iterations that the filter will run. */
  itkSetMacro(NumberOfIterations, unsigned int);
  itkGetMacro(NumberOfIterations, unsigned int);

protected:
  SimpleDenseFiniteDifferenceImageFilter() 
  {
    m_NumberOfIterations = 10;
  }
  
  ~SimpleDenseFiniteDifferenceImageFilter() {}
  
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  }

  /** Supplies the halting criteria for this class of filters.  The
   * algorithm will stop after a user-specified number of iterations. */
  virtual bool Halt() 
  {
    if (this->GetElapsedIterations() >= m_NumberOfIterations) return true;
    else return false;
  }

private:
  SimpleDenseFiniteDifferenceImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  unsigned int     m_NumberOfIterations;
};




template <class TInputImage, class TOutputImage>
class SimpleSparseFiniteDifferenceImageFilter
  : public SparseFieldLevelSetImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef SimpleSparseFiniteDifferenceImageFilter Self;
  typedef SparseFieldLevelSetImageFilter<TInputImage, TOutputImage>
   Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Run-time type information. */
  itkTypeMacro(SimpleSparseFiniteDifferenceImageFilter,
               DenseFiniteDifferenceImageFilter);

  /** Capture information from the superclass. */
  typedef typename Superclass::InputImageType   InputImageType;
  typedef typename Superclass::OutputImageType  OutputImageType;
  typedef typename Superclass::UpdateBufferType UpdateBufferType;

  /** Dimensionality of input and output data is assumed to be the same.
   * It is inherited from the superclass. */
  itkStaticConstMacro(ImageDimension, unsigned int,Superclass::ImageDimension);

  itkNewMacro(SimpleSparseFiniteDifferenceImageFilter);

  /** The pixel type of the output image will be used in computations.
   * Inherited from the superclass. */
  typedef typename Superclass::PixelType PixelType;
  typedef typename Superclass::TimeStepType TimeStepType;

  /** Set/Get the number of iterations that the filter will run. */
  itkSetMacro(NumberOfIterations, unsigned int);
  itkGetMacro(NumberOfIterations, unsigned int);

protected:
  SimpleSparseFiniteDifferenceImageFilter() 
  {
    m_NumberOfIterations = 10;
  }
  
  ~SimpleSparseFiniteDifferenceImageFilter() {}
  
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  }

  /** Supplies the halting criteria for this class of filters.  The
   * algorithm will stop after a user-specified number of iterations. */
  virtual bool Halt() 
  {
    if (this->GetElapsedIterations() >= m_NumberOfIterations) return true;
    else return false;
  }

private:
  SimpleSparseFiniteDifferenceImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  unsigned int     m_NumberOfIterations;
};




/** 
 * This module compares the ITK snake implementation with the
 * older implementation from SNAP
 */
void EdgePreprocessingTest(const char *inputImageFile)
{
  // Create an IRIS Application object
  IRISApplication app;

  // Load the Grey image for the iris application
  GreyImageWrapper *wrapper = new GreyImageWrapperImplementation();
  wrapper->LoadFromFile(inputImageFile);
  
  // Update the image wrapper with the intensity mapping curve
  wrapper->SetIntensityMapFunction(app.GetIntensityCurve());

  // Stick the grey wrapper into the application
  IRISImageData *iid = app.GetCurrentImageData();
  iid->SetGrey(wrapper);

  // Create SNAP data
  Vector3i roiLow(0);
  Vector3i roiHigh = wrapper->GetSize() - Vector3i(1);
  app.InitializeSNAPImageData(roiLow,roiHigh);
  app.SetCurrentImageDataToSNAP();

  // Initialize the speed image
  SNAPImageData *sid = app.GetSNAPImageData();
  sid->InitializeSpeed();

  // Create an edge preprocessor
  typedef EdgePreprocessingImageFilter<
    GreyImageWrapper::ImageType, 
    SpeedImageWrapper::ImageType> EdgeFilterType;
  EdgeFilterType::Pointer pp = EdgeFilterType::New();

  // Create a set of settings
  EdgePreprocessingSettings settings = 
    EdgePreprocessingSettings::MakeDefaultSettings();
  settings.SetGaussianBlurScale(0.5f);
  settings.SetRemappingExponent(2.0f);
  settings.SetRemappingSteepness(0.016);
       
  pp->SetInput(sid->GetGrey()->GetImage());
  pp->SetEdgePreprocessingSettings(settings);
  
  // Connet the preprocessor to the speed image  
  sid->GetSpeed()->SetImage(pp->GetOutput());
  sid->GetSpeed()->SetModeToEdgeSnake();

  // Run the preprocessor
  pp->UpdateLargestPossibleRegion();

  // Position the slice crosshairs in the middle
  sid->SetCrosshairs(sid->GetVolumeExtents() / 2); 
  
  // Now we have a speed image - output a slice
  SpeedImageWrapper::DisplaySlicePointer slice = 
    sid->GetSpeed()->GetDisplaySlice(0);

  // Write the slice out
  WriteSliceToFile<float>(slice);
}

// Create an inverting functor
class InvertFunctor {
public:
  unsigned char operator()(unsigned char input) { 
    return input == 0 ? 1 : 0; 
  }  
};

// Interior extraction functor
class InteriorFunctor {
public:
  unsigned char operator()(float input) { 
    return input <= 0.0f ? 255 : 0; 
  }  
};

/** 
 * This module compares the ITK snake implementation with the
 * older implementation from SNAP
 */
void EdgeSnakeTest(const char *inputImageFile)
{
  typedef SNAPImageData::SnakeWrapperType::ImageType SnakeImageType;
  typedef Image<float,3> FloatImageType;

  // Create an IRIS Application object
  IRISApplication app;

  // Load the Grey image for the iris application
  GreyImageWrapper *wrapper = new GreyImageWrapperImplementation();
  wrapper->LoadFromFile(inputImageFile);
  
  // Update the image wrapper with the intensity mapping curve
  wrapper->SetIntensityMapFunction(app.GetIntensityCurve());

  // Stick the grey wrapper into the application
  IRISImageData *iid = app.GetCurrentImageData();
  iid->SetGrey(wrapper);

  // Create SNAP data
  Vector3i roiLow(0);
  Vector3i roiHigh = wrapper->GetSize() - Vector3i(1);
  app.InitializeSNAPImageData(roiLow,roiHigh);
  app.SetCurrentImageDataToSNAP();

  // Initialize the speed image
  SNAPImageData *sid = app.GetSNAPImageData();
  sid->InitializeSpeed();

  // Create an edge preprocessor
  typedef EdgePreprocessingImageFilter<
    GreyImageWrapper::ImageType, 
    SpeedImageWrapper::ImageType> EdgeFilterType;
  EdgeFilterType::Pointer pp = EdgeFilterType::New();

  // Create a set of settings
  EdgePreprocessingSettings settings = 
    EdgePreprocessingSettings::MakeDefaultSettings();
  settings.SetGaussianBlurScale(0.5f);
  settings.SetRemappingExponent(2.0f);
  settings.SetRemappingSteepness(0.016);
       
  pp->SetInput(sid->GetGrey()->GetImage());
  pp->SetEdgePreprocessingSettings(settings);
  
  // Connet the preprocessor to the speed image  
  sid->GetSpeed()->SetImage(pp->GetOutput());
  sid->GetSpeed()->SetModeToEdgeSnake();

  // Run the preprocessor
  pp->UpdateLargestPossibleRegion();

  // This is a position in the vetricles of image MRIcrop-orig.gipl
  Vector3i target(33,42,53);

  // Position the slice crosshairs in the middle
  // (This assumes a particular image)
  sid->SetCrosshairs(target); 
  
  // Now we have a speed image - output a slice
  SpeedImageWrapper::DisplaySlicePointer slice = 
    sid->GetSpeed()->GetDisplaySlice(0);

  // Create a bubble image
  Bubble bubble;
  bubble.center = target;
  bubble.radius = 2;
  sid->InitializeSnakeImage(&bubble,1,1);

  // Configure snake parameters
  SnakeParameters sparms = SnakeParameters::GetDefaultEdgeParameters();
  sparms.SetTimeStep(0.4f);
  sparms.SetGround(1000.0f);

  sparms.SetPropagationWeight(-1.5f);
  sparms.SetLaplacianWeight(0.0f);
  sparms.SetCurvatureWeight(0.1f);
  sparms.SetAdvectionWeight(1.0f);

  sparms.SetAdvectionSpeedExponent(0);
  sparms.SetCurvatureSpeedExponent(0);
  sparms.SetPropagationSpeedExponent(1);

  // Number of iterations
  unsigned int nIterations = 10;
  
  // Run the old-style snake
  sid->InitalizeSnakeDriver(EDGE_SNAKE,sparms);
  for(unsigned int i=0;i<nIterations;i++) 
    {
    sid->StepSnake(1);
    }

  // Update the snake in sid
  sid->UpdateSnakeImage();

  // Write out the slice from the snake
  // WriteSliceToFile<unsigned char>(sid->GetSnake()->GetSlice(0),"out01.png");
  WriteVolumeToFile<SnakeImageType>(sid->GetSnake()->GetImage(),"snapSnake10.mha");

  // Now run the new-style snake
  typedef SimpleDenseFiniteDifferenceImageFilter<
    FloatImageType,FloatImageType> DenseFilterType;

  // Create the level set function
  typedef SNAPLevelSetFunction<FloatImageType> LevelSetFunctionType;
  LevelSetFunctionType::Pointer phi = LevelSetFunctionType::New();

  // Set up the level set function
  phi->SetAdvectionWeight(-sparms.GetAdvectionWeight());
  phi->SetAdvectionSpeedExponent(sparms.GetAdvectionSpeedExponent());

  phi->SetCurvatureWeight(sparms.GetCurvatureWeight());
  phi->SetCurvatureSpeedExponent(sparms.GetCurvatureSpeedExponent()+1);
  
  phi->SetPropagationWeight(-sparms.GetPropagationWeight());
  phi->SetPropagationSpeedExponent(sparms.GetPropagationSpeedExponent());

  phi->SetLaplacianSmoothingWeight(sparms.GetLaplacianWeight());
  phi->SetLaplacianSmoothingSpeedExponent(sparms.GetLaplacianSpeedExponent());

  phi->SetTimeStep(sparms.GetTimeStep());

  phi->SetSpeedImage(sid->GetSpeed()->GetImage());
  phi->CalculateInternalImages();
  phi->Initialize(to_itkSize(Vector3i(1)));


  // In order to compute a signed distance image, we need to compute a distance
  // function inside the bubbles, then compute a distance function outside of 
  // the bubbles and merge these distances
  typedef SNAPImageData::SnakeWrapperType::ImageType BubbleImageType;
  BubbleImageType::Pointer imgBubbleNormal = 
    sid->GetSnakeInitialization()->GetImage();

  // Create the inverse image
  typedef UnaryFunctorImageFilter<BubbleImageType,BubbleImageType,
    InvertFunctor> InvertFilterType;
  InvertFilterType::Pointer fltInvert = InvertFilterType::New();
  fltInvert->SetInput(imgBubbleNormal);
  fltInvert->ReleaseDataFlagOn();

  // Compute the signed distance function from the bubble image
  typedef DanielssonDistanceMapImageFilter
    <BubbleImageType,FloatImageType> DistanceFilterType;
  DistanceFilterType::Pointer fltDistanceOutside = DistanceFilterType::New();
  fltDistanceOutside->SetInput(imgBubbleNormal);
  fltDistanceOutside->SetInputIsBinary(true);
  fltDistanceOutside->ReleaseDataFlagOn();

  // Compute the second distance function
  DistanceFilterType::Pointer fltDistanceInside = DistanceFilterType::New();
  fltDistanceInside->SetInput(fltInvert->GetOutput());
  fltDistanceInside->SetInputIsBinary(true);
  fltDistanceInside->ReleaseDataFlagOn();

  // Subtract the inside from the outside, forming a signed distance map
  typedef SubtractImageFilter<FloatImageType,
    FloatImageType,FloatImageType> SubtractFilterType;
  SubtractFilterType::Pointer fltSubtract = SubtractFilterType::New();
  fltSubtract->SetInput1(fltDistanceOutside->GetDistanceMap());
  fltSubtract->SetInput2(fltDistanceInside->GetDistanceMap());

  // Create the dense filter
  DenseFilterType::Pointer fltDense = DenseFilterType::New();
  fltDense->SetInput(fltSubtract->GetOutput());
  fltDense->SetDifferenceFunction(phi);
  fltDense->SetNumberOfIterations(nIterations);

  // Compute the interior of the distance transform (negative pixels map to 1,
  // positive pixels map to 0
  typedef UnaryFunctorImageFilter<
    FloatImageType,BubbleImageType,InteriorFunctor> InteriorFilter;
  InteriorFilter::Pointer fltInterior = InteriorFilter::New();
  fltInterior->SetInput(fltDense->GetOutput());

  // Also create a sparse level set solver
  typedef SimpleSparseFiniteDifferenceImageFilter<
    FloatImageType,FloatImageType> SparseFilterType;
  SparseFilterType::Pointer fltSparse = SparseFilterType::New();
  fltSparse->SetInput(fltSubtract->GetOutput());
  fltSparse->SetDifferenceFunction(phi);
  fltSparse->SetNumberOfLayers(3);
  fltSparse->SetIsoSurfaceValue(0.0f);
  fltSparse->SetNumberOfIterations(nIterations-1);
  
  // Now there is a complete pipeline set up for level set evolution

  // Test 1: Bubbles->Level Set->Interior = SNAP
  fltInterior->SetInput(fltSubtract->GetOutput());
  fltInterior->Update();
  WriteVolumeToFile(fltInterior->GetOutput(),"bubbleInOut.mha");
/*
  // Test 2: Do 10 iterations of the algorithm and write the segmentation
  fltInterior->SetInput(fltDense->GetOutput());
  fltInterior->Update();
  WriteVolumeToFile(fltInterior->GetOutput(),"snake10Dense.mha");
*/
  // Test 3: Run the sparse algorithm
  fltInterior->SetInput(fltSparse->GetOutput());
  fltInterior->Update();
  WriteVolumeToFile(fltInterior->GetOutput(),"snake10Sparse.mha");
  
  // Create a slice and write it out
  typedef IRISSlicer<unsigned char> SlicerType;
  SlicerType::Pointer slicer = SlicerType::New();
  slicer->SetInput(fltInterior->GetOutput());
  slicer->SetImageToDisplayTransform(app.GetImageToDisplayTransform());
  slicer->SetSliceAxis(0);
  slicer->SetSliceIndex(target[0]);
  WriteSliceToFile<unsigned char>(slicer->GetOutput(),"out02.png");
}

