/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkITKArchetypeImageSeriesReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkITKArchetypeImageSeriesReader.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"

#include "itkExceptionObject.h"

// turn itk exceptions into vtk errors
#undef itkExceptionMacro  
#define itkExceptionMacro(x) \
  { \
  ::itk::OStringStream message; \
  message << "itk::ERROR: " << this->GetNameOfClass() \
          << "(" << this << "): " x; \
  std::cerr << message.str().c_str() << std::endl; \
  }

#undef itkGenericExceptionMacro  
#define itkGenericExceptionMacro(x) \
  { \
  ::itk::OStringStream message; \
  message << "itk::ERROR: " x; \
  std::cerr << message.str().c_str() << std::endl; \
  }

#include "itkArchetypeSeriesFileNames.h"
#include "itkImage.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileReader.h"
#include "itkImportImageContainer.h"
#include "itkImageRegion.h"

vtkCxxRevisionMacro(vtkITKArchetypeImageSeriesReader, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkITKArchetypeImageSeriesReader);

//----------------------------------------------------------------------------
vtkITKArchetypeImageSeriesReader::vtkITKArchetypeImageSeriesReader()
{
  this->Archetype = NULL;
  
  this->FileNameSliceOffset = 0;
  this->FileNameSliceSpacing = 1;
  this->FileNameSliceCount = 0;

  for (int i = 0; i < 3; i++)
    {
    this->DefaultDataSpacing[i] = 1.0;
    this->DefaultDataOrigin[i] = 0.0;
    }
}

