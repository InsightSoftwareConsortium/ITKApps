/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageWrapperImplementation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ImageWrapperImplementation_h_
#define __ImageWrapperImplementation_h_

// Include our parent
#include <ImageWrapper.h>

// Include ITK headers
#include <itkSmartPointer.h>
#include <itkImage.h>
#include <itkImageRegion.h>
#include <itkExtractImageFilter.h>
#include <itkCropImageFilter.h>
#include <itkMinimumMaximumImageCalculator.h>
#include <itkVTKImageExport.h>
#include <itkImageIOBase.h>
#include <itkCommand.h>
#include <itkImageRegionIterator.h>
#include <itkVTKImageExport.h>

// Include the slicer headers
#include "IRISSlicer.h"

/**
 * \class ImageWrapperImplementation
 * \brief The implementation (code) behind ImageWrapper
 * 
 * Image Wrapper serves as a wrapper around an image pointer, and 
 * is used to unify the treatment of different kinds of images in
 * SNaP.
 *
 * \sa ImageWrapper
 */
template<class TPixel> class ImageWrapperImplementation 
: public virtual ImageWrapper<TPixel>
{
public:

  // Basic type definitions
  typedef itk::Image<TPixel,3> ImageType;
  typedef typename itk::SmartPointer<ImageType> ImagePointer;

  // Slice image type
  typedef itk::Image<TPixel,2> SliceType;
  typedef typename itk::SmartPointer<SliceType> SlicePointer;

  // VTK export related definitions
  typedef itk::VTKImageExport<ImageType> VTKExportType;
  typedef typename itk::SmartPointer<VTKExportType> VTKExportPointer;

  // Other types related to the image
  typedef itk::MinimumMaximumImageCalculator<ImageType> MinMaxCalculatorType;
  typedef typename itk::SmartPointer<MinMaxCalculatorType> MinMaxCalculatorPointer;

  // Iterator types
  typedef typename itk::ImageRegionIterator<ImageType> Iterator;
  typedef typename itk::ImageRegionConstIterator<ImageType> ConstIterator;

  // Enums
  typedef typename ImageWrapper<TPixel>::RAWImagePixelType RAWImagePixelType;

  // Slicer type
  typedef IRISSlicer<TPixel> SlicerType;
  typedef typename itk::SmartPointer<SlicerType> SlicerPointer;

  // Image IO types
  typedef itk::ImageIOBase ImageIOType;
  typedef typename itk::SmartPointer<ImageIOType> ImageIOPointer;

  ImageWrapperImplementation();
  ImageWrapperImplementation(const ImageWrapper<TPixel> &copy);
  virtual ~ImageWrapperImplementation();
  virtual void InitializeToSize(unsigned int x, unsigned int y, unsigned int z);
  virtual void InitializeToImage(itk::ImageBase<3> *source);
  bool LoadFromFile(const char *fname,itk::Command *progressCommand = NULL);
  
  bool LoadFromRAWFile(const char *file,
                       const Vector3ui &size,const Vector3d &spacing,
                       unsigned int header, RAWImagePixelType pixelType,
                       bool isBigEndian,itk::Command *progressCommand);
  void Reset();
  bool IsInitialized() const;
  virtual TPixel &GetVoxelForUpdate(unsigned int x, unsigned int y, unsigned int z);
  virtual const TPixel &GetVoxel(unsigned int x, unsigned int y, unsigned int z) const;
  TPixel &GetVoxelForUpdate(const Vector3i &index);
  const TPixel &GetVoxel(const Vector3i &index) const;

  ImagePointer GetImage() const {
    return m_Image;
  }

  Vector3i GetSize() const {

    // Cast the size to our vector format
    itk::Size<3> size = m_Image->GetLargestPossibleRegion().GetSize();      
    return Vector3i(size[0],size[1],size[2]);
  }

  TPixel GetImageMin();
  TPixel GetImageMax();

  double GetImageScaleFactor();

  void RemapIntensityToRange(TPixel min, TPixel max);
  void RemapIntensityToMaximumRange();

  vtkImageData *GetVTKImage();

  const TPixel *GetVoxelPointer() const {
    return this->m_Image->GetBufferPointer();
  }

  void PrintDebugInformation();

  void SetImage(ImagePointer newImage);

  ConstIterator GetImageIterator() const;
  Iterator GetImageIterator();

  void SetImageToDisplayTransform(const ImageCoordinateTransform &t)
  {
    for (unsigned int i=0;i<3;i++)
      m_Slicer[i]->SetImageToDisplay(t);
  }

  void SetSliceIndex(const Vector3i &cursor)
  {
    for (unsigned int i=0;i<3;i++)
      m_Slicer[i]->SetSliceIndex(cursor(i));
  }

  Vector3i GetSliceIndex() const
  {
    Vector3i rtn;
    for (unsigned int i=0;i<3;i++)
      rtn(i) = m_Slicer[i]->GetSliceIndex();
    return rtn;
  }

  SlicePointer GetSlice(unsigned int dim) 
  {
    return m_Slicer[dim]->GetOutput();
  }

  irisGetMacro(ImageIO,ImageIOType *);
  
  void ReleaseImageIO();


protected:

  // The image that we are wrapping
  ImagePointer m_Image;

  // The associated slicer filters
  SlicerPointer m_Slicer[3];

  // The min-max calculator used to hold min/max values.  This should be
  // replaced by a filter, when the latter is more efficient
  MinMaxCalculatorPointer m_MinMaxCalc;

  // VTK Image representation constructed using the export filter from the
  // ITK data
  VTKExportPointer m_VTKExporter;
  vtkImageImport *m_VTKImporter;
  vtkImageData *m_VTKImage;

  // Object used for image IO
  ImageIOPointer m_ImageIO;

  // The scaling factor
  double m_ImageScaleFactor;

  // This method is used to perform a deep copy of a region of this image into 
  // another image
  ImagePointer DeepCopyRegion(const itk::ImageRegion<3> region) const;

  // Is the image wrapper initialized?
  // This is a prerequisite for all operations
  bool m_Initialized;

  /**
   * A method to actually load the image from disk
   */
  bool DoLoad(const char *fname, itk::ImageIOBase *io, itk::Command *progressCommand);

  /**
   * Compute the intensity range of the image if it's out of date.  
   * This is done before calling GetImateMin, GetImateMax and GetImageScaleFactor methods.
   */
  void CheckImageIntensityRange();

protected:
  // Handle a change in the image pointer (i.e., a load operation on the image or 
  // an initialization operation)
  virtual void HandleImagePointerUpdate();

  // Common code for the different constructors
  void CommonInitialization();
  };

#include "ImageWrapperImplementation.txx"

#endif // __ImageWrapperImplementation_h_
