/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    EditorConsoleBase.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include <EditorConsoleBase.h>
#include <vtkProperty.h>


/************************************
 *
 *  Constructor
 *
 ***********************************/
EditorConsoleBase 
::EditorConsoleBase()
{
  m_SourceReader = itk::ImageFileReader<itk::Image<float, 3> >::New();
  
  // segmented
  segmented_initialized = false;
  
  labeledImgReader = vtkPatchedImageReader::New();
  labeledImgReader->SetFileDimensionality(3);

  resamplerSeg = vtkImageResample::New();
  resamplerSeg->InterpolateOff();
  resamplerSeg->SetAxisMagnificationFactor(0, 1.0);
  resamplerSeg->SetAxisMagnificationFactor(1, 1.0);

  manager = vtkWSLookupTableManager::New();
  manager->SetHighlightColor(1.0, 1.0, 1.0);
  manager->SetRepaintHighlights(1);

  mapToRGBA = vtkImageMapToColors::New();
  mapToRGBA->SetOutputFormatToRGBA();

  boundingBoxes = vtkWSBoundingBoxManager::New();

  // source
  source_initialized = false;  colorImgReader = vtkImageReader::New();
  colorImgReader->SetHeaderSize(0);

  resamplerCol = vtkImageResample::New();
  resamplerCol->InterpolateOff();
  resamplerCol->SetAxisMagnificationFactor(0, 1.0);
  resamplerCol->SetAxisMagnificationFactor(1, 1.0);

  lut = vtkColorTransferFunction::New();
  lut->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
  lut->AddRGBPoint(255.0, 1.0, 1.0, 1.0);

  colorMapImg = vtkImageMapToColors::New();
  colorMapImg->SetOutputFormatToRGBA();

  // binary
  binaryVolume = vtkBinaryVolume::New();
  binaryVolume->SetExtent(0, 1, 0, 1, 0, 1);
  binaryVolume->SetUpdateExtent(0, 1, 0, 1, 0, 1);
  binaryVolume->SetLabelValue(1);

  resamplerBin = vtkImageResample::New();
  resamplerBin->InterpolateOff();
  resamplerBin->SetAxisMagnificationFactor(0, 1.0);
  resamplerBin->SetAxisMagnificationFactor(1, 1.0);

  lutBin = vtkLookupTable::New();
  lutBin->SetNumberOfTableValues(256);
  lutBin->SetTableRange(0, 255);
  lutBin->SetTableValue(0, 0.0, 0.0, 0.0, 0.0);    
  lutBin->SetTableValue(1, 1.0, 0.0, 0.0, 0.3);    
  lutBin->SetTableValue(2, 0.0, 1.0, 0.0, 0.3);    
  lutBin->SetTableValue(3, 0.0, 0.0, 1.0, 0.3);    

  colorMapBin = vtkImageMapToColors::New();

  logic = vtkBinaryVolumeLogic::New();

  binary_initialized = false;

  // image mapper for the overlay
  overlayMapper = vtkImageMapper::New();
  overlayMapper->SetColorWindow(255);
  overlayMapper->SetColorLevel(129);
  overlayMapper->SetZSlice(0);

  overlayActor = vtkActor2D::New();
  overlayActor->SetMapper(overlayMapper);
  //(sourceWin->GetRenderer())->AddActor2D(overlayActor);
  overlayActor->VisibilityOn();

  // 3D Renderer
  renWin = vtkRenderWindow::New();
  renWin->SetSize(256, 256);

  ren1 = vtkRenderer::New();
  ren1->SetViewport(0, 0, 1, 1);

  //interactor = vtkFlRenderWindowInteractor::New();
  //interactor->SetRenderWindow( renWin );
  //interactor->Initialize();

  has_data[0] = has_data[1] = has_data[2] = has_data[3] = false;

  AddSurfaceRenderer(1, 0.5, 0.0, 0.0); 

  for(int i=0; i<4; i++) {
    vtk_antialiaser[i] = vtkWindowedSincPolyDataFilter::New();
    vtk_antialiaser[i]->SetNumberOfIterations(20);
  }


}




