


#include "DeformableModelApplication.h"

#include "FL/Fl_File_Chooser.H"

#include "vtkImageShiftScale.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"

#include "vtkLine.h"
#include "ClickedPointEvent.h"

#include "itkImageFileWriter.h"

#ifndef vtkDoubleType
#define vtkDoubleType double
#endif


DeformableModelApplication
::DeformableModelApplication()
{
  m_AxialViewer.SetOrientation(    ImageSliceViewer::Axial    );
  m_CoronalViewer.SetOrientation(  ImageSliceViewer::Coronal  );
  m_SagittalViewer.SetOrientation( ImageSliceViewer::Sagittal );
  m_SimplexMeshViewer.SetOrientation( ImageSliceViewer::Axial );

  m_Dummy.SetOrientation(ImageSliceViewer::Axial);

  m_ShiftScaleImageFilter = vtkImageShiftScale::New();

  m_ShiftScaleImageFilter->SetInput(  m_ITK2VTKAdaptor->GetOutput() );
  m_ShiftScaleImageFilter->SetOutputScalarTypeToUnsignedChar();
  m_ShiftScaleImageFilter->ClampOverflowOn();

  m_AxialViewerCommand = itk::SimpleMemberCommand<DeformableModelApplication>::New();
  m_AxialViewerCommand->SetCallbackFunction(this, &DeformableModelApplication::ProcessAxialViewInteraction);
  m_AxialViewer.AddObserver(ClickedPointEvent(), m_AxialViewerCommand);


  m_CoronalViewerCommand = itk::SimpleMemberCommand<DeformableModelApplication>::New();
  m_CoronalViewerCommand->SetCallbackFunction(this, &DeformableModelApplication::ProcessCoronalViewInteraction);
  m_CoronalViewer.AddObserver(ClickedPointEvent(), m_CoronalViewerCommand);


  m_SagittalViewerCommand = itk::SimpleMemberCommand<DeformableModelApplication>::New();
  m_SagittalViewerCommand->SetCallbackFunction(this, &DeformableModelApplication::ProcessSagittalViewInteraction);
  m_SagittalViewer.AddObserver(ClickedPointEvent(), m_SagittalViewerCommand);

  
  m_SurfaceViewerCommand = itk::SimpleMemberCommand<DeformableModelApplication>::New();
  m_SurfaceViewerCommand->SetCallbackFunction(this, &DeformableModelApplication::ProcessAxialViewInteraction);
  m_SimplexMeshViewer.AddObserver(ClickedPointEvent(), m_SurfaceViewerCommand);

  const float alpha = 0.2;
  const float beta  = 0.1;
  const float kappa = 0.5;

  m_InternalForceValueInput->value( alpha );
  m_ExternalForceValueInput->value( beta );
  m_BalloonForceValueInput->value( kappa );

  m_DeformFilter->SetAlpha( alpha );
  m_DeformFilter->SetBeta( beta );
  m_DeformFilter->SetKappa( kappa );
  m_DeformFilter->SetRigidity( 0 );
 
  m_InternalForcesComputed = false;
}




DeformableModelApplication
::~DeformableModelApplication()
{
   if( m_ShiftScaleImageFilter )
     {
     m_ShiftScaleImageFilter->Delete();
     }
}



void
DeformableModelApplication
::Show()
{
  mainWindow->show();
  axialView->show();
  coronalView->show();
  sagittalView->show();
  surfaceView->show();

  m_AxialViewer.SetInteractor( axialView );
  m_CoronalViewer.SetInteractor( coronalView );
  m_SagittalViewer.SetInteractor( sagittalView );
  m_SimplexMeshViewer.SetInteractor( surfaceView );

  axialView->Initialize();
  coronalView->Initialize();
  sagittalView->Initialize();
  surfaceView->Initialize();

}



void 
DeformableModelApplication
::Hide()
{
  mainWindow->hide();
}



void 
DeformableModelApplication
::Quit()
{
  this->Hide();
}



void
DeformableModelApplication
::LoadMesh()
{
  m_SphereMeshSource->SetCenter(m_SeedPoint);
  m_SimplexFilter->Update();

  m_SimplexMesh  = m_SimplexFilter->GetOutput();
  m_SimplexMesh->DisconnectPipeline();

  this->LoadMeshProcessing();
   
  // force a redraw
  axialView->redraw();
  coronalView->redraw();
  sagittalView->redraw();
  surfaceView->redraw();

  Fl::check(); 
}

