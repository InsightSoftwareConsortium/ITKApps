


#include "LiverTumorSegmentation.h"

#include "FL/Fl_File_Chooser.H"

#include "vtkImageShiftScale.h"

#include "ClickedPointEvent.h"

#include "vtkImageBlend.h"

static char *LiverTumorSegmentationModuleNames[] = { "Threshold Module",
  "Threshold Level Set Module", "Confidence Connected Module", 
  "Connected Threshold Module", "Isolated Connected Module", 
  "Fast Marching Module", "Geodesic Active Contour Module", "Watershed Module"
    }; 

LiverTumorSegmentation::LiverTumorSegmentation()
  {
  m_AxialViewer.SetOrientation(    ISIS::ImageSliceViewer::Axial    );
  m_CoronalViewer.SetOrientation(  ISIS::ImageSliceViewer::Coronal  );
  m_SaggitalViewer.SetOrientation( ISIS::ImageSliceViewer::Saggital );
  
  m_ShiftScaleImageFilter = vtkImageShiftScale::New();
  
  m_ShiftScaleImageFilter->SetInput(  m_ITK2VTKAdaptor->GetOutput() );
  m_ShiftScaleImageFilter->SetOutputScalarTypeToUnsignedChar();
  m_ShiftScaleImageFilter->ClampOverflowOn();
  
  m_AxialViewerCommand = itk::SimpleMemberCommand<LiverTumorSegmentation>::New();
  m_AxialViewerCommand->SetCallbackFunction(this, &LiverTumorSegmentation::ProcessAxialViewInteraction);
  m_AxialViewer.AddObserver(ISIS::ClickedPointEvent(), m_AxialViewerCommand);
  
  m_CoronalViewerCommand = itk::SimpleMemberCommand<LiverTumorSegmentation>::New();
  m_CoronalViewerCommand->SetCallbackFunction(this, &LiverTumorSegmentation::ProcessCoronalViewInteraction);
  m_CoronalViewer.AddObserver(ISIS::ClickedPointEvent(), m_CoronalViewerCommand);
  
  m_SaggitalViewerCommand = itk::SimpleMemberCommand<LiverTumorSegmentation>::New();
  m_SaggitalViewerCommand->SetCallbackFunction(this, &LiverTumorSegmentation::ProcessSaggitalViewInteraction);
  m_SaggitalViewer.AddObserver(ISIS::ClickedPointEvent(), m_SaggitalViewerCommand);
  
  m_DicomReaderCommand = itk::SimpleMemberCommand<LiverTumorSegmentation>::New();
  m_DicomReaderCommand->SetCallbackFunction(this, &LiverTumorSegmentation::ProcessDicomReaderInteraction);
  m_DicomVolumeReader.AddObserver( m_DicomReaderCommand );
  
  m_ProgressSlider->Observe( m_ThresholdLevelSetModule.GetNotifier() );
  
  m_SegmentedVolumeOpacity = 0.5;
  
  m_RescaleIntensity->SetInput( m_LoadedVolume );
  m_SegmentedVolumeRescaleIntensity->SetInput( m_SegmentedVolume );
  
  m_vtkImageBlender = vtkImageBlend::New();
  m_vtkImageBlender->SetInput( 0, m_ITK2VTKAdaptor->GetOutput() );
  m_vtkImageBlender->SetInput( 1, m_SegmentedVolumeITK2VTKAdaptor->GetOutput() );
  m_vtkImageBlender->SetOpacity(0, 1.0 - m_SegmentedVolumeOpacity );
  m_vtkImageBlender->SetOpacity(1, m_SegmentedVolumeOpacity );
  
  m_ShiftScaleImageFilter->SetInput( m_ITK2VTKAdaptor->GetOutput() );
  
//  m_ShowVolumeView = true;
  
  m_ModuleType = THRESHOLD_LEVEL_SET;
  
  for(unsigned int i=0; i<NUMBER_OF_ALGORITHMS; i++)
    {
    m_Module->add( LiverTumorSegmentationModuleNames[i] );
    }
}


LiverTumorSegmentation::~LiverTumorSegmentation()
{
  if( m_ShiftScaleImageFilter )
  {
    m_ShiftScaleImageFilter->Delete();
  }
}


