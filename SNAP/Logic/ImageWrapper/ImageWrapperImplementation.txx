/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageWrapperImplementation.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ImageWrapper_txx_
#define __ImageWrapper_txx_

#include <itkImageFileReader.h>
#include <itkImageIOFactory.h>
#include <itkImageRegionIterator.h>
#include <itkImageSliceConstIteratorWithIndex.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkRawImageIO.h>

#include <vtkImageData.h>
#include <vtkImageImport.h>

#include <iostream>
using namespace std;

template <class TPixel> 
ImageWrapperImplementation<TPixel>
::ImageWrapperImplementation() 
{
  CommonInitialization();
}

template <class TPixel>
ImageWrapperImplementation<TPixel>
::~ImageWrapperImplementation() 
{
  Reset();
}

template <class TPixel>
void 
ImageWrapperImplementation<TPixel>
::CommonInitialization()
{
  // Set initial state    
  m_Initialized = false;

  // Create slicer objects
  for(unsigned int i=0;i<3;i++)
    {
    m_Slicer[i] = SlicerType::New();
    m_Slicer[i]->SetSliceAxis(i);
    }
}

template <class TPixel>
ImageWrapperImplementation<TPixel>
::ImageWrapperImplementation(const ImageWrapper<TPixel> &copy) 
{
  CommonInitialization();

  // If the source contains an image, make a copy of that image
  if(copy.IsInitialized() && copy.GetImage()) 
    {
    m_Image = copy.DeepCopyRegion(copy.GetImage()->GetLargestPossibleRegion());
    HandleImagePointerUpdate();
    }
}

template <class TPixel> 
typename itk::ImageIOBase::Pointer 
CreateRawImageIO(unsigned int header) 
{
  typename itk::RawImageIO<TPixel, 3>::Pointer io = itk::RawImageIO<TPixel, 3>::New(); 
  io->SetHeaderSize(header);        
  typename itk::ImageIOBase::Pointer io2 = io.GetPointer();
  return io2;
}

template <class TPixel> 
bool 
ImageWrapperImplementation<TPixel>
::LoadFromRAWFile(const char *fname, unsigned int dimX, 
                  unsigned int dimY, unsigned int dimZ,
                  unsigned int header, RAWImagePixelType pixelType, 
                  bool isBigEndian, itk::Command *progressCommand) 
{
  typename itk::ImageIOBase::Pointer io;

  switch(pixelType) {
  case BYTE : 
    io = CreateRawImageIO<unsigned char>(header);
    break;

  case SHORT : 
    io = CreateRawImageIO<unsigned short>(header);
    break;

  case INT : 
    io = CreateRawImageIO<int>(header);
    break;

  case FLOAT : 
    io = CreateRawImageIO<float>(header);
    break;
  }

  io->SetDimensions(0,dimX);
  io->SetDimensions(1,dimY);
  io->SetDimensions(2,dimZ);
  if(isBigEndian)
    io->SetByteOrderToBigEndian();
  else
    io->SetByteOrderToLittleEndian();

  return DoLoad(fname,io,progressCommand);
}

template <class TPixel> 
bool 
ImageWrapperImplementation<TPixel>
::LoadFromFile(const char *fname, itk::Command *progressCommand) 
{
  // Call the internal load method with NULL io base
  return DoLoad(fname,NULL,progressCommand);
}

template <class TPixel> 
bool 
ImageWrapperImplementation<TPixel>
::DoLoad(const char *fname,itk::ImageIOBase::Pointer ioBase,itk::Command *progressCommand) 
{
  typedef itk::ImageFileReader<ImageType> ReaderType;

  try
    {
    // Set up the file reader   
    typename ReaderType::Pointer reader = ReaderType::New();

    // Assign the specified base if necessary
    if (ioBase)
      reader->SetImageIO(ioBase);

    // Set the file name 
    reader->SetFileName(fname);

    // Pass the progress command to the IO base
    if (progressCommand)
      reader->AddObserver(itk::ProgressEvent(), progressCommand);

    // Perform the file reading operation
    reader->Update();

    // Create a temporary image to hold the loaded image
    ImagePointer tmpImage = reader->GetOutput();

    // Get the updated image IO base
    m_ImageIO = reader->GetImageIO();

    // Assign the temporary image to the permanent image
    m_Image = tmpImage;

    // Check if the image actually loaded
    // verbose << "Loaded Image Rgn : " << tmpImage->GetLargestPossibleRegion() << endl;
    } 
  catch (itk::ExceptionObject &exc)
    {
    cerr << "Error reading input image" << endl;
    cerr << exc << endl;
    return false;
    }

  // Respond to change in permanent image
  HandleImagePointerUpdate();

  // Return success
  return true;
}

