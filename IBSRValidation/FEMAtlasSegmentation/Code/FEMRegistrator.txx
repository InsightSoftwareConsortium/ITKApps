/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    FEMRegistrator.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _FEMRegistrator_txx
#define _FEMRegistrator_txx

#include "FEMRegistrator.h"

#include "itkCommand.h"


namespace itk
{



template <typename TFixedImage, typename TMovingImage, typename TFieldValue>
FEMRegistrator<TFixedImage,TMovingImage,TFieldValue>
::FEMRegistrator()
{
  // Images need to be set from the outside
  m_FixedImage  = NULL;
  m_MovingImage = NULL;
  m_DeformedImage = NULL;
    
}


template <typename TFixedImage, typename TMovingImage, typename TFieldValue>
FEMRegistrator<TFixedImage,TMovingImage,TFieldValue>
::~FEMRegistrator()
{

}


template <typename TFixedImage, typename TMovingImage, typename TFieldValue>
void
FEMRegistrator<TFixedImage,TMovingImage,TFieldValue>
::Execute()
{

 
  m_Registration.SetConfigFileName(m_FEMConfigurationFileName.c_str());
  if (!m_Registration.ReadConfigFile(  (m_Registration.GetConfigFileName()).c_str())) { return ; }
 
   // Setup the registrator
  m_Registration.SetMovingImage(  m_MovingImage);
  m_Registration.SetFixedImage( m_FixedImage );

  // Choose the material properties
  itk::fem::MaterialLinearElasticity::Pointer m;
  m=itk::fem::MaterialLinearElasticity::New();
  m->GN=0;       // Global number of the material ///
  m->E=m_Registration.GetElasticity(0);  // Young modulus -- used in the membrane ///
  m->A=1.0;     // Crossection area ///
  m->h=1.0;     // Crossection area ///
  m->I=1.0;    // Moment of inertia ///
  m->nu=0.; //.0;    // poissons -- DONT CHOOSE 1.0!!///
  m->RhoC=1.0;
  

  // Create the element type 
  ElementType::Pointer e1=ElementType::New();
  e1->m_mat=dynamic_cast<itk::fem::MaterialLinearElasticity*>( m );
  m_Registration.SetElement(e1);
  m_Registration.SetMaterial(m);

  try
    {
    // Register the images
    m_Registration.RunRegistration();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cout << "Caught an exception: " << std::endl;
    std::cout << err << std::endl;
    throw err;
    }

  m_DeformedImage=m_Registration.GetWarpedImage();


  typedef itk::CastImageFilter<MovingImageType,LabelImageType> CasterType1;
  typedef itk::CastImageFilter<LabelImageType,MovingImageType> CasterType2;
  typename CasterType1::Pointer Rcaster1 = CasterType1::New();
  typename CasterType2::Pointer Rcaster2 = CasterType2::New();

 

  typedef MinimumMaximumImageFilter<MovingImageType> MinMaxFilterType;
  typename MinMaxFilterType::Pointer minMaxFilter = MinMaxFilterType::New();
  minMaxFilter->SetInput( m_DeformedImage );
  minMaxFilter->Update();
  float min = minMaxFilter->GetMinimum();
  double shift = -1.0 * static_cast<double>( min );
  double scale = static_cast<double>( minMaxFilter->GetMaximum() );
  scale += shift;
  scale = 255.0 / scale;
  typedef ShiftScaleImageFilter<MovingImageType, MovingImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput( m_DeformedImage );
  filter->SetShift( shift );
  filter->SetScale( scale );
  filter->Update();
  m_DeformedImage = filter->GetOutput();

// for testing the full field 
  Rcaster2->SetInput(m_AtlasLabelImage); Rcaster2->Update();
  m_Registration.WarpImage(Rcaster2->GetOutput());
  m_DeformedImage=m_Registration.GetWarpedImage();
  

  Rcaster1->SetInput(m_DeformedImage); Rcaster1->Update();
  m_WarpedAtlasLabelImage=Rcaster1->GetOutput();
  
  std::string fn1;
  {
  typename ImageFileWriter<LabelImageType>::Pointer writer;
  writer = itk::ImageFileWriter<LabelImageType>::New();
  fn1="warpedimage.hdr";
  writer->SetFileName(fn1.c_str());
  writer->SetInput( m_WarpedAtlasLabelImage ); 
  writer->Write();
 }
 {
  typedef Image<float,ImageDimension> RealImageType;
  typename ImageFileWriter<RealImageType>::Pointer writer2;
  writer2 = itk::ImageFileWriter<RealImageType>::New();
  fn1="jac2.hdr";
  writer2->SetFileName(fn1.c_str());
  writer2->SetInput( m_Registration.GetJacobianImage() ); 
  writer2->Write();
 }
      // now write the components of the images
      fn1="warp";
      m_Registration.SetDisplacementsFile(fn1.c_str());
      for (unsigned int i=0; i<ImageDimension;i++)
        m_Registration.WriteDisplacementField(i);

  delete e1;
}



} // namespace itk


#endif
