/*=========================================================================

Program:   Insight Segmentation & Registration Toolkit
Module:    BiasFieldEstimator.cxx
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

#include "OptionList.h"
#include "itkMRIBiasFieldCorrectionFilter.h"
#include "metaITKUtils.h"
#include "imageutils.h"

typedef itk::MRIBiasFieldCorrectionFilter<ImageType, ImageType, MaskType> Corrector ;

void print_usage()
{
  print_line("MRIBiasCorrection 1.0 (21.June.2001)");

  print_line("usage: BiasFieldEstimator --input file" ) ;
  print_line("       --class-mean mean(1) ... mean(i)" ) ;
  print_line("       --class-sigma sigma(1) ... sigma(i)" ) ;
  print_line("       --use-log [yes|no]") ;
  print_line("       [--input-mask file]" ) ;
  print_line("       [--degree int] [--coefficients c0..cn]" ) ;
  print_line("       [--growth double] [--shrink double] ") ;
  print_line("       [--volume-max-iteration int]");
  print_line("       [--init-step-size double] ");
  print_line("       [--schedule s0...sn] ");

  print_line("");

  print_line("--input file") ;
  print_line("        input data set [meta image format]" );
  print_line("--class-mean mean(1),...,mean(i)" ) ;
  print_line("        intensity means  of the different i tissue classes") ;
  print_line("--class-sigma sig(1),...,sig(i)" ) ; 
  print_line("        intensity sigmas of the different i tissue clases") ;
  print_line("        NOTE: The sigmas should be listed in the SAME ORDER") ;
  print_line("              of means");
  print_line("        and each value should be SEPARATED BY A SPACE)") ;
  print_line("--input-mask file" ) ;
  print_line("        mask input with file [meta image format]");
  print_line("--degree int") ;
  print_line("        degree of legendre polynomial used for the") ;
  print_line("        approximation of the bias field" );
  print_line("--use-log [yes|no]") ;
  print_line("        if yes, assume a multiplicative bias field") ;
  print_line("        (use log of image, class-mean, class-sigma,") ;
  print_line("         and init-step-size)" );
  print_line("--grow double") ;
  print_line("        stepsize growth factor. must be greater than 1.0") ;
  print_line("        [default 1.05]" ) ;
  print_line("--shrink double") ;
  print_line("        stepsize shrink factor ") ;
  print_line("        [default grow^(-0.25)]" ) ; 
  print_line("--volume-max-iteration int") ;
  print_line("        maximal number of iterations for 3D volume correction") ;
  print_line("        [default 20]" ) ;
  print_line("--init-step-size double") ;
  print_line("        inital step size [default 1.02]" );
  print_line("--coefficients c(0) .. c(n)") ;
  print_line("        coefficients of the polynomial") ;
  print_line("        (used for generating bias field)") ;
  print_line("--schedule s(0) .. s(n)") ;
  print_line("        multires schedule (default 2 2 2 1 1 1)") ;
  print_line("        n has to be a multiple of 3") ;

  print_line("");

  print_line("example: BiasFieldEstimator --input sample.mhd") ;
  print_line("         --class-mean 1500 570") ;
  print_line("         --class-sigma 100 70 --use-log yes");
  print_line("         --input-mask sample.mask.mhd ") ;
  print_line("         --degree 3 --grow 1.05 --shrink 0.9");
  print_line("         --max-iteration 2000 --init-step-size 1.02") ;
  print_line("         --coefficients 0.056789 -1.00004 0.78945 ... -0.02345");
  print_line("         --schedule 4 4 2");
}


void printResult(Corrector* filter, OptionList& options)
{
  options.DumpOption("input") ;
  options.DumpOption("input-mask") ;
  options.DumpOption("class-mean") ;
  options.DumpOption("class-sigma") ;
  options.DumpOption("use-log") ;

  std::cout << " --degree " << filter->GetBiasFieldDegree() ;

  std::cout << " --grow " << filter->GetOptimizerGrowthFactor() ;
  std::cout << " --shrink " << filter->GetOptimizerShrinkFactor() ;
  std::cout << " --volume-max-iteration " << filter->GetVolumeCorrectionMaximumIteration();
  std::cout << " --inter-slice-max-iteration " << filter->GetInterSliceCorrectionMaximumIteration();
  
  std::cout << " --init-step-size " << filter->GetOptimizerInitialRadius() ;


  std::cout << " --coefficients " ;
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
}

int main(int argc, char* argv[])
{

  if (argc <= 1)
    {
      print_usage() ;
      exit(0) ;
    }

  OptionList options(argc, argv) ;

  Corrector::Pointer filter = Corrector::New() ;

  std::string inputFileName ;
  std::string inputMaskFileName = "" ;
  bool useLog = true;
  int degree = 3;
  std::vector<double> InclassMeans ;
  std::vector<double> InclassSigmas ;
  int volumeMaximumIteration = 20; 
  double initialRadius = 1.02;
  double grow  = 1.05;
  double shrink = pow(grow, -0.25);
  std::vector<double> coefficients ;
  std::vector<int> schedule ;

  try
    {
      // get image file options
      options.GetStringOption("input", &inputFileName, true) ;
      options.GetStringOption("input-mask", &inputMaskFileName, false) ;
      
      // get bias field options
      useLog = options.GetBooleanOption("use-log", true, false) ;
      degree = options.GetIntOption("degree", 3, false) ;
      
      options.GetMultiDoubleOption("coefficients", &coefficients, false) ;

      //get schedule
      options.GetMultiIntOption("schedule", &schedule, false) ;

      // get energyfunction options
      options.GetMultiDoubleOption("class-mean", &InclassMeans, true) ;
      options.GetMultiDoubleOption("class-sigma", &InclassSigmas, true) ;

      // get optimizer options
      volumeMaximumIteration = options.GetIntOption("volume-max-iteration", 20, false) ;
      grow = options.GetDoubleOption("grow", 1.05, false) ;
      shrink = pow(grow, -0.25) ;
      shrink = options.GetDoubleOption("shrink", shrink, false) ;
      initialRadius = options.GetDoubleOption("init-step-size", 1.02, false) ;
    }
  catch(OptionList::RequiredOptionMissing e)
    {
      std::cout << "Error: The '" << e.OptionTag 
                << "' option is required but missing." 
                << std::endl ;
    }

      
  // load images
  ImagePointer input ;
  MaskPointer inputMask ;

  ImageReaderType::Pointer imageReader = ImageReaderType::New() ;
  MaskReaderType::Pointer maskReader = MaskReaderType::New() ;
  try
    {
      std::cout << "Loading images..." << std::endl ;
      imageReader->SetFileName(inputFileName.c_str()) ;
      imageReader->Update() ;
      input = imageReader->GetOutput() ;
      ImageType::DirectionType ImageDirection=input->GetDirection();
      filter->SetInput(input) ;
      if (inputMaskFileName != "")
        {
        maskReader->SetFileName(inputMaskFileName.c_str()) ;
        maskReader->Update() ;
        inputMask = OrientImage<MaskType>(maskReader->GetOutput(),ImageDirection);
        if(input->GetDirection() != inputMask->GetDirection()
          || input->GetSpacing() != inputMask->GetSpacing()
          || input->GetLargestPossibleRegion().GetSize() != inputMask->GetLargestPossibleRegion().GetSize())
          {
          std::cout <<  "Input image and input mask must have same orientation, dimensions, and voxel spacings." << std::endl;
          exit(-1); 
          }
          filter->SetInputMask(inputMask) ;
        }
      std::cout << "Images loaded." << std::endl ;
    }
  catch (itk::ExceptionObject e)
    {
      e.Print(std::cout) ;
      exit(0) ;
    }
  
  if (!coefficients.empty())
    {
      std::cout << "Setting initial coeffs" << std::endl ;
      filter->SetInitialBiasFieldCoefficients(coefficients) ;
    }
  if (!schedule.empty())
    {
      if (schedule.size() % 3 == 0 )
   {
     int level = schedule.size() / 3;
     std::cout << "Setting multires schedule :" << level << std::endl ;
     filter->SetNumberOfLevels( level ) ;
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

  filter->IsBiasFieldMultiplicative(useLog) ;
  // sets tissue classes' statistics for creating the energy function
  filter->SetTissueClassStatistics(classMeans, classSigmas) ;
  // setting standard optimizer parameters 
  filter->SetOptimizerGrowthFactor(grow) ;
  filter->SetOptimizerShrinkFactor(shrink) ;
  filter->SetVolumeCorrectionMaximumIteration(volumeMaximumIteration) ;
  filter->SetOptimizerInitialRadius(initialRadius) ;
  filter->SetBiasFieldDegree(degree) ;
  // turn on inter-slice intensity correction 
  filter->SetUsingInterSliceIntensityCorrection(false) ;
  // disable slab identifcation
  // the filter will think the largest possible region as the only one
  // slab.
  filter->SetUsingSlabIdentification(false) ;
  filter->SetUsingBiasFieldCorrection(true) ;
  // disable output image generation
  filter->SetGeneratingOutput(false) ;
  filter->SetSlicingDirection(2) ;
  
  std::cout << "Estimating the bias field..." << std::endl ;
  filter->Update() ;

  printResult(filter.GetPointer(), options) ;

  return 0 ;
}
