/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SegmenterConsole3DBase.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include <SegmenterConsole3DBase.h>


/************************************
 *
 *  Constructor
 *
 ***********************************/
SegmenterConsole3DBase 
::SegmenterConsole3DBase()
{

  m_ImageFileNameAvailable = false;

  m_Reader     = VolumeReaderType::New();
  
  m_FilterGrad = GradientMagnitudeFilterType::New();
  m_FilterGrad->SetInput( m_Reader->GetOutput() );

  m_FilterGauss = DiscreteGaussianImageFilterType::New();
  m_FilterGauss->SetVariance( 0.0 );
  m_FilterGauss->SetInput( m_FilterGrad->GetOutput() );

  m_FilterW = WatershedFilterType::New();
  m_FilterW->SetLevel(0.1);
  m_FilterW->SetThreshold(0.01);
  m_FilterW->SetSegmentationFileName("");
  m_FilterW->SetTreeFileName("");
  m_FilterW->SetInput( m_FilterGauss->GetOutput() );

  m_Rescaler  = RescaleIntensityImageFilterType::New();
  m_Rescaler->SetOutputMinimum( 0 );
  m_Rescaler->SetOutputMaximum( 255 );
  m_Rescaler->SetInput(  m_FilterGauss->GetOutput()  );
}




/************************************
 *
 *  Destructor
 *
 ***********************************/
SegmenterConsole3DBase 
::~SegmenterConsole3DBase()
{

}



 
/************************************
 *
 *  Load
 *
 ***********************************/
void
SegmenterConsole3DBase 
::Load( const char * filename )
{
  if( !filename )
  {
    return;
  }

  m_Reader->SetFileName( filename );

  m_ImageFileNameAvailable = true;

}


/************************************
 *
 *  Show Progress
 *
 ***********************************/
void
SegmenterConsole3DBase 
::ShowProgress( float )
{

}


/************************************
 *
 *  Show Status
 *
 ***********************************/
void
SegmenterConsole3DBase 
::ShowStatus( const char * )
{

}


 
/************************************
 *
 *  Save Segmented Image
 *
 ***********************************/
void
SegmenterConsole3DBase 
::SaveSegmentedImage( const char * filename )
{
  if( !filename )
  {
    return;
  }

  m_FilterW->SetSegmentationFileName( filename );

}

/************************************
 *
 *  Save Tree
 *
 ***********************************/
void
SegmenterConsole3DBase 
::SaveTree( const char * filename )
{
  if( !filename )
  {
    return;
  }

  m_FilterW->SetTreeFileName( filename );
}


/************************************
 *
 *  Show Gradient Image
 *
 ***********************************/
void
SegmenterConsole3DBase 
::ShowGradientImage()
{

}

/************************************
 *
 *  Show Segmented Image
 *
 ***********************************/
void
SegmenterConsole3DBase 
::ShowSegmentedImage()
{

}


/************************************
 *
 *  Set Variance
 *
 ***********************************/
void
SegmenterConsole3DBase 
::SetVariance( double value )
{
  m_FilterGauss->SetVariance( value );
}

/************************************
 *
 *  Set Max Flood
 *
 ***********************************/
void
SegmenterConsole3DBase 
::SetMaxFlood( double value )
{
  m_FilterW->SetLevel( value );
}

/************************************
 *
 *  Set Threshold
 *
 ***********************************/
void
SegmenterConsole3DBase 
::SetThreshold( double value )
{
  m_FilterW->SetThreshold( value );
}


/************************************
 *
 *  Set Tree File Name
 *
 ***********************************/
void
SegmenterConsole3DBase 
::SetTreeFileName( const char* value )
{
  m_FilterW->SetTreeFileName( value );
}

/************************************
 *
 *  Set Segmented File Name
 *
 ***********************************/
void
SegmenterConsole3DBase 
::SetSegmentedFileName( const char* value )
{
  m_FilterW->SetSegmentationFileName( value );
}






