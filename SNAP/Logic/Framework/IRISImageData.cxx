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
#include "IRISImageData.h"
#include "GreyImageWrapperImplementation.h"
#include "LabelImageWrapperImplementation.h"

// ITK Includes
#include "itkImage.h"
#include "itkImageIterator.h"
#include "itkImageRegionConstIterator.h"

#include <sstream>

using namespace itk;

/** A wrapper around a FILE pointer */
class CFilePointer
{
private:
  FILE *m_FilePointer;
public:
  CFilePointer(const char *fname, const char *mode) {
    m_FilePointer = fopen(fname,mode);
  }
  virtual ~CFilePointer() {
    if (m_FilePointer)
      fclose(m_FilePointer);
  }
  FILE * operator ->() {
    return m_FilePointer;
  }
  operator int() {
    return(int)m_FilePointer;
  }
};

/**
 * TODO: Rewrite this method to read labels in an XML format.
 */
void 
IRISImageData
::ReadASCIIColorLabels(const char* filename)
{
  /*
  // FILE* fp = fopen(filename, "r");
  CFilePointer fp(filename,"r");
  char buff[1000];
  char toks[5][500];

  if(!fp) {
  throw IRISExceptionIO("Label file does not exist");
  }

  try {
  // fp = fopen(filename, "r");
  while (!feof(fp)) {
  readline(buff, fp);
  if (strlen(buff) > 1) {
  // comment lines begin with '#'
  if (strncasecmp(buff, "#", 1) == 0) break;

  // toks[0] holds the part of the line up to a quote \" char,
  // toks[1] hold the rest (the label name)
  unsigned int a = 0;
  while (a < (int)strlen(buff) && buff[a] != '\"') a++;
  if (a < 7 || a == (int)strlen(buff)) break;
  strncpy(toks[0], buff, a);
  strcpy(toks[1], buff+a+1);

  // null-terminate the label string
  for (a = 0; a < strlen(toks[1]); a++)
  if (toks[1][a] == '\"') toks[1][a] = '\0';

  // Parse the data
  int idx, r, g, b, v, m;
  float al;
  sscanf(toks[0], " %d %d %d %d %f %d %d", &idx, &r, &g, &b, &al, &v, &m);

  // Merge with the m_ColorLabels table
  if (!m_ColorLabels[idx].IsValid()) m_ColorLabelCount++;

  m_ColorLabels[idx].SetValid(true);
  m_ColorLabels[idx].SetRGB(0,(unsigned char) r);
  m_ColorLabels[idx].SetRGB(1,(unsigned char) g);
  m_ColorLabels[idx].SetRGB(2,(unsigned char) b);
  m_ColorLabels[idx].SetAlpha( (al<0) ? 0. : ((al>1) ? 1. : al) );
  m_ColorLabels[idx].SetVisible(v != 0);
  m_ColorLabels[idx].SetDoMesh(m != 0);
  m_ColorLabels[idx].SetLabel(toks[1]);
  }
  } catch(...) {
  // fclose(fp);
  throw SNAPExceptionIO("Error reading label file");
  }

  // fclose(fp);
  */
}

/**
 * TODO: Rewrite this method to read labels in an XML format.
 */
void 
IRISImageData
::WriteASCIIColorLabels(const char* filename)
{
  /*    int c = 0;

  // Open file for writing
  FILE *fileptr = fopen(filename, "w+");
  if (!fileptr) {
  cerr << "ook, can't open " << filename << " for writing" << endl;
  return 0;
  }

  for (int i=0; i<MAX_COLOR_LABELS; i++) {
  if (c%24 == 0)
  fprintf(fileptr, "# IDX   -R-  -G-  -B-  -A--  VIS MSH  LABEL\n");
  if (m_ColorLabels[i].valid) {
  fprintf(fileptr, "  %3d   %3d  %3d  %3d  %4.2f  %1d   %1d    \"%s\"\n",
  i, 
  m_ColorLabels[i].GetRGB(0), 
  m_ColorLabels[i].GetRGB(1), 
  m_ColorLabels[i].GetRGB(2),
  m_ColorLabels[i].GetAlpha(),
  (int)m_ColorLabels[i].GetVisible(),
  (int)m_ColorLabels[i].GetDomesh(),
  m_ColorLabels[i].GetLabel());
  c++;
  }
  }

  fclose(fileptr);*/
}

