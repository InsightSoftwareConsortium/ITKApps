/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    GUITestPadLogic.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "GUITestPadLogic.h"

#include "GreyImageWrapperImplementation.h"
#include "ImageIOWizardLogic.h"

#include "itkDiscreteGaussianImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"

#include "vtkImageGaussianSmooth.h"
#include "vtkImageConstantPad.h"
#include "vtkImageFFT.h"

#include <ctime>
#include <iostream>

using namespace std;

extern void TestTemplateImageWrapper(const char *);
extern void EdgePreprocessingTest(const char *);
extern void EdgeSnakeTest(const char *);

bool GUITestPadLogic
::LoadShortImage(GreyImageWrapper *wrapper) 
{
  // Some types that we will be using
  typedef ImageIOWizardLogic<GreyType> WizardType;

  // Create an instance of the wizard
  static WizardType *wizard = NULL;
  if(wizard == NULL) {
      wizard = new WizardType();
      wizard->MakeWindow();
  }

  // Show the wizard
  return wizard->DisplayInputWizard(wrapper);
}

void
GUITestPadLogic
::OnTestInputWizard() 
{
  typedef GreyImageWrapperImplementation WrapperType;
  WrapperType *wrapper = new WrapperType;
  
  LoadShortImage(wrapper);
}

#include <IntensityCurveVTK.h>
#include <IntensityCurveUILogic.h>
 
void GUITestPadLogic::OnTestIntensityCurves() {
    
    // Create an intensity curve
    IntensityCurveVTK::Pointer curve = IntensityCurveVTK::New();
    curve->Initialize(5);

    // Update one of the control points
    // float t,x;
    // curve.GetControlPoint(1,t,x);
    // curve.UpdateControlPoint(1,t,x+0.05f);

    // Create the UI window
    IntensityCurveUILogic ui;
    ui.MakeWindow();
    ui.SetCurve(curve);    

    // Show the windows
    ui.DisplayWindow();

    // Run until the window is closed
    while(ui.m_WinCurve->visible())
        Fl::wait();
}

void 
GUITestPadLogic
::OnSingleImageBrowseAction()
{
  static const char *pattern =
    "GIPL Files (*.{gipl,gipl.gz})\t"
    "MetaImage Files (*.{mha,mhd})\t"
    "Analyze Files (*.{img,img.gz})\t"
    "RAW Files (*.{raw,raw.gz})\t"
    "All Files (*.*)";

  // Show a file dialog
  char *fname = fl_file_chooser("Select an image file",pattern, NULL);

  // Stick the file into the text box
  m_InTestSingleImageFile->value(fname);
}

void 
GUITestPadLogic
::OnSingleImageTestAction()
{
  const char *fname = m_InTestSingleImageFile->value();
  int value = m_InTestSingleImageTestName->value();

  cout << "Running test" << value << " on image " << fname << endl;

  switch(value) 
    {
    case 0: TestTemplateImageWrapper(fname);
            break;
    case 1: EdgePreprocessingTest(fname);
            break;
    case 3: EdgeSnakeTest(fname);
            break;
    }
}

using namespace itk;

class GaussianPerformanceTester 
{
public:
  // Typedefs
  typedef itk::Image<float,3> ImageType;
  typedef itk::DiscreteGaussianImageFilter<ImageType,ImageType> DiscreteFilter;
  typedef itk::SmoothingRecursiveGaussianImageFilter<ImageType,ImageType> 
    RecursiveFilter;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  typedef itk::ImageRegionConstIterator<ImageType> IteratorType;

  void RunTest(const char *imageFile2D)
  {
    // TODO: Unhardcode this! 
    // Load an image
    ReaderType::Pointer fltReader = ReaderType::New();
    fltReader->SetFileName(imageFile2D);
    fltReader->Update();

    // Create Gaussian filters
    DiscreteFilter::Pointer fltDiscrete = DiscreteFilter::New();
    fltDiscrete->SetInput(fltReader->GetOutput());

    // Create the 'truth' filter    
    DiscreteFilter::Pointer fltTruth = DiscreteFilter::New();
    fltTruth->SetInput(fltReader->GetOutput());
    fltTruth->SetMaximumError(0.005);
    fltTruth->SetMaximumKernelWidth(32);

    RecursiveFilter::Pointer fltRecursive = RecursiveFilter::New();
    fltRecursive->SetInput(fltReader->GetOutput());

    // For each scale, we will compute a family of discrete gaussians and a
    // recursive gaussian
    
    for (float scale=0.1f;scale<10.0f;scale*=sqrt(2.0f))
      {

      clock_t t0;   

      verbose << "------------------------------------" << endl;
      verbose << "Starting Experiment; Scale = " << scale << endl;

      // Compute what we will call the truth image, that has a very small error
      Vector3f variance(scale*scale);

      fltTruth->SetVariance(variance.data_block());
      fltTruth->Update();
      ImageType::Pointer imgTruth = fltTruth->GetOutput();

      // Set the scale of the tested filters
      fltDiscrete->SetVariance(variance.data_block());

      fltRecursive->SetSigma(scale);
      fltRecursive->SetNormalizeAcrossScale(true);

      // A table of discrete gaussian errors
      const unsigned int errorTableSize = 12;
      float error = 0.01;

      // Compute the discrete images
      for (unsigned int i=0;i<errorTableSize;i++)
        {
        Vector3f maxError(error);
        fltDiscrete->SetMaximumError(maxError.data_block());

        t0 = clock();
        fltDiscrete->Update();
        
        Experiment expDiscrete;
        expDiscrete.Time = clock() - t0;
        expDiscrete.Image = fltDiscrete->GetOutput();

        // Compute and print the statistics of the experiment
        verbose << "DG\t" << error;
        CompareAndPrint(expDiscrete,imgTruth);
        verbose << endl;

        error *= sqrt(2.0f);
        }

      // Compute the recursive image
      Experiment expRecursive;    
      t0 = clock();
      fltRecursive->Update();
      expRecursive.Time = clock() - t0;
      expRecursive.Image = fltRecursive->GetOutput();
      
      // Compute and print the statistics of the experiment
      verbose << "RG\t0.00\t";
      CompareAndPrint(expRecursive,imgTruth);
      verbose << endl;

      verbose << "------------------------------------" << endl << endl;
      }
  }


private:

