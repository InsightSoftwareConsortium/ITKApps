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

// Forward reference to the classes pointed at
class GlobalState;
class IRISImageData;
class SNAPImageData;
class UserInterfaceLogic;

/**
 * \class IRISApplication
 * \brief This class encapsulates the highest level login of SNAP and IRIS.
 *
 * TODO: Organize the interaction between this class, IRISImageData and SNAPImageData
 * in a more intuitive way.  
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

  /**
   * Constructor for the IRIS/SNAP application
   */
  IRISApplication();

  /**
   * Destructor for the application
   */
  ~IRISApplication();

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
   * Initialize SNAP Image data using region of interest extents
   */
  void InitializeSNAPImageData(const RegionType &roi);

  /**
   * Update IRIS image data with the segmentation contained in the SNAP image
   * data.
   */
  void UpdateIRISWithSnapImageData();

  /**
   * Release the SNAP Image data
   */
  void ReleaseSNAPImageData();

  /**
   * Set the transform from the image space to the anatomy space
   * The image-to-display transform will be automatically updated
   */
  void SetImageToAnatomyTransform(const ImageCoordinateTransform &T) {
    m_ImageToAnatomyTransform = T;
    m_ImageToDisplayTransform = 
      m_ImageToAnatomyTransform.Product(m_AnatomyToDisplayTransform);
    m_DisplayToImageTransform = m_ImageToDisplayTransform.Inverse();
  }

  /**
   * Set the transform from the anatomy space to the image space
   * The image-to-display transform will be automatically updated
   */
  void SetAnatomyToDisplayTransform(const ImageCoordinateTransform &T) {
    m_AnatomyToDisplayTransform = T;
    m_ImageToDisplayTransform = 
      m_ImageToAnatomyTransform.Product(m_AnatomyToDisplayTransform);
    m_DisplayToImageTransform = m_ImageToDisplayTransform.Inverse();
  }

  // Get the different transforms
  irisGetMacro(ImageToAnatomyTransform,const ImageCoordinateTransform &);
  irisGetMacro(AnatomyToDisplayTransform,const ImageCoordinateTransform &);
  irisGetMacro(ImageToDisplayTransform,const ImageCoordinateTransform &);
  irisGetMacro(DisplayToImageTransform,const ImageCoordinateTransform &);

  /**
   * Intensity mapping curve used for Grey images
   * in the application
   */
  irisGetMacro(IntensityCurve,IntensityCurvePointer);

  /** 
   * Get the user interface object 
   */
  irisGetMacro(UserInterface,UserInterfaceLogic *);

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

  // GUI driver class
  UserInterfaceLogic *m_UserInterface;

  // Slice transform information
  ImageCoordinateTransform m_ImageToAnatomyTransform;
  ImageCoordinateTransform m_AnatomyToDisplayTransform;
  ImageCoordinateTransform m_ImageToDisplayTransform;
  ImageCoordinateTransform m_DisplayToImageTransform;

  // Slice intensity mapping information
  IntensityCurveInterface::Pointer m_IntensityCurve;

  // Current cursor position
  Vector3i m_CursorPosition;
};

