
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    FEMBrainStripValidationApp.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _FEMBrainStripValidationApp_txx
#define _FEMBrainStripValidationApp_txx

#include "FEMBrainStripValidationApp.h"
#include "itkImageFileWriter.h"

namespace itk
{

template <typename TImage, typename TLabelImage, typename TRealImage>
FEMBrainStripValidationApp<TImage,TLabelImage,TRealImage>
::FEMBrainStripValidationApp()
{ 
  m_ImageDirectoryName = "";
  m_BrainSegmentationDirectoryName = "";
  m_AtlasPatientID = "";
  m_SubjectPatientID = "";
  m_AtlasStartSliceNumber = 1;
  m_SubjectStartSliceNumber = 1;
  m_AtlasNumberOfSlices = 1;
  m_SubjectNumberOfSlices = 1;

  m_ParameterFileName = "";
  m_OutputFileName = "";
  m_AppendOutputFile = true;
}


template <typename TImage, typename TLabelImage, typename TRealImage>
void
FEMBrainStripValidationApp<TImage,TLabelImage,TRealImage>
::InitializeParser()
{
  this->m_Parser->SetImageDirectoryName( m_ImageDirectoryName.c_str() );
  this->m_Parser->SetBrainSegmentationDirectoryName( m_BrainSegmentationDirectoryName.c_str() );

  this->m_Parser->SetAtlasPatientID( m_AtlasPatientID.c_str() );
  this->m_Parser->SetAtlasStartSliceNumber( m_AtlasStartSliceNumber );
  this->m_Parser->SetAtlasNumberOfSlices( m_AtlasNumberOfSlices );

  this->m_Parser->SetSubjectPatientID( m_SubjectPatientID.c_str() );
  this->m_Parser->SetSubjectStartSliceNumber( m_SubjectStartSliceNumber );
  this->m_Parser->SetSubjectNumberOfSlices( m_SubjectNumberOfSlices );
  this->m_Parser->SetImageXSize(m_ImageXSize);
  this->m_Parser->SetImageYSize(m_ImageYSize);

  this->m_Parser->SetParameterFileName( m_ParameterFileName.c_str() );
}


template <typename TImage, typename TLabelImage, typename TRealImage>
void
FEMBrainStripValidationApp<TImage,TLabelImage,TRealImage>
::InitializePreprocessor()
{
  bool Resample=true;

  if ( Resample){  
  
  typedef typename ImageType::SizeType                 ImageSizeType;
  typedef itk::AffineTransform<double,ImageDimension>   AffineTransformType;

  // Create an affine transformation
  typename itk::ResampleImageFilter< ImageType, ImageType >::Pointer resample;
  typename AffineTransformType::Pointer aff = AffineTransformType::New();
  aff->Scale(0.90);

  // Create a linear interpolation image function
  typedef itk::LinearInterpolateImageFunction<ImageType,double>  InterpolatorType;
  typename InterpolatorType::Pointer interp;
  
  
  interp = InterpolatorType::New();
  resample = itk::ResampleImageFilter< ImageType, ImageType >::New();
  ImageSizeType size={{128,128,91}};
  resample->SetSize(size);
  resample->SetTransform(aff.GetPointer());
  resample->SetInterpolator(interp.GetPointer());

  // Run the resampling filter
  
  interp->SetInputImage(this->m_Parser->GetSubjectImage());
  resample->SetInput(this->m_Parser->GetSubjectImage());
  resample->Update();
  this->m_Parser->SetSubjectImage(resample->GetOutput());
  
  interp = InterpolatorType::New();
  resample = itk::ResampleImageFilter< ImageType, ImageType >::New();
  resample->SetSize(size);
  resample->SetTransform(aff.GetPointer());
  resample->SetInterpolator(interp.GetPointer());
  

  interp->SetInputImage(this->m_Parser->GetAtlasImage());
  resample->SetInput(this->m_Parser->GetAtlasImage());
  resample->Update();
  this->m_Parser->SetAtlasImage(resample->GetOutput());



  // Create a linear interpolation image function
  typedef itk::LinearInterpolateImageFunction<LabelImageType,double>  InterpolatorType2;
  typename InterpolatorType2::Pointer interp2;
  // Create and configure a resampling filter
  typename itk::ResampleImageFilter<  LabelImageType, LabelImageType >::Pointer resample2;
  
  
  interp2 = InterpolatorType2::New();
  resample2 = itk::ResampleImageFilter<  LabelImageType,  LabelImageType >::New();
  resample2->SetSize(size);
  resample2->SetTransform(aff.GetPointer());
  resample2->SetInterpolator(interp2.GetPointer());

  // Run the resampling filter
  
  interp2->SetInputImage(this->m_Parser->GetSubjectLabelImage());
  resample2->SetInput(this->m_Parser->GetSubjectLabelImage());
  resample2->Update();
  this->m_Parser->SetSubjectLabelImage(resample2->GetOutput());
  
  
  interp2 = InterpolatorType::New();
  resample2 = itk::ResampleImageFilter<  LabelImageType,  LabelImageType >::New();
  resample2->SetSize(size);
  resample2->SetTransform(aff.GetPointer());
  resample2->SetInterpolator(interp2.GetPointer());

  interp2->SetInputImage(this->m_Parser->GetAtlasLabelImage());
  resample2->SetInput(this->m_Parser->GetAtlasLabelImage());
  resample2->Update();
  this->m_Parser->SetAtlasLabelImage(resample2->GetOutput());
  
  /*
  typedef typename LabelImageType::PixelType PType;
  itk::RawImageIO<PType,ImageDimension>::Pointer io;
  itk::ImageFileWriter<LabelImageType>::Pointer writer;
  io = itk::RawImageIO<PType,ImageDimension>::New();
  writer = itk::ImageFileWriter<LabelImageType>::New();
  writer->SetImageIO(io);
  std::string fn1=m_ImageDirectoryName+"/SmallAIR/whead"+m_SubjectPatientID+".img";
  writer->SetFileName(fn1.c_str());
  writer->SetInput(resample->GetOutput() ); 
  writer->Write();


  std::string fn2=m_ImageDirectoryName+"/SmallAIR/brain"+m_SubjectPatientID+".img";
  writer->SetFileName(fn2.c_str());
  writer->SetInput(resample->GetOutput() ); 
  writer->Write();

  return;
  */
  }

  this->m_Preprocessor->SetInputFixedImage( this->m_Parser->GetSubjectImage() );
  this->m_Preprocessor->SetInputMovingImage( this->m_Parser->GetAtlasImage() );

  this->m_Preprocessor->SetNumberOfHistogramLevels( this->GetNumberOfHistogramLevels() );
  this->m_Preprocessor->SetNumberOfMatchPoints( this->GetNumberOfMatchPoints() );

  this->m_Preprocessor->SetEdgeFilter( this->GetEdgeFilter() );
}


template <typename TImage, typename TLabelImage, typename TRealImage>
void
FEMBrainStripValidationApp<TImage,TLabelImage,TRealImage>
::InitializeRegistrator()
{
  this->m_Registrator->SetFixedImage( this->m_Preprocessor->GetOutputFixedImage() );
  this->m_Registrator->SetMovingImage( this->m_Preprocessor->GetOutputMovingImage() );
  this->m_Registrator->SetFEMConfigurationFileName(this->m_FEMConfigurationFileName.c_str());
  this->m_Registrator->SetAtlasLabelImage( this->m_Parser->GetAtlasLabelImage() );
}


template <typename TImage, typename TLabelImage, typename TRealImage>
void
FEMBrainStripValidationApp<TImage,TLabelImage,TRealImage>
::InitializeLabeler()
{

  this->m_Labeler->SetAtlasLabelImage( this->m_Registrator->GetWarpedAtlasLabelImage() );

  typedef typename TLabelImage::PixelType LabelPixelType;
  this->m_Labeler->SetLowerThreshold( 1 );
  this->m_Labeler->SetUpperThreshold( NumericTraits<LabelPixelType>::max() );

  this->m_Labeler->SetOutputValue( 1 );

}


template <typename TImage, typename TLabelImage, typename TRealImage>
void
FEMBrainStripValidationApp<TImage,TLabelImage,TRealImage>
::InitializeGenerator()
{

  this->m_Generator->SetPatientID( m_SubjectPatientID.c_str() );
  this->m_Generator->SetGroundTruthImage( this->m_Parser->GetSubjectLabelImage() );

  this->m_Generator->SetLabelImage( this->m_Labeler->GetOutputLabelImage() );

  this->m_Generator->SetOutputFileName( m_OutputFileName.c_str() );
  this->m_Generator->SetAppendOutputFile( m_AppendOutputFile );


  typedef typename LabelImageType::PixelType PType;
  typename itk::RawImageIO<PType,ImageDimension>::Pointer io;
  typename itk::ImageFileWriter<LabelImageType>::Pointer writer;
  io = itk::RawImageIO<PType,ImageDimension>::New();
  writer = itk::ImageFileWriter<LabelImageType>::New();
  writer->SetImageIO(io);
  writer->SetFileName("subjectlabel.raw");
  writer->SetInput(this->m_Parser->GetSubjectLabelImage() ); 
  writer->Write();
 
  typename itk::RawImageIO<PType,ImageDimension>::Pointer io2;
  typename itk::ImageFileWriter<LabelImageType>::Pointer writer2;
  io2 = itk::RawImageIO<PType,ImageDimension>::New();
  writer2 = itk::ImageFileWriter<LabelImageType>::New();
  writer2->SetImageIO(io2);
  writer2->SetFileName("outputlabel.raw");
  writer2->SetInput(this->m_Labeler->GetOutputLabelImage()); 
  writer2->Write();

 

}


} // namespace itk

#endif