void LiverTumorSegmentation::Show()
{
  LiverTumorSegmentationGUI::Show();

  axialView->show();
  coronalView->show();
  saggitalView->show();
  
  m_AxialViewer.SetInteractor( axialView );
  m_CoronalViewer.SetInteractor( coronalView );
  m_SaggitalViewer.SetInteractor( saggitalView );
  
  axialView->Initialize();
  coronalView->Initialize();
  saggitalView->Initialize();
}



void LiverTumorSegmentation::Hide()
{
  LiverTumorSegmentationGUI::Hide();
}



void LiverTumorSegmentation::Quit()
{
  this->Hide();
}


void LiverTumorSegmentation::Load( void )
{
  const char * filename = fl_file_chooser("Volume filename","*.*","");
  
  if (LiverTumorSegmentationBase::Load( filename ) )
    {
    /* Put off Image Blending. Show only the loaded image. */
    m_ShiftScaleImageFilter->SetInput( m_ITK2VTKAdaptor->GetOutput() );

    /* Switch off Opacity Control knob. */
    this->SetSegmentedVolumeOpacityControlOff();
  
    this->LoadPostProcessing();
    sprintf( m_MessageString, "File %s has been loaded successfully.", filename );
    m_MessageBar->label( m_MessageString );
  }
  else
  {
    sprintf( m_MessageString, "File %s loading unsuccessful.", filename );
    m_MessageBar->label( m_MessageString );
  }
}



void LiverTumorSegmentation::LoadDICOM()
{
  
  const char * directoryname = fl_dir_chooser("DICOM Volume directory","");
  
  if( !directoryname  || strlen(directoryname) == 0 )
    {
    return;
    }
  
  m_DicomVolumeReader.SetDirectory( directoryname );
  
  // Attempt to read
  m_DicomVolumeReader.CollectSeriesAndSelectOne();
  
}



void LiverTumorSegmentation::LoadPostProcessing()
{  
  m_RescaleIntensity->SetInput( m_LoadedVolume );
  
  try
    {
    m_ShiftScaleImageFilter->UpdateWholeExtent();
    }
  catch( itk::ExceptionObject & exp )
    {
    fl_message( exp.GetDescription() );
    return;
    }
  
  m_AxialViewer.SetInput(    m_ShiftScaleImageFilter->GetOutput() );
  m_CoronalViewer.SetInput(  m_ShiftScaleImageFilter->GetOutput() );
  m_SaggitalViewer.SetInput( m_ShiftScaleImageFilter->GetOutput() );
  
  const unsigned int numberOfZslices = m_LoadedVolume->GetBufferedRegion().GetSize()[2];
  const unsigned int numberOfYslices = m_LoadedVolume->GetBufferedRegion().GetSize()[1];
  const unsigned int numberOfXslices = m_LoadedVolume->GetBufferedRegion().GetSize()[0];
  
  axialViewSlider->bounds( 0.0, numberOfZslices-1 );
  axialViewSlider->value( numberOfZslices / 2 );
  this->SelectAxialSlice( numberOfZslices / 2 );
  
  coronalViewSlider->bounds( 0.0, numberOfYslices-1 );
  coronalViewSlider->value( numberOfYslices / 2 );
  this->SelectCoronalSlice( numberOfYslices / 2 );
  
  saggitalViewSlider->bounds( 0.0, numberOfXslices-1 );
  saggitalViewSlider->value( numberOfXslices / 2 );
  this->SelectSaggitalSlice( numberOfXslices / 2 );
  
  m_AxialViewer.Render();
  m_CoronalViewer.Render();
  m_SaggitalViewer.Render();
  
  VisualizationVolumeType::IndexType index; 
  itk::Point< double, 3 >  point;    
  index[0] = numberOfXslices / 2;
  index[1] = numberOfYslices / 2;
  index[2] = numberOfZslices / 2;
  m_LoadedVolume->TransformIndexToPhysicalPoint( index, point );
  this->SetSeedPoint( point[0], point[1], point[2] );
}


void LiverTumorSegmentation::SelectAxialSlice( int slice )
{
  m_AxialViewer.SelectSlice( slice );
  axialView->redraw();
  Fl::check();
}


void LiverTumorSegmentation::SelectCoronalSlice( int slice )
{
  m_CoronalViewer.SelectSlice( slice );
  coronalView->redraw();
  Fl::check();
}


