/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SegmenterConsole.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

                    
#include <SegmenterConsole.h>
#include <FL/fl_file_chooser.H>
#include <FL/fl_ask.H>
#include <stdio.h>
#include <float.h>

#include <itkImageFileWriter.h>
#include <fstream>


/************************************
 *
 *  Constructor
 *
 ***********************************/
SegmenterConsole
::SegmenterConsole()
{
  // initialize viewers
  m_InputViewer = new InputImageViewerType;
  m_InputViewer->SetLabel("Input Image");
  m_InputViewer->ClickSelectCallBack( ClickSelectCallback, (void *)this );
  m_InputViewer->ViewDetails(false);

  m_SeedViewer = new SeedViewerType;
  m_SeedViewer->SetLabel("Seed Image");
  m_SeedViewer->ViewDetails(false);

  m_SpeedViewer = new SpeedViewerType;
  m_SpeedViewer->SetLabel("Speed Image");
  m_SpeedViewer->ViewDetails(false);

  // initialize gui
  diffusionIterations->value(0);
  diffusionConductance->value(1.0);
  diffusionTimeStep->value(0.125);

  isosurface->value(0.0);

  seedX->value(60);
  seedY->value(116);
  distance->value(5);

  // these should match the initializations for m_thresholdSegmentation
  // in SegmenterConsoleBase.cxx
  curvature->value(1.0);
  propagation->value(1.0);
  edge->value(1.0);
  maxIterations->value(100);
  RMSError->value(0.02);
  reverseExpansionDir->value(0);
  threshIterations->value(0);
  threshConductance->value(0.5);
  threshTimeStep->value(0.100);

  updateIterations->minimum(1);
  updateIterations->maximum(500);
  updateIterations->step(1);
  updateIterations->value(1);

  minThresh->value(0.0);
  minThresh2->value(0.0);
  maxThresh->value(0.0);
  maxThresh2->value(0.0);

  // initialize misc data members
  prevMinThresh = 0.0;
  prevMaxThresh = 0.0;
  guessRadius = 2;
  m_filterCase = 1;
  m_drawCase = 1;
  min = 0.0;
  max = 0.0;  
  loadingSession = false;
  inputFilename = "";
  seedFilename = "";


  // set up the segmentation observer
  typedef itk::SimpleMemberCommand< SegmenterConsole > SimpleCommandType;

  SimpleCommandType::Pointer iterationCommand = SimpleCommandType::New();
  iterationCommand->SetCallbackFunction( this, 
      & SegmenterConsole::UpdateViewerAfterIteration );

  m_thresholdSegmentation->AddObserver( itk::IterationEvent(), iterationCommand );     

  ShowInputButton->Observe( m_InputCaster.GetPointer() );
  DiffuseButton->Observe( m_Curvature.GetPointer() );
  ViewSeedButton->Observe( m_SeedCaster.GetPointer() );
  SegmentButton->Observe( m_thresholdSegmentation.GetPointer() );


  // help text
  helpText->value("ThresholdSegmentationLevelSetFltkGui\n-----------------------------------------------------------\nThis application allows you to do segmentation using the\nThresholdSegmentationLevelSetImageFilter on a 2D\nimage.\n\nTo begin, load in a 2D image (preferably in the Meta Image\nformat) by clicking the \"Load Input Image\" button.\n");
}

/************************************
 *
 *  Destructor
 *
 ***********************************/
SegmenterConsole
::~SegmenterConsole()
{

}

/************************************
 *
 *  Load
 *
 ***********************************/
