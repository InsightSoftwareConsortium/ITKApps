/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IRISApplication.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "IntensityCurveInterface.h"
#include "ImageCoordinateTransform.h"
#include "itkImageRegion.h"
#include "itkExceptionObject.h"
#include "GlobalState.h"

#include <string>

// Forward reference to the classes pointed at
class IRISImageData;
class SNAPImageData;

// ITK forward defs
namespace itk
{
  template<class TPixel,unsigned int VDimensions> class Image;
}

/**
 * \class IRISApplication
 * \brief This class encapsulates the highest level login of SNAP and IRIS.
 *
 * TODO: Organize the interaction between this class, IRISImageData and SNAPImageData
 * in a more intuitive way.  
 *
 * 'RAI' codes used by this class:
 * The code is a string that describes the transform from image space to patient 
 * coordinate system as three letters from RLAPIS.  For instance, PSR
 * means that the image origin is at the posterior-superior-right corner
 * of the image coordinate system and that the x axis maps to A-P axis, 
 * y to I-S and x to R-L.
 *
 * \sa IRISImageData
 * \sa SNAPImageData
 */
class IRISApplication 
{
public:
  // Typedefs
  typedef IntensityCurveInterface::Pointer IntensityCurvePointer;
  typedef itk::ImageRegion<3> RegionType;
  typedef itk::Size<3> SizeType;
  typedef itk::Image<GreyType,3> GreyImageType;
  typedef itk::Image<LabelType,3> LabelImageType;
  typedef itk::Image<float,3> SpeedImageType;


  /**
   * Constructor for the IRIS/SNAP application
   */
  IRISApplication();

  /**
   * Destructor for the application
   */
  virtual ~IRISApplication();

  /**
   * Get image data related to IRIS operations
   */
  irisGetMacro(IRISImageData,IRISImageData *);

  /**
   * Get image data related to SNAP operations
   */
  irisGetMacro(SNAPImageData,SNAPImageData *);

  /**
   * Get the image data currently used
   */
  irisGetMacro(CurrentImageData,IRISImageData *);

  /**
   * Enter the IRIS mode
   */    
  void SetCurrentImageDataToIRIS();

  /**
   * Enter the SNAP mode
   */
  void SetCurrentImageDataToSNAP();

  /**
   * Set a new grey image for the IRIS Image data.  This method is called when the
   * grey image is loaded.  The prerequisite to this method is that the SNAP data
   * not be active (CurrentImageData == IRISImageData).
   */
  void UpdateIRISGreyImage(GreyImageType *newGreyImage,
                           const char *newImageRAICode);

  /** 
   * Update the IRIS image data with an external segmentation image (e.g., 
   * loaded from a file).
   */
  void UpdateIRISSegmentationImage(LabelImageType *newSegmentationImage);

  /** 
   * Update the SNAP image data with an external speed image (e.g., 
   * loaded from a file).
   */
  void UpdateSNAPSpeedImage(SpeedImageType *newSpeedImage, SnakeType snakeMode);
  
  /**
   * Initialize SNAP Image data using region of interest extents
   */
  void InitializeSNAPImageData(const RegionType &roi);

  /**
   * Update IRIS image data with the segmentation contained in the SNAP image
   * data.
   */
  void UpdateIRISWithSnapImageData();

  /**
   * Load label descriptions from a file in ASCII space separated format
   */
  void ReadLabelDescriptionsFromTextFile(const char *file) 
    throw(itk::ExceptionObject);

  /**
   * Save label description file in ASCII space separated format
   */
  void WriteLabelDescriptionsToTextFile(const char *file) 
    throw(itk::ExceptionObject);

  /**
   * Release the SNAP Image data
   */
  void ReleaseSNAPImageData();
  
  /** Update the display-anatomy mapping as an RAI code */
  void SetDisplayToAnatomyRAI(const char *rai0,const char *rai1,const char *rai2);

  /** Get the current image to anatomy RAI code */
  const char *GetImageToAnatomyRAI();

  /** Get the current display to anatomy RAI code */
  const char *GetDisplayToAnatomyRAI(unsigned int slice);

  /**
   * Intensity mapping curve used for Grey images
   * in the application
   */
  irisGetMacro(IntensityCurve,IntensityCurvePointer);

  /**
   * Get the global state object
   */
  irisGetMacro(GlobalState,GlobalState *);

  /**
   * Set the current cursor position.  This will cause all the active image
   * wrappers to update their current slice numbers
   */
  void SetCursorPosition(Vector3i cursor);
  irisGetMacro(CursorPosition,Vector3i);     

private:
  // Image data objects
  IRISImageData *m_IRISImageData,*m_CurrentImageData;
  SNAPImageData *m_SNAPImageData;

  // Global state object
  // TODO: Incorporate GlobalState into IRISApplication more nicely
  GlobalState *m_GlobalState;

  /** RAI between image space and anatomy space */
  std::string m_ImageToAnatomyRAI;

  /** RAI between anatomy space and image space */
  std::string m_DisplayToAnatomyRAI[3];

  // Slice intensity mapping information
  IntensityCurveInterface::Pointer m_IntensityCurve;

  // Current cursor position
  Vector3i m_CursorPosition;
};

