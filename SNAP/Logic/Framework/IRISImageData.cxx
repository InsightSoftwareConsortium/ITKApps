/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IRISImageData.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
// ITK Includes
#include "itkImage.h"
#include "itkImageIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkUnaryFunctorImageFilter.h"
#include "UnaryFunctorCache.h"
#include "itkRGBAPixel.h"
#include "IRISSlicer.h"
#include <list>

/** Borland compiler is very lazy so we need to instantiate the template
 *  by hand */
#if defined(__BORLANDC__)
typedef IRISSlicer<unsigned char> IRISIMageDataDummyIRISSlicerTypeUchar;
typedef itk::SmartPointer<IRISIMageDataDummyIRISSlicerTypeUchar> IRISIMageDataDummySmartPointerSlicerType;
typedef IRISSlicer<short> IRISIMageDataDummyIRISSlicerTypeShort;
typedef itk::SmartPointer<IRISIMageDataDummyIRISSlicerTypeShort> IRISIMageDataDummySmartPointerSlicerShortType;
typedef itk::ImageRegion<3> IRISImageDataBorlandDummyImageRegionType;
typedef itk::ImageRegion<2> IRISImageDataBorlandDummyImageRegionType2;
typedef itk::ImageBase<3> IRISImageDataBorlandDummyImageBaseType;
typedef itk::ImageBase<2> IRISImageDataBorlandDummyImageBaseType2;
typedef itk::Image<unsigned char,3> IRISImageDataBorlandDummyImageType;
typedef itk::Image<unsigned char,2> IRISImageDataBorlandDummyImageType2;
typedef itk::ImageRegionConstIterator<IRISImageDataBorlandDummyImageType> IRISImageDataBorlandDummyConstIteratorType;
typedef itk::Image<short,3> IRISImageDataBorlandDummyShortImageType;
typedef itk::Image<short,2> IRISImageDataBorlandDummyShortImageType2;
typedef itk::Image<itk::RGBAPixel<unsigned char>,2> IRISImageDataBorlandDummyShortImageTypeRGBA;
typedef itk::ImageRegionConstIterator<IRISImageDataBorlandDummyShortImageType> IRISImageDataBorlandDummyConstIteratorShortType;
typedef itk::MinimumMaximumImageCalculator<IRISImageDataBorlandDummyShortImageType> IRISImageDataBorlandDummyMinMaxCalc;
#endif

#include "GreyImageWrapper.h"
#if defined(__BORLANDC__)
typedef CachingUnaryFunctor<short,unsigned char,GreyImageWrapper::IntensityFunctor> IRISImageDataBorlamdCachingUnaryFunctor;
typedef itk::UnaryFunctorImageFilter<IRISImageDataBorlandDummyShortImageType,IRISImageDataBorlandDummyImageType2,IRISImageDataBorlamdCachingUnaryFunctor> IRISIMageDataDummyFunctorType;
typedef itk::SmartPointer<IRISIMageDataDummyFunctorType> IRISIMageDataDummyFunctorTypePointerType;
#endif


#include "IRISImageData.h"

#include "LabelImageWrapper.h"

// System includes
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace itk;