void
SegmenterConsole
::Load( void )
{
  // create a new reader every time in case they are reading in a new image
  m_Reader = ReaderType::New();
  if(loadingSession) {
    try 
    {
      SegmenterConsoleBase::Load( inputFilename.c_str());
    }
    catch( ... ) 
    {
      return;
    }
  }
  else {
    const char* filename = fl_file_chooser("Image filename","*.*","");
    if( !filename )
    {
      return;
    }
    try 
    {
      SegmenterConsoleBase::Load( filename );
    }
    catch( ... ) 
    {
    return;
    }
  }
  
  // activate the other gui options
  ShowInputButton->activate();

  this->InitializeGuessImages();

  // calculate the min max and set the thresholds to be 
  // close to the min+max / 2
  m_minMax->SetImage(m_Reader->GetOutput());
  try {
    m_minMax->Compute();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }  
  min = m_minMax->GetMinimum();
  max = m_minMax->GetMaximum();

  minThresh->value((min+max)/2);
  minThresh2->value((min+max)/2);
  maxThresh->value((min+max)/2+0.5);
  maxThresh2->value((min+max)/2+0.5);

  minThresh->minimum(min);
  minThresh->maximum(max);

  maxThresh->minimum(min);
  maxThresh->maximum(max);

  ShowInputImage();
}

/***********************************
 *
 * InitializeGuessImages
 *
 **********************************/
void SegmenterConsole::InitializeGuessImages()
{
  // Initialize images used

  // m_thresh is a 3D image buffer which stores
  // the threshold points as an overlay to the
  // viewer

  // m_image is a 3D image buffer which stores
  // the guess of the user to be the overlay
  // in the viewer
  m_start1[0] = 0;
  m_start1[1] = 0;
  m_start1[2] = 0;

  m_size1[0] = ((m_Reader->GetOutput())->GetLargestPossibleRegion()).GetSize()[0];
  m_size1[1] = ((m_Reader->GetOutput())->GetLargestPossibleRegion()).GetSize()[1];
  m_size1[2] = 1;

  m_region1.SetSize( m_size1 );
  m_region1.SetIndex( m_start1 );
  m_image->SetRegions( m_region1 );
  m_thresh->SetRegions( m_region1 );

  m_image->Allocate();
  m_thresh->Allocate();

  // fill both buffers with 0's (black in itkColorTable.h)
  WriteImageType3D::PixelType  initialValue1 = 0;
  m_image->FillBuffer( initialValue1 );

  m_thresh->FillBuffer( initialValue1 );

  // m_guessImage is a 2D image that is modifed
  // when the user draws a guess image and can be
  // passed to the threshold filter for input

  m_start2[0] = 0;
  m_start2[1] = 0;

  m_size2[0] = ((m_Reader->GetOutput())->GetLargestPossibleRegion()).GetSize()[0];
  m_size2[1] = ((m_Reader->GetOutput())->GetLargestPossibleRegion()).GetSize()[1];

  m_region2.SetSize( m_size2 );
  m_region2.SetIndex( m_start2 );
  m_guessImage->SetRegions( m_region2 );
  m_guessImage->Allocate();

  // initialize buffer
  WriteImageType::PixelType  initialValue2 = 255;
  m_guessImage->FillBuffer( initialValue2 );

}
   
/************************************
 *
 *  Show
 *
 ***********************************/
void
SegmenterConsole
::Show( void )
{
  consoleWindow->show();
}

/************************************
 *
 *  Hide
 *
 ***********************************/
void
SegmenterConsole
::Hide( void )
{
  consoleWindow->hide();
  helpWin->hide();
  m_InputViewer->Hide();
  m_SeedViewer->Hide();
  m_SpeedViewer->Hide();

}

/************************************
 *
 *  Quit
 *
 ***********************************/
void
SegmenterConsole
::Quit( void )
{
  Hide();
}

/***********************************
 *
 * ShowInputImage
 *
 **********************************/
void 
SegmenterConsole::ShowInputImage(void)
{
  m_InputCaster->SetInput(m_Reader->GetOutput());
  try 
  {
    m_InputCaster->Update();
  }
  catch( ... ) 
  {
    return;
  }
  m_InputViewer->SetImage(m_InputCaster->GetOutput());
  m_InputViewer->SetOverlay( m_image );
  m_InputViewer->Show();

  //this->SmoothImage();
  diffusionGroup->activate();
  radioGroup->activate();
  Case1Group->activate();
  endControlsGroup->activate();
  save->deactivate();

  SmoothImage();
  
}