int 
IRISImageData
::CountVoxels(const char *filename) 
{
  /*

  // Make sure that the segmentation image exists
  assert(IsSegmentationLoaded());

  unsigned int i, c;

  // histogram of the segmentation image
  unsigned long int hist[MAX_COLOR_LABELS];

  // mean greyscale values for each bin
  double mean[MAX_COLOR_LABELS];

  // standard deviations of greyscale intensities for each bin
  double stddev[MAX_COLOR_LABELS];

  // initialize bins
  for (i=0; i<MAX_COLOR_LABELS; i++)
  mean[i] = stddev[i] = hist[i] = 0;

  // First pass to get sum and number of voxels for each bin
  LabelType   *segData = ( LabelType *) m_LabelWrapper->GetScalarPointer();
  GreyType *greyData = (GreyType *) m_GreyWrapper->GetScalarPointer();
  unsigned int num_voxels = m_Size[0]*m_Size[1]*m_Size[2];
  for (c=0; c < num_voxels; c++) {
  double curgreyval = greyData[c]/this->grey_scale_factor + this->grey_min;
  hist[segData[c]]++;
  mean[segData[c]] += curgreyval;
  }

  for (i=0; i<MAX_COLOR_LABELS; i++) mean[i] /= hist[i];

  // Second pass to get the 2nd moments of deviation
  for (c=0; c < num_voxels; c++) {
  double curgreyval = greyData[c]/this->grey_scale_factor + this->grey_min;
  double dev = curgreyval - mean[segData[c]];
  stddev[segData[c]] += dev*dev;
  }

  // Standard deviation
  for (i=0; i<MAX_COLOR_LABELS; i++) 
  stddev[i] = sqrt( stddev[i] / (hist[i]-1) );

  // Get the size of a voxel, in mm^3
  float voxvol = 1.0;
  for (i=0; i<3; i++) voxvol *= this->greyImageData->GetSpacing()[i];

  // Open file for writing
  ofstream *filestr = new ofstream(filename);
  if (!filestr || filestr->fail()) {
  cerr << "ook, can't open " << filename << " for writing" << endl;
  return 0;
  }

   *filestr << "Voxel counts / volumes in mm^3 / mean intensity / stddev:" << endl;

   for (i=1; i<MAX_COLOR_LABELS; i++) {
   if (color_labels[i].valid && hist[i] > 0) {
   *filestr 
   << color_labels[i].label << ": " 
   << hist[i] << " / "
   << hist[i]*voxvol << " / "
   << mean[i] << " / "
   << stddev[i] << endl;
   }
   }

   filestr->close();
   return 1;
   */
  return 0;
}


/**
 * TODO: This whole thing is really silly.  ImageWrapper should be able to work
 * with a region of an ITK image and not with the whole image at once
 */
void 
IRISImageData
::DeepCopyROI(const RegionType &roi, IRISImageData &target,
              LabelType passThroughLabel)
{
  // Create a new grey image wrapper
  GreyWrapperType *wrapGrey = new GreyImageWrapperImplementation();  
  LabelWrapperType *wrapSeg = new LabelImageWrapperImplementation();

  // Place the partial image into this wrapper
  wrapGrey->SetImage(m_GreyWrapper->DeepCopyRegion(roi));
  wrapSeg->SetImage(m_LabelWrapper->DeepCopyRegion(roi));

  // Allow only the pass through label to pass through
  // TODO: Make this more elegant, perhaps
  typedef ImageRegionIterator<LabelWrapperType::ImageType> IteratorType;
  IteratorType itLabel(wrapSeg->GetImage(),
                       wrapSeg->GetImage()->GetBufferedRegion());
  
  while(!itLabel.IsAtEnd())
    {
    if(itLabel.Value() != passThroughLabel)
      itLabel.Value() = (LabelType) 0;
    ++itLabel;
    }

  // The segmentation wrapper needs the label colors
  wrapSeg->SetLabelColorTable(m_ColorLabels);

  // Assign the new wrapper to the target
  target.SetGrey(wrapGrey);
  target.SetSegmentation(wrapSeg);
}

void 
IRISImageData
::SetSegmentationVoxel(const Vector3i &index, LabelType value)
{
  // Make sure that the grey data and the segmentation data exist
  assert(m_GreyWrapper && m_LabelWrapper);

  // Store the voxel
  m_LabelWrapper->GetVoxelForUpdate(index) = value;

  // Make sure this label is set as valid
  if (!m_ColorLabels[value].IsValid())
    {
    m_ColorLabels[value].SetValid(true);
    m_ColorLabelCount++;
    }

  // Mark the image as modified
  m_LabelWrapper->GetImage()->Modified();
}

IRISImageData
::IRISImageData() 
{
  m_GreyWrapper = NULL;
  m_LabelWrapper = NULL;

  InitializeColorLabels();
}


IRISImageData
::~IRISImageData() 
{
  if (m_LabelWrapper)
    delete m_LabelWrapper;
  if (m_GreyWrapper)
    delete m_GreyWrapper;
}

