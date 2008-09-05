#if defined(_MSC_VER)
#pragma warning ( disable : 4018 )
#pragma warning ( disable : 4284 )
#endif



#include "DeformableModelApplication.h"

#include "FL/Fl_File_Chooser.H"

#include "vtkImageShiftScale.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkImageMarchingCubes.h"
#include "vtkDecimatePro.h"

#include "vtkImageImport.h"
#include "vtkPolyDataWriter.h"
#include "vtkPolyDataReader.h"
#include "vtkCellArray.h"

#include "ClickedPointEvent.h"

#include "itkImageFileWriter.h"
#include "itkTimeProbe.h"

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

  
  //   m_SurfaceViewerCommand = itk::SimpleMemberCommand<DeformableModelApplication>::New();
  //  m_SurfaceViewerCommand->SetCallbackFunction(this, &DeformableModelApplication::ProcessSurfaceViewInteraction);
  //  m_SimplexMeshViewer.AddObserver(ClickedPointEvent(), m_SurfaceViewerCommand);


  const float alpha = 0.8; //internal forces
  const float beta  = 0.8; //external forces
  const float gamma = 0.35; 
  const int   range = 2; // how far to go through scan line algorithm
  const int   rigidity = 0; // regularization
  const int   iterations = 100;

  sprintf(m_MessageString, "%4.1f", alpha);
  m_InternalForceValueInput->value(m_MessageString);
  sprintf(m_MessageString, "%4.1f", beta);
  m_ExternalForceValueInput->value(m_MessageString);
  sprintf(m_MessageString, "%5.2f", gamma);
  m_GammaForceValueInput->value(m_MessageString);
  sprintf(m_MessageString, "%d", range);
  m_RangeForceValueInput->value(m_MessageString);
  sprintf(m_MessageString, "%d", rigidity);
  m_RigidityForceValueInput->value(m_MessageString);
  sprintf(m_MessageString, "%d", iterations);
  m_IterationsValueInput->value(m_MessageString);
 
  m_ImageLoaded = false;
  m_MeshLoaded = false;
  m_MeshCreated = false;
  m_PreprocessingFinished = false;

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
  if (m_ImageLoaded )
    {
      if (m_MeshCreated) 
        {
          return;
        }
    }
  else
    {
      return;
    }
  const char * filename = fl_file_chooser("Mesh filename","*.*","");
 
  if( !filename  || strlen(filename) == 0 )
    {
    return;
    }
  
  typedef itk::ImageFileReader< MeshPixelType > ReaderType;
  
  ReaderType::Pointer meshReader = ReaderType::New();
  meshReader->SetFileName(filename);
  meshReader->Update();
 
  // convert ITK image to VTK image to send to marching cubes
  m_ImageToVTKImage->SetInput(meshReader->GetOutput());
  m_ImageToVTKImage->Update();
  
  // now send it to marching cubes to create a vtk mesh
  vtkImageMarchingCubes*  vmarchingcubes = vtkImageMarchingCubes::New();
  vmarchingcubes->SetInput(m_ImageToVTKImage->GetOutput());
  vmarchingcubes->SetValue(0, 0.5);
  vmarchingcubes->ComputeScalarsOff();
  vmarchingcubes->ComputeNormalsOff();
  vmarchingcubes->ComputeGradientsOff();
  vmarchingcubes->SetInputMemoryLimit(1000);
  vmarchingcubes->Update();

  vtkDecimatePro* vdecimate = vtkDecimatePro::New(); 
  vdecimate->SetInput(vmarchingcubes->GetOutput()); 
  vdecimate->SetTargetReduction(0.9); //compression factor, closer to 1 higher compression)
  vdecimate->PreserveTopologyOn();
  vdecimate->Update();
  
  // send polydata to vtkPolyDataToitkMesh to return itkMesh
  m_PolyData.SetInput(vdecimate->GetOutput());
  m_TriangleMesh = m_PolyData.GetOutput();
 
  std::cout << "Number of Points =   " << m_TriangleMesh->GetNumberOfPoints() << std::endl;
  std::cout << "Number of Cells  =   " << m_TriangleMesh->GetNumberOfCells()  << std::endl;
  
  m_SimplexMeshFilter->SetInput( m_TriangleMesh);
  m_SimplexMeshFilter->Update();
   
  m_SimplexMesh = m_SimplexMeshFilter->GetOutput();

  m_SimplexMesh->DisconnectPipeline();
  m_SimplexMeshFilter->Delete();
  
  std::cout << "simplex Number of Points =   " << m_SimplexMesh->GetNumberOfPoints() << std::endl;
  std::cout << "simplex Number of Cells  =   " << m_SimplexMesh->GetNumberOfCells()  << std::endl;

  m_SimplexMeshToShow = m_SimplexMesh;
  m_MeshLoaded = true;
  this->RefreshMeshVisualization();
   
  // force a redraw
  axialView->redraw();
  coronalView->redraw();
  sagittalView->redraw();
  surfaceView->redraw();

  Fl::check(); 

  //delete all VTK objects
  vmarchingcubes->Delete();
  vdecimate->Delete();
}
void
DeformableModelApplication
::CreateMesh()
{
  
   if (m_ImageLoaded )
    {
      if (m_MeshLoaded) 
        {
          return;
        }
    }
  else
    {
      return;
    }
  m_SphereMeshSource->SetCenter(m_SeedPoint);
  m_SimplexFilter->Update();
  
  m_SimplexMesh = m_SimplexFilter->GetOutput();
  m_SimplexMesh->DisconnectPipeline();

  m_SimplexMeshToShow = m_SimplexMesh;
  m_MeshCreated = true;
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
   int numPoints =  m_SimplexMeshToShow->GetNumberOfPoints();
  
 if (numPoints == 0)
   {
     fl_alert( "no points in Grid ");
     return; 
   }
 
 // fill in here the conversion between itkMesh versus vtkUnstructuredGrid

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
     vtkFloatingPointType * pp = const_cast<vtkFloatingPointType*>(i->Value().GetDataPointer());
      
     vpoints->SetPoint(idx, pp);
     
   }

 // Set the points on the vtk grid
 vgrid->SetPoints(vpoints);
  
 // it is probably better not to add scalar to the vis mesh
 // as it makes it harder to visualize
 //  vgrid->GetPointData()->SetScalars(scalars);
 //  vgrid->GetPointData()->CopyAllOn();
 
 SimplexMeshType::CellType::MultiVisitor::Pointer mv =
   SimplexMeshType::CellType::MultiVisitor::New();

  LineVisitor::Pointer lv = LineVisitor::New();
  PolygonVisitor::Pointer pv = PolygonVisitor::New();
  TriangleVisitor::Pointer tv = TriangleVisitor::New();
 
  //set up the visitors
  int vtkCellCount = 0; // running counter for current cell inserted into vtk
  int numCells = m_SimplexMeshToShow->GetNumberOfCells();
  int *types = new int[numCells]; //type array for vtk
  bool onlyTriangles = false;
  //create vtk cells and estimate the size
   vtkCellArray* cells = vtkCellArray::New();
     cells->EstimateSize(numCells, 4);

    lv->SetTypeArray(types);
    lv->SetCellCounter(&vtkCellCount);
    lv->SetCellArray(cells);

    pv->SetTypeArray(types);
    pv->SetCellCounter(&vtkCellCount);
    pv->SetCellArray(cells);

    tv->SetTypeArray(types);
    tv->SetCellCounter(&vtkCellCount);
    tv->SetCellArray(cells);
  

  // Set the TypeArray CellCount and CellArray for both visitors
  //lv->SetTypeArray(types);
  // tv->SetTypeArray(types);
  //lv->SetCellCounter(&vtkCellCount);
    //  tv->SetCellCounter(&vtkCellCount);
  //lv->SetCellArray(cells);
    //  tv->SetCellArray(cells);
 
  // mv->AddVisitor(lv);
  //mv->AddVisitor(tv);
  m_SimplexMeshToShow->Accept(mv);


  //vgrid->SetLines(cells);
  //vgrid->SetStrips(cells);  
   
