/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    GaussianMinimumErrorClassifier.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "OptionList.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkScalarToArrayCastImageFilter.h"

#include "itkImageToListAdaptor.h"
#include "itkMembershipSampleGenerator.h"
#include "itkGaussianDensityFunction.h"
#include "itkMeanCalculator.h"
#include "itkCovarianceCalculator.h"
#include "itkMaximumRatioDecisionRule.h"
#include "itkSampleClassifierWithMask.h"

#include "OutputEvaluator.h"

#define MeasurementVectorSize 1
#define ImageDimension 3

void print_usage()
{
  std::cout << "GaussianClassifier 1.0 (17. Dec. 2001)" << std::endl ;

  std::cout << "usage: GaussianClassifier --trainings file..."  << std::endl ;
  std::cout << "       --targets file..."  << std::endl ;
  std::cout << "       --class-mask file" << std::endl ;
  std::cout << "       --selected-class-labels (unsigned char) ..." 
            << std::endl ;
  std::cout << "       --number-of-classes int" << std::endl ;
  std::cout << "       --other-class-label unsigned char" << std::endl ;
  std::cout << "       --output file"  << std::endl ;

  std::cout << "" << std::endl ;

  std::cout << "--trainings file..." << std::endl ;
  std::cout << "        image file name with intesnity values [meta image format]"  
    << std::endl ;
  std::cout << "--targets file..." << std::endl ;
  std::cout << "        target image file name with intensity values [meta image format]" 
    << std::endl ;
  std::cout << "--class-mask file" << std::endl ;
  std::cout << "        image file name with class labels [meta image format]"  
    << std::endl ;
  std::cout << "--number-of-classes int" << std::endl ;
  std::cout << "        the number of classes in the training image."
    << std::endl ;
  std::cout << "--output file" << std::endl ;
  std::cout << "        output image file name that will have the class labels for pixels" 
    << std::endl ;
  std::cout << "" << std::endl ;

  std::cout << "example: GaussianClassifier --trainings train.mhd train2.mhd" << std::endl ;
  std::cout << "         --targets target.mhd target2.mhd" << std::endl ;
  std::cout << "         --class-mask class_mask.mhd" << std::endl ;
  std::cout << "         --selected-class-labesl 1 2 3" << std::endl ;
  std::cout << "         --other-class-label 0" << std::endl ;
  std::cout << "         --number-of-classes 10" << std::endl ;
  std::cout << "         --output output.mhd" << std::endl ;
}