void LiverTumorSegmentation::SelectSaggitalSlice( int slice )
{
  m_SaggitalViewer.SelectSlice( slice );
  saggitalView->redraw();
  Fl::check();
}


void LiverTumorSegmentation::ProcessDicomReaderInteraction( void )
{
  std::cout << "Processing Dicom Reader Interaction " << std::endl; 
  bool volumeIsLoaded = m_DicomVolumeReader.IsVolumeLoaded();
  
  if( volumeIsLoaded )
    {
    m_LoadedVolume = m_DicomVolumeReader.GetOutput();
    this->LoadPostProcessing();
    }
}


void LiverTumorSegmentation::OnSegmentationParameters( void )
{
  m_Module->value( m_ModuleType );

  this->OnSegmentationModuleSelection( m_ModuleType );
  
  m_SegmentationParametersWindow->show();
}


void LiverTumorSegmentation::OnSegmentationModuleSelection( int module )
{
  double ftmp;
  int   itmp;
  
  m_ModuleType = (SegmentationModuleType) module;
  
  m_Parameter001->hide();
  m_Parameter002->hide();
  m_Parameter003->hide();
  m_Parameter004->hide();
  m_Parameter005->hide();
  m_Parameter006->hide();
  m_Parameter007->hide();
  m_Parameter101->hide();
  m_Parameter102->hide();
  m_Parameter103->hide();
  
  switch( module )
  {
    case THRESHOLD:
      m_Parameter001->label("Lower Threshold");
      ftmp = this->m_ThresholdLevelSetModule.GetLowerThreshold();
      sprintf(m_MessageString, "%4.1f", ftmp);
      m_Parameter001->value( m_MessageString );
      m_Parameter001->show();
      
      m_Parameter002->label("Upper Threshold");
      ftmp = this->m_ThresholdLevelSetModule.GetUpperThreshold();
      sprintf(m_MessageString, "%4.1f", ftmp);
      m_Parameter002->value( m_MessageString );
      m_Parameter002->show();
      break;

    case CONFIDENCE_CONNECTED:
      m_Parameter001->label("Multiplier");
      ftmp = this->m_ConfidenceConnectedModule.GetMultiplier();
      sprintf(m_MessageString, "%4.3f", ftmp);
      m_Parameter001->value( m_MessageString );
      m_Parameter001->show();
      
      m_Parameter002->label("Number Of Iterations");
      itmp = this->m_ConfidenceConnectedModule.GetNumberOfIterations();
      sprintf(m_MessageString, "%d", itmp);
      m_Parameter002->value( m_MessageString );
      m_Parameter002->show();
      
      m_Parameter003->label("Initial Radius");
      ftmp = this->m_ConfidenceConnectedModule.GetInitialNeighborhoodRadius();
      sprintf(m_MessageString, "%4.3f", ftmp);
      m_Parameter003->value( m_MessageString );
      m_Parameter003->show();
      
      m_Parameter101->label("Time Step");
      ftmp = this->m_ConfidenceConnectedModule.GetSmoothingTimeStep();
      sprintf(m_MessageString, "%4.3f", ftmp);
      m_Parameter101->value( m_MessageString );
      m_Parameter101->show();
      
      m_Parameter102->label("Number Of Iterations");
      itmp = this->m_ConfidenceConnectedModule.GetSmoothingIterations();
      sprintf(m_MessageString, "%d", itmp);
      m_Parameter102->value( m_MessageString );
      m_Parameter102->show();
      
      break;
      
    case CONNECTED_THRESHOLD:
      m_Parameter001->label("Lower Threshold");
      ftmp = this->m_ConnectedThresholdModule.GetLowerThreshold();
      sprintf(m_MessageString, "%4.1f", ftmp);
      m_Parameter001->value( m_MessageString );
      m_Parameter001->show();
      
      m_Parameter002->label("Upper Threshold");
      ftmp = this->m_ConnectedThresholdModule.GetUpperThreshold();
      sprintf(m_MessageString, "%4.1f", ftmp);
      m_Parameter002->value( m_MessageString );
      m_Parameter002->show();
      
      m_Parameter101->label("Time Step");
      ftmp = this->m_ConnectedThresholdModule.GetSmoothingTimeStep();
      sprintf(m_MessageString, "%4.3f", ftmp);
      m_Parameter101->value( m_MessageString );
      m_Parameter101->show();
      
      m_Parameter102->label("Number Of Iterations");
      itmp = this->m_ConnectedThresholdModule.GetSmoothingIterations();
      sprintf(m_MessageString, "%d", itmp);
      m_Parameter102->value( m_MessageString );
      m_Parameter102->show();
      break;
      
    case THRESHOLD_LEVEL_SET:
      m_Parameter001->label("Lower Threshold");
      ftmp = this->m_ThresholdLevelSetModule.GetLowerThreshold();
      sprintf(m_MessageString, "%4.1f", ftmp);
      m_Parameter001->value( m_MessageString );
      m_Parameter001->show();
      
      m_Parameter002->label("Upper Threshold");
      ftmp = this->m_ThresholdLevelSetModule.GetUpperThreshold();
      sprintf(m_MessageString, "%4.1f", ftmp);
      m_Parameter002->value( m_MessageString );
      m_Parameter002->show();
      
      m_Parameter003->label("Curvature Scaling");
      ftmp = this->m_ThresholdLevelSetModule.GetCurvatureScaling();
      sprintf(m_MessageString, "%4.3f", ftmp);
      m_Parameter003->value( m_MessageString );
      m_Parameter003->show();
      
      m_Parameter004->label("Propagation Scaling");
      ftmp = this->m_ThresholdLevelSetModule.GetPropagationScaling();
      sprintf(m_MessageString, "%4.3f", ftmp);
      m_Parameter004->value( m_MessageString );
      m_Parameter004->show();
      
      m_Parameter005->label("Advection Scaling");
      ftmp = this->m_ThresholdLevelSetModule.GetAdvectionScaling();
      sprintf(m_MessageString, "%4.3f", ftmp);
      m_Parameter005->value( m_MessageString );
      m_Parameter005->show();
      
      m_Parameter006->label("Maximum RMS Error");
      ftmp = this->m_ThresholdLevelSetModule.GetMaximumRMSError();
      sprintf(m_MessageString, "%4.3f", ftmp);
      m_Parameter006->value( m_MessageString );
      m_Parameter006->show();
      
      m_Parameter007->label("Maximum Iterations");
      itmp = this->m_ThresholdLevelSetModule.GetMaximumIterations();
      sprintf(m_MessageString, "%d", itmp);
      m_Parameter007->value( m_MessageString );
      m_Parameter007->show();
      
      m_Parameter101->label("Multiplier");
      ftmp = this->m_ThresholdLevelSetModule.GetMultiplier();
      sprintf(m_MessageString, "%4.3f", ftmp);
      m_Parameter101->value( m_MessageString );
      m_Parameter101->show();
      
      m_Parameter102->label("Number Of Iterations");
      itmp = this->m_ThresholdLevelSetModule.GetNumberOfIterations();
      sprintf(m_MessageString, "%d", itmp);
      m_Parameter102->value( m_MessageString );
      m_Parameter102->show();
      
      m_Parameter103->label("Initial Radius");
      ftmp = this->m_ThresholdLevelSetModule.GetInitialNeighborhoodRadius();
      sprintf(m_MessageString, "%4.3f", ftmp);
      m_Parameter103->value( m_MessageString );
      m_Parameter103->show();
      break;

    case ISOLATED_CONNECTED:
      break;

    case FAST_MARCHING:
      break;

    case GEODESIC_ACTIVE_CONTOUR:
      break;

    case WATERSHED:
      break;
  }
}


