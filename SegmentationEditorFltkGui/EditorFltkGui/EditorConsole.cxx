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

                    
#include "EditorConsole.h"
#include <vtkProperty.h>
#include <stdio.h>
#include <fstream>
#include <FL/fl_file_chooser.H>
#include <fltkUtils.h>

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

  interactor->SetRenderWindow( renWin );
  interactor->Initialize();
  first_render = true;

  SourceAxes[0] = false;
  SourceAxes[1] = true;
  SourceAxes[2] = false;
  
  flip->SetFlipAxes(SourceAxes);

  clearText->value("Warning! This will clear your binary volume.\n\n            Do you wish to continue?");

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
bool EditorConsole::StartEditor() { 
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
  if(!LoadSource() || !LoadSegmented()) {
    return false;
  }

  mapToRGBA->SetLookupTable( manager->GetLookupTable() );

  // configure the binary volume
  binaryVolume->SetExtent(0, x-1, 0, y-1 , 0, z-1);
  binaryVolume->SetUpdateExtent(0, x-1, 0, y-1, 0, z-1);
  binaryVolume->AllocateScalars();
  binaryVolume->Clear();

  resamplerBin->SetInput(binaryVolume);

  // initialize bounding boxes
  boundingBoxes->SetLabeledImage(labeledImgReader->GetOutput());
  boundingBoxes->GenerateBoundingBoxes();
  
  // configure the viewer widgets
  sliceNumberSlider->range(0, z-1); 
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
  
  sourceWin->SetPaintRadius(paintRadiusSlider->value());
  
  return true;
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
  segmentedConsole->hide();
  sourceConsole->hide();
  binaryConsole->hide();
  renderConsole->hide();
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
 *  ViewBinary
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
  
  scaleLevel = manager->MergeSelected(); 
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
  if(first_render) {
    renderConsole->show();
    first_render = false;
  }

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
  switch(isovalue) {
  case 1:
    EditorConsoleBase::AddSurfaceRenderer(isovalue, 0.5, 0.0, 0.0);
    break;
  case 2:
    EditorConsoleBase::AddSurfaceRenderer(isovalue, 0.0, 0.25, 0.0);
    break;
  case 3:
    EditorConsoleBase::AddSurfaceRenderer(isovalue, 0.0, 0.0, 0.3);
    break;
  default:
    EditorConsoleBase::AddSurfaceRenderer(isovalue, 1.0, 1.0, 1.0);
  }


  thresher[isovalue]->SetInput(binaryVolume);
  marcher[isovalue]->SetInput(thresher[isovalue]->GetOutput());

  mapper[isovalue]->SetInput(marcher[isovalue]->GetOutput());

  ToggleAntialiasing();
  
}

/************************************
 * 
 * SpecifySegmentedFile
 *
 ***********************************/
void EditorConsole::SpecifySegmentedFile()
{
  const char * filename = fl_file_chooser("Segmented Session","*.ws","");
  if( !filename )
  {
    return;
  }

  fileSeg->value(filename);
}

/************************************
 * 
 * SpecifySourceFile
 *
 ***********************************/
void EditorConsole::SpecifySourceFile()
{
  const char * filename = fl_file_chooser("Source File","*.mh[da]","");
  if( !filename )
  {
    return;
  }

  fileSor->value(filename);
}
/************************************
 * 
 * LoadImages
 *
 ***********************************/
void EditorConsole::LoadImages() {

  if(StartEditor())
    ViewImages();
  else
    std::cerr << "IMAGES LOADED UNSUCCESSFULLY\n";
}

/************************************
 * 
 * LoadSegmented
 *
 ***********************************/
bool EditorConsole::LoadSegmented() {
  std::string labeled_filename = fileSeg->value();
  if(labeled_filename == "") {
    std::cerr << "Error! Specify Segmented Session file\n";
    return false;
  }

  char raw_file[256];
  char tree_file[256];
  std::ifstream in;
  int x1, y1, z1;
  in.open(labeled_filename.c_str());
  if(!in) {
    std::cerr << "Error! File " << labeled_filename << " does not exist!\n";
    return false;
  }

  in.getline(raw_file, 256, '\n');
  in.getline(tree_file, 256, '\n');
  in >> x1 >> y1 >> z1;
  in.close();
    
  labeledImgReader->SetDataByteOrderToLittleEndian(); 
  labeledImgReader->SetDataScalarTypeToUnsignedLong();
  labeledImgReader->SetDataExtent(0, x1-1, 0, y1-1, 0, z1-1);
  
  if((x != x1) || (y != y1) || (z != z1) )  {
    std::cerr << "Error! Source Data does not match dimensions of Segmented Data\n";
    return false;
  }
  
  labeledImgReader->SetFileNameSliceOffset(0);
  labeledImgReader->SetHeaderSize(0);
  labeledImgReader->SetFileDimensionality(3);    
  labeledImgReader->SetFileName(raw_file); 
  labeledImgReader->FileLowerLeftOn();
  labeledImgReader->Modified();
  labeledImgReader->Update();
  (labeledImgReader->GetOutput())->SetUpdateExtentToWholeExtent();
  labeledImgReader->Update();  

  resamplerSeg->SetInput(labeledImgReader->GetOutput());

  // make 3 viewer windows size of images
  window_X = x;
  window_Y = y;

  segmentedWin->size(window_X, window_Y);

  sourceWin->size(window_X, window_Y);

  binaryConsole->size(window_X, window_Y);
  binaryWin->size(window_X, window_Y);

  renWin->SetSize(window_X, window_Y);

  // configure the lookup table manager for colorizing
  manager->Initialize();
  manager->LoadTreeFile( tree_file );
  manager->SetNumberOfLabels(labeledImgReader->GetMaximumUnsignedLongValue());
  manager->GenerateColorTable();

  return true;
}

