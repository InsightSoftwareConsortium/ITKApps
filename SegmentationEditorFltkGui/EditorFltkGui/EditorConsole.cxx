/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    EditorConsole.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

                    
#include <EditorConsole.h>
#include <vtkProperty.h>
#include <stdio.h>
#include <fstream.h>
#include <FL/fl_file_chooser.H>

/************************************
 *
 *  Constructor
 *
 ***********************************/
EditorConsole
::EditorConsole()
{
  CreateGUI();

  sliceNumberSlider->range(0, 124);
  sliceNumberSlider->value(0);

  paintRadiusSlider->range(0, 20);
  paintRadiusSlider->value(2);

  previousScale = 0.00;
  scaleLevel = 0.00;
  default_magnification = 1.0;
  basemergeResolution = 0.00001;
  window_X = 256;
  window_Y = 256;

  console_X = consoleWindow->w();
  console_Y = consoleWindow->h();

  (sourceWin->GetRenderer())->AddActor2D(overlayActor);

  helpOut->value("SEGMENTATION EDITOR MODULE:

This module allows you to manipulate the output
of the segmentation module to produce labeled volumes.

To begin the editing process, you must choose the source,
segmentation, and tree (scale information) to use.  The 
\"Source data\" is the original image data on which the 
segmenation is based.  The \"Segmented\" data is the base 
labeled image data produced by the watershed segmentation 
algorithm.  The \"Tree File Name\" is the tree of merges 
calculated by the watershed segmentation algorithm that 
is used to visualize the output at different scales.

Fill out the information for the Segmented and Source
data and press the \"Load Images\" button.  You can
save this session for future use by selecting the
\"Save Session\" button from the console.  For future
sessions, simply select \"Load Session\" and specify
the .ws file.  

SEGMENTATION EDITOR CONSOLE WINDOW

This section describes the controls you will see in the 
Editor window.

INTRODUCTION

The purpose of the editor is to facilitate a construction 
of a labeled image using a combination of automatic and 
hand labeling of voxels.  Think of the regions produced 
by the watershed filter as pieces of a 3D puzzle which 
you can assemble into a labeled image, which is then 
fine-tuned or augmented by hand painting of regions.

The controls of the editor are described in detail below,  
but the basic idea is this: Regions in the \"Segmented\"  
window can be selected with the mouse and their  
corresponding pixels labeled (or unlabeled) in the image  
in the \"Source\" window.  In addition to painting by region  
(3D), you can paint by hand (2D) to fill in gaps or erase  
unwanted areas.  At any point the resulting 3D volume can be  
rendered in a separate window.  The labeled image is stored 
as a binary volume which can be saved to disk and loaded 
back into the editor.

INTERACTING WITH WINDOWS

There are three data windows, numbered below from the left 
side of the screen.  

Segmented Window -- This window displays the segmentation 
data at whichever scale is selected via the \"Scale\" slider.  
Left-Click will select one of the colored regions (after 
selecting it will highlight in white).  Shift-Left-Click 
allows you to select multiple regions at one time.

Source Window -- This window displays the color data from 
which the segmentation was constructed.  The binary mask of 
the segmentation is overlayed in blue on this window. Left-Click 
freehand draws pixels into the binary volume.  Right-Click 
freehand erases pixels from the binary volume.  The size of 
the paintbrush is controlled by the \"Paint Radius\" slider. 
Middle-Click Selects the region at that location in the 
Segmented Window.  Shift-Middle-Click allows you to select 
multiple regions in the Segmentation Window.

Binary Window --  This window displays the segmentation that 
you are constructing.  No meaningful interaction can be had 
or is necessary with this window.

BUTTONS AND SLIDERS

Save Session: Allows you to save the image information for 
future use.

Load Session: Allows you to reload the images from a previous 
session.

Save Binary:  Allows you to write the binary label volume 
you are creating to disk.  You can use save to remember 
the state of the volume and undo changes back to that saved 
state.  The output of this tool is a binary volume with
 1's at pixel locations inside the structure and 0's at 
pixel locations outside the structure.

Load Binary: Loads a binary volume, replacing current volume.

Randomize Colors: Re-colorizes the regions in the segmentation 
window.

Render/Update 3D Image: Generate a three dimensional rendering 
of the surface of the segmentation.  This spawns a new window 
which can be raised/lowered and resized.  The rendering can be 
manipulated in this window.

Zoom: Change the scale at which you view the data.

Toggle Zoom: Turns zooming on/off allowing you to view the 
data at full resolution.

Add Selected Region: Adds the highlighed region(s) in the 
Segmentated Window to the binary volume.

Subtract Selected Region: Subtracts the highlighed region(s) 
in the Segmentated Window from the binary volume.

Merge Selected (when selecting a single region in the 
Segmentated Window): This function merges regions until the 
next merge with the selected region occurs.  You can move 
up the merge hierarchy in a non-linear fashion with this 
button.

Undo Last Merge:  Reverts the merge hierarchy to the state 
before the last merge operation.

Scale:  This slider moves the threshold value at which you
are viewing the segmentation up or down.

Scale units:  Changes the resolution of the Scale slider to 
move in finer or coarser increments.

Slice number:  Changes the 2D slice of the volume that you 
are currently viewing.

Clear All:  Erases the entire binary volume.

Show Help: Display a small help window.

Toggle Overlay: Toggles the display of the binary volume 
overlay in the Source Window.

Paint Radius:  Controls the size of the circle used to 
free-hand paint/erase pixels in the binary volume.


Known Bugs 7/26/02

Re-colorizing when deselecting multiple regions.  When multiple 
regions are deselected all at once, the distinct regions will 
sometimes remain the same color.  You can press the Randomize 
Colors button to recolor these regions.");
}




/************************************
 *
 *  Destructor
 *
 ***********************************/
EditorConsole
::~EditorConsole()
{
}

/************************************
 *
 * StartEditor
 *
 ***********************************/
void EditorConsole::StartEditor() {
  // connect the pipeline
  mapToRGBA->SetInput( resamplerSeg->GetOutput() );
  mapToRGBA->SetLookupTable( manager->GetLookupTable() );
  
  colorMapImg->SetLookupTable( lut );
  colorMapImg->SetInput( resamplerCol->GetOutput() );

  colorMapBin->SetInput(resamplerBin->GetOutput());
  colorMapBin->SetLookupTable(lutBin);

  sourceWin->SetInput(colorMapImg->GetOutput());
  segmentedWin->SetInput(mapToRGBA->GetOutput());
  binaryWin->SetInput(colorMapBin->GetOutput());

  overlayMapper->SetInput(colorMapBin->GetOutput());
  mapper[1]->SetInput(marcher[1]->GetOutput());
  renWin->AddRenderer(ren1);

  // read the data
  LoadSource();
  LoadSegmented();

  // configure the lookup table manager for colorizing
  manager->Initialize();
  manager->LoadTreeFile( treeSeg->value() );
  manager->SetNumberOfLabels(labeledImgReader->GetMaximumUnsignedLongValue());
  manager->GenerateColorTable();

  mapToRGBA->SetLookupTable( manager->GetLookupTable() );

  // configure the binary volume
  int x = (int)XSeg->value();
  int y = (int)YSeg->value();
  int z = (int)ZSeg->value();
  binaryVolume->SetExtent(0, x, 0, y , 0, z);
  binaryVolume->SetUpdateExtent(0, x, 0, y, 0, z);
  binaryVolume->AllocateScalars();

  resamplerBin->SetInput(binaryVolume);

  // initialize bounding boxes
  boundingBoxes->SetLabeledImage(labeledImgReader->GetOutput());
  boundingBoxes->GenerateBoundingBoxes();
  
  // configure the viewer widgets
  sliceNumberSlider->range(0, (int)ZSeg->value()-1);
  sliceNumberSlider->value(0);

  segmentedWin->SetZSlice(0);
  segmentedWin->SetColorWindow(255.0);
  segmentedWin->SetColorLevel(127.5);

  segmentedWin->SetResampler(resamplerSeg);
  segmentedWin->SetDataReader(labeledImgReader);
  segmentedWin->SetWSManager(manager);
  segmentedWin->SetSourceViewer(sourceWin);
  
  sourceWin->SetZSlice(0);
  sourceWin->SetColorWindow(255.0);
  sourceWin->SetColorLevel(127.5);

  sourceWin->SetResampler(resamplerSeg);
  sourceWin->SetDataReader(labeledImgReader);
  sourceWin->SetWSManager(manager);
  sourceWin->SetLabeledViewer(segmentedWin);
  sourceWin->SetBinaryVolume(binaryVolume);
  sourceWin->SetBinaryViewer(binaryWin);

  sourceWin->SetPaintRadius(2);

  binaryWin->SetColorWindow(255.0);
  binaryWin->SetColorLevel(127.5);
  binaryWin->SetZSlice(0);
}


/************************************
 *
 *  Show
 *
 ***********************************/
void EditorConsole
::Show()
{
  consoleWindow->show();
  segmentedConsole->show();
  sourceConsole->show();
  binaryConsole->show();


}

/************************************
 *
 *  Hide
 *
 ***********************************/
void EditorConsole
::Hide()
{
  consoleWindow->hide();
  interactor->hide();
  segmentedConsole->hide();
  sourceConsole->hide();
  binaryConsole->hide();
}

/************************************
 *
 *  Quit
 *
 ***********************************/
void EditorConsole
::Quit()
{
  binaryVolume->Clear();
  Hide();
}



/************************************
 *
 * ClearAll
 *
 ***********************************/
void EditorConsole::ClearAll() 
{
  binaryVolume->Clear();
  binaryWin->Render();
  sourceWin->Render();
}

/************************************
 *
 *  ViewImages
 *
 ***********************************/
void EditorConsole::ViewImages()
{
  ViewSegmented();
  ViewSource();
  ViewBinary();
}


/************************************
 *
 *  ViewSegmented
 *
 ***********************************/
void EditorConsole::ViewSegmented()
{
  segmentedWin->Render();
}


/************************************
 *
 *  ViewSource
 *
 ***********************************/
void EditorConsole::ViewSource()
{
  sourceWin->Render();
}

/************************************
 *
 *  ViewSource
 *
 ***********************************/
void EditorConsole::ViewBinary()
{
  binaryWin->Render();
}


/************************************
 *
 *  SelectSlice
 *
 ***********************************/
void EditorConsole::SelectSlice(unsigned int s)
{
  sliceNumberSlider->value(s);

  segmentedWin->SetZSlice(s);
  sourceWin->SetZSlice(s);
  binaryWin->SetZSlice(s);
  overlayMapper->SetZSlice(s);

  segmentedWin->Render();
  sourceWin->Render();
  binaryWin->Render();

}


/************************************
 *
 *  AddSelectedRegion
 *
 ***********************************/
void EditorConsole::AddSelectedRegion()
{
  logic->SetBinaryVolume(binaryVolume);
  logic->SetSourceVolume(labeledImgReader->GetOutput());
  logic->AddEquivalencies(manager, boundingBoxes);
  
  sourceWin->Render();
  binaryWin->Render();
}

/************************************
 *
 *  SubtractSelectedRegion
 *
 ***********************************/
void EditorConsole::SubtractSelectedRegion()
{
  logic->SetBinaryVolume(binaryVolume);
  logic->SetSourceVolume(labeledImgReader->GetOutput());
  logic->SubtractEquivalencies(manager, boundingBoxes);
  
  sourceWin->Render();
  binaryWin->Render();
}

/************************************
 *
 *  SetMerge
 *
 ***********************************/
void EditorConsole::SetMerge(double scale)
{
  manager->ClearHighlightedValuesToSameColor();
  manager->Merge((float)scale);
  scaleLevel = scale;
  
  segmentedWin->Render();
}

/************************************
 *
 *  SetMergeResolution
 *
 ***********************************/
void EditorConsole::SetMergeResolution(double mag) {

  double val = scaleSlider->value();
  double length = scaleSlider->w();
  double res = 0.00001 * mag;
  double min = local_max(0.0, scaleLevel-((length/2)*res));
  double max = local_min(1.0, min+(length*res));

  scaleSlider->step(res);
  scaleSlider->minimum(min);
  scaleSlider->maximum(max);
  scaleSlider->value( round(min, max, res, val) );
  SetMerge(scaleSlider->value());
  
}

double EditorConsole::round(double min, double max, double step, double val) {
  int x = 0; 

  while( (min + (x*step)) < val) {
    x += 1;
  }

  double less = min + (x-1) * step;
  double more = min + x * step;
  double mid = step/2.0;

  if( (less + mid) <= val) {
    return more;
  }

  return less;
}

double EditorConsole::local_min(double a, double b) {
  if(a < b)
    return a;
  else
    return b;
}

double EditorConsole::local_max(double a, double b) {
  if (a > b)
    return a;
  else
    return b;
}

/************************************
 *
 * MergeSelected
 *
 ************************************/
void EditorConsole::MergeSelected() {
  
  scaleLevel = manager->MergeSelected(); // ??
  manager->ClearHighlightedValuesToSameColor();
  manager->RecompileEquivalencies();
  manager->HighlightComputedEquivalencyList();

  segmentedWin->Render();
}

/************************************
 *
 * UndoLastMerge
 *
 ************************************/
void EditorConsole::UndoLastMerge() {
  manager->ClearHighlightedValuesToSameColor();
  manager->ClearComputedEquivalencyList();

  float t = manager->UndoLastMerge();
  previousScale = t;
  scaleLevel = t;

  segmentedWin->Render();
}


/************************************
 *
 * ToggleAntialiasing
 *
 ************************************/
void EditorConsole::ToggleAntialiasing() {

  if(vtk_antialiasing->value()) {
    for(int i=1; i<4; i++) {
      if(has_data[i]) {
        thresher[i]->SetInput(binaryVolume);
        marcher[i]->SetInput(thresher[i]->GetOutput());
        vtk_antialiaser[i]->SetInput(marcher[i]->GetOutput());
        mapper[i]->SetInput(vtk_antialiaser[i]->GetOutput());
        (blob[i]->GetProperty())->SetInterpolationToFlat();
      }
    }
  } 
  else {
    for(int i=1; i<4; i++) {
      if(has_data[i]) {
        thresher[1]->SetInput(binaryVolume);
        marcher[1]->SetInput(thresher[1]->GetOutput());
        mapper[1]->SetInput(marcher[1]->GetOutput());
      }
    }
  }
  interactor->show();
}

/***********************************
 *
 * ChangePaintValue
 *
 **********************************/
void EditorConsole::ChangePaintValue(int p) {
  binaryVolume->SetLabelValue(p);
}


/************************************
 * 
 * ToggleZoom
 *
 ***********************************/
void EditorConsole::ToggleZoom() {
  if(toggleZoom->value()) {
    ScaleZoom(zoomSlider->value());
  }
  else {
    toggleZoom->value(1);
    ScaleZoom(1.0);
    toggleZoom->value(0);
  }
}

/***********************************
 *
 * DoRecolor
 *
 **********************************/
void EditorConsole::DoRecolor() {
  manager->RandomizeColors();
  segmentedWin->Render();
}

/***********************************
 *
 * ChangeColormap
 *
 **********************************/
void EditorConsole::ChangeColormap() {
  lut->RemoveAllPoints();
  lut->AddRGBPoint(colormapMin->value(), 0.0, 0.0, 0.0);
  lut->AddRGBPoint(colormapMax->value(), 1.0, 1.0, 1.0);
  
  sourceWin->Render();
}

/***********************************
 *
 * ChangeWindowLevel
 *
 **********************************/
void EditorConsole::ChangeWindowLevel() {
  sourceWin->SetColorWindow(window->value());
  sourceWin->SetColorLevel(level->value());
  sourceWin->Render();
}


/************************************
 * 
 * ScaleZoom
 *
 ***********************************/
void EditorConsole::ScaleZoom(double mag) {
  if(toggleZoom->value()) {
    // resample the data
    
    resamplerSeg->SetAxisMagnificationFactor(0, mag);
    resamplerSeg->SetAxisMagnificationFactor(1, mag);
    resamplerCol->SetAxisMagnificationFactor(0, mag);
    resamplerCol->SetAxisMagnificationFactor(1, mag);
    resamplerBin->SetAxisMagnificationFactor(0, mag);
    resamplerBin->SetAxisMagnificationFactor(1, mag);
    
    // resize te windows if needed
    if(mag > 1.0) {
      double increaseX = (mag * window_X) - window_X;
      double increaseY = (mag * window_Y) - window_Y;
      int w = segmentedWin->w();
      int h = segmentedWin->h();
      
      segmentedConsole->size(w+(int)increaseX, h+(int)increaseY);
      sourceConsole->size(w+(int)increaseX, h+(int)increaseY);
      binaryConsole->size(w+(int)increaseX, h+(int)increaseY);
      segmentedWin->size(w+(int)increaseX, h+(int)increaseY);
      sourceWin->size(w+(int)increaseX, h+(int)increaseY);
      binaryWin->size(w+(int)increaseX, h+(int)increaseY);
      
    }
    else {
      segmentedWin->size(window_X, window_Y);
      sourceWin->size(window_X, window_Y);
      binaryWin->size(window_X, window_Y);

      segmentedConsole->size(window_X, window_Y);
      sourceConsole->size(window_X, window_Y);
      binaryConsole->size(window_X, window_Y);
    }
    
    // re-render
    segmentedWin->Render();
    sourceWin->Render();
    binaryWin->Render();
  }
  
}

/************************************
 * 
 * ToggleOverlay
 *
 ***********************************/
void EditorConsole::ToggleOverlay() {
  if( toggleOverlay->value()) {
    overlayActor->VisibilityOn();
  }
  else {
    overlayActor->VisibilityOff();
  }
  sourceWin->Render();
}

/************************************
 *
 * AddSurfaceRenderer
 *
 ***********************************/
void EditorConsole::AddSurfaceRenderer(int isovalue, float r, float g, float b) {
}


/************************************
 *
 * AddRenderer
 *
 ***********************************/
void EditorConsole::AddRenderer(int isovalue) 
{
  float r = 0.5;
  float g = 0.2;
  float b = 0.3;

  EditorConsoleBase::AddSurfaceRenderer(isovalue, r, g, b);

  thresher[isovalue]->SetInput(binaryVolume);
  marcher[isovalue]->SetInput(thresher[isovalue]->GetOutput());

  mapper[isovalue]->SetInput(marcher[isovalue]->GetOutput());

  ToggleAntialiasing();
  
}

/************************************
 * 
 * LoadImages
 *
 ***********************************/
void EditorConsole::LoadImages() {

  StartEditor();

  ViewImages();
}

/************************************
 * 
 * LoadSegmented
 *
 ***********************************/
void EditorConsole::LoadSegmented() {
  if(littleSeg->value() == 1) { 
    labeledImgReader->SetDataByteOrderToLittleEndian(); 
  } else {
    labeledImgReader->SetDataByteOrderToBigEndian();
  }

  labeledImgReader->SetNumberOfScalarComponents((int)componentsSeg->value());

  if(strcmp(dataTypeSeg->value(),"UnsignedChar")==0) {
    labeledImgReader->SetDataScalarTypeToUnsignedChar();
  }
  else if(strcmp(dataTypeSeg->value(),"UnsignedLong")==0) {
    labeledImgReader->SetDataScalarTypeToUnsignedLong();
  }
  else if(strcmp(dataTypeSeg->value(),"UnsignedShort")==0) {
    labeledImgReader->SetDataScalarTypeToUnsignedShort();
  }
  else if(strcmp(dataTypeSeg->value(),"Short")==0) {
    labeledImgReader->SetDataScalarTypeToShort();
  }
  else if(strcmp(dataTypeSeg->value(),"Int")==0) {
    labeledImgReader->SetDataScalarTypeToInt();
  }
  else if(strcmp(dataTypeSeg->value(),"Float")==0) {
    labeledImgReader->SetDataScalarTypeToFloat();
  }
  else {
    std::cerr << "Error! No data type selected for Segmented.\n";
    return;
  }

  labeledImgReader->SetDataExtent(0, (int)XSeg->value()-1, 0, (int)YSeg->value()-1, 0, (int)ZSeg->value()-1);

  if((XSor->value() != XSeg->value()) || (YSor->value() != YSeg->value()) || (ZSor->value() != ZSeg->value()) )  {
    std::cerr << "Error! Source Data does not match dimensions of Segmented Data\n";
    return;
  }

  // make 3 viewer windows size of images
  window_X = XSeg->value();
  window_Y = YSeg->value();

  segmentedConsole->size(window_X, window_Y);
  segmentedWin->size(window_X, window_Y);

  sourceConsole->size(window_X, window_Y);
  sourceWin->size(window_X, window_Y);

  binaryConsole->size(window_X, window_Y);
  binaryWin->size(window_X, window_Y);

  renWin->SetSize(window_X, window_Y);
  

  labeledImgReader->SetFileNameSliceOffset((int)sliceSeg->value());

  labeledImgReader->SetHeaderSize((int)headerSeg->value());

  if(multFilesSeg->value()) {
    labeledImgReader->SetFileDimensionality(2);
    labeledImgReader->SetFilePrefix(fileSeg->value());
    labeledImgReader->SetFilePattern(extSeg->value());
  } 
  else {
    labeledImgReader->SetFileDimensionality(3);    
    labeledImgReader->SetFileName(fileSeg->value() ); 
  }

  labeledImgReader->Modified();
  labeledImgReader->Update();
  (labeledImgReader->GetOutput())->SetUpdateExtentToWholeExtent();
  labeledImgReader->Update();  


  resamplerSeg->SetInput(labeledImgReader->GetOutput());

}

/************************************
 * 
 * LoadSource
 *
 ***********************************/
void EditorConsole::LoadSource() {
  // read in image
  if(littleSor->value() == 1) { 
    colorImgReader->SetDataByteOrderToLittleEndian(); 
  } else {
    colorImgReader->SetDataByteOrderToBigEndian();
  }

  colorImgReader->SetNumberOfScalarComponents((int)componentsSor->value());

  if(strcmp(dataTypeSor->value(),"UnsignedChar")==0) {
    colorImgReader->SetDataScalarTypeToUnsignedChar();
  }
  else if(strcmp(dataTypeSor->value(),"UnsignedShort")==0) {
    colorImgReader->SetDataScalarTypeToUnsignedShort();
  }
  else if(strcmp(dataTypeSor->value(),"Short")==0) {
    colorImgReader->SetDataScalarTypeToShort();
  }
  else if(strcmp(dataTypeSor->value(),"Int")==0) {
    colorImgReader->SetDataScalarTypeToInt();
  }
  else if(strcmp(dataTypeSor->value(),"Float")==0) {
    colorImgReader->SetDataScalarTypeToFloat();
  }
  else {
    std::cerr << "Error! No data type selected for Source.\n";
    return;
  }

  colorImgReader->SetDataExtent(0, (int)XSor->value()-1, 0, (int)YSor->value()-1, 0, (int)ZSor->value()-1);

  
  // set window size
  window_X = (int)XSor->value();
  window_Y = (int)YSor->value();

  colorImgReader->SetFileNameSliceOffset((int)sliceSor->value());

  colorImgReader->SetHeaderSize((int)headerSor->value());

  if(multFilesSor->value()) {
    colorImgReader->SetFileDimensionality(2);  
    colorImgReader->SetFilePrefix(fileSor->value());
    colorImgReader->SetFilePattern(extSor->value());
  } 
  else { 
    colorImgReader->SetFileDimensionality(3);
    colorImgReader->SetFileName(fileSor->value());
  }

  colorImgReader->Modified();
  colorImgReader->Update();
  (colorImgReader->GetOutput())->SetUpdateExtentToWholeExtent();
  colorImgReader->Update();

  resamplerCol->SetInput(colorImgReader->GetOutput());

}

/**********************************
 *
 * WriteBinaryVolume
 *
 **********************************/
void EditorConsole::WriteBinaryVolume() {
  const char * filename = fl_file_chooser("Binary Filename","*.binaryvolume","");
  if( !filename )
  {
    return;
  }

  bool retVal = binaryVolume->WriteToDisk(filename);
  if(retVal != 0) {
    std::cerr << "Error in writing binary volume!\n";
  }
}


/**********************************
 *
 * ReadBinaryVolume
 *
 **********************************/
void EditorConsole::ReadBinaryVolume() {
  const char * filename = fl_file_chooser("Binary Filename","*.binaryvolume","");
  if( !filename )
  {
    return;
  }

  int retVal = binaryVolume->ReadFromDisk(filename);
  if(retVal == -2) {
    std::cerr << "Volume dimensions do not match!\n";
  }
  else if(retVal != 0) {
    std::cerr << "Load failed!\n";
  }

  sourceWin->Render();
  binaryWin->Render();

}


/****************************
 * 
 * SetDataTypeSeg
 *
 ***********************************/
void EditorConsole::SetDataTypeSeg(const char* type) {
  dataTypeSeg->value(type);
}

/************************************
 * 
 * SetDataTypeSor
 *
 ***********************************/
void EditorConsole::SetDataTypeSor(const char* type) {
  dataTypeSor->value(type);
}


/****************************
 * 
 * LoadSession
 *
 ***********************************/
void EditorConsole::LoadSession() {
  binaryVolume->Clear();
  const char * filename = fl_file_chooser("Session Filename","*.ws","");
  if( !filename )
  {
    return;
  }
  ifstream data_in;
  data_in.open(filename); 
  int temp;
  std::string buffer;


  // read in segmented information
  data_in >> buffer; // SEGMENTED
  data_in >> buffer; // file 
  fileSeg->value(buffer.c_str());
  data_in >> buffer; // tree
  treeSeg->value(buffer.c_str());
  data_in >> buffer; // data type
  dataTypeSeg->value(buffer.c_str());
  data_in >> temp; // size x
  XSeg->value(temp);
  data_in >> temp; // size y
  YSeg->value(temp);
  data_in >> temp; // size z
  ZSeg->value(temp);
  data_in >> buffer; // Endianess
  if(buffer == "Little") { 
    littleSeg->value(1); 
    bigSeg->value(0); 
  } 
  else { 
    bigSeg->value(1); 
    littleSeg->value(0); 
  }
  data_in >> buffer; // stored in mult files
  bool mult = false;
  if(buffer == "Yes") {
    multFilesSeg->value(1); 
    mult=true;
  } 
  else {
    multFilesSeg->value(0);
  }
  data_in >> temp; // components
  componentsSeg->value(temp);
  if(mult) {
    data_in >> buffer; // VTK file extension
    extSeg->value(buffer.c_str());
  }
  data_in >> temp; // first slice
  sliceSeg->value(temp);
  data_in >> temp; // header to skip
  headerSeg->value(temp);

  // read in source information
  data_in >> buffer; // SOURCE
  data_in >> buffer; // file 
  fileSor->value(buffer.c_str());
  data_in >> buffer; // data type
  dataTypeSor->value(buffer.c_str());
  data_in >> temp; // size x
  XSor->value(temp);
  data_in >> temp; // size y
  YSor->value(temp);
  data_in >> temp; // size z
  ZSor->value(temp);
  data_in >> buffer; // Endianess
  if(buffer=="Little") { 
    littleSor->value(1); 
    bigSor->value(0); 
  } 
  else { 
    bigSor->value(1); 
    littleSor->value(0); 
  }
  data_in >> buffer; // stored in mult files
  mult = false;
  if(buffer == "Yes") {
    multFilesSor->value(1); 
    mult=true;
  } 
  else {
    multFilesSor->value(0);
  }
  data_in >> temp; // components
  componentsSor->value(temp);
  if(mult) {
    data_in >> buffer; // VTK file extension
    extSor->value(buffer.c_str());
  }
  data_in >> temp; // first slice
  sliceSor->value(temp);
  data_in >> temp; // header to skip
  headerSor->value(temp); 
  
  LoadImages();
}

/****************************
 * 
 * SaveSession
 *
 ***********************************/
void EditorConsole::SaveSession() {
  const char * filename = fl_file_chooser("Session Filename","*.ws","");
  if( !filename )
  {
    return;
  }
  ofstream data_out;
  data_out.open(filename); 

  // output segmented information
  data_out << "SEGMENTED " << fileSeg->value() << " " << treeSeg->value() << " " << dataTypeSeg->value() << " ";
  data_out << XSeg->value() << " " <<  YSeg->value() << " " << ZSeg->value() << " ";

  if(bigSeg->value() == 1) { data_out << "Big ";  }
  else { data_out << "Little ";  }

  if(multFilesSeg->value() == 1) { data_out << "Yes ";  }
  else { data_out << "No "; }

  data_out << componentsSeg->value() << " " << extSeg->value() << " ";
  data_out << sliceSeg->value() << " " << headerSeg->value() << " ";


  // output source information
  data_out << "SOURCE " << fileSor->value() << " " << dataTypeSor->value() << " ";
  data_out << XSor->value() << " " <<  YSor->value() << " " << ZSor->value() << " ";

  if(bigSor->value() == 1) { data_out << "Big ";  }
  else { data_out << "Little ";  }

  if(multFilesSor->value() == 1) { data_out << "Yes ";  }
  else { data_out << "No "; }

  data_out << componentsSor->value() << " " << extSor->value() << " ";
  data_out << sliceSor->value() << " " << headerSor->value() << " ";

  data_out.close();
}