if (onlyTriangles) {
      mv->AddVisitor(tv);
      m_SimplexMeshToShow->Accept(mv);
      vgrid->SetStrips(cells);  
    }
    else 
    {
      mv->AddVisitor(tv);
      mv->AddVisitor(lv);
      //      mv->AddVisitor(pv);
  //    mv->AddVisitor(qv);
      // Now ask the mesh to accept the multivisitor which
      // will Call Visit for each cell in the mesh that matches the
      // cell types of the visitors added to the MultiVisitor
  m_SimplexMeshToShow->Accept(mv);
      // Now set the cells on the vtk grid with the type array and cell array
  vgrid->SetPolys(cells);  
      //vgrid->SetStrips(cells);  
  vgrid->SetLines(cells);
    // Clean up vtk objects (no vtkSmartPointer ... )
  }

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
  if ( !m_ImageLoaded || (!m_MeshLoaded && !m_MeshCreated))
    {
      return;
    }
  std::cout << " Performing Preprocessing ... " << std::endl;
  m_CastImage->SetInput( m_VolumeReader->GetOutput() );
  m_CastImage->Update();
  /* removed this as it was too slow
  m_GradientAnisotropicImage->SetInput( m_CastImage->GetOutput());
  m_GradientAnisotropicImage->SetNumberOfIterations(5);
  m_GradientAnisotropicImage->SetTimeStep(0.0625);
  m_GradientAnisotropicImage->SetConductanceParameter(3);
  */
  //m_GradientMagnitude->SetInput( m_GradientAnisotropicImage->GetOutput() );
  m_GradientMagnitude->SetInput(m_CastImage->GetOutput() );
  m_GradientMagnitude->SetSigma(0.5);
  
  m_SigmoidImage->SetInput( m_GradientMagnitude->GetOutput());
  m_SigmoidImage->SetOutputMinimum(0);
  m_SigmoidImage->SetOutputMaximum(1);
  m_SigmoidImage->SetAlpha(230);
  m_SigmoidImage->SetBeta(1300);
  
  m_GradientFilter->SetInput( m_SigmoidImage->GetOutput());
  m_GradientFilter->SetSigma( 0.5);

  m_GradientFilter->Update();
  std::cout << " Preprocessing DONE!... " << std::endl;
  m_DeformFilter->SetGradient( m_GradientFilter->GetOutput() );
  VolumeType::Pointer   Image = m_VolumeReader->GetOutput();
  Image->DisconnectPipeline();
  m_DeformFilter->SetImage(Image );
  m_PreprocessingFinished = true;

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

  vtkPolyDataWriter *vpolywriter = vtkPolyDataWriter::New();
  vpolywriter->SetInput(m_SimplexMeshViewer.GetSimplexMesh());
  vpolywriter->SetFileName(filename);
  vpolywriter->Write();

  vpolywriter->Delete();
      
}

