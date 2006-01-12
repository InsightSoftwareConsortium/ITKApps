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
#include "vtkDataArray.h"

#ifndef vtkFloatingPointType
#define vtkFloatingPointType float
#endif

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
#include "itkOrientImageFilter.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileReader.h"
#include "itkImportImageContainer.h"
#include "itkImageRegion.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkGDCMImageIO.h"
#include <itksys/SystemTools.hxx>

vtkCxxRevisionMacro(vtkITKArchetypeImageSeriesReader, "$Revision: 1.13 $");
vtkStandardNewMacro(vtkITKArchetypeImageSeriesReader);

//----------------------------------------------------------------------------
vtkITKArchetypeImageSeriesReader::vtkITKArchetypeImageSeriesReader()
{
  this->Archetype = NULL;
  this->RasToIjkMatrix = NULL;
  this->SetDesiredCoordinateOrientationToAxial();
  this->UseNativeCoordinateOrientation = 0;
  this->FileNameSliceOffset = 0;
  this->FileNameSliceSpacing = 1;
  this->FileNameSliceCount = 0;

  this->OutputScalarType = VTK_FLOAT;
  this->UseNativeScalarType = 0;
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
 if (RasToIjkMatrix)
   {
   RasToIjkMatrix->Delete();
   RasToIjkMatrix = NULL;
   }
}

