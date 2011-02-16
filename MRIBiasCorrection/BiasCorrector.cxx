/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    BiasCorrector.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) 2002 Insight Consortium. All rights reserved.
See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <string>
#include <vector>
#include <vnl/vnl_math.h>

#include <iostream>
#include <fstream>
#include "OptionList.h"
#include <itkMRIBiasFieldCorrectionFilter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkImageRegionIterator.h>
#include <itkZeroCrossingBasedEdgeDetectionImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkBinaryCrossStructuringElement.h>
#include <itkBinaryDilateImageFilter.h>
#include <itkMaskImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkIndent.h>
#include <itkHistogram.h>
#include "imageutils.h"

typedef itk::MRIBiasFieldCorrectionFilter<ImageType, ImageType, MaskType> Corrector ;

void print_usage()
{
  print_line("MRIBiasCorrection 1.0 (21.June.2001)");

  print_line("usage: BiasCorrector --input file" ) ;
  print_line("       --output file") ;
  print_line("       --class-mean mean(1) ... mean(i)" ) ;
  print_line("       --class-sigma sigma(1) ... sigma(i)" ) ;
  print_line("       [--use-log [yes|no]]") ;
  print_line("       [--input-mask file]" ) ;
  print_line("       [--output-mask file]" ) ;
  print_line("       [--degree int] ") ;
  print_line("       [--growth double] [--shrink double] ") ;
  print_line("       [--volume-max-iteration int] [--inter-slice-max-iteration int]");
  print_line("       [--init-step-size double] ");
  print_line("       [--use-slab-identification [yes|no]]") ;
  print_line("       [--use-interslice-correction [yes|no]]") ;
  print_line("       [--slice-direction [0-2]]" ) ;
  print_line("       [--schedule s0...sm] [--coefficients c0..cn]");
  print_line("       [--use-auto-outputmask [yes|no]]") ;
  print_line("       [--use-auto-inputmask [yes|no]] ") ;
  print_line("       [--outputmaskPerc [double] ") ;
  print_line("       [--write-mask [yes|no]] ") ;

  print_line("");

  print_line("--input file") ;
  print_line("        input image file name [meta image format]" );
  print_line("--output file") ;
  print_line("        output image file name [meta image format]" );
  print_line("--class-mean mean(1),...,mean(i)" ) ;
  print_line("        intensity means  of the different i tissue classes") ;
  print_line("--class-sigma sig(1),...,sig(i)" ) ;
  print_line("        intensity sigmas of the different i tissue clases") ;
  print_line("        NOTE: The sigmas should be listed in the SAME ORDER") ;
  print_line("              of means");
  print_line("        and each value should be SEPARATED BY A SPACE)") ;
  print_line("--input-mask file" ) ;
  print_line("        mask input with file [meta image format]");
  print_line("--output-mask file" ) ;
  print_line("        mask output with file [meta image format]");
  print_line("--degree int") ;
  print_line("        degree of legendre polynomial used for the") ;
  print_line("        approximation of the bias field" );
  print_line("--use-log [yes|no]") ;
  print_line("        if yes, assume a multiplicative bias field") ;
  print_line("        (use log of image, class-mean, class-sigma,") ;
  print_line("         and init-step-size) [default yes]" );
  print_line("--growth double") ;
  print_line("        stepsize growth factor. must be greater than 1.0") ;
  print_line("        [default 1.05]" ) ;
  print_line("--shrink double") ;
  print_line("        stepsize shrink factor ") ;
  print_line("        [default growth^(-0.25)]" ) ;
  print_line("--volume-max-iteration int") ;
  print_line("        maximal number of iterations for 3D volume correction") ;
  print_line("        [default 20]" ) ;
  print_line("--inter-slicemax-iteration int") ;
  print_line("        maximal number of iterations for each inter-slice correction") ;
  print_line("        [default 20]" ) ;
  print_line("--init-step-size double") ;
  print_line("        inital step size [default 1.02]" );
  print_line("--use-slab-identification [yes|no]") ;
  print_line("       if yes, the bias correction will first identify slabs,") ;
  print_line("       and then apply the bias correction to each slab [default  no]") ;
  print_line("--use-interslice-correction [yes|no]") ;
  print_line("       if yes, the bias correction will first do an interslice intensity correction,") ;
  print_line("       and then apply the bias correction to the full volume [default  no]") ;
  print_line("--slice-direction [0-2]" ) ;
  print_line("        slice creation direction ( 0 - x axis, 1 - y axis") ;
  print_line("        2 - z axis) [default 2]") ;
  print_line("--schedule s(0) .. s(m)") ;
  print_line("        multires schedule [default 2 2 2 1 1 1]") ;
  print_line("        m has to be a multiple of 3") ;
  print_line("--coefficients c(0) .. c(n)") ;
  print_line("        coefficients of the polynomial [default all 0]") ;
  print_line("--use-auto-outputmask [yes|no]]") ;
  print_line("       if yes, the outputmask will be automatically copmuted using the 1% quantile");
  print_line("       threshold [default  yes if output mask is not supplied]") ;
  print_line("--outputmaskPerc [double] ") ;
  print_line("       supply optionally the historam quantile (default 10) for the binary thresholding ") ;
  print_line("       that determines the outputmask") ;
  print_line("--use-auto-inputmask [yes|no]]") ;
  print_line("       if yes, the outputmask will be automatically computed using the 1% quantile");
  print_line("       threshold, plus all partial volume voxels are masked out using an edge filter");
  print_line("       [default  yes if input mask is not supplied]") ;
  print_line("--write-mask [yes|no]] ") ;
  print_line("       if yes, both output and input mask will be written out if they get computed by the program") ;

  print_line("");

  print_line("example: BiasCorrector --input sample.mhd") ;
  print_line("         --output sample.corrected.mhd") ;
  print_line("         --class-mean 1500 570") ;
  print_line("         --class-sigma 100 70 --use-log yes");
  print_line("         --input-mask sample.mask.mhd ") ;
  print_line("         --output-mask sample.mask2.mhd ") ;
  print_line("         --degree 3 --growth 1.05 --shrink 0.9");
  print_line("         --volume-max-iteration 2000 --init-step-size 1.1") ;
  print_line("         --slice-direction 2") ;
  print_line("         --coefficients 0.056789 -1.00004 0.78945 ... -0.02345");
  print_line("         --schedule 4 4 2");
}

