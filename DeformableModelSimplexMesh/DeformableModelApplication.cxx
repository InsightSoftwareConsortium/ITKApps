


#include "DeformableModelApplication.h"

#include "FL/Fl_File_Chooser.H"

#include "vtkImageShiftScale.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkImageMarchingCubes.h"
#include "vtkDecimate.h"

#include "vtkImageImport.h"
#include "vtkPolyDataWriter.h"
#include "vtkPolyDataReader.h"
#include "vtkCellArray.h"
//#include "itkVTKImageToImageFilter.h"

//#include "vtkLine.h"
#include "ClickedPointEvent.h"

#include "itkImageFileWriter.h"

#ifndef vtkDoubleType
#define vtkDoubleType double
#endif

#ifndef vtkFloatingPointType
# define vtkFloatingPointType vtkFloatingPointType
typedef float vtkFloatingPointType;
#endif

DeformableModelApplication
::DeformableModelApplication()
{
  m_AxialViewer.SetOrientation(    ImageSliceViewer::Axial    );
  m_CoronalViewer.SetOrientation(  ImageSliceViewer::Coronal  );
  m_SagittalViewer.SetOrientation( ImageSliceViewer::Sagittal );
  //m_SimplexMeshViewer.SetOrientation( ImageSliceViewer::Axial );

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
  m_SurfaceViewerCommand->SetCallbackFunction(this, &DeformableModelApplication::ProcessSurfaceViewInteraction);
  m_SimplexMeshViewer.AddObserver(ClickedPointEvent(), m_SurfaceViewerCommand);

  const float alpha = 0.1;
  const float beta  = 1.0;
  const float kappa = 0.5;
  const float gamma = 0.1;

  m_InternalForceValueInput->value( alpha );
  m_ExternalForceValueInput->value( beta );
  //m_BalloonForceValueInput->value( kappa );
  m_GammaForceValueInput->value( gamma );

  m_DeformFilter->SetAlpha( alpha );
  m_DeformFilter->SetBeta( beta );
  //m_DeformFilter->SetKappa( kappa );
  m_DeformFilter->SetGamma (gamma);

  m_DeformFilter->SetRigidity( 1 );
 
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
  creditsWindow->show();
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

  const char * filename = fl_file_chooser("Mesh filename","*.*","");
 
  if( !filename  || strlen(filename) == 0 )
    {
    return;
    }
  
  typedef itk::ImageFileReader< MeshPixelType > ReaderType;
  
  ReaderType::Pointer meshReader = ReaderType::New();
  meshReader->SetFileName(filename);
  meshReader->Update();
 
  
  m_ImageToVTKImage->SetInput(meshReader->GetOutput());
  m_ImageToVTKImage->Update();

  //vtkImageImport* vimageimport = vtkImageImport::New();
  //vimageimport->SetInput(m_VTKImageExport->GetInput());
  
  // now send it to marching cubes to create a vtk mesh
  vtkImageMarchingCubes*  vmarchingcubes = vtkImageMarchingCubes::New();
  vmarchingcubes->SetInput(m_ImageToVTKImage->GetOutput());
  vmarchingcubes->SetValue(0, 0.5);
  vmarchingcubes->ComputeScalarsOff();
  vmarchingcubes->SetInputMemoryLimit(1000);
  vmarchingcubes->Update();

  vtkDecimate* vdecimate = vtkDecimate::New(); 
  vdecimate->SetInput(vmarchingcubes->GetOutput()); 
  vdecimate->SetTargetReduction(0.90); //compression factor (number closer to 1 higher compression)
  vdecimate->SetAspectRatio(20);
  vdecimate->SetInitialError(0.02); //control decimate criterion initially set
  vdecimate->SetErrorIncrement(0.05); //decimation criterion is incremented by this at each iteration
  vdecimate->SetMaximumIterations(6); 
  vdecimate->SetInitialFeatureAngle(35); //to compute feature edges smaller angle ->more surface detail
  vdecimate->Update();

  vtkPolyDataWriter* vpoly = vtkPolyDataWriter::New();
  vpoly->SetInput(vdecimate->GetOutput());
  vpoly->SetFileName("original.vtk");
  vpoly->Write();
  
  
  // vtkPolyDataReader* vreader = vtkPolyDataReader::New();
  // vreader->SetFileName(filename);
  // vreader->Update();

  //
  // Transfer the points from the vtkPolyData into the itk::Mesh
  //
  const unsigned int numberOfPoints = vdecimate->GetOutput()->GetNumberOfPoints();
  
  vtkPoints * vtkpoints = vdecimate->GetOutput()->GetPoints();
  
  //const unsigned int numberOfPoints = vreader->GetOutput()->GetNumberOfPoints();
  // vtkPoints * vtkpoints = vreader->GetOutput()->GetPoints();
  m_TriangleMesh->GetPoints()->Reserve( numberOfPoints );

  for(int p =0; p < numberOfPoints; p++)
    {

    vtkFloatingPointType * apoint = vtkpoints->GetPoint( p );

    // Need to convert the point to PoinType
    TriangleMeshType::PointType pt;
    for(unsigned int i=0;i<3; i++)
      {
      pt[i] = apoint[i];
      }

    m_TriangleMesh->SetPoint( p, pt);
    
    }

  //
  // Transfer the cells from the vtkPolyData into the itk::Mesh
  //
  vtkCellArray * triangleStrips = vdecimate->GetOutput()->GetStrips();
  //vtkCellArray * triangleStrips = vreader->GetOutput()->GetStrips();

  vtkIdType  * cellPoints;
  vtkIdType    numberOfCellPoints;
 //
  // First count the total number of triangles from all the triangle strips.
  //
  unsigned int numberOfTriangles = 0;

  triangleStrips->InitTraversal();
   while( triangleStrips->GetNextCell( numberOfCellPoints, cellPoints ) )
    {
    numberOfTriangles += numberOfCellPoints-2;
    }


   vtkCellArray * polygons = vdecimate->GetOutput()->GetPolys();
  
   //vtkCellArray * polygons = vreader->GetOutput()->GetPolys();
  polygons->InitTraversal();

  while( polygons->GetNextCell( numberOfCellPoints, cellPoints ) )
    {
    if( numberOfCellPoints == 3 )
      {
      numberOfTriangles ++;
      }
    }

   //
  // Reserve memory in the itk::Mesh for all those triangles
  //
  m_TriangleMesh->GetCells()->Reserve( numberOfTriangles );

  // 
  // Copy the triangles from vtkPolyData into the itk::Mesh
  //
  //

  typedef TriangleMeshType::CellType   CellType;

  typedef itk::TriangleCell< CellType > TriangleCellType;

  int cellId = 0;

  // first copy the triangle strips
  triangleStrips->InitTraversal();
  while( triangleStrips->GetNextCell( numberOfCellPoints, cellPoints ) )
    {
    
    unsigned int numberOfTrianglesInStrip = numberOfCellPoints - 2;

    unsigned long pointIds[3];
    pointIds[0] = cellPoints[0];
    pointIds[1] = cellPoints[1];
    pointIds[2] = cellPoints[2];

    for( unsigned int t=0; t < numberOfTrianglesInStrip; t++ )
      {
      TriangleMeshType::CellAutoPointer c;
      TriangleCellType * tcell = new TriangleCellType;
      tcell->SetPointIds( pointIds );
      c.TakeOwnership( tcell );
      m_TriangleMesh->SetCell( cellId, c );
      cellId++;
      pointIds[0] = pointIds[1];
      pointIds[1] = pointIds[2];
      pointIds[2] = cellPoints[t+3];
      }

    
    }

   // then copy the normal triangles
  polygons->InitTraversal();
  while( polygons->GetNextCell( numberOfCellPoints, cellPoints ) )
    {
    if( numberOfCellPoints !=3 ) // skip any non-triangle.
      {
      continue;
      }
    TriangleMeshType::CellAutoPointer c;
    TriangleCellType * t = new TriangleCellType;
    t->SetPointIds( (unsigned long*)cellPoints );
    c.TakeOwnership( t );
    m_TriangleMesh->SetCell( cellId, c );
    cellId++;
    }
  
  std::cout << "Number of Points =   " << m_TriangleMesh->GetNumberOfPoints() << std::endl;
  std::cout << "Number of Cells  =   " << m_TriangleMesh->GetNumberOfCells()  << std::endl;
 
  m_SimplexMeshFilter->SetInput( m_TriangleMesh);
 m_SimplexMeshFilter->Update();
  
  
  m_SimplexMesh = m_SimplexMeshFilter->GetOutput();

  m_SimplexMeshToShow = m_SimplexMesh;

  this->RefreshMeshVisualization();
   
  // force a redraw
  axialView->redraw();
  coronalView->redraw();
  sagittalView->redraw();
  surfaceView->redraw();

  Fl::check(); 

  
}

