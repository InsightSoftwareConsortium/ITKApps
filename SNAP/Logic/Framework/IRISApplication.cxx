/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IRISApplication.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/

/** Borland compiler is very lazy so we need to instantiate the template
 *  by hand */
#include <itkImage.h>
#include <itkImageRegionConstIterator.h>
#include <itkFixedArray.h>
typedef itk::Image<unsigned char,3> IRISApplicationBorlandDummyImageType;
typedef itk::ImageRegionConstIterator<IRISApplicationBorlandDummyImageType> IRISApplicationBorlandDummyConstIteratorType;
typedef itk::Image<short,3> IRISApplicationBorlandDummyImageType2;
typedef itk::Image<float,3> IRISApplicationBorlandDummyImageType3;
typedef itk::ImageRegionConstIterator<IRISApplicationBorlandDummyImageType3> IRISApplicationBorlandDummyConstIteratorType2;
typedef itk::Image<unsigned short,3> IRISApplicationBorlandDummyImageType4;
typedef itk::Image<double,3> IRISApplicationBorlandDummyImageType5;
typedef itk::ImageRegionConstIterator<IRISApplicationBorlandDummyImageType5> IRISApplicationBorlandDummyConstIteratorType3;
typedef itk::Image<itk::FixedArray<float,3>,3> IRISApplicationBorlandDummyImageType6;

#include "IRISApplication.h"

#include "GlobalState.h"
#include "IRISImageData.h"
#include "IRISVectorTypesToITKConversion.h"
#include "SNAPImageData.h"
#include "IntensityCurveVTK.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkPasteImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkIdentityTransform.h"
#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkWindowedSincInterpolateImageFunction.h"

#include <cstdio>
#include <sstream>

using namespace itk;
using namespace std;

IRISApplication
::IRISApplication() 
{
  // Construct new global state object
  m_GlobalState = new GlobalState;

  // Contruct the IRIS and SNAP data objects
  m_IRISImageData = new IRISImageData();
  m_SNAPImageData = NULL;

  // Set the current IRIS pointer
  m_CurrentImageData = m_IRISImageData;

  // Construct a new intensity curve
  m_IntensityCurve = IntensityCurveVTK::New();
  m_IntensityCurve->Initialize(4);

  // Initialize the image-anatomy transformation with RAI code
  m_ImageToAnatomyRAI = "RAI";

  // Initialize the display-anatomy transformation with RPI code
  m_DisplayToAnatomyRAI[0] = "RPS";
  m_DisplayToAnatomyRAI[1] = "AIL";
  m_DisplayToAnatomyRAI[2] = "RIP";
}


const char *
IRISApplication::
GetImageToAnatomyRAI()
{
  return m_ImageToAnatomyRAI.c_str();
}


const char *
IRISApplication::
GetDisplayToAnatomyRAI(unsigned int slice)
{
  return m_DisplayToAnatomyRAI[slice].c_str();
}


IRISApplication
::~IRISApplication() 
{
  m_IntensityCurve = NULL;

  delete m_IRISImageData;
  delete m_SNAPImageData;
  delete m_GlobalState;
}