void LiverTumorSegmentation::OnSegmentationParametersOk( int module )
{
  switch( module )
  {
    case THRESHOLD:

      this->m_ThresholdLevelSetModule.SetLowerThreshold( atof(m_Parameter001->value() ) );
      this->m_ThresholdLevelSetModule.SetUpperThreshold( atof(m_Parameter002->value() ) );
      break;

     case CONFIDENCE_CONNECTED:
      this->m_ConfidenceConnectedModule.SetMultiplier( atof(m_Parameter001->value() ) );
      this->m_ConfidenceConnectedModule.SetNumberOfIterations( (unsigned int)atof(m_Parameter002->value() ) );
      this->m_ConfidenceConnectedModule.SetInitialNeighborhoodRadius( (unsigned int)atof(m_Parameter003->value() ) );
      this->m_ConfidenceConnectedModule.SetSmoothingTimeStep( atof(m_Parameter101->value() ) );
      this->m_ConfidenceConnectedModule.SetSmoothingIterations( atoi(m_Parameter102->value() ) );
      break;
    case CONNECTED_THRESHOLD:
      this->m_ConnectedThresholdModule.SetLowerThreshold( (unsigned int)atof(m_Parameter001->value() ) );
      this->m_ConnectedThresholdModule.SetUpperThreshold( (unsigned int)atof(m_Parameter002->value() ) );
      this->m_ConnectedThresholdModule.SetSmoothingTimeStep( atof(m_Parameter101->value() ) );
      this->m_ConnectedThresholdModule.SetSmoothingIterations( atoi(m_Parameter102->value() ) );
      break;
    case THRESHOLD_LEVEL_SET:
      this->m_ThresholdLevelSetModule.SetLowerThreshold( atof(m_Parameter001->value() ) );
      this->m_ThresholdLevelSetModule.SetUpperThreshold( atof(m_Parameter002->value() ) );
      this->m_ThresholdLevelSetModule.SetCurvatureScaling( atof(m_Parameter003->value() ) );
      this->m_ThresholdLevelSetModule.SetPropagationScaling( atof(m_Parameter004->value() ) );
      this->m_ThresholdLevelSetModule.SetAdvectionScaling( atof(m_Parameter005->value() ) );
      this->m_ThresholdLevelSetModule.SetMaximumRMSError( atof(m_Parameter006->value() ) );
      this->m_ThresholdLevelSetModule.SetMaximumIterations( atoi(m_Parameter007->value() ) );
      this->m_ThresholdLevelSetModule.SetMultiplier( atof(m_Parameter101->value() ) );
      this->m_ThresholdLevelSetModule.SetNumberOfIterations( atoi(m_Parameter102->value() ) );
      this->m_ThresholdLevelSetModule.SetInitialNeighborhoodRadius( atoi(m_Parameter103->value() ) );
      break;
    case ISOLATED_CONNECTED:
      break;

    case FAST_MARCHING:
      break;

    case GEODESIC_ACTIVE_CONTOUR:
      break;

    case WATERSHED:
      break;
   }
   m_SegmentationParametersWindow->hide();
}