int main(int argc, char* argv[])
{

  namespace stat = itk::Statistics ;
 
  if (argc <= 1)
    {
      print_usage() ;
      exit(0) ;
    }

  OptionList options(argc, argv) ;
  
  typedef short PixelType ;
  typedef unsigned int ClassLabelType ;
  typedef itk::Image< PixelType, ImageDimension > ImageType ;
  typedef itk::Image< ClassLabelType, ImageDimension > ClassMaskImageType ;

  typedef std::vector< std::string > StringVectorType ;
  StringVectorType trainingFileNames ;
  StringVectorType targetFileNames ;
  std::vector< ClassLabelType > selectedClassLabels ;
  std::string classMaskFileName ;
  std::string outputFileName ;
  unsigned int numberOfClasses ;
  ClassLabelType otherClassLabel ;
  
  try
    {
      options.GetMultiStringOption("trainings", &trainingFileNames, true) ;
      options.GetMultiStringOption("targets", &targetFileNames, true) ;
      options.GetStringOption("class-mask", &classMaskFileName, true) ;
      options.GetStringOption("output", &outputFileName, true) ;
      numberOfClasses = options.GetIntOption("number-of-classes", 10, true) ;
      options.GetMultiUIntOption("selected-class-labels", 
                                &selectedClassLabels, false) ;
      otherClassLabel = options.GetUIntOption("other-class-label", 0, false) ;
    }
  catch(OptionList::RequiredOptionMissing e)
    {
      std::cout << "Error: The '" << e.OptionTag 
                << "' option is required but missing." 
                << std::endl ;
      return 1 ;
    }

  if ( trainingFileNames.size() != MeasurementVectorSize ||
       targetFileNames.size() != MeasurementVectorSize )
    {
      std::cout << "Error: The number of training or target image files" 
                << std::endl ;
      std::cout << " mismatches the dimension of measurement vector."
                << std::endl ;
      std::cout << "Dimension of measurement vector = " 
                << MeasurementVectorSize << std::endl ; 
      std::cout << "Number of training image files = " 
                << trainingFileNames.size() << std::endl ; 
      std::cout << "Number of target image files = " 
                << targetFileNames.size() << std::endl ; 
      exit(1) ;
    }

  typedef itk::Vector< PixelType, MeasurementVectorSize >
    MeasurementVectorType ;


  // ----
  std::cout << "DEBUG: Loading class mask image..." << std::endl ;

  typedef itk::ImageFileReader< ClassMaskImageType > ClassMaskImageReaderType ;
  typedef itk::Vector< ClassLabelType, 1 > ClassMaskMeasurementVectorType ;
  typedef itk::Image< ClassMaskMeasurementVectorType, 
    ImageDimension > ClassMaskVectorImageType ;
  typedef itk::ScalarToArrayCastImageFilter< ClassMaskImageType, 
    ClassMaskVectorImageType > ClassMaskVectorImageGeneratorType ;
  typedef itk::Statistics::ImageToListAdaptor< ClassMaskVectorImageType > 
    ClassMaskSampleType ;

  ClassMaskImageReaderType::Pointer classMaskImageReader = 
    ClassMaskImageReaderType::New() ;
  classMaskImageReader->SetFileName(classMaskFileName.c_str()) ;

  ClassMaskVectorImageGeneratorType::Pointer classMaskVectorImageGenerator = 
    ClassMaskVectorImageGeneratorType::New() ;
  classMaskVectorImageGenerator->SetInput(classMaskImageReader->GetOutput()) ;
  classMaskVectorImageGenerator->Update() ;

  ClassMaskSampleType::Pointer classMaskSample = ClassMaskSampleType::New() ;
  classMaskSample->SetImage(classMaskVectorImageGenerator->GetOutput()) ;

  // ----
  std::cout << "DEBUG: Loading the training images..." << std::endl ;

  typedef itk::ImageFileReader< ImageType > ImageReaderType ;
  typedef std::vector< ImageReaderType::Pointer > 
    ImageReaderVectorType ;
  typedef itk::Image< MeasurementVectorType, ImageDimension > 
    VectorImageType ;
  typedef itk::ScalarToArrayCastImageFilter< ImageType, VectorImageType >
    VectorImageGeneratorType ;
  typedef itk::Statistics::ImageToListAdaptor< VectorImageType > 
    ImageSampleType ;

  StringVectorType::const_iterator fn_iter = 
    trainingFileNames.begin() ;

  VectorImageGeneratorType::Pointer trainingVectorImageGenerator = 
    VectorImageGeneratorType::New() ;
  
  ImageReaderVectorType trainingImageReaders ;

  unsigned int index = 0 ;
  while ( fn_iter != trainingFileNames.end() )
    {
      trainingImageReaders.push_back(ImageReaderType::New()) ;
      trainingImageReaders[index]->
        SetFileName(trainingFileNames[index].c_str()) ;
      //      trainingImageReaders[index]->Update() ;
      trainingVectorImageGenerator->
        SetInput(index, trainingImageReaders[index]->GetOutput()) ;
      ++fn_iter ;
      ++index ;
    }
  trainingVectorImageGenerator->Update() ;

  ImageSampleType::Pointer trainingSample = ImageSampleType::New() ;
  trainingSample->SetImage(trainingVectorImageGenerator->GetOutput()) ;

  // ----
  std::cout << "DEBUG: Loading the target images..." << std::endl ;
  VectorImageGeneratorType::Pointer targetVectorImageGenerator = 
    VectorImageGeneratorType::New() ;
  
  ImageReaderVectorType targetImageReaders ;

  fn_iter = targetFileNames.begin() ;

  index = 0 ;
  while ( fn_iter != targetFileNames.end() )
    {
      targetImageReaders.push_back(ImageReaderType::New()) ;
      targetImageReaders[index]->
        SetFileName(targetFileNames[index].c_str()) ;
      //      targetImageReaders[index]->Update() ;
      targetVectorImageGenerator->
        SetInput(index, targetImageReaders[index]->GetOutput()) ;
      ++fn_iter ;
      ++index ;
    }
  targetVectorImageGenerator->Update() ;

  ImageSampleType::Pointer targetSample = ImageSampleType::New() ;
  targetSample->SetImage(targetVectorImageGenerator->GetOutput()) ;

  // ----
  std::cout << "Creating the membership sample for training.." << std::endl ;

  typedef itk::Statistics::MembershipSample< ImageSampleType > 
    MembershipSampleType ;
  typedef MembershipSampleType::ClassSampleType ClassSampleType ;

  typedef itk::Statistics::MembershipSampleGenerator< ImageSampleType, 
    ClassMaskSampleType > MembershipSampleGeneratorType ;  
  
  MembershipSampleGeneratorType::Pointer generator = 
    MembershipSampleGeneratorType::New() ;

  generator->SetInput(trainingSample.GetPointer()) ;
  generator->SetClassMask(classMaskSample.GetPointer()) ;
  generator->SetNumberOfClasses(numberOfClasses) ;
  generator->GenerateData() ;
 
  MembershipSampleType::ConstPointer membershipSample = generator->GetOutput() ;

  /* =================================================== */
  std::cout << "Inducing the gaussian density function parameters and"
            << " apriori probabilities..." << std::endl ;

  typedef stat::GaussianDensityFunction< MeasurementVectorType > 
    DensityFunctionType ;
  typedef stat::MeanCalculator< ClassSampleType > MeanCalculatorType ;
  typedef stat::CovarianceCalculator< ClassSampleType >
    CovarianceCalculatorType ;
  typedef itk::Statistics::SampleClassifierWithMask
    < ImageSampleType, ClassMaskSampleType > ClassifierType ;

  typedef itk::MaximumRatioDecisionRule DecisionRuleType ;

  std::vector< DensityFunctionType::Pointer > densityFunctions ;
  std::vector< MeanCalculatorType::Pointer > meanCalculatorVector ;
  std::vector< CovarianceCalculatorType::Pointer > covarianceCalculatorVector ;
  std::vector< unsigned int > sampleSizes ;

  DecisionRuleType::Pointer rule = DecisionRuleType::New() ;
  ClassifierType::Pointer classifier = ClassifierType::New() ;
  
  unsigned int numberOfSelectedClasses = selectedClassLabels.size() ;
  classifier->SetNumberOfClasses(numberOfSelectedClasses) ;
  classifier->SetSample(targetSample) ;
  classifier->SetDecisionRule((itk::DecisionRuleBase::Pointer)rule) ;

  densityFunctions.resize(numberOfSelectedClasses) ;
  meanCalculatorVector.resize(numberOfSelectedClasses) ;
  covarianceCalculatorVector.resize(numberOfSelectedClasses) ;
  sampleSizes.resize(numberOfSelectedClasses) ;
  DecisionRuleType::APrioriVectorType apriori ;
  apriori.resize(numberOfSelectedClasses) ;
  unsigned int totalSampleSize = 0 ;
  unsigned int classLabel ;
  for (unsigned int i = 0 ; i < numberOfSelectedClasses ; i++)
    {
      classLabel = selectedClassLabels[i] ;

      std::cout << "gaussian [" << classLabel << "]" << std::endl ;
      // add the class sample size to the decision rule 
      // for the a priori probability calculation
      sampleSizes[i] = 
        membershipSample->GetClassSampleSize(classLabel) ;
      std::cout << "  Sample size = " << sampleSizes[i] << std::endl ;

      meanCalculatorVector[i] = MeanCalculatorType::New() ;
      meanCalculatorVector[i]->
        SetInputSample(membershipSample->GetClassSample(classLabel)) ;
      meanCalculatorVector[i]->Update() ;
      
      covarianceCalculatorVector[i] = CovarianceCalculatorType::New() ;
      covarianceCalculatorVector[i]->
        SetInputSample(membershipSample->GetClassSample(classLabel)) ;
      covarianceCalculatorVector[i]->
        SetMean(meanCalculatorVector[i]->GetOutput()) ;
      covarianceCalculatorVector[i]->Update() ;
      
      densityFunctions[i] = DensityFunctionType::New() ;
      (densityFunctions[i])->SetMean(meanCalculatorVector[i]->GetOutput()) ;
      (densityFunctions[i])->
        SetCovariance(covarianceCalculatorVector[i]->GetOutput() ) ;
      std::cout << "  mean = " << *((densityFunctions[i])->GetMean())
                << std::endl ;
      std::cout << "  covariance = " << std::endl ;
      (densityFunctions[i])->GetCovariance()->GetVnlMatrix().print(std::cout) ;

      classifier->AddMembershipFunction(densityFunctions[i]) ;
      apriori[i] = sampleSizes[i] ;
      totalSampleSize += sampleSizes[i] ;
    }
  
  for ( unsigned int i = 0 ; i < apriori.size() ; i++ )
    {
      apriori[i] = apriori[i] / totalSampleSize ;
    }

  rule->SetAPriori(apriori) ;

  /* =================================================== */
  std::cout << "Classifying..." << std::endl ;

  classifier->SetMask(classMaskSample) ;
  classifier->SetMembershipFunctionClassLabels(selectedClassLabels) ;
  classifier->SetSelectedClassLabels(selectedClassLabels) ;
  classifier->SetNumberOfClasses(selectedClassLabels.size()) ;
  classifier->SetOtherClassLabel(otherClassLabel) ;
  classifier->Update() ;
  
  /* ===================================================== */
  std::cout << "Creating a image with result class labels..." << std::endl ;

  ClassifierType::OutputType* result = classifier->GetOutput() ;

  typedef itk::ImageRegionIterator< ClassMaskImageType > ImageIteratorType ;

  ClassMaskImageType::Pointer output = ClassMaskImageType::New() ;
  output->SetBufferedRegion(classMaskImageReader->GetOutput()->
                            GetLargestPossibleRegion()) ;
  output->SetLargestPossibleRegion(classMaskImageReader->GetOutput()->
                                   GetLargestPossibleRegion()) ;
  output->Allocate() ;

  ImageIteratorType i_iter(output, output->GetLargestPossibleRegion()) ;
  i_iter.GoToBegin() ;
  ClassifierType::OutputType::Iterator m_iter = result->Begin() ;

  while (!i_iter.IsAtEnd())
    {
      i_iter.Set((ImageType::PixelType)m_iter.GetClassLabel()) ;
      ++i_iter ;
      ++m_iter ;
    }

  typedef itk::ImageFileWriter< ClassMaskImageType > WriterType ;
  
  WriterType::Pointer writer = WriterType::New() ;
  
  writer->SetInput(output) ;
  writer->SetFileName(outputFileName.c_str()) ;
  writer->Update() ;

  std::cout << "DEBUG: Creating classification matrix ..." << std::endl ;
  OutputEvaluator< ClassMaskImageType > evaluator ;
  evaluator.SetTruth(classMaskImageReader->GetOutput()) ;
  evaluator.SetTarget(output) ;
  evaluator.SetNumberOfClasses(numberOfClasses) ;
  evaluator.GenerateData() ;
  evaluator.Print() ;

  return 0 ;
}