void
DeformableModelApplication
::LoadMeshProcessing()
{
  
  CellIterator cellIterator = m_SimplexMesh->GetCells()->Begin();
  CellIterator cellEnd      = m_SimplexMesh->GetCells()->End();
  int i=0;
  while (cellIterator != cellEnd)
    {
    i++;
    cellType * cell = cellIterator.Value();
    if (cell->GetType() == cellType::LINE_CELL)
      {
      lineType * line = static_cast<lineType *>( cell );
      }
    ++cellIterator;
    }

 int numPoints =  m_SimplexMesh->GetNumberOfPoints();

 if (numPoints == 0)
    {
    fl_alert( "no points in Grid ");
    return; 
    }

 // fill in here the conversion between itkMesh versus vtkUnstructuredGrid
 // Create a vtkUnstructuredGrid
 //vtkUnstructuredGrid* vgrid = vtkUnstructuredGrid::New();
 vtkPolyData* vgrid = vtkPolyData::New();

 // Create the vtkPoints object and set the number of points
 vtkPoints* vpoints = vtkPoints::New();
 vpoints->SetNumberOfPoints(numPoints);


 // iterate over all the points in the itk mesh filling in
  // the vtkPoints object as we go
 SimplexMeshType::PointsContainer::Pointer points = m_SimplexMesh->GetPoints();
 for(SimplexMeshType::PointsContainer::Iterator i = points->Begin();
     i != points->End(); ++i)
    {
    // Get the point index from the point container iterator
    int idx = i->Index();
    // Set the vtk point at the index with the the coord array from itk
    // itk returns a const pointer, but vtk is not const correct, so
    // we have to use a const cast to get rid of the const
    vtkDoubleType * pp = const_cast<vtkDoubleType*>(i->Value().GetDataPointer());
    vpoints->SetPoint(idx, pp);
    }

 // Set the points on the vtk grid
 vgrid->SetPoints(vpoints);

 SimplexMeshType::CellType::MultiVisitor::Pointer mv =
   SimplexMeshType::CellType::MultiVisitor::New();

 LineVisitor::Pointer lv = LineVisitor::New();

 //set up the visitors
 int vtkCellCount = 0; // running counter for current cell inserted into vtk
 int numCells = m_SimplexMesh->GetNumberOfCells();
 int *types = new int[numCells]; //type array for vtk

 //create vtk cells and estimate the size
 vtkCellArray* cells = vtkCellArray::New();
 cells->EstimateSize(numCells, 4);

 // Set the TypeArray CellCount and CellArray for both visitors
 lv->SetTypeArray(types);
 lv->SetCellCounter(&vtkCellCount);
 lv->SetCellArray(cells);

 mv->AddVisitor(lv);

 m_SimplexMesh->Accept(mv);


 vgrid->SetLines(cells);

 cells->Delete();
 vpoints->Delete();

 m_AxialViewer.SetSimplexMesh(vgrid);
 m_CoronalViewer.SetSimplexMesh(vgrid);
 m_SagittalViewer.SetSimplexMesh(vgrid);
 m_SimplexMeshViewer.SetSimplexMesh(vgrid);
 
 return;
}


void 
DeformableModelApplication
::ComputeInternalForces()
{
  m_GradientFilter->SetSigma( 1.0);
  m_GradientFilter->Update();
  m_DeformFilter->SetGradient( m_GradientFilter->GetOutput() );
  m_InternalForcesComputed = true;

  typedef itk::ImageFileWriter< GradientImageType > WriterType;
  WriterType::Pointer gradientWriter = WriterType::New();
  gradientWriter->SetInput( m_GradientFilter->GetOutput() );
  gradientWriter->SetFileName("GradientImage.mhd");
  gradientWriter->Update();
}