/************************************
 *
 *  Destructor
 *
 ***********************************/
EditorConsoleBase 
::~EditorConsoleBase()
{
  if(labeledImgReader) {
    labeledImgReader->Delete();
  }
  if(resamplerSeg) {
    resamplerSeg->Delete();
  }
  if(mapToRGBA) {
    mapToRGBA->Delete();
  }
  if(boundingBoxes) {
    boundingBoxes->Delete();
  }
  if(colorImgReader) {
    colorImgReader->Delete();
  }
  if(resamplerCol) {
    resamplerCol->Delete();
  }
  if(lut) {
    lut->Delete();
  }
  if(colorMapImg) {
    colorMapImg->Delete();
  }
  if(binaryVolume) {
    binaryVolume->Delete();
  }
  if(resamplerBin) {
    resamplerBin->Delete();
  }
  if(lutBin) {
    lutBin->Delete();
  }
  if(colorMapBin) {
    colorMapBin->Delete();
  }
  if(logic) {
    logic->Delete();
  }
  if(overlayMapper) {
    overlayMapper->Delete();
  }
  if(overlayActor) {
    overlayActor->Delete();
  }
  if(renWin) {
    renWin->Delete();
  }
  if(ren1) {
    ren1->Delete();
  }
  //  if(interactor) {
  //    interactor->Delete();
  //}
  for(int i=1; i<4; i++) {
    if(has_data[i]) {
      thresher[i]->Delete();
      marcher[i]->Delete();
      mapper[i]->Delete();
      blob[i]->Delete();
      vtk_antialiaser[i]->Delete();
    }
  }
}

/***********************************
 *
 * StartEditor
 *
 **********************************/
void EditorConsoleBase::StartEditor() {

}


/************************************
 *
 * ClearAll
 *
 ***********************************/
void EditorConsoleBase::ClearAll() 
{
}

/************************************
 *
 *  ViewImages
 *
 ***********************************/
void EditorConsoleBase::ViewImages()
{

}


/************************************
 *
 *  ViewSegmented
 *
 ***********************************/
void EditorConsoleBase::ViewSegmented()
{

}

/************************************
 *
 *  ViewSource
 *
 ***********************************/
void EditorConsoleBase::ViewSource()
{

}

/************************************
 *
 *  ViewBinary
 *
 ***********************************/
void EditorConsoleBase::ViewBinary()
{

}


/************************************
 *
 *  ViewSlice
 *
 ***********************************/
void EditorConsoleBase::SelectSlice(unsigned int s)
{

}

/************************************
 *
 *  AddSelectedRegion
 *
 ***********************************/
void EditorConsoleBase::AddSelectedRegion()
{

}

/************************************
 *
 *  SubtractSelectedRegion
 *
 ***********************************/
void EditorConsoleBase::SubtractSelectedRegion()
{

}

/************************************
 * 
 * SetMerge
 *
 ***********************************/
void EditorConsoleBase::SetMerge(double scale) {

}

/************************************
 *
 * SetMergeResolution
 *
 ***********************************/
void EditorConsoleBase::SetMergeResolution(double res) {

}


/************************************
 * 
 * MergeSelected
 *
 ***********************************/
void EditorConsoleBase::MergeSelected() {

}

/************************************
 * 
 * UndoLastMerge
 *
 ***********************************/
void EditorConsoleBase::UndoLastMerge() {

}


/************************************
 * 
 * ToggleAntialiasing
 *
 ***********************************/
void EditorConsoleBase::ToggleAntialiasing() {

}


/************************************
 * 
 * ChangePaintValue
 *
 ***********************************/
void EditorConsoleBase::ChangePaintValue(int p) {

}

