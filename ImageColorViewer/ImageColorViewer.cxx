/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageColorViewer.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <iostream>

#include <itkImage.h>
#include <itkRGBPixel.h>
#include <itkImageFileReader.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include "itkMinimumMaximumImageCalculator.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_File_Chooser.H>

#include <GLColorSliceView.h>
#include <GLSliceView.h>
#include <SliceView.h>

#include "ImageColorViewerGUI.h"


Fl_Window *form;

int usage(void)
   {
   std::cout << "ImageColorViewer" << std::endl;
   std::cout << std::endl;
   std::cout << "ImageColorViewer <Filename>" << std::endl;
   std::cout << "ImageColorViewer <Filename> <filename> [<filename>]" 
             << std::endl;
   std::cout << std::endl;

   return 1;
   }

int main(int argc, char **argv)
  {
  char *fName;
  typedef itk::Image< itk::RGBPixel<unsigned char>, 3 > ColorImageType;
  ColorImageType::Pointer imP;

  if(argc < 2 || argc > 4)
    {
    return usage();
    }
   else
    {
    if(argc == 1)
      {
      fName = fl_file_chooser("Pick a Image file", "*.mh*", ".");
      if(fName == NULL || strlen(fName)<1)
        {
        return 0;
        }
      }
    else
      {
      if(argc == 2 && argv[1][0] != '-')
        {
        fName = argv[argc-1];
        std::cout << "Loading File: " << fName << std::endl;
        typedef itk::ImageFileReader< ColorImageType > VolumeReaderType;
        VolumeReaderType::Pointer reader = VolumeReaderType::New();
      
        reader->SetFileName(fName);
      
        imP = reader->GetOutput();
      
        try
          {
          reader->Update();
          }
        catch( ... )
          {
          std::cout << "Problems reading file format" << std::endl;
          return 1;
          }
        std::cout << "...Done Loading File" << std::endl;
        }
      else
        {
        if( argc == 3 || argc == 4 )
          {
          int argNum = 1;
          while( argNum < argc ) 
            {
            fName = argv[argNum];
            std::cout << "Loading File: " << fName << std::endl;
            typedef itk::Image< float, 3 > ImageType;
            typedef itk::ImageFileReader< ImageType > VolumeReaderType;
            VolumeReaderType::Pointer reader = VolumeReaderType::New();
          
            reader->SetFileName(fName);
          
            try
              {
              reader->Update();
              }
            catch( ... )
              {
              std::cout << "Problems reading file format" << std::endl;
              return 1;
              }
            std::cout << "...Done Loading File" << std::endl;

            ImageType::Pointer tmpImP;
            tmpImP = reader->GetOutput();

            typedef itk::MinimumMaximumImageCalculator<ImageType>
              CalculatorType;
            CalculatorType::Pointer calculator = CalculatorType::New();
            calculator->SetImage( tmpImP );
            calculator->Compute();
        
            double tmpImPMin = calculator->GetMinimum();
            double tmpImPMax = calculator->GetMaximum();
  
            if( argNum == 1 )
              {
              imP = ColorImageType::New();
              imP->CopyInformation( tmpImP );
              imP->SetRegions( tmpImP->GetLargestPossibleRegion() );
              imP->Allocate();
              itk::ImageRegionIterator< ColorImageType > imIter( imP,
                imP->GetLargestPossibleRegion() );
              itk::ImageRegionIterator< ImageType > tmpImIter( tmpImP,
                tmpImP->GetLargestPossibleRegion() );
              while( !imIter.IsAtEnd() )
                {
                RGBPixel< unsigned char > p;
                p[0] = ((tmpImIter.Get()-tmpImPMin) 
                        / (tmpImPMax-tmpImPMin)) * 255;
                imIter.Set( p );
                ++imIter;
                ++tmpImIter;
                }
              }
            else
              {
              itk::ImageRegionIteratorWithIndex< ColorImageType > imIter(
                imP, imP->GetLargestPossibleRegion() );
              ColorImageType::PointType pnt;
              ColorImageType::IndexType indx;
              while( !imIter.IsAtEnd() )
                {
                RGBPixel< unsigned char > p;
                p = imIter.Get();
                indx = imIter.GetIndex();
                imP->TransformIndexToPhysicalPoint( indx, pnt );
                if( tmpImP->TransformPhysicalPointToIndex( pnt, indx ) )
                  {
                  p[argNum-1] = ((tmpImP->GetPixel(indx)-tmpImPMin) 
                                 / (tmpImPMax-tmpImPMin)) * 255;
                  }
                imIter.Set( p );
                ++imIter;
                }
              }
            ++argNum;
            }
          }
        else
          {
          return usage();
          }
        }
      }
    }


  char mainName[255];
  sprintf(mainName, "metaView: %s", fName);

  std::cout << std::endl;
  std::cout << "For directions on interacting with the window," 
            << std::endl;
  std::cout << "   type 'h' within the window" << std::endl;
  
  form = make_window();

  tkMain->label(mainName);
  
  tkWin->SetInputImage(imP);
  
  form->show();
  tkWin->show();
  tkWin->update();

  Fl::run();
  
  return 1;
  }