void 
IRISImageData
::CountVoxels(const char *filename) throw(ExceptionObject)
{
  unsigned int i;
  
  // Make sure that the segmentation image exists
  assert(IsSegmentationLoaded());

  // A structure to describe each label
  struct Entry {
    unsigned long int count;
    unsigned long int sumGrey;
    unsigned long int sumGreySqr;
    double mean;
    double stddev;
  
    Entry() : count(0),sumGrey(0),sumGreySqr(0),mean(0),stddev(0) {}
  };
  
  // histogram of the segmentation image
  Entry data[MAX_COLOR_LABELS];

  // Create an iterator for parsing the segmentation image
  LabelImageWrapper::ConstIterator itLabel = 
    m_LabelWrapper.GetImageConstIterator();

  // Another iterator for accessing the grey image
  GreyImageWrapper::ConstIterator itGrey = 
    m_GreyWrapper.GetImageConstIterator();

  // Compute the number, sum and sum of squares of grey intensities for each 
  // label
  while(!itLabel.IsAtEnd())
    {
    LabelType label = itLabel.Value();
    GreyType grey = itGrey.Value();

    data[label].count++;
    data[label].sumGrey += grey;
    data[label].sumGreySqr += grey * grey;

    ++itLabel;
    ++itGrey;
    }
  
  // Compute the mean and standard deviation
  for (i=0; i<MAX_COLOR_LABELS; i++)
    {
    // The mean
    data[i].mean = data[i].sumGrey * 1.0 / data[i].count;

    // The standard deviation
    data[i].stddev = sqrt(
      (data[i].sumGreySqr * 1.0 - data[i].count * data[i].mean * data[i].mean) 
      / (data[i].count - 1));
    }
  
  // Compute the size of a voxel, in mm^3
  const double *spacing = m_GreyWrapper.GetImage()->GetSpacing().GetDataPointer();
  double volVoxel = spacing[0] * spacing[1] * spacing[2];

  // Open the selected file for writing
  std::ofstream fout(filename);

  // Check if the file is readable
  if(!fout.good())
    throw itk::ExceptionObject(__FILE__, __LINE__,
                               "File can not be opened for writing");
  try 
    {
    // Write voxel volumes to the file
    fout << "##########################################################" << endl;
    fout << "# SNAP Voxel Count File" << endl;
    fout << "# File format:" << endl;
    fout << "# LABEL: NUMBER / VOLUME / MEAN / SD" << endl;
    fout << "# Fields:" << endl;
    fout << "#    LABEL         Label description" << endl;
    fout << "#    NUMBER        Number of voxels that have that label " << endl;
    fout << "#    VOLUME        Volume of those voxels in cubic mm " << endl;
    fout << "#    MEAN          Mean intensity of those voxels " << endl;
    fout << "#    SD            Standard deviation of those voxels " << endl;
    fout << "##########################################################" << endl;

    for (i=1; i<MAX_COLOR_LABELS; i++) 
      {
      if(m_ColorLabels[i].IsValid() && data[i].count > 0)
        {
        fout << std::left << std::setw(40) << m_ColorLabels[i].GetLabel() << ": ";
        fout << std::right << std::setw(10) << data[i].count << " / ";
        fout << std::setw(10) << (data[i].count * volVoxel) << " / ";
        fout << std::internal << std::setw(10) << data[i].mean << " / ";
        fout << std::setw(10) << data[i].stddev << endl;
        }      
      }
    }
  catch(...)
    {
    throw itk::ExceptionObject(__FILE__, __LINE__,
                           "File can not be written");
    }

  fout.close();
}

void 
IRISImageData
::SetSegmentationVoxel(const Vector3ui &index, LabelType value)
{
  // Make sure that the grey data and the segmentation data exist
  assert(m_GreyWrapper.IsInitialized() && m_LabelWrapper.IsInitialized());

  // Store the voxel
  m_LabelWrapper.GetVoxelForUpdate(index) = value;

  // Make sure this label is set as valid
  if (!m_ColorLabels[value].IsValid())
    {
    m_ColorLabels[value].SetValid(true);
    m_ColorLabelCount++;
    }

  // Mark the image as modified
  m_LabelWrapper.GetImage()->Modified();
}

IRISImageData
::IRISImageData() 
{
  // Populate the array of linked wrappers
  m_LinkedWrappers.push_back(&m_GreyWrapper);
  m_LinkedWrappers.push_back(&m_LabelWrapper);

  // Initialize the color labels
  InitializeColorLabels();
}

