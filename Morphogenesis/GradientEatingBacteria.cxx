/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    GradientEatingBacteria.cxx
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
#include "GradientEatingBacteria.h"
#include "CellularAggregate.h"




namespace bio {


// Class static variables
double    GradientEatingBacteria::ChemoAttractantLowThreshold  = 220.0f;
double    GradientEatingBacteria::ChemoAttractantHighThreshold = 200.0f;

Cell::ColorType GradientEatingBacteria::WellNourishedColor;
Cell::ColorType GradientEatingBacteria::HopefullColor;
Cell::ColorType GradientEatingBacteria::StarvingColor;



/**
 *    Constructor
 */ 
GradientEatingBacteria
::GradientEatingBacteria()
{
  m_ScheduleApoptosis    = false;
  m_ChemoAttractantLevel = 200.0f;
}







/**
 *    Destructor
 */ 
GradientEatingBacteria
::~GradientEatingBacteria()
{
}





/**
 *    Cell Division
 */ 
Cell *
GradientEatingBacteria
::CreateNew()
{
  GradientEatingBacteria * bacteria = new GradientEatingBacteria;
  bacteria->m_ParentIdentifier = m_SelfIdentifier;
  return bacteria;
}


/**
 *    Create Egg (A cell with generation counter in zero)
 */ 
Cell *
GradientEatingBacteria
::CreateEgg()
{

  // Create the representation for 3D
  SphereShape = fltk::Sphere3D::New();
  SphereShape->SetNumberOfSlices( 12 );
  SphereShape->SetNumberOfStacks( 6 );

  std::ifstream file;

  file.open("GradientEatingBacteria.dat");
  file >> Cell::DefaultRadius;
  file >> Cell::GrowthRadiusIncrement;
  file >> Cell::GrowthRadiusLimit;
  file >> GradientEatingBacteria::ChemoAttractantLowThreshold;
  file >> GradientEatingBacteria::ChemoAttractantHighThreshold;
  file.close();

  SetGrowthMaximumLatencyTime( 100 );
  SetDivisionMaximumLatencyTime( 100 );

  SetMaximumGenerationLimit( 40 );

  WellNourishedColor.Set(    0.0, 0.0, 1.0 );
  HopefullColor.Set(         0.0, 1.0, 0.0 );
  StarvingColor.Set(         1.0, 0.0, 0.0 );

  SetDefaultColor( HopefullColor );

  GradientEatingBacteria * bacteria = new GradientEatingBacteria;
  bacteria->m_ParentIdentifier = 0;
  bacteria->m_SelfIdentifier = 1;
  bacteria->m_Generation = 0;
  
  bacteria->m_Genome = new Genome;

  return bacteria;
  
}



/**
 *    Check point for Apoptosis
 */ 
bool
GradientEatingBacteria
::CheckPointApoptosis(void) 
{
  bool super = SuperClass::CheckPointApoptosis();

  if( !super )
    {
    return super;
    }

  bool here = false;

  if( m_ChemoAttractantLevel < ChemoAttractantLowThreshold  ||
      m_ChemoAttractantLevel > ChemoAttractantHighThreshold    )
    {
    // here = true;
    }

  return ( super || here );

}



/**
 *    Check point before DNA replication (S phase)
 *    This check point will control the entrance in the replication stage.
 *    It returns true when conditions required for replication are satisfied.
 */ 
bool
GradientEatingBacteria
::CheckPointDNAReplication(void) 
{
  bool super = SuperClass::CheckPointDNAReplication();

  if( !super )
    {
    return super;
    }

  bool here = false;

  if( !m_ScheduleApoptosis )
    {

    if( m_ChemoAttractantLevel > ChemoAttractantLowThreshold  &&
        m_ChemoAttractantLevel < ChemoAttractantHighThreshold    )
      {
      here = true;
      }
    }

  return ( super && here );

}



/**
 *    Add a force to the cumulator for evaluating pressure
 */ 
void
GradientEatingBacteria
::AddForce( const VectorType & force )
{
  if( m_ChemoAttractantLevel > ChemoAttractantLowThreshold &&
      m_ChemoAttractantLevel < ChemoAttractantHighThreshold   )
    {
    SuperClass::AddForce( force );
    }
  else
    {
    // no force so it is fixed in place....
    }

}



/**
 *    Clear the Pressure cumulator at 
 *    the same time the Force cumulator
 *    is cleared.
 */ 
void
GradientEatingBacteria
::ClearForce( void )
{
  SuperClass::ClearForce();
}





void
GradientEatingBacteria
::Mitosis( void )
{
  SuperClass::Mitosis();
}
 



void
GradientEatingBacteria
::ReceptorsReading( void )
{
  SuperClass::ReceptorsReading();
  
  CellularAggregate::SubstrateType::PixelType substrate0 =
              m_Aggregate->GetSubstrateValue( m_SelfIdentifier, 0 );
  
  m_ChemoAttractantLevel = substrate0;

}
 


/**
 *   Compute the Gene Network
 *   This method update the level of expression of 
 *   all the genes in the cell's genome.
 *   see: http://www.ingeneue.org  for details
 */ 
void
GradientEatingBacteria
::ComputeGeneNetwork(void) 
{
  // Color the bacteria acording to substrate.
  // This is done by generating pigments.

  if( m_ChemoAttractantLevel > ChemoAttractantHighThreshold )
    {
    m_Genome->SetExpressionLevel( RedGene,   WellNourishedColor.GetRed() );
    m_Genome->SetExpressionLevel( GreenGene, WellNourishedColor.GetGreen() );
    m_Genome->SetExpressionLevel( BlueGene,  WellNourishedColor.GetBlue() );
    }
  else if( m_ChemoAttractantLevel > ChemoAttractantLowThreshold )
    {
    m_Genome->SetExpressionLevel( RedGene,   HopefullColor.GetRed() );
    m_Genome->SetExpressionLevel( GreenGene, HopefullColor.GetGreen() );
    m_Genome->SetExpressionLevel( BlueGene,  HopefullColor.GetBlue() );
    }
  else
    {
    m_Genome->SetExpressionLevel( RedGene,   StarvingColor.GetRed() );
    m_Genome->SetExpressionLevel( GreenGene, StarvingColor.GetGreen() );
    m_Genome->SetExpressionLevel( BlueGene,  StarvingColor.GetBlue() );
    }

  const double cdk2E = Genome::Sigmoide( 2.0, -0.5, m_Pressure );
  m_Genome->SetExpressionLevel( Cdk2E, cdk2E );

}





}  // end namespace bio