void
DeformableModelApplication
::CreateMesh()
{
  m_SphereMeshSource->SetCenter(m_SeedPoint);
  m_SimplexFilter->Update();

  m_SimplexMesh = m_SimplexFilter->GetOutput();

  m_SimplexMeshToShow = m_SimplexMesh;

  this->RefreshMeshVisualization();
   
  // force a redraw
  axialView->redraw();
  coronalView->redraw();
  sagittalView->redraw();
  surfaceView->redraw();

  Fl::check(); 
}

void
DeformableModelApplication
::RefreshMeshVisualization()
{
  
  CellIterator cellIterator = m_SimplexMeshToShow->GetCells()->Begin();
  CellIterator cellEnd      = m_SimplexMeshToShow->GetCells()->End();
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

 int numPoints =  m_SimplexMeshToShow->GetNumberOfPoints();

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
 SimplexMeshType::PointsContainer::Pointer points = m_SimplexMeshToShow->GetPoints();
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
 int numCells = m_SimplexMeshToShow->GetNumberOfCells();
 int *types = new int[numCells]; //type array for vtk

 //create vtk cells and estimate the size
 vtkCellArray* cells = vtkCellArray::New();
 cells->EstimateSize(numCells, 4);

 // Set the TypeArray CellCount and CellArray for both visitors
 lv->SetTypeArray(types);
 lv->SetCellCounter(&vtkCellCount);
 lv->SetCellArray(cells);

 mv->AddVisitor(lv);

 m_SimplexMeshToShow->Accept(mv);


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

  
  m_CastImage->SetInput( m_VolumeReader->GetOutput() );
  m_CastImage->Update();
  std::cout << "Casting Image is DONE!" << std::endl;

  // CurvatureAnisotropicDifussionImageFilter
  /*
  m_AnisotropicImage->SetInput( m_CastImage->GetOutput());
  m_AnisotropicImage->SetNumberOfIterations(5);
  m_AnisotropicImage->SetTimeStep(0.0625);
  m_AnisotropicImage->SetConductanceParameter(3);
  */
  m_GradientAnisotropicImage->SetInput( m_CastImage->GetOutput());
  m_GradientAnisotropicImage->SetNumberOfIterations(5);
  m_GradientAnisotropicImage->SetTimeStep(0.0625);
  m_GradientAnisotropicImage->SetConductanceParameter(3);
  std::cout << "GradientAnisotropicDiffusion is DONE!" << std::endl;
 
  m_GradientMagnitude->SetInput( m_GradientAnisotropicImage->GetOutput() );
  m_GradientMagnitude->SetSigma(0.5);
  std::cout << "GradientMagnitude is DONE!" << std::endl;
  /*
  typedef itk::ImageFileWriter< CastType > WriterType;
  WriterType::Pointer gradientWriter = WriterType::New();
  gradientWriter->SetInput( m_GradientMagnitude->GetOutput() );
  gradientWriter->SetFileName("gradmag.mhd");
  gradientWriter->Update();
  */
  m_SigmoidImage->SetInput( m_GradientMagnitude->GetOutput());
  m_SigmoidImage->SetOutputMinimum(0);
  m_SigmoidImage->SetOutputMaximum(1);
  m_SigmoidImage->SetAlpha(-200);
  m_SigmoidImage->SetBeta(1350);
  std::cout << "gradient mag is DONE!" << std::endl;
  
  m_GradientFilter->SetInput( m_SigmoidImage->GetOutput());
  m_GradientFilter->SetSigma( 0.5);
  m_GradientFilter->Update();
  std::cout << "Gradient is DONE!" << std::endl;

  
  std::cout << "Saving the Sigmoid Image!" << std::endl;

  
  typedef itk::ImageFileWriter< CastType > WriterType;
  WriterType::Pointer gradientWriter = WriterType::New();
  gradientWriter->SetInput( m_SigmoidImage->GetOutput() );
  gradientWriter->SetFileName("sigmoid.mhd");
  gradientWriter->Update();

  m_DeformFilter->SetGradient( m_GradientFilter->GetOutput() );
  m_InternalForcesComputed = true;

  std::cout << " Computed Image Force" << std::endl;
 
}