void 
IRISApplication
::InitializeSNAPImageData(const SNAPSegmentationROISettings &roi,
                          CommandType *progressCommand) 
{
  assert(m_SNAPImageData == NULL);

  // Create the SNAP image data object
  m_SNAPImageData = new SNAPImageData();

  // Get the roi chunk from the grey image
  GreyImageType::Pointer imgNewGrey = 
    m_IRISImageData->GetGrey()->DeepCopyRegion(roi,progressCommand);

  // Get the size of the region
  Vector3ui size = to_unsigned_int(
    Vector3ul(imgNewGrey->GetLargestPossibleRegion().GetSize().GetSize()));

  // Compute an image coordinate geometry for the region of interest
  ImageCoordinateGeometry icg(m_ImageToAnatomyRAI,m_DisplayToAnatomyRAI,size);

  // Assign the new wrapper to the target
  m_SNAPImageData->SetGreyImage(imgNewGrey,icg);
  
  // Override the interpolator in ROI for label interpolation, or we will get
  // nonsense
  SNAPSegmentationROISettings roiLabel = roi;
  roiLabel.SetInterpolationMethod(SNAPSegmentationROISettings::NEAREST_NEIGHBOR);

  // Get chunk of the label image
  LabelImageType::Pointer imgNewLabel = 
    m_IRISImageData->GetSegmentation()->DeepCopyRegion(roiLabel,progressCommand);

  // Filter the segmentation image to only allow voxels of 0 intensity and 
  // of the current drawing color
  LabelType passThroughLabel = m_GlobalState->GetDrawingColorLabel();

  typedef ImageRegionIterator<LabelImageType> IteratorType;  
  IteratorType itLabel(imgNewLabel,imgNewLabel->GetBufferedRegion());  
  while(!itLabel.IsAtEnd())
    {
    if(itLabel.Value() != passThroughLabel)
      itLabel.Value() = (LabelType) 0;
    ++itLabel;
    }

  // Pass the cleaned up segmentation image to SNAP
  m_SNAPImageData->SetSegmentationImage(imgNewLabel);

  // Pass the label description of the drawing label to the SNAP image data
  m_SNAPImageData->SetColorLabel(
    passThroughLabel,
    m_IRISImageData->GetColorLabel(passThroughLabel));

  // Assign the intensity mapping function to the Snap data
  m_SNAPImageData->GetGrey()->SetIntensityMapFunction(m_IntensityCurve);

  // Initialize the speed image of the SNAP image data
  m_SNAPImageData->InitializeSpeed();

  // Remember the ROI object
  m_GlobalState->SetSegmentationROISettings(roi);
}

void 
IRISApplication
::SetDisplayToAnatomyRAI(const char *rai0,const char *rai1,const char *rai2)
{
  // Store the new RAI code
  m_DisplayToAnatomyRAI[0] = rai0;
  m_DisplayToAnatomyRAI[1] = rai1;
  m_DisplayToAnatomyRAI[2] = rai2;

  if(!m_IRISImageData->IsGreyLoaded()) 
    return;
  
  // Create the appropriate transform and pass it to the IRIS data
  m_IRISImageData->SetImageGeometry(
    ImageCoordinateGeometry(
      m_ImageToAnatomyRAI,
      m_DisplayToAnatomyRAI,
      m_IRISImageData->GetVolumeExtents()));

  // Do the same for the SNAP data if needed
  if(!m_SNAPImageData)
    return;

  // Create the appropriate transform and pass it to the SNAP data
  m_SNAPImageData->SetImageGeometry(
    ImageCoordinateGeometry(
      m_ImageToAnatomyRAI,
      m_DisplayToAnatomyRAI,
      m_SNAPImageData->GetVolumeExtents()));
}

void 
IRISApplication
::UpdateIRISGreyImage(GreyImageType *newGreyImage, 
                      const char *newImageRAI)
{
  // This has to happen in 'pure' IRIS mode
  assert(m_SNAPImageData == NULL);

  // Get the size of the image as a vector of uint
  Vector3ui size = 
    to_unsigned_int(
      Vector3ul(
        newGreyImage->GetBufferedRegion().GetSize().GetSize()));
  
  // Store the new RAI code
  m_ImageToAnatomyRAI[0] = newImageRAI[0];
  m_ImageToAnatomyRAI[1] = newImageRAI[1];
  m_ImageToAnatomyRAI[2] = newImageRAI[2];

  // Compute the new image geometry for the IRIS data
  ImageCoordinateGeometry icg(m_ImageToAnatomyRAI,m_DisplayToAnatomyRAI,size);
  
  // Update the image information in m_Driver->GetCurrentImageData()
  m_IRISImageData->SetGreyImage(newGreyImage,icg);    
  
  // Reinitialize the intensity mapping curve 
  m_IntensityCurve->Initialize(4);

  // Update the new grey image wrapper with the intensity mapping curve
  m_IRISImageData->GetGrey()->SetIntensityMapFunction(m_IntensityCurve);

  // Update the crosshairs position
  Vector3ui cursor = size;
  cursor /= 2;
  m_IRISImageData->SetCrosshairs(cursor);
  
  // TODO: Unify this!
  m_GlobalState->SetCrosshairsPosition(cursor) ;

  // Update the preprocessing settings in the global state
  m_GlobalState->SetEdgePreprocessingSettings(
    EdgePreprocessingSettings::MakeDefaultSettings());
  m_GlobalState->SetThresholdSettings(
    ThresholdSettings::MakeDefaultSettings(
      m_IRISImageData->GetGrey()));
}

