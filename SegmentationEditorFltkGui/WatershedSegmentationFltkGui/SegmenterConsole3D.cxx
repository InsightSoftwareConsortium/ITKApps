/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    SegmenterConsole3D.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

                    
#include <SegmenterConsole3D.h>
#include <FL/fl_file_chooser.H>
#include <stdio.h>


/************************************
 *
 *  Constructor
 *
 ***********************************/
SegmenterConsole3D
::SegmenterConsole3D()
{

  m_InputViewer = new InputImageViewerType;

  m_ViewerGrad = new ImageViewerType;

  m_InputViewer->SetLabel( "Input Image" );

  m_ViewerGrad->SetLabel( "Gradient Magnitude Image" );

  m_SegmentViewer = new RGBImageViewerType;

  m_SegmentViewer->SetLabel( "Segmented Image" );

  m_Colormapper = ColorMapFilterType::New();

  loadButton->Observe( m_Reader.GetPointer() );

  inputButton->Observe( m_Reader.GetPointer() );

  GradientButton->Observe( m_FilterGauss.GetPointer() );

  SegmentButton->Observe( m_FilterW.GetPointer() );

  m_Reader->AddObserver( itk::ModifiedEvent(), GradientButton->GetRedrawCommand());


  this->ShowStatus("Let's start by loading an image...");

  instructTextOutput->value("Help Text to be Updated Soon");
  /*
  instructTextOutput->value( 
"This module runs data through the actual watershed segmentation algorithm.  
The output is a labeled image of unsiged long integers which represents the 
basic, oversegmented image, and a tree of merges among segments which allow 
for real-time resegmentation at different scales.  First choose the source 
data you want to use (MetaImage).  In most cases you will want a gradient 
volume which has been preprocessed. 

The next step is to choose the parameters for the segmentation.  
The \"Max Flood Level\" parameter specifies to what extent the tree of 
segment merges is calculated.  Usually you will only be interested in a 
certain range of merges among segments.  For example, say you are looking 
for very fine-grained structures in an image. Merging to a high scale level 
in this case would be wasted effort since the structures you are interested 
are probably obscured after a very low scale level.  All the continuous 
scale levels up to the \"Max Flood Level\" you specify will be available 
in your final output. 

It is important to note that the generation of the scale levels is the most 
time consuming part of the segmentation process, so it is wise to be 
conservative in setting this parameter, especially with large data sets.  
The parameter is expressed at a range from 0.0 to 1.0, which 1.0 being the 
full scale of merges.  A good value to start with is about 10%, or 0.10. 

The Lower Threshold is a preprocessing step which thresholds small values 
out of the image.  This is useful for further suppression of background 
noise and is very important in reducing the amount of oversegmentation 
in the basic segmentation of the image, which reduces the complexity of the 
merging hierarchy and increases the speed of the segmentation. Lower 
Threshold is expressed as a value from 0.0 to 1.0, i.e. no thresholding 
at all up to 100% of the height of the image.  Lower values are recommended 
to start.  Start with about a 1% threshold level (0.01) and work up or 
down from there. 

To write out the segmented data and the merge tree, simply click on the 
buttons \"Image Filename\" and \"Tree Filename\" and specify filenames.  
Then click on the \"Show Watershed Segmentation Image\" to re-execute the 
filter and write out the files. Merely changing the filenames won't require 
the original execution time.

A NOTE ON PROCESSING TIMES The time the algorithm takes to run is very 
sensitive to the amount of complexity in the image.  A small difference in the
Lower Threshold value, for example, could change execution time from a minute 
or two into several hours for a large volume.  As a general rule, eliminate 
as much detail in the image as you can afford to, either through more smoothing 
or by using a higher threshold value.\n\n" );
*/

}




/************************************
 *
 *  Destructor
 *
 ***********************************/
