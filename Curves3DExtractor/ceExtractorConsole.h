/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ceExtractorConsole.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef ce_ExtractorConsole_h
#define ce_ExtractorConsole_h

#include "ceExtractorConsoleGUI.h"
#include "fltkImageViewer.h"
#include "fltkDisplayGlWindowGUI.h"
#include "fltkPointSet3D.h"
#include "fltkPointSet2D.h"
#include "itkPointSet.h"


class ceExtractorConsole : public ceExtractorConsoleGUI 
{

public:

  typedef fltk::ImageViewer< PixelType, OverlayPixelType >        ImageViewerType;
  typedef fltk::ImageViewer< InputPixelType, OverlayPixelType >   InputImageViewerType;

  typedef fltkDisplayGlWindowGUI                GlDisplayType;
  
  typedef ceExtractorConsoleBase::MeshType      PointSetType;
  typedef fltk::PointSet3D< PointSetType >      PointSetShapeType;
  typedef PointSetShapeType::Pointer            PointSetShapePointer;

  typedef ceExtractorConsoleBase::ImageSpaceMeshType ImageSpaceMeshType;
  typedef fltk::PointSet2D< ImageSpaceMeshType > ImageSpacePointSetShapeType;   
  typedef ImageSpacePointSetShapeType::Pointer   ImageSpacePointSetShapePointer;
  typedef ceExtractorConsoleBase::RealType       RealType;

public:
  ceExtractorConsole();
  virtual ~ceExtractorConsole();
  virtual void Show(void);
  virtual void Hide(void);
  virtual void Load(void);
  virtual void Load(const char * filename);
  virtual void Quit(void);
  virtual void ComputeGradient(void);
  virtual void ComputeHessian(void);
  virtual void ComputeHessianEigenAnalysis(void);
  virtual void ShowStatus(const char * text);
  virtual void ShowInput(void);
  virtual void ShowLaplacian(void);
  virtual void ShowSmoothed(void);
  virtual void ShowGradientModulus(void);
  virtual void ShowGradientOnEigenVector(void);
  virtual void ShowParametricSpace(void);
  virtual void ShowCurve3DPoints(void);
  virtual void ShowExtractedParametricPoints(void);
  virtual void ResetViewOfParametricSpace(void);
  virtual void ResetViewOfExtractedParametricSpace(void);
  virtual void SetSigma( RealType );
  virtual void Execute(void);

private:

  InputImageViewerType      m_InputViewer;

  ImageViewerType           m_Viewer_Smoothed;
  ImageViewerType           m_Viewer_Laplacian;
  ImageViewerType           m_Viewer_Gradient_Modulus;

  ImageViewerType           m_Viewer_Gradient_On_EigenVector;

  ImageViewerType           m_Viewer_Extracted_Points;
    
  GlDisplayType             m_ParametricSpaceViewer;
  GlDisplayType             m_ExtractedParametricSpaceViewer;

  PointSetShapePointer         m_ParametricSpaceSamplesShape;
  PointSetShapePointer         m_ExtractedParametricSpaceSamplesShape;
  
  ImageSpacePointSetShapePointer    m_ImageSpaceSamplesShape;

};

#endif