void 
DeformableModelApplication
::DeformMesh()
{
  if( !m_InternalForcesComputed )
    {
    this->ComputeInternalForces();
    }
  
  m_SimplexMesh->DisconnectPipeline();

  for (unsigned int i=0; i< 10; i++ ) 
    {
    m_DeformFilter->SetInput( m_SimplexMesh );
    m_DeformFilter->SetIterations(10); 
    m_DeformFilter->Update();

    m_SimplexMesh =  m_DeformFilter->GetOutput();
    m_SimplexMesh->DisconnectPipeline();

    this->LoadMeshProcessing();
     
    // force a redraw
    axialView->redraw();
    coronalView->redraw();
    sagittalView->redraw();
    surfaceView->redraw();

    Fl::check(); 
    }
    
}
void 
DeformableModelApplication
::Load()
{

  const char * filename = fl_file_chooser("Volume filename","*.*","");
 
  if( !filename  || strlen(filename) == 0 )
    {
    return;
    }
  
  m_VolumeReader->SetFileName( filename );

  m_VolumeReader->Update();
  
  m_LoadedVolume = m_VolumeReader->GetOutput();

  

  this->LoadPostProcessing();

}



void 
DeformableModelApplication
::LoadPostProcessing()
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

  // of type ImageSliceViewer
  m_AxialViewer.SetInput(    m_ShiftScaleImageFilter->GetOutput() );
  m_CoronalViewer.SetInput(  m_ShiftScaleImageFilter->GetOutput() );
  m_SagittalViewer.SetInput( m_ShiftScaleImageFilter->GetOutput() );

  const unsigned int numberOfZslices = m_LoadedVolume->GetBufferedRegion().GetSize()[2];
  const unsigned int numberOfYslices = m_LoadedVolume->GetBufferedRegion().GetSize()[1];
  const unsigned int numberOfXslices = m_LoadedVolume->GetBufferedRegion().GetSize()[0];

  axialViewSlider->bounds( 0.0, numberOfZslices-1 );
  axialViewSlider->value( numberOfZslices / 2 );
  this->SelectAxialSlice( numberOfZslices / 2 );

  coronalViewSlider->bounds( 0.0, numberOfYslices-1 );
  coronalViewSlider->value( numberOfYslices / 2 );
  this->SelectCoronalSlice( numberOfYslices / 2 );

  sagittalViewSlider->bounds( 0.0, numberOfXslices-1 );
  sagittalViewSlider->value( numberOfXslices / 2 );
  this->SelectSagittalSlice( numberOfXslices / 2 );

  m_AxialViewer.Render();
  m_CoronalViewer.Render();
  m_SagittalViewer.Render();
  m_SimplexMeshViewer.Render();
  
}



void 
DeformableModelApplication
::SelectAxialSlice( int slice )
{
  m_AxialViewer.SelectSlice( slice );
  axialView->redraw();
  Fl::check();
}



void 
DeformableModelApplication
::SelectCoronalSlice( int slice )
{
  m_CoronalViewer.SelectSlice( slice );
  coronalView->redraw();
  Fl::check();
}



void 
DeformableModelApplication
::SelectSagittalSlice( int slice )
{
  m_SagittalViewer.SelectSlice( slice );
  sagittalView->redraw();
  Fl::check();
}



void 
DeformableModelApplication
::ProcessAxialViewInteraction( void )
{
  m_AxialViewer.GetSelectPoint( m_SeedPoint );
  this->SyncAllViews();
}


void  
DeformableModelApplication
::ProcessCoronalViewInteraction( void )
{
  
  m_CoronalViewer.GetSelectPoint( m_SeedPoint );
  this->SyncAllViews();
}


void  
DeformableModelApplication
::ProcessSagittalViewInteraction( void )
{
  
  m_SagittalViewer.GetSelectPoint( m_SeedPoint );
  this->SyncAllViews();
}


void 
DeformableModelApplication
::SyncAllViews(void)
{
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
    sagittalViewSlider->value( index[0]  );

    this->SelectAxialSlice(    index[2] );
    this->SelectCoronalSlice(  index[1] );
    this->SelectSagittalSlice( index[0] );
    }

  // Sync the selected point in all the views even if it is outside the image
  m_AxialViewer.SelectPoint(    m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2] );
  m_CoronalViewer.SelectPoint(  m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2] );
  m_SagittalViewer.SelectPoint( m_SeedPoint[0], m_SeedPoint[1], m_SeedPoint[2] );

}

