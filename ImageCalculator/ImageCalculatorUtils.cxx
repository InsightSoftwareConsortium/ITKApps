/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ImageCalculatorUtils.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "ImageCalculatorUtils.h"
#include "Imgmath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vcl_cmath.h>
#include <iostream>
#include <metaCommand.h>
#include <iostream>

void PrintDataTypeStrings(void)
{
  //Prints the Input and output data type strings.
  std::cout << "UCHAR" << std::endl;
  std::cout << "SHORT" << std::endl;
  std::cout << "USHORT" << std::endl;
  std::cout << "INT" << std::endl;
  std::cout << "UINT" << std::endl;
  std::cout << "FLOAT" << std::endl;
  std::cout << "DOUBLE" << std::endl;
}


int CompareNoCase( const std::string &s, const std::string& s2 )
{
  //Compare strings.
  std::string::const_iterator p = s.begin();
  std::string::const_iterator p2 = s2.begin();

  while ( p != s.end() && p2 != s2.end() )
      {
      if ( toupper(*p) != toupper(*p2) ) return (toupper(*p) < toupper(*p2)) ? -1 : 1;
      p++;
      p2++;
      }

  return ( s2.size() == s.size() ) ? 0 : (s.size() < s2.size()) ? -1 : 1;
}

//Call ImageCalculator process for 2d images.
extern void ImageCalculatorProcess2D(const std::string & InType,MetaCommand &command);

//Call ImageCalculator process for 3d images.
extern void ImageCalculatorProcess3D(const std::string & InType,MetaCommand &command);


bool ValidPixelType(const std::string &PixelType)
{
  const char *s = PixelType.c_str();
  // check to see if valid type
  if( ( CompareNoCase(s, std::string("UCHAR") ) ) &&
      ( CompareNoCase(s, std::string("SHORT") ) ) &&
      ( CompareNoCase(s, std::string("USHORT") ) ) &&
      ( CompareNoCase(s, std::string("INT") ) ) &&
      ( CompareNoCase(s, std::string("UINT") ) ) &&
      ( CompareNoCase(s, std::string("FLOAT") ) ) &&
      ( CompareNoCase(s, std::string("DOUBLE") ) ) )
    {
    return false;
    }
  return true;
}

