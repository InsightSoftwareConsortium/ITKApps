/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ConfidenceConnectedModule.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __ConfidenceConnectedModule_h__
#define __ConfidenceConnectedModule_h__

#include "itkConfidenceConnectedImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkImage.h"

#include "macros.h"

/** 

  \brief Module for segmentation using the ConfidenceConnected filter

  This module encapsulates all the components required for performing
  segmentation with the Confidence connected filter. At the level of
  the module, all the segmentation components are customized for a 
  specific anatomic applications. It is at this level that pixel types 
  and image dimensionalities must be selected according to the expected 
  source of data and the known set of filters that will be used for 
  processing the image.

  The particular clinical application anticipated for this module is
  Tumor Liver Segmentation. The input images are expected to be CT-Scans 
  of the liver without any contrast agent.


*/
namespace ISIS {

class ConfidenceConnectedModule 
{

public:


  /**  The expected input images for this module are stored
       in 8 bits and are the output of a resampling and intensity
       windowing preprocessing.  */

  typedef   unsigned char    InputPixelType;



  /**  Use float pixel type for internal computations given
       that the edge preserving filter used requires a float
       type for numeric operations.  */  
  typedef   float           InternalPixelType;



  /**  Image dimension. This module is specific for 3D images */
  itkStaticConstMacro( Dimension, unsigned int, 3 );
  


  /** Type of the image to be used as input to the module. */
  typedef itk::Image< InputPixelType,    Dimension >   InputImageType;


  /** Type of the image to be used for internal computation
      actually this is the image type to be used in the smoothing
      step with the anisotropic diffusion filter  */
  typedef itk::Image< InternalPixelType, Dimension >  InternalImageType;



  /** Pixel type to be used for representing the output. In this
      module the output is a binary image containing a mask that
      represents the segmented object(s).  */
  typedef unsigned char     OutputPixelType;



  /** Image type used for the output. This is the mask where the 
      segmented regions have been marked.  */
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;


  /**  Smoothing filter with edge-preserving properties. This filter
       is applied as a preprocessing step to the input image in order
       to reduce the statistical variability of the regions in the image. */
  typedef   itk::CurvatureFlowImageFilter< 
                             InputImageType, 
                             InternalImageType >  SmoothingFilterType;


  /**  This filter is the heart of this segmentation method. It applies
       a flood fill approach based on a statistical criteria for adding
       pixels to the region being grown. A nominal mean and variance are
       computed in an initial step. Then, starting from a set of seed points,
       all the neighbors having intensities in a range defined around the
       nominal mean are included in the region. The intensity range is defined
       using a multiplicative factor and the nominal standard deviation.
       The output of this filter is a binary image in which the segmented 
       region is hightlighted.
    */
  typedef  itk::ConfidenceConnectedImageFilter< 
                                  InternalImageType, 
                                  OutputImageType > ConnectedFilterType;



public:
  
  ConfidenceConnectedModule();

  virtual ~ConfidenceConnectedModule();
  
  void SetInput( const InputImageType * image );

  const OutputImageType * GetOutput();

  void SetSeedPoint( int x, int y, int z );
  
  void Execute();
  
  GetMacro( SmoothingTimeStep, double );
  GetMacro( SmoothingIterations, unsigned int );
  
  GetMacro( Multiplier, double );
  GetMacro( InitialNeighborhoodRadius, unsigned int );
  GetMacro( NumberOfIterations, unsigned int );
  
  SetMacro( SmoothingTimeStep, double );
  SetMacro( SmoothingIterations, unsigned int );
  
  SetMacro( Multiplier, double );
  SetMacro( InitialNeighborhoodRadius, unsigned int );

  SetMacro( NumberOfIterations, unsigned int );
  
private:
  
  SmoothingFilterType::Pointer            m_SmoothingFilter;
  
  ConnectedFilterType::Pointer            m_ConfidenceConnectedFilter;
  
  /** Parameters for the Smoothing filter */
  unsigned int   m_SmoothingIterations;
  double         m_SmoothingTimeStep
    ;
  
  /** Parameters for the Confidence Connected filter */
  double         m_Multiplier;
  unsigned int   m_NumberOfIterations;
  unsigned int   m_InitialNeighborhoodRadius;
};


} // end namespace ISIS


#endif