void 
IRISImageData
::RelabelSegmentationWithCutPlane(const Vector3d &normal, double intercept,
                                  GlobalState *state) 
{
  typedef ImageRegionIteratorWithIndex<LabelImageType> IteratorType;
  IteratorType it(m_LabelWrapper.GetImage(), GetImageRegion());

  // Compute a label mapping table based on the color labels
  LabelType table[MAX_COLOR_LABELS];
  
  // The clear label does not get painted over, no matter what
  table[0] = 0;

  // The other labels get painted over, depending on current settings
  for(unsigned int i=1;i<MAX_COLOR_LABELS;i++)
    table[i] = this->DrawOverFunction(state,i);

  // Adjust the intercept by 0.5 for voxel offset
  intercept -= 0.5 * (normal[0] + normal[1] + normal[2]);

  // Iterate over the image, relabeling labels on one side of the plane
  while(!it.IsAtEnd())
    {
    // Compute the distance to the plane
    const long *index = it.GetIndex().GetIndex();
    double distance = 
      index[0]*normal[0] + 
      index[1]*normal[1] + 
      index[2]*normal[2] - intercept;

    // Check the side of the plane
    if(distance > 0)
      {
      LabelType &voxel = it.Value();
      voxel = table[voxel];
      }

    // Next voxel
    ++it;
    }
  
  // Register that the image has been updated
  m_LabelWrapper.GetImage()->Modified();
}

int 
IRISImageData
::GetRayIntersectionWithSegmentation(const Vector3d &point, 
                                     const Vector3d &ray, Vector3i &hit) const
{
  assert(m_LabelWrapper.IsInitialized());

  Vector3ui lIndex;
  double delta[3][3], dratio[3];
  int    signrx, signry, signrz;

  double rx = ray[0];
  double ry = ray[1];
  double rz = ray[2];

  double rlen = rx*rx+ry*ry+rz*rz;
  if(rlen == 0) return -1;

  double rfac = 1.0 / sqrt(rlen);
  rx *= rfac; ry *= rfac; rz *= rfac;

  if (rx >=0) signrx = 1; else signrx = -1;
  if (ry >=0) signry = 1; else signry = -1;
  if (rz >=0) signrz = 1; else signrz = -1;

  // offset everything by (.5, .5) [becuz samples are at center of voxels]
  // this offset will put borders of voxels at integer values
  // we will work with this offset grid and offset back to check samples
  // we really only need to offset "point"
  double px = point[0]+0.5;
  double py = point[1]+0.5;
  double pz = point[2]+0.5;

  // get the starting point within data extents
  int c = 0;
  while ( (px < 0 || px >= m_Size[0]||
           py < 0 || py >= m_Size[1]||
           pz < 0 || pz >= m_Size[2]) && c < 10000)
    {
    px += rx;
    py += ry;
    pz += rz;
    c++;
    }
  if (c >= 9999) return -1;

  // walk along ray to find intersection with any voxel with val > 0
  while ( (px >= 0 && px < m_Size[0]&&
           py >= 0 && py < m_Size[1] &&
           pz >= 0 && pz < m_Size[2]) )
    {

    // offset point by (-.5, -.5) [to account for earlier offset] and
    // get the nearest sample voxel within unit cube around (px,py,pz)
    //    lx = my_round(px-0.5);
    //    ly = my_round(py-0.5);
    //    lz = my_round(pz-0.5);
    lIndex[0] = (int)px;
    lIndex[1] = (int)py;
    lIndex[2] = (int)pz;

    LabelType hitlabel = m_LabelWrapper.GetVoxel(lIndex);
    const ColorLabel &cl = GetColorLabel(hitlabel);

    if (cl.IsValid() && cl.IsVisible())
      {
      hit[0] = lIndex[0];
      hit[1] = lIndex[1];
      hit[2] = lIndex[2];
      return 1;
      }

    // BEGIN : walk along ray to border of next voxel touched by ray

    // compute path to YZ-plane surface of next voxel
    if (rx == 0)
      { // ray is parallel to 0 axis
      delta[0][0] = 9999;
      }
    else
      {
      delta[0][0] = (int)(px+signrx) - px;
      }

    // compute path to XZ-plane surface of next voxel
    if (ry == 0)
      { // ray is parallel to 1 axis
      delta[1][0] = 9999;
      }
    else
      {
      delta[1][1] = (int)(py+signry) - py;
      dratio[1]   = delta[1][1]/ry;
      delta[1][0] = dratio[1] * rx;
      }

    // compute path to XY-plane surface of next voxel
    if (rz == 0)
      { // ray is parallel to 2 axis
      delta[2][0] = 9999;
      }
    else
      {
      delta[2][2] = (int)(pz+signrz) - pz;
      dratio[2]   = delta[2][2]/rz;
      delta[2][0] = dratio[2] * rx;
      }

    // choose the shortest path 
    if ( fabs(delta[0][0]) <= fabs(delta[1][0]) && fabs(delta[0][0]) <= fabs(delta[2][0]) )
      {
      dratio[0]   = delta[0][0]/rx;
      delta[0][1] = dratio[0] * ry;
      delta[0][2] = dratio[0] * rz;
      px += delta[0][0];
      py += delta[0][1];
      pz += delta[0][2];
      }
    else if ( fabs(delta[1][0]) <= fabs(delta[0][0]) && fabs(delta[1][0]) <= fabs(delta[2][0]) )
      {
      delta[1][2] = dratio[1] * rz;
      px += delta[1][0];
      py += delta[1][1];
      pz += delta[1][2];
      }
    else
      { //if (fabs(delta[2][0] <= fabs(delta[0][0] && fabs(delta[2][0] <= fabs(delta[0][0]) 
      delta[2][1] = dratio[2] * ry;
      px += delta[2][0];
      py += delta[2][1];
      pz += delta[2][2];
      }
    // END : walk along ray to border of next voxel touched by ray

    } // while ( (px
  return 0;
}