void
DeformableModelApplication
::SaveMask()
{
  if (!m_ImageLoaded || (!m_MeshLoaded && !m_MeshCreated) || !m_PreprocessingFinished)
    {
      return;
    }

  const char * filename = fl_file_chooser("Save Mask As","*.mhd","");
 
  if( !filename  || strlen(filename) == 0 )
    {
    return;
    }
  double orgn[3];
  orgn[0] = m_LoadedVolume->GetOrigin()[0];
  orgn[1] = m_LoadedVolume->GetOrigin()[1];
  orgn[2] = m_LoadedVolume->GetOrigin()[2];

  std::cout << "Converting Simplex Mesh to Triangle Mesh . . ." << std::endl;
  
  m_SimplexToTriangle->SetInput(m_DeformFilter->GetOutput());
  m_SimplexToTriangle->Update();
  
  TriangleMeshType::Pointer triangleMesh = m_SimplexToTriangle->GetOutput();
  triangleMesh->DisconnectPipeline();
  m_SimplexToTriangle->Delete();
  m_TriangleToImage->SetInput(triangleMesh);
  MeshPixelType::SizeType size;

  size[0] = m_LoadedVolume->GetBufferedRegion().GetSize()[0];
  size[1] = m_LoadedVolume->GetBufferedRegion().GetSize()[1];
  size[2] = m_LoadedVolume->GetBufferedRegion().GetSize()[2];
  m_TriangleToImage->SetSize(size);
  
 
  m_TriangleToImage->SetOrigin(orgn);
  // spacing remains (1,1,1) until we make a change to deformable model class
  float spacing[3];
  spacing[0] = 1;
  spacing[1] = 1;
  spacing[2] = 1;

  m_TriangleToImage->SetSpacing(spacing);

  m_TriangleToImage->Update();

  m_ImageWriter->SetInput(m_TriangleToImage->GetOutput() );
  m_ImageWriter->SetFileName(filename);
  m_ImageWriter->UseInputMetaDataDictionaryOn();
  m_ImageWriter->Update();
}

void 
DeformableModelApplication
::DeformMesh()
{
  if (!m_ImageLoaded || (!m_MeshLoaded && !m_MeshCreated) || !m_PreprocessingFinished)
    {
      return;
    }
 SimplexMeshType::Pointer simplexMesh2 = m_SimplexMesh;
  const unsigned int numberOfIterationsToGo = atoi(m_IterationsValueInput->value());
 
  for( unsigned int i=0; i<numberOfIterationsToGo; i++ )
    {
      std::cout << " Iteration   " << i << std::endl;
      m_SimplexMesh->DisconnectPipeline();
      
      m_DeformFilter->SetInput( m_SimplexMesh );

      m_DeformFilter->SetIterations(1); 
      m_DeformFilter->Update();

      simplexMesh2 =  m_DeformFilter->GetOutput();
     
      m_SimplexMeshToShow  = m_SimplexMesh;

      this->RefreshMeshVisualization();
     
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

  m_ImageLoaded =  true;

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