/***********************************
 *
 * SmoothImage
 *
 **********************************/
void
SegmenterConsole::SmoothImage()
{
  m_Curvature->SetNumberOfIterations((unsigned int)diffusionIterations->value());
  m_Curvature->SetConductanceParameter(diffusionConductance->value());
  m_Curvature->SetTimeStep(diffusionTimeStep->value());
  m_Curvature->SetInput(m_Reader->GetOutput());

  m_InputCaster->SetInput(m_Curvature->GetOutput());
  // this update must be in here in order to get the
  // curvature to update with changes in params
  try 
  {
    m_InputCaster->Update();
  }
  catch( ... ) 
  {
    return;
  }

  // calculate the min and max to set threshold boundaries
  m_minMax->SetImage(m_Curvature->GetOutput());
  try {
    m_minMax->Compute();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }  
  min = m_minMax->GetMinimum();
  max = m_minMax->GetMaximum();

  minThresh->minimum(min);
  minThresh->maximum(max);

  maxThresh->minimum(min);
  maxThresh->maximum(max);

  // show curvature image
  m_InputViewer->SetImage(m_InputCaster->GetOutput());
  m_InputViewer->SetOverlay( m_image );
  m_InputViewer->Show();
  
}

/***********************************
 *
 * ShowSpeedImage
 *
 ***********************************/
void
SegmenterConsole::ShowSpeedImage()
{
  if(maxThresh2->value() > minThresh2->value()) {
    SmoothImage();

    m_thresholdSegmentation->SetUpperThreshold( maxThresh2->value() );
    m_thresholdSegmentation->SetLowerThreshold( minThresh2->value() );
    m_thresholdSegmentation->SetCurvatureScaling( curvature->value() );
    m_thresholdSegmentation->SetPropagationScaling( propagation->value() );
    m_thresholdSegmentation->SetEdgeWeight( edge->value() );
    m_thresholdSegmentation->SetMaximumRMSError( RMSError->value() );
    m_thresholdSegmentation->SetMaximumIterations( 0 );

    m_thresholdSegmentation->SetSmoothingIterations( (unsigned int)threshIterations->value() );
    m_thresholdSegmentation->SetSmoothingConductance( threshConductance->value() );
    m_thresholdSegmentation->SetSmoothingTimeStep( threshTimeStep->value() );
    
    m_thresholdSegmentation->SetInput(m_Curvature->GetOutput() ); 
    m_thresholdSegmentation->SetIsoSurfaceValue(0.0);
    
    try {
      m_thresholdSegmentation->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
    }
    m_minMax->SetImage(const_cast<InputImageType *>(m_thresholdSegmentation->GetSpeedImage()));
    try {
      m_minMax->Compute();
    }
    catch( itk::ExceptionObject & excep )
    {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
    }  
    float a_max = m_minMax->GetMaximum();
    float a_min = m_minMax->GetMinimum();
    
    m_speedThresh->SetInput(const_cast<InputImageType *>(m_thresholdSegmentation->GetSpeedImage()));
    m_speedThresh->SetOutsideValue(6);
    m_speedThresh->SetInsideValue(1);
    m_speedThresh->SetUpperThreshold(a_max);
    m_speedThresh->SetLowerThreshold(0.0);
    
    m_SpeedCaster->SetInput(m_speedThresh->GetOutput());
    
    try {
      m_SpeedCaster->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
    }

    // update speed image
    m_SpeedViewer->SetImage(m_InputCaster->GetOutput());
    m_SpeedViewer->SetOverlay(m_SpeedCaster->GetOutput());
    m_SpeedViewer->SetOverlayOpacity(0.5);
    m_SpeedViewer->Show();
  }
  else {
    fl_alert("Error! Please set appropriate thresholds.", "OK");
  }

}

/***********************************
 *
 * ShowSegmentedImage
 *
 **********************************/