Vector3f 
IRISImageData
::GetVoxelScaleFactor() 
{
  const double *spacing = this->m_GreyWrapper.GetImage()->GetSpacing().GetDataPointer();
  Vector3f rtn;
  rtn[0]=(float)spacing[0];
  rtn[1]=(float)spacing[1];
  rtn[2]=(float)spacing[2];

  return rtn;
}

void 
IRISImageData
::SetGreyImage(GreyImageType *newGreyImage,
               const ImageCoordinateGeometry &newGeometry) 
{
  // Make a new grey wrapper
  m_GreyWrapper.SetImage(newGreyImage);

  // Clear the segmentation data to zeros
  m_LabelWrapper.InitializeToWrapper(&m_GreyWrapper, (LabelType) 0);

  // The segmentation wrapper needs the label colors
  m_LabelWrapper.SetLabelColorTable(m_ColorLabels);

  // Store the image size info
  m_Size = m_GreyWrapper.GetSize();

  // Pass the coordinate transform to the wrappers
  SetImageGeometry(newGeometry);
}

void 
IRISImageData
::SetSegmentationImage(LabelImageType *newLabelImage) 
{
  // Check that the image matches the size of the grey image
  assert(m_GreyWrapper.IsInitialized() &&
    m_GreyWrapper.GetImage()->GetBufferedRegion() == 
         newLabelImage->GetBufferedRegion());

  // Pass the image to the segmentation wrapper
  m_LabelWrapper.SetImage(newLabelImage);

  // Sync up spacing between the grey and label image
  newLabelImage->SetSpacing(m_GreyWrapper.GetImage()->GetSpacing());
  newLabelImage->SetOrigin(m_GreyWrapper.GetImage()->GetOrigin());

  // Update the validity of the labels
  LabelImageWrapper::ConstIterator it = m_LabelWrapper.GetImageConstIterator();
  while(!it.IsAtEnd())
    {
    if (!m_ColorLabels[it.Get()].IsValid())
      {
      m_ColorLabels[it.Get()].SetValid(true);
      m_ColorLabelCount++;
      }
    ++it;
    }

  // There is no need to update the image-to-display transform because the 
  // image dimensions have not changed
}

bool 
IRISImageData
::IsGreyLoaded() 
{
  return m_GreyWrapper.IsInitialized();
}

bool 
IRISImageData
::IsSegmentationLoaded() 
{
  return m_LabelWrapper.IsInitialized();
}    