//Ensure that the template code is only compiled once for both the real program and the test programs.
int PrimaryImageCalculatorRoutine(int argc, char *argv[])
{
  MetaCommand command;
  /*Input Image filenames. Any  number of input images may be given. The input filenames must be preceded by the number of inputs given.*/
  command.SetOption("in","in",true,"InputFile names");
  command.AddOptionField("in","in",MetaCommand::STRING,true);
  command.SetOptionComplete("in",true);

  //The images will be read using the Input pixel type.All the operations are performed in this pixel type.
  command.SetOption("InputPixelType","intype",false,"InputPixel Type");
  command.AddOptionField("InputPixelType","PixelType",MetaCommand::STRING,false,"FLOAT");

  //The dimensions of the input images. All the images should be of the same dimension.
  command.SetOption("InputDimensions","d",false,"Input Dimension 2 or 3");
  command.AddOptionField("InputDimensions","dims",MetaCommand::INT,false,"3");


  //Output filename.
  command.SetOption("OutputFilename","out",false,"OutputFile name");
  command.AddOptionField("OutputFilename","filename",MetaCommand::STRING,false);

  //The images will be written in this type. The default is input pixel type.
  command.SetOption("OutputPixelType","outtype",false,"OutputPixel Type");
  command.AddOptionField("OutputPixelType","PixelType",MetaCommand::STRING,false,"FLOAT");

  //Add the images.
  command.SetOption("Add","add",false,"Add Images");
  command.AddOptionField("Add","add",MetaCommand::FLAG,false);

  //Subtract the images.
  command.SetOption("Sub","sub",false,"Subtract Images");
  command.AddOptionField("Sub","sub",MetaCommand::FLAG,false);

  //Divide the images.
  command.SetOption("Div","div",false,"Divide Images");
  command.AddOptionField("Div","div",MetaCommand::FLAG,false);

  //Multiply the images.
  command.SetOption("Mul","mul",false,"Multiply Images");
  command.AddOptionField("Mul","mul",MetaCommand::FLAG,false);

  //Get the variance image
  command.SetOption("Var","var",false,"Variance of Images");
  command.AddOptionField("Var","var",MetaCommand::FLAG,false);

  //Get the Average image
  command.SetOption("Avg","avg",false,"Average Images");
  command.AddOptionField("Avg","avg",MetaCommand::FLAG,false);


  //Multiply the output with a constant scalar value.
  command.SetOption("OMulC","ofmulc",false,"Multiply Output Image with constant value");
  command.AddOptionField("OMulC","constant",MetaCommand::INT,false);

  //Multiply the inputs with a constant scalar value.
  command.SetOption("IMulC","ifmulc",false,"Multiply Accumulator Image with constant value");
  command.AddOptionField("IMulC","constant",MetaCommand::INT,false);

  //Divide the output with a constant scalar value.
  command.SetOption("ODivC","ofdivc",false,"Divide Output Image with constant value");
  command.AddOptionField("ODivC","constant",MetaCommand::INT,false);

  //Divide the inputs with a constant scalar value.
  command.SetOption("IDivC","ifdivc",false,"Divide Accumulator Image with constant value");
  command.AddOptionField("IDivC","constant",MetaCommand::INT,false);

  //Add a constant scalar value to the output.
  command.SetOption("OAddC","ofaddc",false,"Add Output Image with constant value");
  command.AddOptionField("OAddC","constant",MetaCommand::INT,false);

  //Add a constant scalar value to the inputs.
  command.SetOption("IAddC","ifaddc",false,"Add Accumulator Image with constant value");
  command.AddOptionField("IAddC","constant",MetaCommand::INT,false);

  //Subtract a constant scalar value from the output.
  command.SetOption("OSubC","ofsubc",false,"Subtract Output Image with constant value");
  command.AddOptionField("OSubC","constant",MetaCommand::INT,false);

  //Subtract a constant scalar value from the inputs.
  command.SetOption("ISubC","ifsubc",false,"Subtract Accumulator Image with constant value");
  command.AddOptionField("ISubC","constant",MetaCommand::INT,false);

  //Square the output image pixel values.
  command.SetOption("OSqr","ofsqr",false,"Square Accumulator Image before writing");
  command.AddOptionField("OSqr","ofsqr",MetaCommand::FLAG,false);

  //Input Binary Image.
  command.SetOption("Ifbin","ifbin",false,"Input Binary Image.");
  command.AddOptionField("Ifbin","ifbin",MetaCommand::FLAG,false);

  //Output Binary Image.
  command.SetOption("Ofbin","ofbin",false,"Output Binary Image.");
  command.AddOptionField("Ofbin","ofbin",MetaCommand::FLAG,false);


  //Square the input image pixel values.
  command.SetOption("ISqr","ifsqr",false,"Square Accumulator Image After reading");
  command.AddOptionField("ISqr","ifsqr",MetaCommand::FLAG,false);

  //Get the square root of the output image pixel values.
  command.SetOption("OSqrt","ofsqrt",false,"Square Root  Accumulator Image before writing");
  command.AddOptionField("OSqrt","ofsqrt",MetaCommand::FLAG,false);

  //Get the square root of the input image pixel values.
  command.SetOption("ISqrt","ifsqrt",false,"Sqrt Accumulator Image After reading");
  command.AddOptionField("ISqrt","ifsqrt",MetaCommand::FLAG,false);

  //Get the average pixel value of the output image.
  command.SetOption("StatAvg","statAVG",false,"Average Output Image Value");
  command.AddOptionField("StatAvg","statAVG",MetaCommand::FLAG,false);

  //Get the variance of the pixel value of the output image.
  command.SetOption("StatVAR","statVAR",false,"Variance of output Image");
  command.AddOptionField("StatVAR","statVAR",MetaCommand::FLAG,false);

  //Get the sum of the pixel value of the output image.
  command.SetOption("StatSUM","statSUM",false,"Sum of output Image Values");
  command.AddOptionField("StatSUM","statSUM",MetaCommand::FLAG,false);

  //Mask the output image with another image. The statistics are given for the masked portion.
  command.SetOption("Statmask","statmask",false,"Image to mask against.");
  command.AddOptionField("Statmask","File Name",MetaCommand::STRING,false);

  //If a mask is given then a pixel value should be entered and statitsics will be calculated for input image under this value in the mask.
  command.SetOption("Statmaskvalue","statmaskvalue",false,"Statistics in the image will be calculated for the pixels masked by this value.");
  command.AddOptionField("Statmaskvalue","constant",MetaCommand::INT,false);

  //Get the number of pixels in the output image.
  command.SetOption("StatNPX","statNPX",false,"Number of Pixels.");
  command.AddOptionField("StatNPX","statNPX",MetaCommand::FLAG,false);

  //Get the maximum pixel value of the output image.
  command.SetOption("StatMAX","statMAX",false,"Maximum of  Pixels.");
  command.AddOptionField("StatMAX","statMAX",MetaCommand::FLAG,false);

  //Get the minimum pixel value of the output image.
  command.SetOption("StatMIN","statMIN",false,"Minimum of Pixels.");
  command.AddOptionField("StatMIN","statMIN",MetaCommand::FLAG,false);

  //Get the absolute maximum pixel value of the output image.
  command.SetOption("StatAMX","statAMX",false,"Absolute Maximum of Pixels.");
  command.AddOptionField("StatAMX","statAMX",MetaCommand::FLAG,false);

  //Get the absolute minimum pixel value of the output image.
  command.SetOption("StatAMN","statAMN",false,"Absolute Minimum of Pixels.");
  command.AddOptionField("StatAMN","statAMN",MetaCommand::FLAG,false);

  //Show the description of the stat values which can be calculated.
  command.SetOption("Statallcodes","statallcodes",false,"Prints the coding of statistical varibles.");
  command.AddOptionField("Statallcodes","statallcodes",MetaCommand::FLAG,false);

  if (!command.Parse(argc,argv))
    {
    return 1;
    }


  //Test if the input data type is valid
  const std::string PixelType(command.GetValueAsString("InputPixelType","PixelType"));
  if (PixelType != "")
    {
    if(!ValidPixelType(PixelType))
      {
      std::cout << "Error. Invalid data type string specified with -intype!" << std::endl;
      std::cout << "Use one of the following:" << std::endl;
      PrintDataTypeStrings();
      exit(-1);
      }
    }


  const std::string OutPixelType(command.GetValueAsString("OutputPixelType","PixelType" ));

  if (OutPixelType!= "")
    {
    // check to see if valid type
    if(!ValidPixelType(OutPixelType))
      {
      std::cout << "Error. Invalid data type string specified with -outtype!" << std::endl;
      std::cout << "Use one of the following:" << std::endl;
      PrintDataTypeStrings();
      exit(-1);
      }
    }



  //Test that only one operation is set
  int opcount=0;
  if(command.GetValueAsBool("Add","add"))
    {
    opcount++;
    }
  if(command.GetValueAsBool("Sub","sub"))
    {
    opcount++;
    }
  if(command.GetValueAsBool("Mul","mul"))
    {
    opcount++;
    }
  if(command.GetValueAsBool("Div","div"))
    {
    opcount++;
    }
  if(command.GetValueAsBool("Var","var"))
    {
    opcount++;
    }
  if(command.GetValueAsBool("Avg","avg"))
    {
    opcount++;
    }
  if(opcount > 1)
    {
    std::cout << "Can only supply one operation to do [-add|-sub|-mul|-div|-var|-avg]" << std::endl;
    exit(-1);
    }

  //Call the ImageCalculatorReadWrite function based on the dimension.
  const std::string InType(command.GetValueAsString("InputPixelType",
          "PixelType"));
  const int dims = command.GetValueAsInt("InputDimensions","dims");


  switch(dims)
    {
    case 2:
      ImageCalculatorProcess2D(InType,command);
      break;
    case 3:
      ImageCalculatorProcess3D(InType,command);
      break;
    default:
      return 1;
    }

  return 0;
}