SegmenterConsole3D
::~SegmenterConsole3D()
{
  if( m_InputViewer ) 
    {
      delete m_InputViewer;
      m_InputViewer = 0;
    }
  if( m_ViewerGrad ) 
    {
      delete m_ViewerGrad;
      m_ViewerGrad = 0;
    }
   if( m_SegmentViewer ) 
     {
       delete m_SegmentViewer;
       m_SegmentViewer = 0;
     }
 }



 
/************************************
 *
 *  Load
 *
 ***********************************/
void
SegmenterConsole3D
::Load( void )
{

  const char * filename = fl_file_chooser("Image filename","*.mh[da]","");
  if( !filename )
  {
    return;
  }

  this->ShowStatus("Loading image file...");
  
  try 
  {
    SegmenterConsole3DBase::Load( filename );
  }
  catch( ... ) 
  {
    this->ShowStatus("Problems reading file format");
    controlsGroup->deactivate();
    return;
  }


  this->ShowStatus("File Loaded");

  controlsGroup->activate();


}

   
/************************************
 *
 *  Show
 *
 ***********************************/
void
SegmenterConsole3D
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
SegmenterConsole3D
::Hide( void )
{
  consoleWindow->hide();

  m_InputViewer->Hide();

  m_ViewerGrad->Hide();

  m_SegmentViewer->Hide();
  
  helpWin->hide();

}





/************************************
 *
 *  Quit
 *
 ***********************************/
void
SegmenterConsole3D
::Quit( void )
{
  Hide();
}




 
/************************************
 *
 *  Show Status
 *
 ***********************************/
void
SegmenterConsole3D
::ShowStatus( const char * message )
{
  SegmenterConsole3DBase::ShowStatus( message );
  statusTextOutput->value( message );
  Fl::check();
}




 
/************************************
 *
 *  Show Input Image
 *
 ***********************************/
void
SegmenterConsole3D
::ShowInputImage( void )
{

  if( ! (m_ImageFileNameAvailable) ) 
  {
    this->ShowStatus("Please provide a image filename first");
    return;
  }

  m_Reader->Update();
  m_InputViewer->SetImage( m_Reader->GetOutput() ); 
  m_InputViewer->Show();
}


/************************************
 *
 *  Show Gradient Image
 *
 ***********************************/
void
SegmenterConsole3D
::ShowGradientImage( void )
{

  m_FilterGrad->Update();
  m_FilterGauss->Update();
  m_ViewerGrad->SetImage( m_FilterGauss->GetOutput() );  
  m_ViewerGrad->Show();

}

/************************************
 *
 *  Show Segmented Image
 *
 ***********************************/
void
SegmenterConsole3D
::ShowSegmentedImage( void )
{
  m_FilterGauss->Update();
  m_FilterW->SetInput( m_FilterGauss->GetOutput() );
  m_FilterW->Update();
    
  m_Colormapper->SetInput( m_FilterW->GetOutput() );
  m_Colormapper->Update();

  m_SegmentViewer->SetImage( m_Colormapper->GetOutput() );
  m_SegmentViewer->Show();
  
}


/************************************
 *
 *  Save Segmented Image
 *
 ***********************************/
void
SegmenterConsole3D
::SaveSegmentedImage( void )
{

  const char * filename = fl_file_chooser("Image filename","*.raw","");
  if( !filename )
  {
    return;
  }

  this->ShowStatus("Saving image file...");
  
  try 
  {
    SegmenterConsole3DBase::SaveSegmentedImage( filename );
  }
  catch( ... ) 
  {
    this->ShowStatus("Problems writing file format");
    return;
  }

  this->ShowStatus("File Saved");

}

/************************************
 *
 *  Save Tree
 *
 ***********************************/
void
SegmenterConsole3D
::SaveTree( void )
{

  const char * filename = fl_file_chooser("Tree filename","*.tree","");
  if( !filename )
  {
    return;
  }

  this->ShowStatus("Saving tree file...");
  
  try 
  {
    SegmenterConsole3DBase::SaveTree( filename );
  }
  catch( ... ) 
  {
    this->ShowStatus("Problems writing file format");
    return;
  }

  this->ShowStatus("File Saved");

}