static double compute_PercVal(ImagePointer& input, double quantile)
{
  typedef itk::MinimumMaximumImageCalculator< ImageType > minMaxCalcType;
  typedef itk::ImageRegionIterator< ImageType > Iterator;

  minMaxCalcType::Pointer minMaxCalc = minMaxCalcType::New();
  minMaxCalc->SetImage(input);
  minMaxCalc->Compute();
  const float maxval = minMaxCalc->GetMaximum();
  const float minval = minMaxCalc->GetMinimum();
  const int numBins =  (int) (maxval - minval + 1);

  typedef itk::Statistics::Histogram<double > HistogramType;

  // Histogram computation
  HistogramType::SizeType size;
  HistogramType::MeasurementVectorType minValVector, maxValVector;
  size.SetSize(1);
  minValVector.SetSize(1);
  maxValVector.SetSize(1);

  size[0] = numBins;
  minValVector[0] = minval;
  maxValVector[0] = maxval + 1;

  //std::cout << "Histo values " << minval <<" ...  " << maxval << " -> " << numBins << std::endl ;
  // put each image pixel into the histogram
  HistogramType::Pointer histogram = HistogramType::New();
  HistogramType::MeasurementVectorType measurement;
  histogram->SetMeasurementVectorSize( 1 );
  measurement.SetSize(1);

  histogram->Initialize( size, minValVector, maxValVector );
  Iterator iter (input, input->GetBufferedRegion());
  while ( !iter.IsAtEnd() )
    {
      const float value = iter.Get();
      measurement[0] = value;
      histogram->IncreaseFrequencyOfMeasurement( measurement , 1.0 );

      ++iter;
    }

  // Compute 10% level
  ImageType::RegionType imageRegion = input->GetBufferedRegion();
  const int numVoxels = imageRegion.GetSize(0) * imageRegion.GetSize(1) * imageRegion.GetSize(2);
  const double PercVoxval = (double) numVoxels / (100.0000001 - quantile);
  double curVoxval = 0;
  double PercIntval = 0;

  HistogramType::ConstIterator histoIter(histogram);
  HistogramType::IndexType index;
  HistogramType::InstanceIdentifier instance;
  bool exitLoop = false;

  for (histoIter = histogram->Begin() ; (histoIter != histogram->End() && ! exitLoop ) ; ++histoIter)
    {
      curVoxval  = curVoxval + histoIter.GetFrequency();
      if (curVoxval >= PercVoxval)
   {
     instance =  histoIter.GetInstanceIdentifier();
     index =       histogram->GetIndex(instance);
     maxValVector = histogram->GetHistogramMaxFromIndex(index);
     PercIntval = maxValVector[0];
     exitLoop = true;
   }
    }

  return PercIntval;
}