void 
IRISApplication
::UpdateSNAPSpeedImage(SpeedImageType *newSpeedImage, 
                       SnakeType snakeMode)
{
  // This has to happen in SNAP mode
  assert(m_SNAPImageData);

  // Make sure the dimensions of the speed image are appropriate
  assert(m_SNAPImageData->GetGrey()->GetImage()->GetBufferedRegion().GetSize()
    == newSpeedImage->GetBufferedRegion().GetSize());

  // Initialize the speed wrapper
  if(!m_SNAPImageData->IsSpeedLoaded())
    m_SNAPImageData->InitializeSpeed();
  
  // Send the speed image to the image data
  m_SNAPImageData->GetSpeed()->SetImage(newSpeedImage);

  // Save the snake mode 
  m_GlobalState->SetSnakeMode(snakeMode);

  // Set the speed as valid
  m_GlobalState->SetSpeedValid(true);

  // Set the snake state
  if(snakeMode == EDGE_SNAKE)
    {
    m_SNAPImageData->GetSpeed()->SetModeToEdgeSnake();
    }
  else
    {
    m_SNAPImageData->GetSpeed()->SetModeToInsideOutsideSnake();
    }
}

void
IRISApplication
::UpdateIRISSegmentationImage(LabelImageType *newSegmentationImage)
{
  // This has to happen in 'pure' IRIS mode
  assert(m_SNAPImageData == NULL);

  // Update the iris data
  m_IRISImageData->SetSegmentationImage(newSegmentationImage);  
}

