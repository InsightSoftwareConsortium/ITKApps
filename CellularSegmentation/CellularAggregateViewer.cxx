/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    CellularAggregateViewer.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4503 )
#endif
#include <fstream>
#include "CellularAggregateViewer.h"

#include "vtkLine.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkDelaunay2D.h"
#include "vtkGlyph2D.h"
#include "vtkProperty.h"
#include "vtkCylinderSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"


namespace itk {

namespace bio {




CellularAggregateViewer
::CellularAggregateViewer()
{
  m_Renderer = NULL;

  m_SurfaceActor  = vtkActor::New();
  m_SurfaceMapper = vtkPolyDataMapper::New();
  m_PolyData      = vtkPolyData::New();

  m_Delaunay2DFilter = vtkDelaunay2D::New();

  m_CellGlyphSource = vtkCylinderSource::New();
  m_CellGlyphSource->SetResolution(12);

  m_CellGlyphTransform = vtkTransform::New();
  m_CellGlyphTransform->RotateX( 90.0 );

  m_CellGlyphTransformFilter = vtkTransformPolyDataFilter::New();
  m_CellGlyphTransformFilter->SetTransform( m_CellGlyphTransform );
  m_CellGlyphTransformFilter->SetInput( m_CellGlyphSource->GetOutput() );

  m_GlyphFilter = vtkGlyph2D::New();
  m_GlyphFilter->SetSource( m_CellGlyphTransformFilter->GetOutput() );


  m_SurfaceMapper->SetInput( m_PolyData );

  m_SurfaceActor->SetMapper( m_SurfaceMapper );
  m_SurfaceActor->GetProperty()->SetColor(1.0,0.0,0.0);

  m_DisplayOption = CellWalls;
}




CellularAggregateViewer
::~CellularAggregateViewer()
{

  if( m_Renderer )
    {
    m_Renderer->RemoveActor( m_SurfaceActor );
    m_Renderer = NULL;
    }
  if( m_GlyphFilter )
    {
    m_GlyphFilter->Delete();
    m_GlyphFilter = NULL;
    }
  if( m_CellGlyphTransformFilter )
    {
    m_CellGlyphTransformFilter->Delete();
    m_CellGlyphTransformFilter = NULL;
    }
  if( m_CellGlyphTransform )
    {
    m_CellGlyphTransform->Delete();
    m_CellGlyphTransform = NULL;
    }
  if( m_Delaunay2DFilter )
    {
    m_Delaunay2DFilter->Delete();
    m_Delaunay2DFilter = NULL;
    }
  if( m_CellGlyphSource )
    {
    m_CellGlyphSource->Delete();
    m_CellGlyphSource = NULL;
    }
  if( m_SurfaceActor )
    {
    m_SurfaceActor->Delete();
    m_SurfaceActor = NULL;
    }
  if( m_SurfaceMapper )
    {
    m_SurfaceMapper->Delete();
    m_SurfaceMapper = NULL;
    }
  if( m_PolyData )
    {
    m_PolyData->Delete();
    m_PolyData = NULL;
    }
}



void
CellularAggregateViewer
::Draw(void) const
{
  switch(m_DisplayOption)
  {
  case CellWalls:
    this->DrawCellWalls();
    break;
  case RealNeighbors:
    this->DrawRealNeighbors();
    break;
  case Delaunay:
    this->DrawDelaunay();
    break;
  }
}


void
CellularAggregateViewer
::DrawCellWalls(void) const
{
  vtkPoints* newPoints = vtkPoints::New();

  MeshConstPointer mesh = this->m_CellularAggregate->GetMesh();
    
  const unsigned int numberOfPoints = mesh->GetNumberOfPoints();

  newPoints->Allocate( numberOfPoints );

  PointType position;

  CellsConstIterator cellIt = mesh->GetPointData()->Begin();
  CellsConstIterator end    = mesh->GetPointData()->End();

  PointsContainer::ConstPointer cellPositions = mesh->GetPoints();

  vtkIdType pointId = 0;
  while( cellIt != end )
    {
    BioCellType * cell = cellIt.Value();
    const IdentifierType id = cell->GetSelfIdentifier();
    if( cellPositions->GetElementIfIndexExists( id, &position ) )
      {
      newPoints->InsertPoint(pointId++,position[0],position[1],position[2]);
      }
    cellIt++;
    }

  this->m_PolyData->SetPoints(newPoints);
  newPoints->Delete();

  m_GlyphFilter->SetInput( this->m_PolyData );
  m_SurfaceMapper->SetInput( m_GlyphFilter->GetOutput() );
}


void
CellularAggregateViewer
::DrawRealNeighbors(void) const
{
  
  MeshConstPointer mesh = this->m_CellularAggregate->GetMesh();

  vtkPoints* newPoints = vtkPoints::New();

  const unsigned int numberOfPoints = mesh->GetNumberOfPoints();

  newPoints->Allocate( numberOfPoints );

  PointType position;

  CellsConstIterator cellIt = mesh->GetPointData()->Begin();
  CellsConstIterator end    = mesh->GetPointData()->End();

  PointsContainer::ConstPointer cellPositions = mesh->GetPoints();

  typedef std::map< IdentifierType, vtkIdType > MapType;
  MapType pointIdMap;
    
  vtkIdType pointId = 0;
  while( cellIt != end )
    {
    BioCellType * cell = cellIt.Value();
    const IdentifierType id = cell->GetSelfIdentifier();
    if( cellPositions->GetElementIfIndexExists( id, &position ) )
      {
      pointIdMap[id] = pointId;
      newPoints->InsertPoint(pointId++,position[0],position[1],position[2]);
      }
    cellIt++;
    }

  this->m_PolyData->SetPoints(newPoints);
  newPoints->Delete();

  // Adding now the triangle strips
  vtkCellArray *lines  = vtkCellArray::New();
  lines->Allocate( lines->EstimateSize( numberOfPoints, 20));

  vtkIdType pointIds[2];

  // Draw edges connecting neighbor cells
  cellIt = mesh->GetPointData()->Begin();
  while( cellIt != end )
    {
    BioCellType * cell = cellIt.Value();
    const IdentifierType id1 = cell->GetSelfIdentifier();

    pointIds[0] = pointIdMap[id1];

    VoronoiRegionAutoPointer voronoiRegion;
    m_CellularAggregate->GetVoronoi( id1, voronoiRegion );
              
    VoronoiRegionType::PointIdIterator neighbor = voronoiRegion->PointIdsBegin();
    VoronoiRegionType::PointIdIterator end      = voronoiRegion->PointIdsEnd();

    while( neighbor != end )
      {
      const IdentifierType id2 = (*neighbor);  
      
      if( !cellPositions->IndexExists( id2) )
        {
        ++neighbor;
        continue;// if the neigbor has been removed, skip it
        }
      else
        {
        pointIds[1] = pointIdMap[id2];
        lines->InsertNextCell( 2, pointIds );
        ++neighbor;
        }
      }
    cellIt++;
    }

  this->m_PolyData->SetLines( lines );
  lines->Delete();

  m_SurfaceMapper->SetInput( m_PolyData );

}



void
CellularAggregateViewer
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << "Cellular aggregate " << m_CellularAggregate << std::endl;

}


void
CellularAggregateViewer
::DrawDelaunay(void) const
{
  vtkPoints* newPoints = vtkPoints::New();

  MeshConstPointer mesh = this->m_CellularAggregate->GetMesh();

  const unsigned int numberOfPoints = mesh->GetNumberOfPoints();

  newPoints->Allocate( numberOfPoints );

  PointType position;

  CellsConstIterator cellIt = mesh->GetPointData()->Begin();
  CellsConstIterator end    = mesh->GetPointData()->End();

  PointsContainer::ConstPointer cellPositions = mesh->GetPoints();

  vtkIdType pointId = 0;
  while( cellIt != end )
    {
    BioCellType * cell = cellIt.Value();
    const IdentifierType id = cell->GetSelfIdentifier();
    if( cellPositions->GetElementIfIndexExists( id, &position ) )
      {
      newPoints->InsertPoint(pointId++,position[0],position[1],position[2]);
      }
    cellIt++;
    }

  this->m_PolyData->SetPoints(newPoints);
  newPoints->Delete();

  if( pointId > 3 )
    {
    m_Delaunay2DFilter->SetInput( this->m_PolyData );
    m_SurfaceMapper->SetInput( m_Delaunay2DFilter->GetOutput() );
    }
  else
    {
    m_SurfaceMapper->SetInput( this->m_PolyData );
    }
}


void
CellularAggregateViewer
::SetRenderer(vtkRenderer * renderer)
{
  m_Renderer = renderer;
  m_Renderer->AddActor( m_SurfaceActor );
}



void
CellularAggregateViewer
::SetDisplayOption( DisplayOptionType option )
{
  m_DisplayOption = option;
}





} // end namespace bio

} // end namespace itk


