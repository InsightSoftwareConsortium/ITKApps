/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageWrapper.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ImageWrapper_h_
#define __ImageWrapper_h_

// Smart pointers have to be included from ITK, can't forward reference them
#include <itkSmartPointer.h>
#include <IRISTypes.h>

// Forward declarations of the ITK classes
namespace itk {
  template <unsigned int size> class ImageBase;
  template <unsigned int size> class ImageRegion;
  template <class TPixel, unsigned int size> class Image;
  template <class ImageType> class ImageRegionIterator;
  template <class ImageType> class ImageRegionConstIterator;
  class Command;
  class ImageIOBase;
}

// Forward declarations to the VTK classes
class vtkImageImport;
class vtkImageData;

// Forward declarations to IRIS classes
template <class TPixel> class IRISSlicer;

/**
 * \class ImageWrapper
 * \brief A wrapper around an itk::Image and related pipelines.
 * 
 * Image Wrapper serves as a wrapper around an image pointer, and 
 * is used to unify the treatment of different kinds of images in
 * SNaP.  This class is the public interface to the image wrapper;
 * the actual implementation is implemented in ImageWrapperImplementation
 *
 * \sa ImageWrapperImplementation
 */
template<class TPixel> class ImageWrapper 
{
public:

  // Basic type definitions
  typedef itk::Image<TPixel,3> ImageType;
  typedef typename itk::SmartPointer<ImageType> ImagePointer;

  // Slice image type
  typedef itk::Image<TPixel,2> SliceType;
  typedef typename itk::SmartPointer<SliceType> SlicePointer;

  // Iterator types
  typedef typename itk::ImageRegionIterator<ImageType> Iterator;
  typedef typename itk::ImageRegionConstIterator<ImageType> ConstIterator;

  // Slicer type
  typedef IRISSlicer<TPixel> SlicerType;

  /**
   * An enumeration of data types that can be used to load a raw image
   */
  enum RAWImagePixelType
    {
    BYTE, SHORT, INT, FLOAT
    };

  // Destructor
  virtual ~ImageWrapper() {};

  /**
   * Initialize the internal image to a blank image of size x,y,z.
   */
  virtual void InitializeToSize(unsigned int x, unsigned int y, 
                                unsigned int z) = 0;

  /**
   * Initialize the image to match another image but with constant zero 
   * intensity
   */
  virtual void InitializeToImage(itk::ImageBase<3> *source) = 0;

  /**
   * Load the image from a file that can be parsed without additional 
   * information (i.e. not RAW files).  This method should be followed by
   * a call to ReleaseImageIO for memory efficiency.
   */
  virtual bool LoadFromFile(const char *fname,
                            itk::Command *progressCommand = NULL) = 0;

  /**
   * Load the image from a RAW file.  This method should be followed by
   * a call to ReleaseImageIO for memory efficiency.
   */
  virtual bool LoadFromRAWFile(const char *file,
                               unsigned int dimX, unsigned int dimY, 
                               unsigned int dimZ,unsigned int header, 
                               RAWImagePixelType pixelType,bool isBigEndian,
                               itk::Command *progressCommand = NULL) = 0;

  /**
   * Get a pointer to the ImageIO used internally by the LoadFromFile and
   * LoadFromRAWFile methods.  This is useful for getting basic information
   * about the format of the input image.  This method will return NULL
   */
  virtual itk::ImageIOBase *GetImageIO() const = 0;
  
  /**
   * Deallocate the ImageIOBase used for loading
   */
  virtual void ReleaseImageIO() = 0;

  /**
   * Clear the data associated with storing an image
   */
  virtual void Reset() = 0;

  /**
   * Is the image initialized?
   */
  virtual bool IsInitialized() const = 0;

  /**
   *  Simple save routine
   */
  // bool SaveToFile(const char *fname);

  /**
   * Get reference to a voxel at a given position.
   */
  virtual TPixel &GetVoxel(unsigned int x, unsigned int y, 
                           unsigned int z) = 0;

  /**
   * Get a constant reference to a voxel at a given position.
   */
  virtual const TPixel &GetVoxel(unsigned int x, unsigned int y, 
                                 unsigned int z) const = 0;

  virtual TPixel &GetVoxel(const Vector3i &index) = 0;
  virtual const TPixel &GetVoxel(const Vector3i &index) const = 0;


  /** 
   * Return the pointed to the ITK image encapsulated by this wrapper.
   */
  virtual ImagePointer GetImage() const = 0;

  /**
   * Get the size of the image
   */
  virtual Vector3i GetSize() const = 0;

  /**
   * Get the minimum intensity value.  Call ComputeImageIntensityRange() 
   * first.
   */
  virtual TPixel GetImageMin() = 0;

  /**
   * Get the maximum intensity value.  Call ComputeImageIntensityRange() 
   * first.
   */
  virtual TPixel GetImageMax() = 0;

  /**
   * Get the scaling factor used to convert between intensities stored
   * in this image and the 'true' image intensities
   */
  virtual double GetImageScaleFactor() = 0;

  /**
   * Remap the intensity range of the image to a given range
   */
  virtual void RemapIntensityToRange(TPixel min, TPixel max) = 0;

  /**
   * Remap the intensity range to max possible range
   */
  virtual void RemapIntensityToMaximumRange() = 0;

  /**
   * Set the current slice index in all three dimensions.  The index should
   * be specified in the image coordinates, the slices will be generated
   * in accordance with the transforms that are specified
   */
  virtual void SetSliceIndex(const Vector3i &cursor) = 0;

  /**
   * Return the current slice index
   */
  virtual Vector3i GetSliceIndex() const = 0;

  /**
   * Get a slice of the image in a given direction
   */
  virtual SlicePointer GetSlice(unsigned int dimension) = 0;    

  /**
   * This method returns a pointer to the VTKImage exported from this 
   * ITK image
   */
  virtual vtkImageData *GetVTKImage() = 0;

  /**
   * This method exposes the scalar pointer in the image
   */
  virtual const TPixel *GetVoxelPointer() const = 0;

  /**
   * Pring debugging info
   * TODO: Delete this or make is worthwhile
   */
  virtual void PrintDebugInformation() = 0;
                       
  /**
   * Replace the image internally stored in this wrapper by another image.
   */
  virtual void SetImage(ImagePointer newImage) = 0;

  /**
   * This method is used to perform a deep copy of a region of this image 
   * into another image
   */
  virtual ImagePointer DeepCopyRegion(const itk::ImageRegion<3> region) const = 0;

  /**
   * Get an iterator for traversing the image.  The iterator is initialized
   * to point to the beginning of the image
   */
  virtual ConstIterator GetImageIterator() const = 0;
  virtual Iterator GetImageIterator() = 0;
};

#endif // __ImageWrapper_h_