/************************************
 * 
 * ToggleZoom
 *
 ***********************************/
void EditorConsoleBase::ToggleZoom() {
}

/************************************
 * 
 * ToggleOverlay
 *
 ***********************************/
void EditorConsoleBase::ToggleOverlay() {
}

/************************************
 *
 * DoRecolor
 *
 ***********************************/
void EditorConsoleBase::DoRecolor() {

}

/************************************
 *
 * ChangeColormap
 *
 ***********************************/
void EditorConsoleBase::ChangeColormap() {

}

/************************************
 *
 * ChangeWindowLevel
 *
 ***********************************/
void EditorConsoleBase::ChangeWindowLevel() {

}

/************************************
 * 
 * ScaleZoom
 *
 ***********************************/
void EditorConsoleBase::ScaleZoom(double mag) {
}


/***********************************
 *
 * AddSurfaceRenderer
 *
 ***********************************/
void EditorConsoleBase::AddSurfaceRenderer(int isovalue, float r, float g, float b) {

  if (isovalue <= 3 ) {    
    
    // threshold filter to remove other isovalues
    thresher[isovalue] = vtkImageThreshold::New();
    vtkImageThreshold* threshname = thresher[isovalue];
    threshname->ThresholdBetween(isovalue-0.9, isovalue+0.9);
    threshname->SetOutValue(0.0);
    
    threshname->SetInput(binaryVolume);

    threshname->Update();
    
    // marching cubes algorithm
    marcher[isovalue] = vtkMarchingCubes::New();
    vtkMarchingCubes* marchername = marcher[isovalue];
    marchername->SetNumberOfContours(1);
    marchername->SetValue(0, isovalue);
    
    marchername->SetInput(threshname->GetOutput());

    marchername->Update();
    
    // polygon mapper for surface rendering
    mapper[isovalue] = vtkOpenGLPolyDataMapper::New();
    vtkOpenGLPolyDataMapper* mapname = mapper[isovalue];
    mapname->ScalarVisibilityOff();
    mapname->ImmediateModeRenderingOn();
    mapname->SetInput(marchername->GetOutput());
    mapname->Update();

    // create an actor for the surface
    blob[isovalue] = vtkActor::New();
    vtkActor* blobname = blob[isovalue];
    blobname->SetMapper(mapname);
    (blobname->GetProperty())->SetColor(r, g, b);


    ren1->AddActor(blobname);

    has_data[isovalue] = true;


  } 
  else {
    std::cerr << "Error! Only 3 isovalues allowed\n";
  }
  
}
 
/***********************************
 *
 * AddRenderer
 *
 **********************************/
void EditorConsoleBase::AddRenderer(int isovalue) 
{
}


/************************************
 * 
 * LoadImages
 *
 ***********************************/
void EditorConsoleBase::LoadImages() {
}

/************************************
 * 
 * LoadSegmented
 *
 ***********************************/
void EditorConsoleBase::LoadSegmented() {
}

/************************************
 * 
 * LoadSource
 *
 ***********************************/
void EditorConsoleBase::LoadSource() {
}

/************************************
 * 
 * LoadSession
 *
 ***********************************/
void EditorConsoleBase::LoadSession() {
}

/************************************
 * 
 * SaveSession
 *
 ***********************************/
void EditorConsoleBase::SaveSession() {
}

/**********************************
 *
 * WriteBinaryVolume
 *
 **********************************/
void EditorConsoleBase::WriteBinaryVolume() {
}


/**********************************
 *
 * ReadBinaryVolume
 *
 **********************************/
void EditorConsoleBase::ReadBinaryVolume() {
}


/************************************
 * 
 * SetDataTypeSeg
 *
 ***********************************/
void EditorConsoleBase::SetDataTypeSeg(const char* type) {
}

/************************************
 * 
 * SetDataTypeSor
 *
 ***********************************/
void EditorConsoleBase::SetDataTypeSor(const char* type) {
}