void 
SegmenterConsole::ShowSegmentedImage(void)
{
  m_thresholdSegmentation->SetUpperThreshold( maxThresh2->value() );
  m_thresholdSegmentation->SetLowerThreshold( minThresh2->value() );
  m_thresholdSegmentation->SetCurvatureScaling( curvature->value() );
  m_thresholdSegmentation->SetPropagationScaling( propagation->value() );
  m_thresholdSegmentation->SetEdgeWeight( edge->value() );
  m_thresholdSegmentation->SetMaximumRMSError( RMSError->value() );
  m_thresholdSegmentation->SetMaximumIterations( (unsigned int)maxIterations->value() );
  
  m_thresholdSegmentation->SetSmoothingIterations( (unsigned int)threshIterations->value() );
  m_thresholdSegmentation->SetSmoothingConductance( threshConductance->value() );
  m_thresholdSegmentation->SetSmoothingTimeStep( threshTimeStep->value() );
  
  if(reverseExpansionDir->value()) {
    m_thresholdSegmentation->ReverseExpansionDirectionOn();
  }
  else {
    m_thresholdSegmentation->ReverseExpansionDirectionOff();
  }

  m_thresholder->SetInput(m_thresholdSegmentation->GetOutput());
  m_thresholder->SetOutsideValue(0);
  m_thresholder->SetInsideValue(1);
  m_thresholder->SetUpperThreshold(0.5);
  m_thresholder->SetLowerThreshold(-0.5);

  m_SegmentedCaster->SetInput(m_thresholder->GetOutput());
  
  m_thresholdSegmentation->SetInput(m_Curvature->GetOutput() ); 
  m_thresholdSegmentation->SetIsoSurfaceValue(0.0);
  m_thresholdSegmentation->SetMaximumIterations(0);
  try {
    m_SegmentedCaster->Update();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }
  
  m_thresholdSegmentation->SetMaximumIterations( (unsigned int)maxIterations->value() );

  switch(m_filterCase) {
  case 1:
    if(strcmp(m_SeedReader->GetFileName(), "") == 0) {
      fl_alert("Error! Please read in a Seed Image.", "OK");
      m_thresholdSegmentation->Modified();
      return;
      break;
    }
    m_thresholdSegmentation->SetInput(m_SeedReader->GetOutput() ); // LEAVE
    m_thresholdSegmentation->SetIsoSurfaceValue(isosurface->value());
    try {
      m_thresholdSegmentation->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
    }
    break;
  case 2: 
    m_thresholdSegmentation->SetInput( m_fastMarching->GetOutput() );

    m_seedPosition[0] = (unsigned long)seedX->value();
    m_seedPosition[1] = (unsigned long)seedY->value();
    
    m_node.SetValue( -distance->value() );
    m_node.SetIndex( m_seedPosition );
    
    m_seeds->Initialize();
    m_seeds->InsertElement( 0, m_node );
    m_fastMarching->SetTrialPoints( m_seeds );
    
    try {
      m_fastMarching->SetOutputSize(m_Curvature->GetOutput()->GetBufferedRegion().GetSize() );
      m_thresholdSegmentation->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
    }

    break;
  case 3:
    m_thresholdSegmentation->SetInput( m_guessImage );
    m_thresholdSegmentation->SetIsoSurfaceValue(127.5);
    try {
      m_thresholdSegmentation->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
    }
    break;
  default:
    break;
  }

  try {
    m_SegmentedCaster->Update();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }

  // update segmented image when done
  m_InputViewer->SetOverlay(m_SegmentedCaster->GetOutput());
  m_InputViewer->SetOverlayOpacity(1.0);
  m_InputViewer->Update();
  Fl::check();

  // activate segmented saving options
  save->activate();
  binarySave->activate();
}

/***********************************
 *
 * SwitchCase
 *
 **********************************/
