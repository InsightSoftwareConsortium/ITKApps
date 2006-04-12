/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    CannySegmentationLevelSet.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef SHAPEDETECTIONLEVELSET
#define SHAPEDETECTIONLEVELSET

#include "CannySegmentationLevelSetGUI.h"
#include "fltkImageViewer.h"

#include "fltkVTKImageViewer.h"


/**
 * \brief CannySegmentationLevelSet class that instantiate
 * the elements required for a LevelSet approach for segmentation
 *
 */
class CannySegmentationLevelSet : public CannySegmentationLevelSetGUI
{
public:

  /** Dimension of the images to be registered */ 
  enum { ImageDimension = CannySegmentationLevelSetBase::ImageDimension };

  /** Pixel type used for reading the input image */
  typedef   CannySegmentationLevelSetBase::InputPixelType       InputPixelType;

  /** Pixel type to be used internally */
  typedef   CannySegmentationLevelSetBase::InternalPixelType    InternalPixelType;

  /** Input image type */
  typedef   CannySegmentationLevelSetBase::InputImageType       InputImageType;

  /** Internal image type */
  typedef   CannySegmentationLevelSetBase::InternalImageType    InternalImageType;

  /** Image Viewer Types */
  typedef CannySegmentationLevelSetBase::SeedPixelType        OverlayPixelType;

  /** FLTK viewer module */
  typedef fltk::ImageViewer< InternalPixelType, 
                                    OverlayPixelType >     InternalImageViewerType;

  typedef fltk::ImageViewer< InternalPixelType, 
                                    ThresholdPixelType >   SegmentedImageViewerType;

  typedef fltk::VTKImageViewer< ThresholdPixelType >       VTKImageViewerType;


public:
  CannySegmentationLevelSet();
  virtual ~CannySegmentationLevelSet();

  virtual void LoadInputImage();

  virtual void ShowConsole();

  virtual void ShowStatus(const char * text);

  virtual void ShowInputImage();

  virtual void ShowOutputLevelSet();

  virtual void ShowOutputCannyEdges();

  virtual void ShowGradientMagnitudeImage();

  virtual void ShowEdgePotentialImage();

  virtual void ShowThresholdedImage();

  virtual void ShowFastMarchingResultImage();
    
  virtual void ShowThresholdedImageWithVTK();

  virtual void Quit();

  virtual void SelectSeedPoint( float x, float y, float z );
  
  static  void ClickSelectCallback(float x, float y, float z, float value, void * args );

  virtual void ClearSeeds();

private:

  SegmentedImageViewerType        m_ThresholdedImageViewer;

  SegmentedImageViewerType        m_FastMarchingImageViewer;
  
  InternalImageViewerType         m_OutputLevelSetViewer;

  InternalImageViewerType         m_OutputCannyEdgesViewer;

  InternalImageViewerType         m_InputImageViewer;

  InternalImageViewerType         m_GradientMagnitudeImageViewer;

  InternalImageViewerType         m_EdgePotentialImageViewer;

  InternalImageViewerType         m_ZeroSetImageViewer;

  VTKImageViewerType::Pointer     m_VTKSegmentedImageViewer;

};



#endif