static void remove_edges(ImagePointer input, ImagePointer& NoEdgeImage)
{

  const int Dimension = 3;
  typedef itk::ZeroCrossingBasedEdgeDetectionImageFilter<ImageType, ImageType> zeroCrossFilterType;
  typedef itk::CastImageFilter< ImageType,  MaskType> castFilterType;
  typedef itk::MaskImageFilter< ImageType,MaskType,ImageType > maskFilterType;
  //typedef itk::BinaryBallStructuringElement<MaskPixelType,Dimension> StructuringElementType;
  typedef itk::BinaryCrossStructuringElement<MaskPixelType,Dimension> StructuringElementType;
  typedef itk::BinaryDilateImageFilter<MaskType, MaskType, StructuringElementType> dilateFilterType;
  typedef itk::BinaryThresholdImageFilter< MaskType,  MaskType> BinaryThresholdType;

  double variance[3];
  variance[0] = 2.5;
  variance[1] = 2.5;
  variance[2] = 2.5;

  //std::cout << " edge detection" << std::endl;
  zeroCrossFilterType::Pointer edgeFilter = zeroCrossFilterType::New();
  edgeFilter->SetInput(input);
  edgeFilter->SetVariance(variance);
  edgeFilter->Update();

  castFilterType::Pointer convFilter = castFilterType::New();
  convFilter->SetInput(edgeFilter->GetOutput());
  convFilter->Update();

  StructuringElementType structuringElement;
  int ballSize = 1;
  structuringElement.SetRadius( ballSize );
  structuringElement.CreateStructuringElement( );

  //   //std::cout << "dilate  edge image" << std::endl;
  dilateFilterType::Pointer dilateFilter = dilateFilterType::New();
  dilateFilter->SetInput(convFilter->GetOutput());
  dilateFilter->SetDilateValue (1);
  dilateFilter->SetKernel( structuringElement );
  dilateFilter->Update();

  const int MaskValue = 1;
  const int NoMaskValue = 0;

  //std::cout << "invert edge image" << std::endl;
  // Not Operation on Image -> select places where there is NO Edge
  BinaryThresholdType::Pointer NotFilter = BinaryThresholdType::New();
  //NotFilter->SetInput(convFilter->GetOutput());
  NotFilter->SetInput(dilateFilter->GetOutput());
  NotFilter->SetOutsideValue(MaskValue);
  NotFilter->SetInsideValue(NoMaskValue);
  NotFilter->SetUpperThreshold(255);
  NotFilter->SetLowerThreshold(1);
  NotFilter->Update();


  //std::cout << "mask with inverted edge image" << std::endl;
  /** masking of the input image with the inverted&dilated edge image */
  maskFilterType::Pointer maskFilter = maskFilterType::New();
  maskFilter->SetInput1(input);
  maskFilter->SetInput2(NotFilter->GetOutput());
  maskFilter->Update();

  NoEdgeImage = maskFilter->GetOutput();
}

static int ImageMaskDisagreement(ImagePointer imgPtr, MaskPointer maskPtr)
{
  if(imgPtr->GetDirection() != maskPtr->GetDirection())
    {
    std::cout <<  "image and mask must have same GetDirection() matrix values";
    return 1;
    }
  const int nDims = imgPtr->GetSpacing().Size();
  for (int i=0; i<nDims; i++) {
    float err = 0.0;
    float ppn = 0.0;
    if (imgPtr->GetSpacing()[i] < maskPtr->GetSpacing()[i])
    {
      err += maskPtr->GetSpacing()[i] - imgPtr->GetSpacing()[i];
      ppn += imgPtr->GetSpacing()[i];
    }
    if (imgPtr->GetSpacing()[i] > maskPtr->GetSpacing()[i])
    {
      err += imgPtr->GetSpacing()[i] - maskPtr->GetSpacing()[i];
      ppn += maskPtr->GetSpacing()[i];
    }
    if (ppn != 0.0)
    {
      if (err/ppn > 0.01)
      {
      std::cout <<  "image and mask must have same voxel spacings";
      return 1;
      }
    }
  }
  if(imgPtr->GetLargestPossibleRegion().GetSize() != maskPtr->GetLargestPossibleRegion().GetSize())
    {
    std::cout <<  "image and mask must have same GetLargestPossibleRegion().GetSize() vector values";
    return 1;
    }
  return 0;
}