void 
SegmenterConsole::SwitchCase(int c)
{
  this->ClearThresh();
  
  // uncheck option to be in threshold drawing mode

  m_filterCase = c;
  if(m_filterCase == 1) { // loading seed image
    Case1Group->activate();
    Case2Group->deactivate();
    Case3Group->deactivate();
    m_thresholdSegmentation->Modified();
  }
  else if(m_filterCase == 2) { // specifying coords for FastMarching
    Case2Group->activate();
    Case1Group->deactivate();
    Case3Group->deactivate();
    m_thresholdSegmentation->Modified();
    calcThresh->value(0);
    ToggleThresholdGuess();
    m_drawCase = 2;
  }
  else if(m_filterCase == 3) { // draw initial guess
    Case3Group->activate();
    Case1Group->deactivate();
    Case2Group->deactivate();
    m_thresholdSegmentation->Modified();
    calcThresh->value(0);
    ToggleThresholdGuess();
    m_drawCase = 3;
  }
}


/**********************************
 *
 * LoadSeedImage
 *
 **********************************/
void 
SegmenterConsole::LoadSeedImage()
{
  m_SeedReader = ReaderType::New();

  if(loadingSession) {
    m_SeedReader->SetFileName(seedFilename.c_str());
  }
  else {
    const char * filename = fl_file_chooser("Image filename","*.*","");
    if( !filename )
    {
      return;
    }
    m_SeedReader->SetFileName(filename);
  }

  // this update must be here, otherwise the
  // isosurface value doesn't get calculated
  try {
    m_SeedReader->Update();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }

  // calculate the min and max to determine a good isosurface
  m_minMax->SetImage(m_SeedReader->GetOutput());
  try {
    m_minMax->Compute();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }  
  float m_min = m_minMax->GetMinimum();
  float m_max = m_minMax->GetMaximum();

  isosurface->value((m_min+m_max)/2.0);

  ShowSeedImage();
}

/**********************************
 *
 * ShowSeedImage
 *
 **********************************/
void 
SegmenterConsole::ShowSeedImage()
{
  m_SeedCaster->SetInput(m_SeedReader->GetOutput());
  try {
    m_SeedCaster->Update();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }

  // view the seed image
  m_SeedViewer->SetImage( m_SeedCaster->GetOutput());
  m_SeedViewer->Show();
}

/**********************************
 *
 * GetClickPoints
 *
 **********************************/
void SegmenterConsole::GetClickPoints(float x, float y)
{
  WriteImageType3D::IndexType pixelIndex1;
  WriteImageType::IndexType pixelIndex2;

  switch(m_drawCase) {
  case 1: // drawing thresholds
    m_InputViewer->SetOverlay( m_thresh );
    m_InputViewer->SetOverlayOpacity(0.3);
  
    pixelIndex1[2] = 0;

    for(int i=-2; i< 2; i++) {
      for(int j=-2; j< 2; j++) {
      pixelIndex1[0] = (int)x+i;
      pixelIndex1[1] = (int)y+j;
      m_thresh->SetPixel(pixelIndex1, 5);
      }
    }
    m_InputViewer->Update();
    break;
  case 2: // setting x and y seed points
    seedX->value(x);
    seedY->value(y);
    m_thresholdSegmentation->Modified();
    break;
  case 3: // drawing initial segmentation guess
    m_InputViewer->SetOverlay( m_image );
    m_InputViewer->SetOverlayOpacity(0.3);

    pixelIndex1[2] = 0;

    for(int i=-guessRadius; i< guessRadius; i++) {
      for(int j=-guessRadius; j< guessRadius; j++) {
      pixelIndex1[0] = (int)x+i;
      pixelIndex1[1] = (int)y+j;
      m_image->SetPixel(pixelIndex1, 2);
    
      pixelIndex2[0] = (int)x+i;
      pixelIndex2[1] = (int)y+j;
      m_guessImage->SetPixel(pixelIndex2, 0);
      }
    }
    m_InputViewer->Update();
    break;
  default:
    break;
  }
}

/**********************************
 *
 * ClickSelectCallback
 *
 **********************************/