void 
IRISApplication
::UpdateIRISWithSnapImageData(CommandType *progressCommand)
{
  assert(m_SNAPImageData != NULL);

  // Get pointers to the source and destination images
  typedef LevelSetImageWrapper::ImageType SourceImageType;
  typedef LabelImageWrapper::ImageType TargetImageType;

  // If the voxel size of the image does not match the voxel size of the 
  // main image, we need to resample the region  
  SourceImageType::Pointer source = m_SNAPImageData->GetSnake()->GetImage();
  TargetImageType::Pointer target = m_IRISImageData->GetSegmentation()->GetImage();

  // Construct are region of interest into which the result will be pasted
  SNAPSegmentationROISettings roi = m_GlobalState->GetSegmentationROISettings();

  // If the ROI has been resampled, resample the segmentation in reverse direction
  if(roi.GetResampleFlag())
    {
    // Create a resampling filter
    typedef itk::ResampleImageFilter<SourceImageType,SourceImageType> ResampleFilterType;
    ResampleFilterType::Pointer fltSample = ResampleFilterType::New();

    // Initialize the resampling filter with an identity transform
    fltSample->SetInput(source);
    fltSample->SetTransform(itk::IdentityTransform<double,3>::New());

    // Typedefs for interpolators
    typedef itk::NearestNeighborInterpolateImageFunction<
      SourceImageType,double> NNInterpolatorType;
    typedef itk::LinearInterpolateImageFunction<
      SourceImageType,double> LinearInterpolatorType;
    typedef itk::BSplineInterpolateImageFunction<
      SourceImageType,double> CubicInterpolatorType;

    // More typedefs are needed for the sinc interpolator
    const static unsigned int VRadius = 5;
    typedef itk::Function::HammingWindowFunction<VRadius> WindowFunction;
    typedef itk::ConstantBoundaryCondition<SourceImageType> Condition;
    typedef itk::WindowedSincInterpolateImageFunction<
      SourceImageType, VRadius, 
      WindowFunction, Condition, double> SincInterpolatorType;

    // Choose the interpolator
    switch(roi.GetInterpolationMethod())
      {
      case SNAPSegmentationROISettings::NEAREST_NEIGHBOR :
        fltSample->SetInterpolator(NNInterpolatorType::New());
        break;

      case SNAPSegmentationROISettings::TRILINEAR : 
        fltSample->SetInterpolator(LinearInterpolatorType::New());
        break;

      case SNAPSegmentationROISettings::TRICUBIC :
        fltSample->SetInterpolator(CubicInterpolatorType::New());
        break;  

      case SNAPSegmentationROISettings::SINC_WINDOW_05 :
        cout << "Using Window Sinc" << endl;
        fltSample->SetInterpolator(SincInterpolatorType::New());
        break;
      };

    // Set the image sizes and spacing
    fltSample->SetSize(roi.GetROI().GetSize());
    fltSample->SetOutputSpacing(target->GetSpacing());

    // Watch the segmentation progress
    if(progressCommand) 
      fltSample->AddObserver(itk::AnyEvent(),progressCommand);

    // Set the unknown intensity to positive value
    fltSample->SetDefaultPixelValue(4.0f);

    // Perform resampling
    fltSample->UpdateLargestPossibleRegion();

    // Change the source to the output
    source = fltSample->GetOutput();
    }  
  
  // Create iterators for copying from one to the other
  typedef ImageRegionConstIterator<SourceImageType> SourceIteratorType;
  typedef ImageRegionIterator<TargetImageType> TargetIteratorType;
  SourceIteratorType itSource(source,source->GetLargestPossibleRegion());
  TargetIteratorType itTarget(target,roi.GetROI());

  // Figure out which color draws and which color is clear
  unsigned int iClear = m_GlobalState->GetPolygonInvert() ? 1 : 0;

  // Construct a merge table that contains an output intensity for every 
  // possible combination of two input intensities (note that snap image only
  // has two possible intensities
  LabelType mergeTable[2][MAX_COLOR_LABELS];
  for(unsigned int i=0;i<MAX_COLOR_LABELS;i++)
    {
    // Whe the SNAP image is clear, IRIS passes through to the output
    // except for the IRIS voxels of the drawing color, which get cleared out
    mergeTable[iClear][i] = (i!=m_GlobalState->GetDrawingColorLabel()) ? i : 0;

    // Assign the output intensity based on the current drawing mode
    mergeTable[1-iClear][i] = 
      m_IRISImageData->DrawOverFunction(m_GlobalState, (LabelType) i);
    }

  // Go through both iterators, copy the new over the old
  itSource.GoToBegin();
  itTarget.GoToBegin();
  while(!itSource.IsAtEnd())
    {
    // Get the two voxels
    LabelType &voxIRIS = itTarget.Value();    
    float voxSNAP = itSource.Value();

    // Check that we're ok (debug mode only)
    assert(!itTarget.IsAtEnd());

    // Perform the merge
    voxIRIS = mergeTable[voxSNAP <= 0 ? 1 : 0][voxIRIS];

    // Iterate
    ++itSource;
    ++itTarget;
    }

  // The target has been modified
  target->Modified();
}

void 
IRISApplication
::ReleaseSNAPImageData() 
{
  assert(m_SNAPImageData && m_CurrentImageData != m_SNAPImageData);

  delete m_SNAPImageData;
  m_SNAPImageData = NULL;
}

void 
IRISApplication
::SetCurrentImageDataToIRIS() 
{
  assert(m_IRISImageData);
  m_CurrentImageData = m_IRISImageData;
}

void IRISApplication
::SetCurrentImageDataToSNAP() 
{
  assert(m_SNAPImageData);
  m_CurrentImageData = m_SNAPImageData;
}