// TODO: Clean up this code
void 
IRISImageData
::RelabelSegmentationWithCutPlane(const Vector3d &plane,
                                  bool dZero,LabelType newlabel) 
{
  double threshold = dZero ? 0 : 1;

  for (int x=0; x<m_Size[0]; x++)
    {
    for (int y=0; y<m_Size[1]; y++)
      {
      for (int z=0; z<m_Size[2]; z++)
        {

        // Check the distance to the cut plane
        double distance =  x*plane[0] + y*plane[1] + z*plane[2] - threshold;

        if (distance > 0)
          {

          // Get the next voxel
          LabelType &voxel = m_LabelWrapper->GetVoxelForUpdate(x,y,z);

          // Get the color label associated with voxel
          const ColorLabel &cl = GetColorLabel(voxel);

          // Check if relabelling applies
          if (cl.IsValid() && cl.IsVisible())
            {
            voxel = newlabel;
            }
          }
        }
      }
    }

  m_LabelWrapper->GetImage()->Modified();
}


// TODO: Stick this somewhere
int normalize(double* x, double* y, double* z)
{
  double x1 = *x;
  double y1 = *y;
  double z1 = *z;
  double div = sqrt(x1*x1 + y1*y1 + z1*z1);

  if (div == 0)
    return 0;

  *x = x1/div;
  *y = y1/div;
  *z = z1/div;

  return 1;
}

int 
IRISImageData
::GetRayIntersectionWithSegmentation(const Vector3d &point, 
                                     const Vector3d &ray, Vector3i &hit) const
{
  assert(m_LabelWrapper && m_LabelWrapper->GetImage());

  Vector3i lIndex;
  double delta[3][3], dratio[3];
  int    signrx, signry, signrz;

  double rx = ray[0];
  double ry = ray[1];
  double rz = ray[2];

  if ( (normalize(&rx, &ry, &rz)) < 0 )
    return -1;

  if (rx >=0) signrx = 1;
  else signrx = -1;
  if (ry >=0) signry = 1;
  else signry = -1;
  if (rz >=0) signrz = 1;
  else signrz = -1;

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

    LabelType hitlabel = m_LabelWrapper->GetVoxel(lIndex);
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
  const double *spacing = this->m_GreyWrapper->GetImage()->GetSpacing();
  Vector3f rtn;
  rtn[0]=(float)spacing[0];
  rtn[1]=(float)spacing[1];
  rtn[2]=(float)spacing[2];

  return rtn;
}

void 
IRISImageData
::SetGrey(GreyWrapperType *inWrapper) 
{
  // Dispose of the old wrapper
  if (m_GreyWrapper)
    delete m_GreyWrapper;

  // Dispose of the old segmentation wrapper
  if (m_LabelWrapper)
    delete m_LabelWrapper;

  // Make a copy of the wrapper
  m_GreyWrapper = inWrapper;

  // Clear the segmentation data to zeros (...)
  m_LabelWrapper = new LabelImageWrapperImplementation;
  m_LabelWrapper->InitializeToImage(m_GreyWrapper->GetImage());
  m_LabelWrapper->GetImage()->FillBuffer(0);

  // The segmentation wrapper needs the label colors
  m_LabelWrapper->SetLabelColorTable(m_ColorLabels);

  // Store the image size info
  m_Size = m_GreyWrapper->GetSize();
}

void 
IRISImageData
::SetSegmentation(LabelWrapperType *inWrapper) 
{
  // The dimensions of the images must match
  assert(m_Size == inWrapper->GetSize());

  // Dispose of the old segmentation wrapper
  if (m_LabelWrapper)
    delete m_LabelWrapper;

  // Copy the passed in wrapper
  m_LabelWrapper = inWrapper;

  // Sync up spacing of seg and grey ImageDatas 
  m_LabelWrapper->GetImage()->SetSpacing(m_GreyWrapper->GetImage()->GetSpacing());

  // Update the validity of the labels
  for (LabelWrapperType::ConstIterator it = m_LabelWrapper->GetImageIterator();!it.IsAtEnd();++it)
    {
    if (!m_ColorLabels[it.Get()].IsValid())
      {
      m_ColorLabels[it.Get()].SetValid(true);
      m_ColorLabelCount++;
      }
    }
}

bool 
IRISImageData
::IsGreyLoaded() 
{
  return(m_GreyWrapper != NULL);
}

bool 
IRISImageData
::IsSegmentationLoaded() 
{
  return(m_LabelWrapper != NULL);
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

    std::stringstream sout;
    sout << "Label" << i;
    m_ColorLabels[i].SetLabel(sout.str().c_str());
    }
}

void
IRISImageData
::SetCrosshairs(const Vector3i &crosshairs)
{
  assert(m_GreyWrapper && m_LabelWrapper);  
  m_GreyWrapper->SetSliceIndex(crosshairs);
  m_LabelWrapper->SetSliceIndex(crosshairs);
}


IRISImageData::RegionType
IRISImageData
::GetImageRegion() const
{
  assert(m_GreyWrapper != NULL);
  return m_GreyWrapper->GetImage()->GetLargestPossibleRegion();
}

