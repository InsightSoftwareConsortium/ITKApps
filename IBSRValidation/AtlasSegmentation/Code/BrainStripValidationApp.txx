/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    BrainStripValidationApp.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _BrainStripValidationApp_txx
#define _BrainStripValidationApp_txx

#include "BrainStripValidationApp.h"

namespace itk
{

template <typename TImage, typename TLabelImage, typename TRealImage>
BrainStripValidationApp<TImage,TLabelImage,TRealImage>
::BrainStripValidationApp()
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
BrainStripValidationApp<TImage,TLabelImage,TRealImage>
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

  this->m_Parser->SetParameterFileName( m_ParameterFileName.c_str() );
}


template <typename TImage, typename TLabelImage, typename TRealImage>
void
BrainStripValidationApp<TImage,TLabelImage,TRealImage>
::InitializePreprocessor()
{
  this->m_Preprocessor->SetInputFixedImage( this->m_Parser->GetSubjectImage() );
  this->m_Preprocessor->SetInputMovingImage( this->m_Parser->GetAtlasImage() );

  this->m_Preprocessor->SetNumberOfHistogramLevels( this->m_Parser->GetNumberOfHistogramLevels() );
  this->m_Preprocessor->SetNumberOfMatchPoints( this->m_Parser->GetNumberOfMatchPoints() );
}


template <typename TImage, typename TLabelImage, typename TRealImage>
void
BrainStripValidationApp<TImage,TLabelImage,TRealImage>
::InitializeRegistrator()
{
  this->m_Registrator->SetFixedImage( this->m_Preprocessor->GetOutputFixedImage() );
  this->m_Registrator->SetMovingImage( this->m_Preprocessor->GetOutputMovingImage() );

  this->m_Registrator->SetNumberOfLevels( this->m_Parser->GetNumberOfLevels() );
  this->m_Registrator->SetNumberOfIterations( this->m_Parser->GetNumberOfIterations() );

  this->m_Registrator->SetFixedImageShrinkFactors( this->m_Parser->GetSubjectImageShrinkFactors() );
  this->m_Registrator->SetMovingImageShrinkFactors( this->m_Parser->GetAtlasImageShrinkFactors() );

}


template <typename TImage, typename TLabelImage, typename TRealImage>
void
BrainStripValidationApp<TImage,TLabelImage,TRealImage>
::InitializeLabeler()
{

  this->m_Labeler->SetAtlasLabelImage( this->m_Parser->GetAtlasLabelImage() );
  this->m_Labeler->SetDeformationField( this->m_Registrator->GetDeformationField() );

  typedef typename TLabelImage::PixelType LabelPixelType;
  this->m_Labeler->SetLowerThreshold( 1 );
  this->m_Labeler->SetUpperThreshold( NumericTraits<LabelPixelType>::max() );

  this->m_Labeler->SetOutputValue( 1 );

}


template <typename TImage, typename TLabelImage, typename TRealImage>
void
BrainStripValidationApp<TImage,TLabelImage,TRealImage>
::InitializeGenerator()
{

  this->m_Generator->SetPatientID( m_SubjectPatientID.c_str() );
  this->m_Generator->SetGroundTruthImage( this->m_Parser->GetSubjectLabelImage() );

  this->m_Generator->SetLabelImage( this->m_Labeler->GetOutputLabelImage() );

  this->m_Generator->SetOutputFileName( m_OutputFileName.c_str() );
  this->m_Generator->SetAppendOutputFile( m_AppendOutputFile );
}


} // namespace itk

#endif