void  
LiverTumorSegmentation::SetSeedPoint( double x, double y, double z )
{
  if (!m_LoadedVolume) return ;
  LiverTumorSegmentationBase::SetSeedPoint( x, y, z );
  this->SyncAllViews();
}

void LiverTumorSegmentation::ProcessAxialViewInteraction( void )
{
  m_AxialViewer.GetSelectPoint( m_SeedPoint );
  this->SyncAllViews();
}


void LiverTumorSegmentation::ProcessCoronalViewInteraction( void )
{
  m_CoronalViewer.GetSelectPoint( m_SeedPoint );
  this->SyncAllViews();
}


void LiverTumorSegmentation::ProcessSaggitalViewInteraction( void )
{
  m_SaggitalViewer.GetSelectPoint( m_SeedPoint );
  this->SyncAllViews();
}


void LiverTumorSegmentation::SyncAllViews(void)
{
  sprintf( m_MessageString, "%5.2f", m_SeedPoint[0] );
  m_SeedX->value( m_MessageString );
  sprintf( m_MessageString, "%5.2f", m_SeedPoint[1] );
  m_SeedY->value( m_MessageString );
  sprintf( m_MessageString, "%5.2f", m_SeedPoint[2] );
  m_SeedZ->value( m_MessageString );

  itk::Point< double, 3 > point;
  point[0] = m_SeedPoint[0];
  point[1] = m_SeedPoint[1];
  point[2] = m_SeedPoint[2];
  
  VisualizationVolumeType::IndexType index; 
  
  m_LoadedVolume->TransformPhysicalPointToIndex( point, index );
  
  // If the point is outside the volume, do not change slice selection
  if( m_LoadedVolume->GetLargestPossibleRegion().IsInside( index ) )
    {
    axialViewSlider->value(    index[2]  );
    coronalViewSlider->value(  index[1]  );
    saggitalViewSlider->value( index[0]  );
    
    this->SelectAxialSlice(    index[2] );
    this->SelectCoronalSlice(  index[1] );
    this->SelectSaggitalSlice( index[0] );
    
    m_SeedValue = m_LoadedVolume->GetPixel( index );
    
    m_SeedIndex[0] = index[0];
    m_SeedIndex[1] = index[1];
    m_SeedIndex[2] = index[2];
    }
  // Sync the selected point in all the views even if it is outside the image
  sprintf( m_MessageString, "Clicked Point:Indices(%d,%d,%d),Position(%4.3f,%4.3f,%4.3f),Value(%4.3f)", 
    index[0],index[1],index[2],m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2], m_SeedValue );
  printf( m_MessageString );
  printf("\n");

  m_MessageBar->label( m_MessageString );

  m_AxialViewer.SelectPoint(    m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2] );
  m_CoronalViewer.SelectPoint(  m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2] );
  m_SaggitalViewer.SelectPoint( m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2] );
  
}