void 
IRISImageData
::InitializeColorLabels() 
{
  unsigned int i;

  // Set up the clear color
  m_ColorLabels[0].SetRGB(0,0,0);
  m_ColorLabels[0].SetAlpha(0);
  m_ColorLabels[0].SetValid(true);
  m_ColorLabels[0].SetVisible(false);
  m_ColorLabels[0].SetDoMesh(false);
  m_ColorLabels[0].SetLabel("Clear");

  // Some well-spaced sample colors to work with
  m_ColorLabels[1].SetRGB(255,0,0);
  m_ColorLabels[2].SetRGB(0,255,0);
  m_ColorLabels[3].SetRGB(0,0,255);
  m_ColorLabels[4].SetRGB(255,255,0);
  m_ColorLabels[5].SetRGB(0,255,255);
  m_ColorLabels[6].SetRGB(255,0,255);

  // Set the number of active color labels
  m_ColorLabelCount = 7;

  // Fill the rest of the labels with color ramps
  for (i=m_ColorLabelCount; i < MAX_COLOR_LABELS; i++)
    {
    if (i < 85)
      {
      m_ColorLabels[i].SetRGB(0,(unsigned char) ((84.0-i)/85.0 * 200.0 + 50));
      m_ColorLabels[i].SetRGB(1,(unsigned char) (i/85.0 * 200.0 + 50));
      m_ColorLabels[i].SetRGB(2,0);
      }
    else if (i < 170)
      {
      m_ColorLabels[i].SetRGB(0,0);
      m_ColorLabels[i].SetRGB(1,(unsigned char) ((169.0-i)/85.0 * 200.0 + 50));
      m_ColorLabels[i].SetRGB(2,(unsigned char) ((i-85)/85.0 * 200.0 + 50));
      }
    else
      {
      m_ColorLabels[i].SetRGB(0,(unsigned char) ((i-170)/85.0 * 200.0 + 50));
      m_ColorLabels[i].SetRGB(1,0);
      m_ColorLabels[i].SetRGB(2,(unsigned char) ((255.0-i)/85.0 * 200.0 + 50));
      }
    }

  // Set the properties of all non-clear labels
  for (i=1; i<MAX_COLOR_LABELS; i++)
    {
    m_ColorLabels[i].SetAlpha(255);
    m_ColorLabels[i].SetValid(i < m_ColorLabelCount);
    m_ColorLabels[i].SetVisible(true);
    m_ColorLabels[i].SetDoMesh(true);

    IRISOStringStream sout;
    sout << "Label" << i;
    m_ColorLabels[i].SetLabel(sout.str().c_str());
    }
}

void
IRISImageData
::SetCrosshairs(const Vector3ui &crosshairs)
{
  std::list<ImageWrapperBase *>::iterator it = m_LinkedWrappers.begin();
  while(it != m_LinkedWrappers.end())
    {
    ImageWrapperBase *wrapper = *it++;
    if(wrapper->IsInitialized())
      wrapper->SetSliceIndex(crosshairs);
    }
}


IRISImageData::RegionType
IRISImageData
::GetImageRegion() const
{
  assert(m_GreyWrapper.IsInitialized());
  return m_GreyWrapper.GetImage()->GetBufferedRegion();
}

void
IRISImageData
::SetColorLabel(unsigned int index, const ColorLabel &label)
{
  assert(index < MAX_COLOR_LABELS);

  // Update the counter
  m_ColorLabelCount -= (m_ColorLabels[index].IsValid() ? 1 : 0);
  m_ColorLabelCount += (label.IsValid() ? 1 : 0);
  
  // Copy the label
  m_ColorLabels[index] = label;

  // Propagate the change to the label wrapper
  m_LabelWrapper.SetLabelColorTable(m_ColorLabels);
}                                                                                 

void 
IRISImageData
::SetImageGeometry(const ImageCoordinateGeometry &geometry)
{
  // Save the geometry
  m_ImageGeometry = geometry;

  // Propagate the geometry to the image wrappers
  std::list<ImageWrapperBase *>::iterator it = m_LinkedWrappers.begin();
  while(it != m_LinkedWrappers.end())
    {
    ImageWrapperBase *wrapper = *it++;
    if(wrapper->IsInitialized())
      {
      // Update the geometry for each slice
      for(unsigned int iSlice = 0;iSlice < 3;iSlice ++)
        {
        wrapper->SetImageToDisplayTransform(
          iSlice,m_ImageGeometry.GetImageToDisplayTransform(iSlice));
        }
      }
    }
}