//----------------------------------------------------------------------------
vtkITKArchetypeImageSeriesReader::~vtkITKArchetypeImageSeriesReader()
{ 
  if (this->Archetype)
    {
    delete [] this->Archetype;
    this->Archetype = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkITKArchetypeImageSeriesReader::PrintSelf(ostream& os, vtkIndent indent)
{
  int idx;
  
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Archetype: " <<
    (this->Archetype ? this->Archetype : "(none)") << "\n";

  os << indent << "FileNameSliceOffset: " 
     << this->FileNameSliceOffset << "\n";
  os << indent << "FileNameSliceSpacing: " 
     << this->FileNameSliceSpacing << "\n";
  os << indent << "FileNameSliceCount: " 
     << this->FileNameSliceCount << "\n";

  os << indent << "DefaultDataSpacing: (" << this->DefaultDataSpacing[0];
  for (idx = 1; idx < 3; ++idx)
    {
    os << ", " << this->DefaultDataSpacing[idx];
    }
  os << ")\n";
  
  os << indent << "DefaultDataOrigin: (" << this->DefaultDataOrigin[0];
  for (idx = 1; idx < 3; ++idx)
    {
    os << ", " << this->DefaultDataOrigin[idx];
    }
  os << ")\n";
  
}


//----------------------------------------------------------------------------
// This method returns the largest data that can be generated.
void vtkITKArchetypeImageSeriesReader::ExecuteInformation()
{
  vtkImageData *output = this->GetOutput();
  int extent[6];  

  // Generate filenames from the Archetype
  itk::ArchetypeSeriesFileNames::Pointer fit = itk::ArchetypeSeriesFileNames::New();
  fit->SetArchetype (this->Archetype);

  std::vector<std::string> candidateFiles = fit->GetFileNames();

  // Reduce the selection of filenames
  int maximumCount;
  if (this->FileNameSliceCount == 0)
    {
    maximumCount = candidateFiles.size();
    }
  else
    {
    maximumCount = this->FileNameSliceCount;
    }

  this->FileNames.resize(0);
  for (int f = this->FileNameSliceOffset;
       f < maximumCount;
       f += this->FileNameSliceSpacing)
    {
    this->FileNames.push_back(candidateFiles[f]);
    }

  vtkFloatingPointType spacing[3];
  vtkFloatingPointType origin[3];
  
#define vtkITKOutputInformationFromSeries(typeN, type) \
    case typeN: \
    {\
      typedef itk::Image<type,3> image##typeN;\
      itk::ImageSeriesReader<image##typeN>::Pointer reader##typeN = \
            itk::ImageSeriesReader<image##typeN>::New(); \
      reader##typeN->SetFileNames(this->FileNames); \
      reader##typeN->GenerateOutputInformation(); \
      for (int i = 0; i < 3; i++) \
        { \
        spacing[i] = reader##typeN->GetOutput()->GetSpacing()[i]; \
        origin[i] = reader##typeN->GetOutput()->GetOrigin()[i]; \
        } \
      itk::ImageRegion<3> region##typeN = reader##typeN->GetOutput()->GetLargestPossibleRegion();\
      extent[0] = region##typeN.GetIndex()[0];\
      extent[1] = region##typeN.GetIndex()[0] + region##typeN.GetSize()[0] - 1;\
      extent[2] = region##typeN.GetIndex()[1];\
      extent[3] = region##typeN.GetIndex()[1] + region##typeN.GetSize()[1] - 1;\
      extent[4] = region##typeN.GetIndex()[2];\
      extent[5] = region##typeN.GetIndex()[2] + region##typeN.GetSize()[2] - 1;\
    }\
    break

#define vtkITKOutputInformationFromFile(typeN, type) \
    case typeN: \
    {\
      typedef itk::Image<type,3> image2##typeN;\
      itk::ImageFileReader<image2##typeN>::Pointer reader2##typeN = \
            itk::ImageFileReader<image2##typeN>::New(); \
      reader2##typeN->SetFileName(this->FileNames[0].c_str()); \
      reader2##typeN->GenerateOutputInformation(); \
      for (int i = 0; i < 3; i++) \
        { \
        spacing[i] = reader2##typeN->GetOutput()->GetSpacing()[i]; \
        origin[i] = reader2##typeN->GetOutput()->GetOrigin()[i]; \
        } \
      itk::ImageRegion<3> region2##typeN = reader2##typeN->GetOutput()->GetLargestPossibleRegion();\
      extent[0] = region2##typeN.GetIndex()[0];\
      extent[1] = region2##typeN.GetIndex()[0] + region2##typeN.GetSize()[0] - 1;\
      extent[2] = region2##typeN.GetIndex()[1];\
      extent[3] = region2##typeN.GetIndex()[1] + region2##typeN.GetSize()[1] - 1;\
      extent[4] = region2##typeN.GetIndex()[2];\
      extent[5] = region2##typeN.GetIndex()[2] + region2##typeN.GetSize()[2] - 1;\
    }\
    break

  // If there is only one file in the series, just use an image file reader
  if (this->FileNames.size() == 1)
    {
    switch (this->OutputScalarType)
      {
      vtkITKOutputInformationFromFile(VTK_DOUBLE, double);
      vtkITKOutputInformationFromFile(VTK_FLOAT, float);
      vtkITKOutputInformationFromFile(VTK_LONG, long);
      vtkITKOutputInformationFromFile(VTK_UNSIGNED_LONG, unsigned long);
      vtkITKOutputInformationFromFile(VTK_INT, int);
      vtkITKOutputInformationFromFile(VTK_UNSIGNED_INT, unsigned int);
      vtkITKOutputInformationFromFile(VTK_SHORT, short);
      vtkITKOutputInformationFromFile(VTK_UNSIGNED_SHORT, unsigned short);
      vtkITKOutputInformationFromFile(VTK_CHAR, char);
      vtkITKOutputInformationFromFile(VTK_UNSIGNED_CHAR, unsigned char);
      default:
        vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
      }
    }
  else
    {
    switch (this->OutputScalarType)
      {
      vtkITKOutputInformationFromSeries(VTK_DOUBLE, double);
      vtkITKOutputInformationFromSeries(VTK_FLOAT, float);
      vtkITKOutputInformationFromSeries(VTK_LONG, long);
      vtkITKOutputInformationFromSeries(VTK_UNSIGNED_LONG, unsigned long);
      vtkITKOutputInformationFromSeries(VTK_INT, int);
      vtkITKOutputInformationFromSeries(VTK_UNSIGNED_INT, unsigned int);
      vtkITKOutputInformationFromSeries(VTK_SHORT, short);
      vtkITKOutputInformationFromSeries(VTK_UNSIGNED_SHORT, unsigned short);
      vtkITKOutputInformationFromSeries(VTK_CHAR, char);
      vtkITKOutputInformationFromSeries(VTK_UNSIGNED_CHAR, unsigned char);
      default:
        vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
      }
    }
  // If it looks like the reader did not provide the spacing and
  // origin, modify the spacing and origin with the defaults
  for (int j = 0; j < 3; j++)
    {
    if (spacing[j] == 1.0)
      {
      spacing[j] = this->DefaultDataSpacing[j];
      }
    if (origin[j] == 0.0)
      {
      origin[j] = this->DefaultDataOrigin[j];
      }
    }

  output->SetSpacing(spacing);
  output->SetOrigin(origin);

  output->SetWholeExtent(extent);

  output->SetScalarType(this->OutputScalarType);
  output->SetNumberOfScalarComponents(1);
}

//----------------------------------------------------------------------------
// This function reads a data from a file.  The datas extent/axes
// are assumed to be the same as the file extent/order.
void vtkITKArchetypeImageSeriesReader::ExecuteData(vtkDataObject *output)
{
  if (!this->Archetype)
    {
    vtkErrorMacro("An Archetype must be specified.");
    return;
    }

  vtkImageData *data = vtkImageData::SafeDownCast(output);
  data->UpdateInformation();
  data->SetExtent(0,0,0,0,0,0);
  data->AllocateScalars();
  data->SetExtent(data->GetWholeExtent());
#define vtkITKExecuteDataFromSeries(typeN, type) \
    case typeN: \
    {\
      typedef itk::Image<type,3> image##typeN;\
      itk::ImageSeriesReader<image##typeN>::Pointer reader##typeN = \
            itk::ImageSeriesReader<image##typeN>::New(); \
      reader##typeN->SetFileNames(this->FileNames); \
      reader##typeN->UpdateLargestPossibleRegion();\
      itk::ImportImageContainer<unsigned long, type>::Pointer PixelContainer##typeN;\
      PixelContainer##typeN = reader##typeN->GetOutput()->GetPixelContainer();\
      this->Ptr = static_cast<void *> (PixelContainer##typeN->GetBufferPointer());\
      (dynamic_cast<vtkImageData *>( output))->GetPointData()->GetScalars()->SetVoidArray(this->Ptr, PixelContainer##typeN->Size(), 0);\
      PixelContainer##typeN->ContainerManageMemoryOff();\
    }\
    break

#define vtkITKExecuteDataFromFile(typeN, type) \
    case typeN: \
    {\
      typedef itk::Image<type,3> image2##typeN;\
      itk::ImageFileReader<image2##typeN>::Pointer reader2##typeN = \
            itk::ImageFileReader<image2##typeN>::New(); \
      reader2##typeN->SetFileName(this->FileNames[0].c_str()); \
      reader2##typeN->UpdateLargestPossibleRegion();\
      itk::ImportImageContainer<unsigned long, type>::Pointer PixelContainer2##typeN;\
      PixelContainer2##typeN = reader2##typeN->GetOutput()->GetPixelContainer();\
      this->Ptr = static_cast<void *> (PixelContainer2##typeN->GetBufferPointer());\
      (dynamic_cast<vtkImageData *>( output))->GetPointData()->GetScalars()->SetVoidArray(this->Ptr, PixelContainer2##typeN->Size(), 0);\
      PixelContainer2##typeN->ContainerManageMemoryOff();\
    }\
    break

  // If there is only one file in the series, just use an image file reader
  if (this->FileNames.size() == 1)
    {
    switch (this->OutputScalarType)
      {
      vtkITKExecuteDataFromFile(VTK_DOUBLE, double);
      vtkITKExecuteDataFromFile(VTK_FLOAT, float);
      vtkITKExecuteDataFromFile(VTK_LONG, long);
      vtkITKExecuteDataFromFile(VTK_UNSIGNED_LONG, unsigned long);
      vtkITKExecuteDataFromFile(VTK_INT, int);
      vtkITKExecuteDataFromFile(VTK_UNSIGNED_INT, unsigned int);
      vtkITKExecuteDataFromFile(VTK_SHORT, short);
      vtkITKExecuteDataFromFile(VTK_UNSIGNED_SHORT, unsigned short);
      vtkITKExecuteDataFromFile(VTK_CHAR, char);
      vtkITKExecuteDataFromFile(VTK_UNSIGNED_CHAR, unsigned char);
      default:
        vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
      }
    }
  else
    {
    switch (this->OutputScalarType)
      {
      vtkITKExecuteDataFromSeries(VTK_DOUBLE, double);
      vtkITKExecuteDataFromSeries(VTK_FLOAT, float);
      vtkITKExecuteDataFromSeries(VTK_LONG, long);
      vtkITKExecuteDataFromSeries(VTK_UNSIGNED_LONG, unsigned long);
      vtkITKExecuteDataFromSeries(VTK_INT, int);
      vtkITKExecuteDataFromSeries(VTK_UNSIGNED_INT, unsigned int);
      vtkITKExecuteDataFromSeries(VTK_SHORT, short);
      vtkITKExecuteDataFromSeries(VTK_UNSIGNED_SHORT, unsigned short);
      vtkITKExecuteDataFromSeries(VTK_CHAR, char);
      vtkITKExecuteDataFromSeries(VTK_UNSIGNED_CHAR, unsigned char);
      default:
        vtkErrorMacro(<< "UpdateFromFile: Unknown data type");
      }
    }
}