int main(int argc, char* argv[])
{
  OptionList options(argc, argv) ;
  if (argc <= 1 || options.GetBooleanOption("help", false, false) || options.GetBooleanOption("usage", false, false))
    {
      print_usage() ;
      exit(0) ;
    }

  // load images
  ImagePointer input ;
  MaskPointer inputMask ;
  MaskPointer outputMask ;
  ImageReaderType::Pointer imageReader = ImageReaderType::New() ;
  MaskReaderType::Pointer maskReader = MaskReaderType::New() ;
  MaskReaderType::Pointer maskReader2 = MaskReaderType::New() ;

  typedef itk::BinaryThresholdImageFilter< ImageType,  MaskType> threshFilterType;
  typedef itk::CastImageFilter< ImageType,  WriteImageType> castFilterType;
  static const int TRESH_VAL = 1;
  static const int BG_VAL = 0;

  std::string inputFileName ;
  std::string outputFileName ;
  std::string inputMaskFileName = "" ;
  std::string outputMaskFileName = "" ;
  bool useLog = true;
  int degree = 3;
  int sliceDirection = 2;
  std::vector<double> InclassMeans ;
  std::vector<double> InclassSigmas ;
  int volumeMaximumIteration = 200;
  int interSliceMaximumIteration = 200;
  double initialRadius = 1.01;
  double growth = 1.05;
  double shrink = pow(growth, -0.25);
  double outputmaskPerc = 10;

  bool usingSlabIdentification = false;
  bool useIntersliceCorrection = false;
  bool useAutoOutputmask = true ;
  bool useAutoInputmask = true ;
  bool writeMask = true;
  std::vector<int> schedule ;
  std::vector<double> initCoefficients ;

  try
    {
      // get image file options
      options.GetStringOption("input", &inputFileName, true) ;
      options.GetStringOption("output", &outputFileName, true) ;
      options.GetStringOption("input-mask", &inputMaskFileName, false) ;
      options.GetStringOption("output-mask", &outputMaskFileName, false) ;

      useAutoOutputmask = options.GetBooleanOption("use-auto-outputmask", true, false) ;
      useAutoInputmask = options.GetBooleanOption("use-auto-inputmask", true, false) ;
      outputmaskPerc = options.GetDoubleOption("outputmaskPerc", outputmaskPerc, false) ;

      // get bias field options
      useLog = options.GetBooleanOption("use-log", true, false) ;
      degree = options.GetIntOption("degree", 3, false) ;
      sliceDirection = options.GetIntOption("slice-direction", 2, false) ;
      writeMask = options.GetBooleanOption("write-mask",false,false);

      options.GetMultiIntOption("schedule", &schedule, false) ;
      options.GetMultiDoubleOption("coefficients", &initCoefficients, false) ;

      // get energyfunction options
      options.GetMultiDoubleOption("class-mean", &InclassMeans, true) ;
      options.GetMultiDoubleOption("class-sigma", &InclassSigmas, true) ;

      // get optimizer options
      volumeMaximumIteration = options.GetIntOption("volume-max-iteration", 200, false) ;
      interSliceMaximumIteration = options.GetIntOption("inter-slice-max-iteration", 200, false) ;
      growth = options.GetDoubleOption("growth", 1.02, false) ;
      shrink = pow(growth, -0.25) ;
      shrink = options.GetDoubleOption("shrink", shrink, false) ;
      initialRadius = options.GetDoubleOption("init-step-size", 1.01, false) ;

      // get the filter operation option
      usingSlabIdentification =
        options.GetBooleanOption("use-slab-identification", false, false) ;
      useIntersliceCorrection =
        options.GetBooleanOption("use-interslice-correction", false, false) ;
    }
  catch(OptionList::RequiredOptionMissing e)
    {
      std::cout << "Error: The '" << e.OptionTag
                << "' option is required but missing."
                << std::endl ;
    }

  Corrector::Pointer filter = Corrector::New() ;

  try
    {
      std::cout << "Loading images..." << std::endl ;
      imageReader->SetFileName(inputFileName.c_str()) ;
      imageReader->Update() ;
      input = imageReader->GetOutput() ;
      ImageType::DirectionType ImageDirection=input->GetDirection();
      filter->SetInput(input) ;
      std::cout << "Input image loaded." << std::endl ;
      if (inputMaskFileName != "")
        {
        maskReader->SetFileName(inputMaskFileName.c_str()) ;
        maskReader->Update() ;
        inputMask = OrientImage<MaskType>(maskReader->GetOutput(),ImageDirection);
        if(ImageMaskDisagreement(input, inputMask))
          {
          std::cout <<  ", comparing input to inputMask." << std::endl;
          exit(-1);
          }
        filter->SetInputMask(inputMask) ;
        std::cout << "Input mask image loaded." << std::endl ;
        }
      if (outputMaskFileName != "")
        {
        maskReader2->SetFileName(outputMaskFileName.c_str()) ;
        maskReader2->Update() ;
        outputMask = OrientImage<MaskType>(maskReader2->GetOutput(),ImageDirection);
        if(ImageMaskDisagreement(input, outputMask))
          {
          std::cout <<  ", comparing input to outputMask." << std::endl;
          exit(-1);
          }
        filter->SetOutputMask(outputMask) ;
        std::cout << "Output mask image loaded." << std::endl ;
        }
      std::cout << "Images loaded." << std::endl ;
    }
  catch (itk::ExceptionObject e)
    {
      e.Print(std::cout) ;
      exit(0) ;
    }

  if (!initCoefficients.empty())
    {
      std::cout << "Setting initial coeffs" << std::endl ;
      filter->SetInitialBiasFieldCoefficients(initCoefficients) ;
    }
  if (!schedule.empty())
    {
      if (schedule.size() % 3 == 0 )
   {
     int level = schedule.size() / 3;
     filter->SetNumberOfLevels( level ) ;
     std::cout << "Setting multires schedule: " << level << std::endl ;
     Corrector::ScheduleType CorrSchedule ( level, 3) ;
     for( int lev = 0; lev < level; lev++ ) // Copy schedule
       {
         for( int dim = 0; dim < 3; dim++ )
      {
        CorrSchedule[lev][dim] = schedule[lev * 3 + dim];
      }
       }

     filter->SetSchedule(CorrSchedule) ;
   }
      else
   {
     std::cout << "Schedule: number of elements not divisible by 3, using default" << std::endl ;
   }
    }

  //Copy class model
  itk::Array<double> classMeans ;
  itk::Array<double> classSigmas ;
  classMeans.SetSize(InclassMeans.size());
  classSigmas.SetSize(InclassSigmas.size());
  for (unsigned int i = 0; i < InclassMeans.size(); i++)
    {
    classMeans.SetElement(i, InclassMeans[i]);
    }
  for (unsigned int i = 0; i < InclassSigmas.size(); i++)
    {
    classSigmas.SetElement(i, InclassSigmas[i]);
    }

  try
    {
      if (outputMaskFileName == "" && useAutoOutputmask )
   {
     // generate output mask using 10% treshold
     double percVal = compute_PercVal(input, outputmaskPerc);
     std::cout << "Computing OutputMask: " <<  outputmaskPerc << " percent histo is at : " << percVal << std::endl;

     // Threshold the image
     threshFilterType::Pointer threshFilter = threshFilterType::New();
     threshFilter->SetInput(input);
     threshFilter->SetUpperThreshold( 32000);
     threshFilter->SetLowerThreshold( percVal + 1);
     threshFilter->SetOutsideValue( BG_VAL );
     threshFilter->SetInsideValue( TRESH_VAL );
     threshFilter->Update();

     outputMask = threshFilter->GetOutput() ;
     filter->SetOutputMask(outputMask) ;
     // DEBUG
     if (writeMask) {
       MaskWriterType::Pointer writer = MaskWriterType::New() ;
       writer->SetInput(outputMask) ;
       writer->SetFileName("outmask.gipl") ;
       writer->Write() ;
     }
   }

      if (inputMaskFileName == "" && useAutoInputmask )
   {
     // setting inputmask
     double percVal = compute_PercVal(input, outputmaskPerc);
     std::cout << "Computing InputMask:" <<  outputmaskPerc << "  percent histo is at : " << percVal << std::endl;

     ImagePointer NoEdgeImage ;
     // remove partial volume voxel using an edge map
     remove_edges(input, NoEdgeImage);

     // Threshold the NoEdgeImage
     threshFilterType::Pointer threshFilter = threshFilterType::New();
     threshFilter->SetInput(NoEdgeImage);
     threshFilter->SetUpperThreshold( 32000 );
     threshFilter->SetLowerThreshold( percVal + 1 );
     threshFilter->SetOutsideValue( BG_VAL );
     threshFilter->SetInsideValue( TRESH_VAL );
     threshFilter->Update();

     inputMask = threshFilter->GetOutput() ;
     filter->SetInputMask(inputMask) ;
     // DEBUG
     if (writeMask) {
       MaskWriterType::Pointer writer = MaskWriterType::New() ;
       writer->SetInput(inputMask) ;
       writer->SetFileName("inmask.gipl") ;
       writer->Write() ;
     }
   }
    }
  catch (itk::ExceptionObject e)
    {
      e.Print(std::cout) ;
      exit(0) ;
    }

  //filter->DebugOn() ;
  try
    {
      filter->IsBiasFieldMultiplicative(useLog) ;
      filter->SetTissueClassStatistics(classMeans, classSigmas) ;
      filter->SetOptimizerGrowthFactor(growth) ;
      filter->SetOptimizerShrinkFactor(shrink) ;
      filter->SetVolumeCorrectionMaximumIteration(volumeMaximumIteration) ;
      filter->SetInterSliceCorrectionMaximumIteration(interSliceMaximumIteration) ;
      filter->SetOptimizerInitialRadius(initialRadius) ;
      filter->SetBiasFieldDegree(degree) ;
      filter->SetUsingSlabIdentification(usingSlabIdentification) ;
      filter->SetUsingInterSliceIntensityCorrection(useIntersliceCorrection) ;
      filter->SetSlicingDirection(sliceDirection) ;
      filter->SetUsingBiasFieldCorrection(true) ;
      filter->SetGeneratingOutput(true) ;
      std::cout << "Starting the bias correction" << std::endl ;
      filter->Update() ;
    }
  catch (itk::ExceptionObject e)
    {
      e.Print(std::cout) ;
      exit(0) ;
    }

  try
    {
      // writes the corrected image
      std::cout << "Writing corrected image..." << std::endl ;

      castFilterType::Pointer convFilter = castFilterType::New();
      convFilter->SetInput(filter->GetOutput());
      convFilter->Update();

      ImageWriterType::Pointer writer = ImageWriterType::New() ;
      writer->SetInput(convFilter->GetOutput()) ;
      writer->SetFileName(outputFileName.c_str()) ;
      writer->Write() ;
    }
  catch (itk::ExceptionObject e)
    {
      e.Print(std::cout) ;
    }

  std::cout << " coefficients :" ;
  Corrector::BiasFieldType::CoefficientArrayType coefficients =
    filter->GetEstimatedBiasFieldCoefficients() ;

  Corrector::BiasFieldType::CoefficientArrayType::iterator iter =
    coefficients.begin() ;

  while (iter != coefficients.end())
    {
      std::cout << *iter << " " ;
      iter++ ;
    }
  std::cout << std::endl ;

  // write coeffs into a separate file
  std::string outFileNameCoef = outputFileName + std::string(".bcoef");

  std::ofstream efile(outFileNameCoef.c_str(), std::ios::out);
  if (!efile) {
    std::cerr << "Error: open(w) of file \"" << outFileNameCoef << "\" failed." << std::endl;
    exit(-1);
  }
  efile.precision(16);
  iter = coefficients.begin() ;
  while (iter != coefficients.end())
    {
      efile << *iter << " " ;
      iter++ ;
    }
  efile.close();

  return 0 ;
}