  // A structure representing an experiment
  struct Experiment 
  {
    ImageType::Pointer Image;
    clock_t Time;

    float MeanSquareError, MaxError, Bias;
  };

  // Compare to perfect image
  void CompareAndPrint(Experiment &exp, ImageType *truth)
  {
    // Compute the mean square difference and max difference between images
    IteratorType itExp(exp.Image,exp.Image->GetLargestPossibleRegion());
    IteratorType itTruth(truth,truth->GetLargestPossibleRegion());

    exp.MeanSquareError = 0.0f;
    exp.MaxError = 0.0f;

    // The mean intensities
    float meanTruth = 0.0f, meanExp = 0.0f;
    
    // Loop over all pixels
    while(!itExp.IsAtEnd())
      {
      float vTruth = itTruth.Value();
      float vExp = itExp.Value();
      exp.MeanSquareError += (vTruth - vExp) * (vTruth - vExp);
      exp.MaxError = exp.MaxError > fabs(vTruth - vExp) ? 
        exp.MaxError : fabs(vTruth - vExp);
      
      meanTruth += vTruth;
      meanExp += vExp;

      // Update iterators
      ++itTruth;
      ++itExp;
      }

    // Divide by the number of pixels
    unsigned long size = truth->GetLargestPossibleRegion().GetNumberOfPixels();
    
    exp.MeanSquareError /= size;
    exp.Bias = (meanExp - meanTruth) / size;

    // Print out the experiment's results
    //verbose << "   Processing Time (ms) : " << exp.Time << endl;
    //verbose << "   Mean Squared Error   : " << exp.MeanSquareError << endl;
    //verbose << "   Max Absolute Error   : " << exp.MaxError << endl;
    //verbose << "   Bias of the Mean     : " << exp.Bias << endl;
    verbose 
      << "\t" << exp.Time
      << "\t" << exp.MeanSquareError
      << "\t" << exp.MaxError
      << "\t" << exp.Bias;
  }
};


void GUITestPadLogic::OnTestGaussianPerformance()
{
  GaussianPerformanceTester tester;
  // tester.RunTest("C:/grant/itk/Insight/Examples/Data/FatMRISlice.png");
  tester.RunTest("MRIcrop-orig.gipl");

  /*
  // Create a wrapper
  typedef GreyImageWrapperImplementation WrapperType;
  typedef GreyImageWrapper::ImageType ImageType;
  WrapperType *wrapper = new WrapperType;
  
  // Load wrapper from file
  LoadShortImage(wrapper);

  // Create discrete filter
  typedef itk::DiscreteGaussianImageFilter<ImageType,ImageType> DGFilterType;
  DGFilterType::Pointer fltDG = DGFilterType::New();
  fltDG->SetInput(wrapper->GetImage());
  
  // Create recursive filter
  typedef itk::RecursiveGaussianImageFilter<ImageType,ImageType> RGFilterType;
  RGFilterType::Pointer fltRG = RGFilterType::New();
  fltRG->SetInput(wrapper->GetImage());

  // Create a VTK filter
  vtkImageGaussianSmooth *fltVTK = vtkImageGaussianSmooth::New();
  fltVTK->SetInput(wrapper->GetVTKImage());

  // Create an FFT filter
  vtkImageConstantPad *fltPad = vtkImageConstantPad::New();
  fltPad->SetInput(wrapper->GetVTKImage());
  fltPad->SetOutputWholeExtent(0,255,0,255,0,255);
  fltPad->SetConstant(0);
  fltPad->Update();
  
  vtkImageFFT *fltFFT = vtkImageFFT::New();
  fltFFT->SetInput(fltPad->GetOutput());

  // Loop over different values of sigma
  for(float sigma=0.5;sigma < 5.0;sigma+=0.5)
    {
    fltDG->SetVariance(sigma*sigma);
    fltRG->SetSigma(sigma);
    fltVTK->SetStandardDeviation(sigma);

    fltDG->SetMaximumError(0.1);
    fltVTK->SetRadiusFactor(3.0f);

    // Report
    cout << "Standard Deviation: " << sigma << endl;
    
    clock_t t0 = clock();
    fltDG->Update();
    clock_t t1 = clock();
    fltRG->Update();
    clock_t t2 = clock();
    fltVTK->Update();
    clock_t t3 = clock();

    cout << "   ITK Discrete Gaussian   : " << (t1-t0) << "ms." << endl;
    cout << "   ITK Recursive Gaussian  : " << (t2-t1) << "ms." << endl;
    cout << "   VTK Gaussian            : " << (t3-t2) << "ms." << endl << endl;    
    }
    
    */
}