void SegmenterConsole::ClickSelectCallback(float x, float y, float z, float value, void * args )
{
  SegmenterConsole* self = static_cast<SegmenterConsole *>( args );
  self->GetClickPoints(x, y);
}

/**********************************
 *
 * ChangePaintRadius
 *
 **********************************/
void SegmenterConsole::ChangePaintRadius(float r)
{
  guessRadius = (int)r;
}

/**********************************
 *
 * ClearGuess
 *
 **********************************/
void SegmenterConsole::ClearGuess()
{
  WriteImageType::PixelType  initialValue2; 

  // clear guessImage
  initialValue2 = 255;
  m_guessImage->FillBuffer( initialValue2 );
  m_image->FillBuffer( 0 );
  m_InputViewer->Update();

}

/**********************************
 *
 * ClearThresh
 *
 **********************************/
void SegmenterConsole::ClearThresh()
{
  m_InputViewer->ClearClickedPoints();
  m_thresh->FillBuffer( 0 );
  m_InputViewer->Update();
}

/**********************************
 *
 * UpdateViewerAfterIteration
 *
 **********************************/
void SegmenterConsole::UpdateViewerAfterIteration()
{
  
  static unsigned int iterationCounter = 0;

  if( (iterationCounter%((int)updateIterations->value()) == 0) && (iterationCounter != 0) )
    {
    //if( (iterationCounter != 0) ) {
    
    // Move the pixel container and image information of the image we are working
    // on into a temporary image to  use as the input to the mini-pipeline.  This
    // avoids a complete copy of the image.
    InputImageType::Pointer tmp = InputImageType::New();
    tmp->SetRequestedRegion( m_thresholdSegmentation->GetOutput()->GetRequestedRegion() );
    tmp->SetBufferedRegion( m_thresholdSegmentation->GetOutput()->GetBufferedRegion() );
    tmp->SetLargestPossibleRegion( m_thresholdSegmentation->GetOutput()->GetLargestPossibleRegion() );
    tmp->SetPixelContainer( m_thresholdSegmentation->GetOutput()->GetPixelContainer() );
    tmp->CopyInformation( m_thresholdSegmentation->GetOutput() );
    
    // update overlay
    m_thresholder->Modified();
    m_thresholder->SetInput(tmp);
    m_SegmentedCaster->SetInput(m_thresholder->GetOutput());

    try {
      m_SegmentedCaster->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
      std::cerr << "Exception caught !" << std::endl;
      std::cerr << excep << std::endl;
    }

    // update the viewer with the current segmentation
    m_InputViewer->SetOverlay(m_SegmentedCaster->GetOutput());
    m_InputViewer->SetOverlayOpacity(1.0);
    m_InputViewer->Update();
    Fl::check();

    elapsedIterations->value(m_thresholdSegmentation->GetElapsedIterations());
    lastRMSChange->value(m_thresholdSegmentation->GetRMSChange());

  }
  iterationCounter++;
  
}

/**********************************
 *
 * ToggleThresholdGuess
 *
 **********************************/
void SegmenterConsole::ToggleThresholdGuess()
{
  this->ClearThresh();

  if(calcThresh->value()) {
    m_drawCase = 1;
  }
  else {
    if(m_filterCase == 2)
      m_drawCase = 2;
    if(m_filterCase == 3)
      m_drawCase = 3;
  }
}

/**********************************
 *
 * CalculateThresholds
 *
 **********************************/
void SegmenterConsole::CalculateThresholds()
{
  const int numPoints = (m_InputViewer->GetSliceViewPointer())->numClickedPointsStored();
  float area_max = min;
  float area_min = max;

  for(int i=numPoints-1; i>=0; i--)
    {
    ClickPoint point;
    if( (m_InputViewer->GetSliceViewPointer())->getClickedPoint(i,point) )
      {
      const float value = point.value;
      if(value > area_max)
        area_max = value;
      if(value < area_min)
        area_min = value;

      }
    }

  // the order matters because if the minimum is changed first
  // and is set to higher than the max, the user will get an error
  if(maxThresh2->value() <= area_min) {
    ChangeMaxThresh(area_max);
    ChangeMinThresh(area_min);
  }
  else {
    ChangeMinThresh(area_min);
    ChangeMaxThresh(area_max);
  }
  ClearThresh();
}