vtkMatrix4x4* vtkITKArchetypeImageSeriesReader::GetRasToIjkMatrix()
{
  this->ExecuteInformation();
  return RasToIjkMatrix;
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

  os << indent << "OutputScalarType: "
     << vtkImageScalarTypeNameMacro(this->OutputScalarType)
     << std::endl;
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
  std::vector<std::string> candidateFiles;
  std::vector<std::string> candidateSeries;
  int extent[6];  
  std::string fileNameCollapsed = itksys::SystemTools::CollapseFullPath( this->Archetype);

  // First see if the archetype exists
  if (!itksys::SystemTools::FileExists (fileNameCollapsed.c_str()))
    {
    itkGenericExceptionMacro ( "vtkITKArchetypeImageSeriesReader::ExecuteInformation: Archetype file " << fileNameCollapsed.c_str() << " does not exist.");
    return;
    }

  // Test whether the input file is a DICOM file
  itk::GDCMImageIO::Pointer dicomIO = itk::GDCMImageIO::New();
  bool isDicomFile = dicomIO->CanReadFile(this->Archetype);
  if (isDicomFile)
    {
    typedef itk::GDCMSeriesFileNames DICOMNameGeneratorType;
    DICOMNameGeneratorType::Pointer inputImageFileGenerator = DICOMNameGeneratorType::New();
    std::string fileNameName = itksys::SystemTools::GetFilenameName( this->Archetype );
    std::string fileNamePath = itksys::SystemTools::GetFilenamePath( this->Archetype );
    if (fileNamePath == "")
      {
      fileNamePath = ".";
      }
    inputImageFileGenerator->SetDirectory( fileNamePath );

    // Find the series that contains the archetype
    candidateSeries = inputImageFileGenerator->GetSeriesUIDs();
    int found = 0;
    for (int s = 0; s < candidateSeries.size() && found == 0; s++)
      {
      candidateFiles = inputImageFileGenerator->GetFileNames(candidateSeries[s]);
      for (int f = 0; f < candidateFiles.size(); f++)
        {
        if (itksys::SystemTools::CollapseFullPath(candidateFiles[f].c_str()) ==
            fileNameCollapsed)
          {
          found = 1;
          break;
          }
        }
      }

    if (candidateFiles.size() == 0)
      {
      candidateFiles.push_back(this->Archetype);
      }
    }
  else
    {  
    // Generate filenames from the Archetype
    itk::ArchetypeSeriesFileNames::Pointer fit = itk::ArchetypeSeriesFileNames::New();
    fit->SetArchetype (this->Archetype);
    candidateFiles = fit->GetFileNames();
    }

  // Reduce the selection of filenames
  int lastFile;
  if (this->FileNameSliceCount == 0)
    {
    lastFile = candidateFiles.size();
    }
  else
    {
    lastFile = this->FileNameSliceOffset + this->FileNameSliceCount - 1;
    if (lastFile > candidateFiles.size())
      {
      lastFile = candidateFiles.size();      
      }
    }
  this->FileNames.resize(0);
  for (int f = this->FileNameSliceOffset;
       f < lastFile;
       f += this->FileNameSliceSpacing)
    {
    this->FileNames.push_back(candidateFiles[f]);
    }

  if (RasToIjkMatrix)
    {
    RasToIjkMatrix->Delete();
    }
  RasToIjkMatrix = vtkMatrix4x4::New();
  
  vtkMatrix4x4* IjkToLpsMatrix = vtkMatrix4x4::New();

  RasToIjkMatrix->Identity();
  IjkToLpsMatrix->Identity();

  vtkFloatingPointType spacing[3];
  vtkFloatingPointType origin[3];
  
  // Since we only need origin, spacing and extents, we can use one
  // image type.
  typedef itk::Image<float,3> ImageType;
  itk::ImageRegion<3> region;

  typedef itk::ImageSource<ImageType> FilterType;
  FilterType::Pointer filter;

  itk::ImageIOBase::Pointer imageIO;

  // If there is only one file in the series, just use an image file reader
  if (this->FileNames.size() == 1)
    {
    itk::OrientImageFilter<ImageType,ImageType>::Pointer orient =
      itk::OrientImageFilter<ImageType,ImageType>::New();
    itk::ImageFileReader<ImageType>::Pointer imageReader =
      itk::ImageFileReader<ImageType>::New();
    imageReader->SetFileName(this->FileNames[0].c_str());

    if (this->UseNativeCoordinateOrientation)
      {
      imageReader->UpdateOutputInformation();
      filter = imageReader;
      }
    else
      {
      orient->SetInput(imageReader->GetOutput());
      orient->UseImageDirectionOn();
      orient->SetDesiredCoordinateOrientation(this->DesiredCoordinateOrientation);
      orient->UpdateOutputInformation();
      filter = orient;
      }
    for (int i = 0; i < 3; i++)
      {
      spacing[i] = filter->GetOutput()->GetSpacing()[i];
      origin[i] = filter->GetOutput()->GetOrigin()[i];

      // Get IJK to RAS direction vector
      for ( unsigned int j=0; j < filter->GetOutput()->GetImageDimension (); j++ )
        {
        IjkToLpsMatrix->SetElement(j, i, spacing[i]*filter->GetOutput()->GetDirection()[j][i]);
        }
      }
    region = filter->GetOutput()->GetLargestPossibleRegion();
    extent[0] = region.GetIndex()[0];
    extent[1] = region.GetIndex()[0] + region.GetSize()[0] - 1;
    extent[2] = region.GetIndex()[1];
    extent[3] = region.GetIndex()[1] + region.GetSize()[1] - 1;
    extent[4] = region.GetIndex()[2];
    extent[5] = region.GetIndex()[2] + region.GetSize()[2] - 1;
    imageIO = imageReader->GetImageIO();
    }
  else
    {
    itk::OrientImageFilter<ImageType,ImageType>::Pointer orient =
      itk::OrientImageFilter<ImageType,ImageType>::New();
    itk::ImageSeriesReader<ImageType>::Pointer seriesReader =
      itk::ImageSeriesReader<ImageType>::New();
    seriesReader->SetFileNames(this->FileNames);
    
    if (this->UseNativeCoordinateOrientation)
      {
      seriesReader->UpdateOutputInformation();
      filter = seriesReader;
      }
    else
      {
      orient->SetInput(seriesReader->GetOutput());
      orient->UseImageDirectionOn();
      orient->SetDesiredCoordinateOrientation(this->DesiredCoordinateOrientation);
      orient->UpdateOutputInformation();
      filter = orient;
      }
    for (int i = 0; i < 3; i++)
      {
      spacing[i] = filter->GetOutput()->GetSpacing()[i];
      origin[i] = filter->GetOutput()->GetOrigin()[i];
      // Get IJK to RAS direction vector
      for ( unsigned int j=0; j < filter->GetOutput()->GetImageDimension (); j++ )
        {
        IjkToLpsMatrix->SetElement(j, i, spacing[i]*filter->GetOutput()->GetDirection()[j][i]);
        }
      }

    region = filter->GetOutput()->GetLargestPossibleRegion();
    extent[0] = region.GetIndex()[0];
    extent[1] = region.GetIndex()[0] + region.GetSize()[0] - 1;
    extent[2] = region.GetIndex()[1];
    extent[3] = region.GetIndex()[1] + region.GetSize()[1] - 1;
    extent[4] = region.GetIndex()[2];
    extent[5] = region.GetIndex()[2] + region.GetSize()[2] - 1;
    imageIO = seriesReader->GetImageIO();
    }

  // Transform from LPS to RAS
  vtkMatrix4x4* LpsToRasMatrix = vtkMatrix4x4::New();
  LpsToRasMatrix->Identity();
  LpsToRasMatrix->SetElement(0,0,-1);
  if (this->UseNativeCoordinateOrientation)
    {
    LpsToRasMatrix->SetElement(1,1,1);
    }
  else
    {
    LpsToRasMatrix->SetElement(1,1,-1);
    }
  vtkMatrix4x4::Multiply4x4(IjkToLpsMatrix, LpsToRasMatrix, RasToIjkMatrix);
  RasToIjkMatrix->Invert();

  // If it looks like the pipeline did not provide the spacing and
  // origin, modify the spacing and origin with the defaults
  for (int j = 0; j < 3; j++)
    {
    RasToIjkMatrix->SetElement(j, 3, (extent[2*j+1] - extent[2*j])/2.0);
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
  RasToIjkMatrix->SetElement(3,3,1.0);
  IjkToLpsMatrix->Delete();

  output->SetWholeExtent(extent);
  if (this->UseNativeScalarType)
    {
    if (imageIO->GetComponentType() == itk::ImageIOBase::UCHAR)
      {
      this->SetOutputScalarType(VTK_UNSIGNED_CHAR);
      }
    else if (imageIO->GetComponentType() == itk::ImageIOBase::CHAR)
      {
      this->SetOutputScalarType(VTK_CHAR);
      }
    else if (imageIO->GetComponentType() == itk::ImageIOBase::USHORT)
      {
      this->SetOutputScalarType(VTK_UNSIGNED_SHORT);
      }
    else if (imageIO->GetComponentType() == itk::ImageIOBase::SHORT)
      {
      this->SetOutputScalarType(VTK_SHORT);
      }
    else if (imageIO->GetComponentType() == itk::ImageIOBase::UINT)
      {
      this->SetOutputScalarType(VTK_UNSIGNED_INT);
      }
    else if (imageIO->GetComponentType() == itk::ImageIOBase::INT)
      {
      this->SetOutputScalarType(VTK_INT);
      }
    else if (imageIO->GetComponentType() == itk::ImageIOBase::ULONG)
      {
      this->SetOutputScalarType(VTK_UNSIGNED_LONG);
      }
    else if (imageIO->GetComponentType() == itk::ImageIOBase::LONG)
      {
      this->SetOutputScalarType(VTK_LONG);
      }
    else if (imageIO->GetComponentType() == itk::ImageIOBase::FLOAT)
      {
      this->SetOutputScalarType(VTK_FLOAT);
      }
    else if (imageIO->GetComponentType() == itk::ImageIOBase::DOUBLE)
      {
      this->SetOutputScalarType(VTK_DOUBLE);
      }
    }

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
      typedef itk::ImageSource<image##typeN> FilterType; \
      FilterType::Pointer filter; \
      itk::ImageSeriesReader<image##typeN>::Pointer reader##typeN = \
            itk::ImageSeriesReader<image##typeN>::New(); \
      reader##typeN->SetFileNames(this->FileNames); \
      reader##typeN->ReleaseDataFlagOn(); \
      if (this->UseNativeCoordinateOrientation) \
        { \
        filter = reader##typeN; \
        } \
      else \
        { \
        itk::OrientImageFilter<image##typeN,image##typeN>::Pointer orient##typeN = \
            itk::OrientImageFilter<image##typeN,image##typeN>::New(); \
        if (this->Debug) {orient##typeN->DebugOn();} \
        orient##typeN->SetInput(reader##typeN->GetOutput()); \
        orient##typeN->UseImageDirectionOn(); \
        orient##typeN->SetDesiredCoordinateOrientation(this->DesiredCoordinateOrientation); \
        filter = orient##typeN; \
        }\
      filter->UpdateLargestPossibleRegion(); \
      itk::ImportImageContainer<unsigned long, type>::Pointer PixelContainer##typeN;\
      PixelContainer##typeN = filter->GetOutput()->GetPixelContainer();\
      void *ptr = static_cast<void *> (PixelContainer##typeN->GetBufferPointer());\
      (dynamic_cast<vtkImageData *>( output))->GetPointData()->GetScalars()->SetVoidArray(ptr, PixelContainer##typeN->Size(), 0);\
      PixelContainer##typeN->ContainerManageMemoryOff();\
    }\
    break

#define vtkITKExecuteDataFromFile(typeN, type) \
    case typeN: \
    {\
      typedef itk::Image<type,3> image2##typeN;\
      typedef itk::ImageSource<image2##typeN> FilterType; \
      FilterType::Pointer filter; \
      itk::ImageFileReader<image2##typeN>::Pointer reader2##typeN = \
            itk::ImageFileReader<image2##typeN>::New(); \
      reader2##typeN->SetFileName(this->FileNames[0].c_str()); \
      if (this->UseNativeCoordinateOrientation) \
        { \
        filter = reader2##typeN; \
        } \
      else \
        { \
        itk::OrientImageFilter<image2##typeN,image2##typeN>::Pointer orient2##typeN = \
              itk::OrientImageFilter<image2##typeN,image2##typeN>::New(); \
        if (this->Debug) {orient2##typeN->DebugOn();} \
        orient2##typeN->SetInput(reader2##typeN->GetOutput()); \
        orient2##typeN->UseImageDirectionOn(); \
        orient2##typeN->SetDesiredCoordinateOrientation(this->DesiredCoordinateOrientation); \
        filter = orient2##typeN; \
        } \
       filter->UpdateLargestPossibleRegion();\
      itk::ImportImageContainer<unsigned long, type>::Pointer PixelContainer2##typeN;\
      PixelContainer2##typeN = filter->GetOutput()->GetPixelContainer();\
      void *ptr = static_cast<void *> (PixelContainer2##typeN->GetBufferPointer());\
      (dynamic_cast<vtkImageData *>( output))->GetPointData()->GetScalars()->SetVoidArray(ptr, PixelContainer2##typeN->Size(), 0);\
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