/************************************
 * 
 * LoadSource
 *
 ***********************************/
bool EditorConsole::LoadSource() {
  std::string source_filename = fileSor->value();
  if(source_filename == "") {
    std::cerr << "Error! Specify Source File!\n";
    return false;
  }

  // read in source image
  colorImgReader->SetFileName(source_filename.c_str());

  flip->SetInput(colorImgReader->GetOutput());

  try {
    flip->Update();
  } catch (itk::ExceptionObject & err) {
    std::cout << "Error! Exception caught\n";
    std::cout << err << std::endl;
    return false;
  }

  // execute converter
  converter->SetInput(flip->GetOutput());
  converter->GetExporter()->Update();
  converter->GetImporter()->Update();
  converter->Update();

  resamplerCol->SetInput(converter->GetOutput());

  // change colormap for source image
  float range[2];
  converter->GetOutput()->GetScalarRange(range);
  colormapMin->value(range[0]);
  colormapMax->value(range[1]);
  ChangeColormap();


  x = (colorImgReader->GetOutput()->GetLargestPossibleRegion()).GetSize()[0];
  y = (colorImgReader->GetOutput()->GetLargestPossibleRegion()).GetSize()[1];
  z = (colorImgReader->GetOutput()->GetLargestPossibleRegion()).GetSize()[2];
  
  return true;
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
 * LoadSession
 *
 ***********************************/
void EditorConsole::LoadSession() {

  binaryVolume->Clear();
  const char * filename = fl_file_chooser("Session Filename","*.ws_session","");
  if( !filename )
  {
    return;
  }
  std::ifstream data_in;
  data_in.open(filename); 
  char segmented_file[256];
  char source_file[256];

  // read in segmented information
  data_in.getline(segmented_file, 256, '\n');
  data_in.getline(source_file, 256, '\n'); 
  data_in.close();
  
  fileSeg->value(segmented_file);
  fileSor->value(source_file);

  LoadImages();

}

/****************************
 * 
 * SaveSession
 *
 ***********************************/
void EditorConsole::SaveSession() {

  const char * filename = fl_file_chooser("Session Filename","*.ws_session","");
  if( !filename )
  {
    return;
  }
  std::ofstream data_out;
  data_out.open(filename); 

  // output segmented information
  data_out << fileSeg->value() << "\n" << fileSor->value();
  data_out.close();

}


/***************************************
 *
 * FlipSource
 *
 ***************************************/
void EditorConsole::FlipSource(int a)
{
  if(SourceAxes[a]) {
    SourceAxes[a] = false;
  }
  else {
    SourceAxes[a] = true;
  }

  flip->SetFlipAxes(SourceAxes);
  ViewSource();
}


/****************************************
 *
 * ShowHelp
 *
 ***************************************/
void EditorConsole::ShowHelp()
{
  
  ifuShowText("SEGMENTATION EDITOR MODULE:\n \
This module allows you to manipulate the output\n \
of the segmentation module to produce labeled volumes.\n \
To begin the editing process, you must choose the source,\n \
segmentation, and tree (scale information) to use.  The\n \
\"Source data\" is the original image data on which the\n \
segmenation is based.  The \"Segmented\" data is the base\n \
labeled image data produced by the watershed segmentation\n \
algorithm.  The \"Tree File Name\" is the tree of merges\n \
calculated by the watershed segmentation algorithm that\n \
is used to visualize the output at different scales.\n \
\n \
Fill out the information for the Segmented and Source\n \
data and press the \"Load Images\" button.  You can\n \
save this session for future use by selecting the\n \
\"Save Session\" button from the console.  For future\n \
sessions, simply select \"Load Session\" and specify\n \
the .ws file.\n \
\n \
SEGMENTATION EDITOR CONSOLE WINDOW\n \
\n \
This section describes the controls you will see in the\n \
Editor window.\n \
\n \
INTRODUCTION\n \
\n \
The purpose of the editor is to facilitate a construction\n \
of a labeled image using a combination of automatic and\n \
hand labeling of voxels.  Think of the regions produced\n \
by the watershed filter as pieces of a 3D puzzle which\n \
you can assemble into a labeled image, which is then\n \
fine-tuned or augmented by hand painting of regions.\n \
\n \
The controls of the editor are described in detail below,\n \
but the basic idea is this: Regions in the \"Segmented\"\n \
window can be selected with the mouse and their\n \
corresponding pixels labeled (or unlabeled) in the image\n \
in the \"Source\" window.  In addition to painting by region\n \
(3D), you can paint by hand (2D) to fill in gaps or erase\n \
unwanted areas.  At any point the resulting 3D volume can be\n \
rendered in a separate window.  The labeled image is stored\n \
as a binary volume which can be saved to disk and loaded\n \
back into the editor.\n \
\n\n \
Please see README for more information\n\n");
/*
INTERACTING WITH WINDOWS\n \
\n \
There are three data windows, numbered below from the left\n \
side of the screen.\n \
Segmented Window -- This window displays the segmentation\n \
data at whichever scale is selected via the \"Scale\" slider.\n \
Left-Click will select one of the colored regions (after\n \
selecting it will highlight in white).  Shift-Left-Click\n \
allows you to select multiple regions at one time.\n \
\n \
Source Window -- This window displays the color data from\n \
which the segmentation was constructed.  The binary mask of \n \
the segmentation is overlayed in blue on this window. Left-Click\n \
freehand draws pixels into the binary volume.  Right-Click\n \
freehand erases pixels from the binary volume.  The size of \n \
the paintbrush is controlled by the \"Paint Radius\" slider.\n \
Middle-Click Selects the region at that location in the\n \
Segmented Window.  Shift-Middle-Click allows you to select \n \
multiple regions in the Segmentation Window.\n \
\n \
Binary Window --  This window displays the segmentation that \n \
you are constructing.  No meaningful interaction can be had \n \
or is necessary with this window.\n \
\n \
BUTTONS AND SLIDERS\n \
\n \
Save Session: Allows you to save the image information for \n \
future use.\n \
\n \
Load Session: Allows you to reload the images from a previous \n \
session.\n \
\n \
Save Binary:  Allows you to write the binary label volume \n \
you are creating to disk.  You can use save to remember \n \
the state of the volume and undo changes back to that saved \n \
state.  The output of this tool is a binary volume with\n \
1's at pixel locations inside the structure and 0's at \n \
pixel locations outside the structure.\n \
\n \
Load Binary: Loads a binary volume, replacing current volume.\n \
\n \
Randomize Colors: Re-colorizes the regions in the segmentation \n \
window.\n \
\n \
Render/Update 3D Image: Generate a three dimensional rendering \n \
of the surface of the segmentation.  This spawns a new window \n \
which can be raised/lowered and resized.  The rendering can be \n \
manipulated in this window.\n \
\n \
zoom: Change the scale at which you view the data.\n \
\n \
Toggle Zoom: Turns zooming on/off allowing you to view the \n \
data at full resolution.\n \
\n \
Add Selected Region: Adds the highlighed region(s) in the \n \
Segmentated Window to the binary volume.\n \
\n \
Subtract Selected Region: Subtracts the highlighed region(s) \n \
\n \
in the Segmentated Window from the binary volume.\n \
\n \
Merge Selected (when selecting a single region in the \n \
Segmentated Window): This function merges regions until the \n \
next merge with the selected region occurs.  You can move \n \
up the merge hierarchy in a non-linear fashion with this \n \
button.\n \
\n \
Undo Last Merge:  Reverts the merge hierarchy to the state \n \
before the last merge operation.\n \
\n \
Scale:  This slider moves the threshold value at which you\n \
are viewing the segmentation up or down.\n \
\n \
Scale units:  Changes the resolution of the Scale slider to \n \
move in finer or coarser increments.\n \
\n \
Slice number:  Changes the 2D slice of the volume that you \n \
are currently viewing.\n \
\n \
Clear All:  Erases the entire binary volume.\n \
\n \
Show Help: Display a small help window.\n \
\n \
Toggle Overlay: Toggles the display of the binary volume \n \
overlay in the Source Window.\n \
\n \
Paint Radius:  Controls the size of the circle used to \n \
free-hand paint/erase pixels in the binary volume.\n \
\n \
\n \
Known Bugs 7/26/02\n \
\n \
Re-colorizing when deselecting multiple regions.  When multiple \n \
regions are deselected all at once, the distinct regions will \n \
sometimes remain the same color.  You can press the Randomize \n \
Colors button to recolor these regions.\n \
");  
  */
}
