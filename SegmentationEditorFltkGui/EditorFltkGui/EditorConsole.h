/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    EditorConsole.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef EDITORCONSOLE
#define EDITORCONSOLE


#include <EditorConsoleGUI.h>


class EditorConsole : public EditorConsoleGUI {

public:

  EditorConsole();
  virtual ~EditorConsole();

  virtual bool StartEditor();
  virtual void Show(void);
  virtual void Hide(void);
  virtual void Quit(void);

  virtual void ViewImages(void);
  virtual void ViewSegmented(void);
  virtual void ViewSource(void);
  virtual void ViewBinary(void);
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
  
  virtual void SpecifySegmentedFile();
  virtual void SpecifySourceFile();

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
  virtual void FlipSource(int);

  double local_max(double, double);
  double local_min(double, double);
  double round(double min, double max, double step, double val);

private:
  double previousScale;
  double scaleLevel;
  double default_magnification;
  double basemergeResolution;
  int window_X;
  int window_Y;
  int console_X;
  int console_Y;
  bool first_render;

  bool SourceAxes[3];
  
};



#endif
