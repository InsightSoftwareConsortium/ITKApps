


#include "CellularSegmentationApplication.h"

#include "FL/Fl_File_Chooser.H"

#include "ClickedPointEvent.h"

#include "itkImageFileWriter.h"
#include "itkTimeProbe.h"
#include "itkBioCell.h"


CellularSegmentationApplication
::CellularSegmentationApplication()
{
  m_ImageViewer.SetOrientation(    ImageSliceViewer::Axial    );
  m_ImageViewerCommand = itk::SimpleMemberCommand<CellularSegmentationApplication>::New();
  m_ImageViewerCommand->SetCallbackFunction(this, &CellularSegmentationApplication::ProcessImageViewInteraction);
  m_ImageViewer.AddObserver(ClickedPointEvent(), m_ImageViewerCommand);

  m_CellularAggregate = CellularAggregateType::New();
  
  m_CellularAggregateViewer = itk::bio::CellularAggregateViewer::New();

  m_CellularAggregateViewer->SetCellularAggregate( m_CellularAggregate );
  m_CellularAggregateViewer->SetRenderer( m_ImageViewer.GetRenderer() );

  displayChoice->add("Cell Walls");
  displayChoice->add("Real Neighbors");
  displayChoice->add("Delaunay");
  displayChoice->value(0);
}

CellularSegmentationApplication
::~CellularSegmentationApplication()
{
}

void
CellularSegmentationApplication
::Show()
{
  mainWindow->show();
  imageView->show();
  
  m_ImageViewer.SetInteractor( imageView );
 
  imageView->Initialize();
}

void 
CellularSegmentationApplication
::Hide()
{
  this->Stop();
  mainWindow->hide();
}

void 
CellularSegmentationApplication
::Quit()
{
  this->Stop();
  this->Hide();
}


void 
CellularSegmentationApplication::IterationCallback()
{
  this->CellularSegmentationApplicationBase::IterationCallback();
}


void 
CellularSegmentationApplication
::Load()
{
  this->Stop();

  const char * filename = fl_file_chooser("Volume filename","*.*","");
 
  if( !filename  || strlen(filename) == 0 )
    {
      return;
    }

  this->Load( filename );
}



void 
CellularSegmentationApplication
::Load( const char * filename )
{

  this->Stop();

  m_ImageReader->SetFileName( filename );

  try
    {
    m_ImageReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Error reading the image " << filename << std::endl;
    std::cerr << excp << std::endl;
    return;
    }

  m_RescaleIntensity->SetInput( m_ImageReader->GetOutput() );
  
  m_ITK2VTKAdaptor->SetInput( m_RescaleIntensity->GetOutput() );
  
  m_ITK2VTKAdaptor->Update();

  m_ImageViewer.SetInput( m_ITK2VTKAdaptor->GetOutput() );
  m_ImageViewer.Render();
 
  m_CellularAggregate->AddSubstrate( m_ImageReader->GetOutput() );

  runButton->deactivate();
  clearButton->deactivate();
}


void 
CellularSegmentationApplication
  ::ProcessImageViewInteraction( void )
{
  m_ImageViewer.GetSelectPoint( m_SeedPoint );
  runButton->activate();
}



void 
CellularSegmentationApplication
  ::CreateEgg( void )
{
  CellType * egg = CellType::CreateEgg();

  egg->SetChemoAttractantLowThreshold( lowThresholdValueInput->value() );
  egg->SetChemoAttractantHighThreshold( highThresholdValueInput->value() );

  CellType::PointType position;
  const unsigned int spaceDimension = CellType::GetDimension();
  for(unsigned int i=0; i<spaceDimension; i++)
    {
    position[i] = m_SeedPoint[i];
    }
  
  m_CellularAggregate->SetEgg( egg, position );

  runButton->activate();
  stopButton->deactivate();
  clearButton->deactivate();
  loadImageButton->deactivate();
  lowThresholdValueInput->deactivate();
  highThresholdValueInput->deactivate();
}


 
/**
 *    Initiate the execution of the simulation
 */ 
void
CellularSegmentationApplication
::Run(void)
{
  clearButton->deactivate();
  if( m_CellularAggregate->GetNumberOfCells() == 0 )
    {
    this->CreateEgg();
    }
  
  stopButton->activate();
  runButton->deactivate();
  m_Stop = false;
  
  if( !m_StartTime ) 
    {
    m_StartTime = clock();
    }
  
  while( !m_Stop )
    {
    m_CellularAggregate->AdvanceTimeStep();
    m_CellularAggregateViewer->Draw();
    m_ImageViewer.Render();
    Fl::check();
    }
}



/**
 *    Stop the execution of the simulation
 */ 
void 
CellularSegmentationApplication
::Stop(void)
{
  m_Stop = true;
  runButton->activate();
  clearButton->activate();
  stopButton->deactivate();
  lowThresholdValueInput->activate();
  highThresholdValueInput->activate();
}



/**
 *    Update the style used for displaying the cellular aggregate
 */ 
void 
CellularSegmentationApplication
::SetDisplayOption(void)
{
  itk::bio::CellularAggregateViewer::DisplayOptionType option = 
        (itk::bio::CellularAggregateViewer::DisplayOptionType) displayChoice->value();
  m_CellularAggregateViewer->SetDisplayOption( option );
  m_CellularAggregateViewer->Draw();
  m_ImageViewer.Render();
  Fl::check();
}


 
/**
 *    Destroy current Cellular Aggregate and prepare to restart
 */ 
void
CellularSegmentationApplication
::Clear(void)
{
  m_CellularAggregate = CellularAggregateType::New();
  m_CellularAggregate->AddSubstrate( m_ImageReader->GetOutput() );
  m_CellularAggregateViewer->SetCellularAggregate( m_CellularAggregate );
  m_CellularAggregateViewer->SetRenderer( m_ImageViewer.GetRenderer() );
  this->SetDisplayOption();
}