template <class TPixel> 
void 
ImageWrapperImplementation<TPixel>
::ReleaseImageIO()
{
  m_ImageIO = NULL;
}

template <class TPixel> 
void 
ImageWrapperImplementation<TPixel>
::PrintDebugInformation() 
{
  cout << "=== Image Properties ===" << endl;
  cout << "   Dimensions         : " << m_Image->GetLargestPossibleRegion().GetSize() << endl;
  cout << "   Origin             : " << m_Image->GetOrigin() << endl;
  cout << "   Spacing            : " << m_Image->GetSpacing() << endl;
  cout << "------------------------" << endl;
}

template <class TPixel>
void 
ImageWrapperImplementation<TPixel>
::HandleImagePointerUpdate() 
{
  // Update the slicing pipeline
  for(unsigned int i=0;i<3;i++)
    m_Slicer[i]->SetInput(m_Image);

  // Update the max-min pipeline once we have one setup
  m_MinMaxCalc = MinMaxCalculatorType::New();
  m_MinMaxCalc->SetImage(m_Image);

  // Update the VTK export pipeline
  m_VTKExporter = VTKExportType::New();
  m_VTKImporter = vtkImageImport::New();

  m_VTKExporter->SetInput(m_Image);


  m_VTKImporter->SetUpdateInformationCallback(m_VTKExporter->GetUpdateInformationCallback());
  m_VTKImporter->SetPipelineModifiedCallback(m_VTKExporter->GetPipelineModifiedCallback());
  m_VTKImporter->SetWholeExtentCallback(m_VTKExporter->GetWholeExtentCallback());
  m_VTKImporter->SetSpacingCallback(m_VTKExporter->GetSpacingCallback());
  m_VTKImporter->SetOriginCallback(m_VTKExporter->GetOriginCallback());
  m_VTKImporter->SetScalarTypeCallback(m_VTKExporter->GetScalarTypeCallback());
  m_VTKImporter->SetNumberOfComponentsCallback(m_VTKExporter->GetNumberOfComponentsCallback());
  m_VTKImporter->SetPropagateUpdateExtentCallback(m_VTKExporter->GetPropagateUpdateExtentCallback());
  m_VTKImporter->SetUpdateDataCallback(m_VTKExporter->GetUpdateDataCallback());
  m_VTKImporter->SetDataExtentCallback(m_VTKExporter->GetDataExtentCallback());
  m_VTKImporter->SetBufferPointerCallback(m_VTKExporter->GetBufferPointerCallback());

  m_VTKImporter->SetCallbackUserData(m_VTKExporter->GetCallbackUserData());



  m_VTKImage = m_VTKImporter->GetOutput();

  // Mark the image as Modified to enforce correct sequence of 
  // operations with MinMaxCalc
  m_Image->Modified();

  // We have been initialized
  m_Initialized = true;
}

template <class TPixel> 
void 
ImageWrapperImplementation<TPixel>
::InitializeToSize(unsigned int x, unsigned int y, unsigned int z) 
{
  itk::Size<3> size;
  size[0] = x;
  size[1] = y;
  size[2] = z;

  m_Image = ImageType::New();
  m_Image->SetRegions(size);
  m_Image->Allocate();

  HandleImagePointerUpdate();
}

template <class TPixel>
void 
ImageWrapperImplementation<TPixel>
::InitializeToImage(itk::ImageBase<3> *source) 
{
  itk::Size<3> size = source->GetLargestPossibleRegion().GetSize();
  m_Image = ImageType::New();
  m_Image->SetRegions(size);
  m_Image->Allocate();
  m_Image->FillBuffer(0);

  m_Image->SetOrigin(source->GetOrigin());
  m_Image->SetSpacing(source->GetSpacing());

  HandleImagePointerUpdate();
}

template <class TPixel>
void 
ImageWrapperImplementation<TPixel>
::SetImage(ImagePointer newImage) 
{
  m_Image = newImage;    
  HandleImagePointerUpdate();
}


template <class TPixel>
void 
ImageWrapperImplementation<TPixel>
::Reset() 
{
  if(m_Initialized)
    m_Image->ReleaseData();   
  m_Initialized = false;
}

template <class TPixel>
bool 
ImageWrapperImplementation<TPixel>
::IsInitialized() const 
{
  return m_Initialized;
}

template <class TPixel>
typename ImageWrapperImplementation<TPixel>::ImagePointer
ImageWrapperImplementation<TPixel>
::DeepCopyRegion(const itk::ImageRegion<3> region) const
{
  typedef itk::RegionOfInterestImageFilter <ImageType,ImageType> FilterType;

  // Create a filter to 'cut' the image
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(m_Image);
  filter->SetRegionOfInterest(region);
  filter->Update();

  // Return the resulting image
  return filter->GetOutput();
}