void IRISApplication
::ReadLabelDescriptionsFromTextFile(const char *filename)
  throw(ExceptionObject)
{
  // Can not do this in SNAP mode
  assert(m_SNAPImageData == NULL);
  
  // Create a stream for reading the file
  ifstream fin(filename);
  string line;

  // Check that the file is readable
  if(!fin.good())
    {
    throw itk::ExceptionObject(
      __FILE__, __LINE__,"File does not exist or can not be opened");
    }

  // Read each line of the file separately
  for(unsigned int iLine=0;!fin.eof();iLine++)
    {
    // Read the line into a string
    getline(fin,line);

    // Check if the line is a comment or a blank line
    if(line[0] == '#' || line.length() == 0)
      continue;

    // Create a stream to parse that string
    IRISIStringStream iss(line);
    iss.exceptions(ios_base::badbit | ios_base::failbit);

    try 
      {
      // Read in the elements of the file
      int idx, red, green, blue, visible, mesh;
      float alpha;
      iss >> idx;
      iss >> red;
      iss >> green;
      iss >> blue;
      iss >> alpha;
      iss >> visible;
      iss >> mesh;

      // Skip to a quotation mark
      iss.ignore(line.length(),'\"');

      // Allocate a label of appropriate size
      char *label = new char[line.length()+1];

      // Read the label
      iss.get(label,line.length(),'\"');

      // Create a new color label
      ColorLabel cl;

      // Store the results
      cl.SetValid(true);
      cl.SetRGB(0,(unsigned char) red);
      cl.SetRGB(1,(unsigned char) green);
      cl.SetRGB(2,(unsigned char) blue);
      cl.SetAlpha( (unsigned char) (255 * alpha) );
      cl.SetVisible(visible != 0);
      cl.SetDoMesh(mesh != 0);
      cl.SetLabel(label);

      // Store the color label
      m_IRISImageData->SetColorLabel(idx,cl);

      // Clean up the label
      delete label;      
      }
    catch(ios_base::failure)
      {
      // Close the input stream
      fin.close();
      
      // create an exeption string
      IRISOStringStream oss;
      oss << "Syntax error on line " << (iLine+1);

      // throw our own exception
      throw itk::ExceptionObject(
        __FILE__, __LINE__,oss.str().c_str());
      }
    }  

  fin.close();
}

void IRISApplication
::WriteLabelDescriptionsToTextFile(const char *filename)
  throw(ExceptionObject)
{
  // Open the file for writing
  ofstream fout(filename);
  
  // Check that the file is readable
  if(!fout.good())
    {
    throw itk::ExceptionObject(
      __FILE__, __LINE__,"File can not be opened for writing");
    }

  // Print out a header to the file
  fout << "################################################"<< endl;
  fout << "# ITK-SnAP Label Description File"               << endl;
  fout << "# File format: "                                 << endl;
  fout << "# IDX   -R-  -G-  -B-  -A--  VIS MSH  LABEL"     << endl;
  fout << "# Fields: "                                      << endl;
  fout << "#    IDX:   Zero-based index "                   << endl;
  fout << "#    -R-:   Red color component (0..255)"        << endl;
  fout << "#    -G-:   Green color component (0..255)"      << endl;
  fout << "#    -B-:   Blue color component (0..255)"       << endl;
  fout << "#    -A-:   Label transparency (0.00 .. 1.00)"   << endl;
  fout << "#    VIS:   Label visibility (0 or 1)"           << endl;
  fout << "#    IDX:   Label mesh visibility (0 or 1)"      << endl;
  fout << "#  LABEL:   Label description "                  << endl;
  fout << "################################################"<< endl;

  // Print out the labels
  for(unsigned int i=0;i<MAX_COLOR_LABELS;i++)
    {
    ColorLabel cl = m_IRISImageData->GetColorLabel(i);
    if(cl.IsValid())
      {
      fout << "  "  << right << std::setw(3) << i;
      fout << "   " << right << std::setw(3) << (int) cl.GetRGB(0);
      fout << "  "  << right << std::setw(3) << (int) cl.GetRGB(1);
      fout << "  "  << right << std::setw(3) << (int) cl.GetRGB(2);
      fout << "  "  << right << std::setw(7) << std::setprecision(2) 
           << (cl.GetAlpha() / 255.0f);
      fout << "  "  << right << std::setw(1) << (cl.IsVisible() ? 1 : 0);
      fout << "  "  << right << std::setw(1) << (cl.IsDoMesh() ? 1 : 0);
      fout << "    \"" << cl.GetLabel() << "\"" << endl;
      }
    }

  fout.close();
}   