/*************************
 *
 * SaveSegmented
 *
 ************************/
void SegmenterConsole::SaveSegmented()
{
  const char * filename = fl_file_chooser("Segmented filename","*.mh[da]","");
  if( !filename )
  {
    return;
  }

  m_segmentWriter->SetFileName(filename);
  m_segmentWriter->SetInput(m_thresholdSegmentation->GetOutput());

  try {
    m_segmentWriter->Update();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  } 

}


/*************************
 *
 * SaveBinaryMask
 *
 ************************/
void SegmenterConsole::SaveBinaryMask()
{
  const char * filename = fl_file_chooser("BinaryMask filename","*.mh[da]","");
  if( !filename )
  {
    return;
  }

  m_minMax->SetImage(m_thresholdSegmentation->GetOutput());
  try {
    m_minMax->Compute();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }  
  float a_max = m_minMax->GetMaximum();
  float a_min = m_minMax->GetMinimum();
  
  m_maskThresh->SetInput(m_thresholdSegmentation->GetOutput());
  m_maskThresh->SetOutsideValue(1);
  m_maskThresh->SetInsideValue(0);
  m_maskThresh->SetUpperThreshold(0.0);
  m_maskThresh->SetLowerThreshold(a_min);
  
  m_maskWriter->SetFileName(filename);
  m_maskWriter->SetInput(m_maskThresh->GetOutput());
  
  try {
    m_maskWriter->Update();
  }
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  } 
  
}

/****************************
 *
 * ChangeMinThresh
 *
 ****************************/
void SegmenterConsole::ChangeMinThresh(float val)
{
  // check bounds
  if((val > max) || (val < min)) {
    fl_alert("Error! Value out of bounds. Reverting to previous value", "OK");
    val = prevMinThresh;
  }

  // change slider
  minThresh->value(val);

  // change entry box
  minThresh2->value(val);
  prevMinThresh = val;
  ShowSpeedImage();
  m_thresholdSegmentation->Modified();

}

/****************************
 *
 * ChangeMaxThresh
 *
 ****************************/
void SegmenterConsole::ChangeMaxThresh(float val)
{
  // check bounds
  if((val > max) || (val < min)) {
    fl_alert("Error! Value out of bounds. Reverting to previous value", "OK");
    std::cout << "Max Thresh: " << prevMaxThresh << " to " << val << std::endl;
    val = prevMaxThresh;
  }

  // change slider
  maxThresh->value(val);

  // change entry box
  maxThresh2->value(val);

  prevMaxThresh = val;
  ShowSpeedImage();
  m_thresholdSegmentation->Modified();

}

/*****************************
 *
 * SetThresholdFilterToModified
 *
 *****************************/
void SegmenterConsole::SetThresholdFilterToModified()
{
  // this is mainly used when parameters are changed
  m_thresholdSegmentation->Modified();
}

/****************************
 *
 * SaveSession
 *
 ****************************/