void LiverTumorSegmentation::OnSegmentation( void )
{
   if (!LiverTumorSegmentationBase::DoSegmentation( m_ModuleType ))
      return;

  m_RescaleIntensity->SetInput( m_LoadedVolume );
    m_SegmentedVolumeRescaleIntensity->SetInput( m_SegmentedVolume );
      
    /* Put on Image Blending with Input and Segmented image. */
    m_ShiftScaleImageFilter->SetInput( m_vtkImageBlender->GetOutput() );
    m_ShiftScaleImageFilter->UpdateWholeExtent();
      
    m_AxialViewer.Render();
    m_CoronalViewer.Render();
    m_SaggitalViewer.Render();
      
    /* Switch on Opacity Control knob. */
    this->SetSegmentedVolumeOpacityControlOn( m_SegmentedVolumeOpacity );
      
    m_MessageBar->label( "Segmentation completed." );      
 }


double LiverTumorSegmentation::GetImageScale( void )
{
  return m_ShiftScaleImageFilter->GetScale();
}



void LiverTumorSegmentation::SetImageScale( double val )
{
  m_ShiftScaleImageFilter->SetScale( val );
  m_ShiftScaleImageFilter->UpdateWholeExtent();
  m_AxialViewer.Render();
  m_CoronalViewer.Render();
  m_SaggitalViewer.Render();
}



double LiverTumorSegmentation::GetImageShift( void )
{
  return m_ShiftScaleImageFilter->GetShift();
}



void LiverTumorSegmentation::SetImageShift( double val )
{
  m_ShiftScaleImageFilter->SetShift( val );
  m_ShiftScaleImageFilter->UpdateWholeExtent();
  m_AxialViewer.Render();
  m_CoronalViewer.Render();
  m_SaggitalViewer.Render();
}


double LiverTumorSegmentation::GetSegmentedVolumeOpacity( void )
{
  return m_SegmentedVolumeOpacity;
}


bool LiverTumorSegmentation::SetSegmentedVolumeOpacity( const double value )
{
  if ((value>=0.0f) && (value<=1.0f))
  {
    m_SegmentedVolumeOpacity = value;

    m_vtkImageBlender->SetOpacity(0, 1.0 - m_SegmentedVolumeOpacity );
    m_vtkImageBlender->SetOpacity(1, m_SegmentedVolumeOpacity );

    return true;
  }
  return false;
}


void LiverTumorSegmentation::OnOpacityControl( double opacity )
{
    this->SetSegmentedVolumeOpacity( opacity );
    
    m_ShiftScaleImageFilter->UpdateWholeExtent();
    m_AxialViewer.Render();
    m_CoronalViewer.Render();
    m_SaggitalViewer.Render();
}


//void LiverTumorSegmentation::LoadSegmentedVolume( void )
//{
//  const char *filename = fl_file_chooser("Segmented Volume Filename","*.*","");
//
//  temp_VolumeReader->SetFileName( filename );
//  temp_VolumeReader->Update();
//  m_SegmentedVolume = temp_VolumeReader->GetOutput();
//    
//  m_ShiftScaleImageFilter->UpdateWholeExtent();
//
//  m_AxialViewer.Render();
//  m_CoronalViewer.Render();
//  m_SaggitalViewer.Render();
//}


//void LiverTumorSegmentation::DoDebug( void )
//{
//  m_SeedIndex[0] = 56;
//  m_SeedIndex[1] = 69;
//  m_SeedIndex[2] = 91;
//  this->OnSegmentation();
//}


