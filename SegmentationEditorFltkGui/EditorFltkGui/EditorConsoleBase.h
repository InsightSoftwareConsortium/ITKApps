/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    EditorConsoleBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef EDITORCONSOLEBASE
#define EDITORCONSOLEBASE

#include <itkImage.h>
#include <itkImageFileReader.h>

#include "itkUnaryFunctorImageFilter.h"
#include "itkScalarToRGBPixelFunctor.h"

#include "vtkPatchedImageReader.h"
#include <vtkImageResample.h>
#include "vtkWSLookupTableManager.h"
#include <vtkColorTransferFunction.h>
#include <vtkImageMapToColors.h>
#include "vtkWSBoundingBoxManager.h"
#include "vtkBinaryVolume.h"
#include <vtkLookupTable.h>
#include <vtkImageMapper.h>
#include <vtkActor2D.h>
#include <vtkRenderer.h>
#include <vtkFlRenderWindowInteractor.h>
#include "vtkBinaryVolumeLogic.h"
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageThreshold.h>
#include <vtkMarchingCubes.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkImageCast.h>

#include <itkImageToVTKImageFilter.h>
#include <itkFlipImageFilter.h>


class EditorConsoleBase 
{

public:
  typedef itk::Image<float, 3> SourceImageType;
  
  EditorConsoleBase();
  virtual ~EditorConsoleBase();

  virtual bool StartEditor();
  virtual void ViewImages();
  virtual void ViewSegmented();
  virtual void ViewSource();
  virtual void ViewBinary();
  virtual void SelectSlice(unsigned int);

  virtual void AddSelectedRegion();
  virtual void SubtractSelectedRegion();
  virtual void SetMerge(double);
  virtual void SetMergeResolution(double);
  virtual void MergeSelected();
  virtual void UndoLastMerge();
  virtual void ToggleAntialiasing();
  virtual void ChangePaintValue(int);
  virtual void ToggleZoom();
  virtual void ScaleZoom(double mag);
  virtual void ToggleOverlay();
  virtual void DoRecolor();
  virtual void ChangeColormap();
  virtual void ChangeWindowLevel();

  virtual void ClearAll();
  
  virtual void AddSurfaceRenderer(int, float, float, float);
  virtual void AddRenderer(int);

  virtual void LoadImages();
  virtual bool LoadSegmented();
  virtual bool LoadSource();
  virtual void LoadSession();
  virtual void SaveSession();
  virtual void WriteBinaryVolume();
  virtual void ReadBinaryVolume();

protected:

  // segmented image reader
  vtkPatchedImageReader* labeledImgReader;

  // resampler to resize segmented image
  vtkImageResample* resamplerSeg;

  // lookup table manager for segmented image
  vtkWSLookupTableManager* manager;

  // color mapper for segmented image
  vtkImageMapToColors* mapToRGBA;

  // bounding boxes for the segmented image
  vtkWSBoundingBoxManager* boundingBoxes;
  
  bool segmented_initialized;
  
  // reader for the color data
  itk::ImageFileReader<SourceImageType>::Pointer colorImgReader;
  itk::ImageToVTKImageFilter<SourceImageType>::Pointer converter;
  itk::FlipImageFilter<SourceImageType>::Pointer flip;

  vtkImageResample* resamplerCol;
  
  vtkColorTransferFunction* lut;

  vtkImageMapToColors* colorMapImg;

  bool source_initialized;

  // binary
  vtkBinaryVolume* binaryVolume;
  
  vtkImageResample* resamplerBin;

  vtkLookupTable* lutBin;

  vtkImageMapToColors* colorMapBin;

  vtkBinaryVolumeLogic* logic;

  bool binary_initialized;

  // image mapper for the overlay
  vtkImageMapper* overlayMapper;

  vtkActor2D* overlayActor;
  
  // 3D Renderer
  vtkRenderWindow* renWin;
  
  vtkRenderer* ren1;

  vtkImageThreshold* thresher[4];

  vtkMarchingCubes* marcher[4];

  vtkOpenGLPolyDataMapper* mapper[4];

  vtkActor* blob[4];

  bool has_data[4];

  // alternative vtk antialiasing filter - vtk_antialiaster
  vtkWindowedSincPolyDataFilter* vtk_antialiaser[4];

  int x, y, z;

};



#endif