void SegmenterConsole::SaveSession()
{
  const char * filename = fl_file_chooser("Session filename","*.ses","");
  if( !filename )
  {
    return;
  }
  // save all of the parameters
  std::ofstream out;
  out.open(filename);
  out << m_Reader->GetFileName() << "\n";
  out << diffusionIterations->value() << " ";
  out << diffusionConductance->value() << " ";
  out << diffusionTimeStep->value() << " ";

  out << m_filterCase << " ";

  if(calcThresh->value()) {
    out << "1 ";
  }
  else {
    out << "0 ";
  }

  if(strcmp(m_SeedReader->GetFileName(), "") == 0) {
    out << "\nBLANK\n";
  }
  else {
    out << "\n" << m_SeedReader->GetFileName() << "\n";
  }
  out << isosurface->value() << " ";

  out << seedX->value() << " ";
  out << seedY->value() << " ";
  out << distance->value() << " ";

  out << paintRadius->value() << " ";

  out << minThresh2->value() << " ";
  out << maxThresh2->value() << " ";
  out << curvature->value() << " ";
  out << propagation->value() << " ";
  out << edge->value() << " ";
  out << maxIterations->value() << " ";
  out << RMSError->value() << " ";
  
  out << threshIterations->value() << " ";
  out << threshConductance->value() << " ";
  out << threshTimeStep->value() << " ";
  
  if(reverseExpansionDir->value()) {
    out << 1 << " ";
  }
  else {
    out << 0 << " ";
  }
  out << updateIterations->value() << " ";

  out.close();
}

/****************************
 *
 * LoadSession
 *
 ****************************/
void SegmenterConsole::LoadSession()
{
  const char * filename = fl_file_chooser("Session filename","*.ses","");
  if( !filename )
  {
    return;
  }
  
  loadingSession = true;

  // read in the parameters
  std::ifstream in;
  in.open(filename);

  std::string buffer;
  char buffer2[256];
  int intVal;
  float floatVal;
  float min_thresh;
  float max_thresh;

  in.getline(buffer2, 256); // input image filename
  inputFilename = buffer2;
  Load();
  ShowInputImage();

  in >> intVal; // diffusion
  diffusionIterations->value(intVal);
  in >> floatVal;
  diffusionConductance->value(floatVal);
  in >> floatVal;
  diffusionTimeStep->value(floatVal);

  SmoothImage();

  in >> intVal; // segmentation case
  m_filterCase = intVal;
  SwitchCase( intVal );
  if(intVal == 1) {
    radio1->value(1);
    radio2->value(0);
    radio3->value(0);
  }
  else if(intVal == 2) {
    radio2->value(1);
    radio1->value(0);
    radio3->value(0);
  }
  else if(intVal ==3) {
    radio3->value(1);
    radio1->value(0);
    radio2->value(0);
  }

  in >> intVal; // drawing threshold mode
  calcThresh->value(intVal);
  ToggleThresholdGuess();

  in.getline(buffer2, 256); // return
  in.getline(buffer2, 256); // seedreader


  if(m_filterCase == 1 && strcmp(buffer2,"BLANK")!= 0) {
    seedFilename = buffer2;
    LoadSeedImage();
  }

  in >> floatVal;
  isosurface->value(floatVal);

  in >> floatVal; // coords for generating guesss
  seedX->value( floatVal );
  in >> floatVal;
  seedY->value( floatVal );
  in >> floatVal;
  distance->value( floatVal );
  
  in >> intVal; //paint radius
  paintRadius->value( intVal );
  
  in >> min_thresh; // segmentation parameters
  in >> max_thresh;

  minThresh->value(min_thresh);
  minThresh2->value(min_thresh);
  prevMinThresh = min_thresh;

  maxThresh->value(max_thresh);
  maxThresh2->value(max_thresh);
  prevMaxThresh = max_thresh;

  in >> floatVal;
  curvature->value(floatVal);

  in >> floatVal;
  propagation->value(floatVal);

  in >> floatVal;
  edge->value(floatVal);

  in >> intVal;
  maxIterations->value(intVal);
  in >> floatVal;
  RMSError->value(floatVal);

  in >> floatVal;
  threshIterations->value((int)floatVal);

  in >> floatVal;
  threshConductance->value(floatVal);

  in >> floatVal;
  threshTimeStep->value(floatVal);

  in >> intVal;
  if(intVal == 1) {
    reverseExpansionDir->value(1);
  }
  else {
    reverseExpansionDir->value(0);
  }
  
  ShowSpeedImage();

  in >> intVal; // update iteration value
  updateIterations->value(intVal);
  
  in.close();
  loadingSession = false;
}