void 
DeformableModelApplication::IterationCallback()
{
  // The visualization cannot be refreshed at every iteration because the
  // deformable model filter do not fully recomputes its output at every
  // iteration. It only does it when the iterations are completed.
  // 
  // this->DeformableModelApplicationBase::IterationCallback();
  //
}
void
DeformableModelApplication
::SaveMesh()
{
 
  const char * filename = fl_file_chooser("Save Mesh As","*.vtk","");
 
  if( !filename  || strlen(filename) == 0 )
    {
    return;
    }
  std::cout << "implement saving mesh" << filename << std::endl;

  vtkPolyDataWriter *vpolywriter = vtkPolyDataWriter::New();
  vpolywriter->SetInput(m_SimplexMeshViewer.GetSimplexMesh());
  vpolywriter->SetFileName(filename);
  vpolywriter->Write();

}

void 
DeformableModelApplication
::DeformMesh()
{
  if( !m_InternalForcesComputed )
    {
    this->ComputeInternalForces();
    }
 
  const unsigned int numberOfIterationsToGo = (unsigned int)(m_IterationsValueInput->value());
  std::cout << " Please Wait ..." << std::endl;
  for( unsigned int i=0; i<numberOfIterationsToGo; i++ )
    {
    m_SimplexMesh->DisconnectPipeline();

    m_DeformFilter->SetInput( m_SimplexMesh );

    m_DeformFilter->SetIterations(1); 
    m_DeformFilter->Update();

    m_SimplexMesh =  m_DeformFilter->GetOutput();
    
    m_SimplexMeshToShow  = m_SimplexMesh;

     this->RefreshMeshVisualization();
     
    // force a redraw
    axialView->redraw();
    coronalView->redraw();
    sagittalView->redraw();
    surfaceView->redraw();

    Fl::check(); 
    }

  std::cout << " Done Deformation " << std::endl;
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
  m_SimplexMeshViewer.SetInput( m_ShiftScaleImageFilter->GetOutput() );

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
::ProcessSurfaceViewInteraction( void )
{
  
  //m_SagittalViewer.GetSelectPoint( m_SeedPoint );
  //this->SyncAllViews();
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

