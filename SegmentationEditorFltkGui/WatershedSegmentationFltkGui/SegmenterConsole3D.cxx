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
#include <fstream>

#include<fltkUtils.h>


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

  GradientButton->Observe( m_FilterGauss.GetPointer() );

  SegmentButton->Observe( m_FilterW.GetPointer() );

  m_Reader->AddObserver( itk::ModifiedEvent(), GradientButton->GetRedrawCommand());

  axes[0] = false;
  axes[1] = false;
  axes[2] = false;

  this->ShowStatus("Let's start by loading an image...");

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

  ShowInputImage();
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

  m_Flip->Update();
  m_InputViewer->SetImage( m_Flip->GetOutput() ); 
  m_InputViewer->Show();

  this->ShowStatus("Input Image");
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

  this->ShowStatus("Gradient Image");

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

  this->ShowStatus("Segmented Image");
  
}


/************************************
 *
 *  Save Segmented Image
 *
 ***********************************/
// void
// SegmenterConsole3D
// ::SaveSegmentedImage( void )
// {

//   const char * filename = fl_file_chooser("Image filename","*.raw","");
//   if( !filename )
//   {
//     return;
//   }

//   this->ShowStatus("Saving image file...");
  
//   try 
//   {
//     SegmenterConsole3DBase::SaveSegmentedImage( filename );
//   }
//   catch( ... ) 
//   {
//     this->ShowStatus("Problems writing file format");
//     return;
//   }

//   this->ShowStatus("File Saved");

// }

/************************************
 *
 *  Save Tree
 *
 ***********************************/
// void
// SegmenterConsole3D
// ::SaveTree( void )
// {

//   const char * filename = fl_file_chooser("Tree filename","*.tree","");
//   if( !filename )
//   {
//     return;
//   }

//   this->ShowStatus("Saving tree file...");
  
//   try 
//   {
//     SegmenterConsole3DBase::SaveTree( filename );
//   }
//   catch( ... ) 
//   {
//     this->ShowStatus("Problems writing file format");
//     return;
//   }

//   this->ShowStatus("File Saved");

// }

/***********************************
 *
 * SaveSegmentedData
 *
 ***********************************/
void SegmenterConsole3D::SaveSegmentedData()
{
 const char * filename = fl_file_chooser("Segmented Data","*.ws","");
   if( !filename )
   {
     return;
   }
   this->ShowStatus("Saving .ws, .raw, and .tree files...");
   std::string header = filename;

   int length = header.length();

   std::string raw_data = header.substr(0,length-3);
   raw_data += ".raw";
   std::string tree_data = header.substr(0,length-3);
   tree_data += ".tree";
   int x, y, z;
   x = (m_FilterW->GetOutput()->GetLargestPossibleRegion()).GetSize()[0];
   y = (m_FilterW->GetOutput()->GetLargestPossibleRegion()).GetSize()[1];
   z = (m_FilterW->GetOutput()->GetLargestPossibleRegion()).GetSize()[2];


   // write out filename.ws header
   std::ofstream out;
   out.open( header.c_str() );
   out << raw_data << "\n" << tree_data << "\n" << x << " " << y << " " << z << std::endl;

   out.close();

   // write out raw data
   m_FilterW->SetSegmentationFileName( raw_data.c_str() );

   // write out tree
   m_FilterW->SetTreeFileName( tree_data.c_str() );

   m_FilterW->Update();

   this->ShowStatus("Files Saved");
}

/**********************************
 *
 * Flip
 *
 **********************************/
void SegmenterConsole3D::Flip(int a)
{
  if(axes[a]) {
    axes[a] = false;
  }
  else {
    axes[a] = true;
  }

  m_Flip->SetFlipAxes(axes);
  ShowInputImage();
}

/***********************************
 *
 * ShowHelp
 *
 **********************************/
void SegmenterConsole3D::ShowHelp( void ) 
{
  ifuShowText(
"This application runs data through the watershed segmentation algorithm.\n \
The output is a labeled image of unsiged longs which represents the basic,\n\
oversegmented image, and a tree of merges among segments which allow\n \
for real-time resegmentation at different scales.  First choose the source \n \
data you want to use (MetaImage).  In most cases you will want a gradient \n \
volume which has been preprocessed. \n \
\n \
Next choose the parameters for the segmentation. The \"Max Flood Level\" \n \
parameter specifies to what extent the tree of segment merges is calculated.\n \
Usually you will only be interested in a certain range of merges among \n \
segments. All the continuous scale levels up to the \"Max Flood Level\" you \n \
specify will be available in your final output. \n \
\n \
Generation of the scale levels is the most time consuming part of the \n \
segmentation process, so be conservative in setting this parameter,\n \
especially with large data sets. The parameter ranges from 0.0 to 1.0, \n \
which 1.0 being the full scale of merges.  A good value to start with is\n \
 about 10%, or 0.10. \n \
\n \
The Lower Threshold thresholds small values out of the image. This is\n \
useful for further suppression of background noise in reducing the amount\n \
of oversegmentation in the basic segmentation of the image, which reduces\n \
the complexity of the merging hierarchy and increases the speed of the\n \
segmentation. Lower Threshold is expressed as a value from 0.0 to 1.0,\n \
i.e. no thresholding at all up to 100% of the height of the image.  Lower\n \
values are recommended to start. Start with about a 1% threshold level (0.01)\n \
and work up or down from there. \n \
\n \
To save the segmented data and the merge tree, click on  \"Save\" and\n \
specify a session. This will save a raw file and a tree file with that name.\n \
\n \
The time the algorithm takes to run is very sensitive to the amount of\n\
complexity in the image. As a general rule, eliminate as much detail\n \
in the image as you can afford to, either through more smoothing or by\n \
using a higher threshold value.\n\n");
}