template <class TPixel>
inline const TPixel& 
ImageWrapperImplementation<TPixel>
::GetVoxel(const Vector3i &index) const 
{
  return GetVoxel(index[0],index[1],index[2]);
}

template <class TPixel>
inline TPixel& 
ImageWrapperImplementation<TPixel>
::GetVoxel(const Vector3i &index) 
{
  return GetVoxel(index[0],index[1],index[2]);
}

template <class TPixel>
inline TPixel& 
ImageWrapperImplementation<TPixel>
::GetVoxel(unsigned int x, unsigned int y, unsigned int z) 
{
  itk::Index<3> index;
  index[0] = x;
  index[1] = y;
  index[2] = z;

  // Verify that the pixel is contained by the image at debug time
  assert(m_Image && m_Image->GetLargestPossibleRegion().IsInside(index));

  // Return the pixel
  return m_Image->GetPixel(index);
}

template <class TPixel>
inline const TPixel& 
ImageWrapperImplementation<TPixel>
::GetVoxel(unsigned int x, unsigned int y, unsigned int z) const
{
  itk::Index<3> index;
  index[0] = x;
  index[1] = y;
  index[2] = z;

  // Verify that the pixel is contained by the image at debug time
  assert(m_Image && m_Image->GetLargestPossibleRegion().IsInside(index));

  // Return the pixel
  return m_Image->GetPixel(index);
}


template <class TPixel> 
void 
ImageWrapperImplementation<TPixel>
::CheckImageIntensityRange() 
{
  // Image should be loaded
  assert(m_Image && m_MinMaxCalc);

  // Check if the image has been updated since the last time that
  // the min/max has been computed
  if(m_Image->GetMTime() > m_MinMaxCalc->GetMTime()) {
    m_MinMaxCalc->Compute();
    m_MinMaxCalc->Modified();
    m_ImageScaleFactor = 1.0 / (m_MinMaxCalc->GetMaximum() - m_MinMaxCalc->GetMinimum());

    verbose << "Computing intensity range:" << endl;
    verbose << "   " << m_MinMaxCalc->GetMinimum() << " to " << m_MinMaxCalc->GetMaximum() << endl;
  }    
}

template <class TPixel> 
TPixel 
ImageWrapperImplementation<TPixel>
::GetImageMin() 
{
  // Make sure min/max are up-to-date
  CheckImageIntensityRange();

  // Return the max or min
  return m_MinMaxCalc->GetMinimum();
}

template <class TPixel>
double 
ImageWrapperImplementation<TPixel>
::GetImageScaleFactor()
{
  // Make sure min/max are up-to-date
  CheckImageIntensityRange();

  // Return the max or min
  return m_ImageScaleFactor;    
}

template <class TPixel> 
TPixel 
ImageWrapperImplementation<TPixel>
::GetImageMax()
{
  // Make sure min/max are up-to-date
  CheckImageIntensityRange();

  // Return the max or min
  return m_MinMaxCalc->GetMaximum();
}

template <class TPixel> 
typename ImageWrapperImplementation<TPixel>::ConstIterator 
ImageWrapperImplementation<TPixel>
::GetImageIterator() const 
{
  ConstIterator it(m_Image,m_Image->GetLargestPossibleRegion());
  it.GoToBegin();
  return it;
}

template <class TPixel> 
typename ImageWrapperImplementation<TPixel>::Iterator 
ImageWrapperImplementation<TPixel>
::GetImageIterator() 
{
  Iterator it(m_Image,m_Image->GetLargestPossibleRegion());
  it.GoToBegin();
  return it;
}

template <class TPixel> 
vtkImageData *ImageWrapperImplementation<TPixel>
::GetVTKImage() 
{

  m_VTKImporter->Update();
  return m_VTKImporter->GetOutput();
}

template <class TPixel>    
void 
ImageWrapperImplementation<TPixel>
::RemapIntensityToRange(TPixel min, TPixel max)
{
  typedef itk::RescaleIntensityImageFilter<ImageType> FilterType;
  typedef typename FilterType::Pointer FilterPointer;

  // Create a filter to remap the intensities
  FilterPointer filter = FilterType::New();
  filter->SetInput(m_Image);
  filter->SetOutputMinimum(min);
  filter->SetOutputMaximum(max);

  // Run the filter
  filter->Update();

  // Store the output and point everything to it
  m_Image = filter->GetOutput();
  HandleImagePointerUpdate();
}

template <class TPixel>    
void 
ImageWrapperImplementation<TPixel>
::RemapIntensityToMaximumRange()
{
  // Only allowed on integer data types, o/w makes no sense!
  assert(numeric_limits<TPixel>::is_integer);

  RemapIntensityToRange(numeric_limits<TPixel>::min(),numeric_limits<TPixel>::max());
}

#endif // __ImageWrapper_txx_
