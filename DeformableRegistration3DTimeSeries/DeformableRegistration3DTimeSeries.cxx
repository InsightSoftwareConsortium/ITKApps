/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    DeformableRegistration3DTimeSeries.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "DeformableRegistration3DTimeSeries.h"
#include "FL/Fl_File_Chooser.h"
#include "itkImageFileWriter.h"
#include <iostream>
#include "itkImageRegionIterator.h"
#include "itkHistogramMatchingImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkExceptionObject.h"
#include "itkEventObject.h"
#include "itkTimeProbe.h"



#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

namespace fltk {

DeformableRegistration3DTimeSeries
::DeformableRegistration3DTimeSeries()
{
  m_file1_reader = ImageReaderType::New();
  m_extractfilter = ExtractFilterType::New();
  m_AdaptorFilter  = AdaptorFilterType::New();
  m_file2_reader = DeformableReaderImageType::New();
  m_extractdeformablefilter =ExtractDeformableFilterType::New();
  m_ConnectorFilter = ConnectorFilterType::New();
  m_file3_reader =  SegmentedImageReaderType::New();
  m_SegmentedAdaptorFilter =SegmentedAdaptorFilterType::New();
  m_VectorImage4D =InputDeformableImageType::New();
  m_interpolator = InterpolatorType::New();
  m_WarperField = WarperType::New();
  calculator=ImageMomentsType::New();
  m_timeserie=0;
  m_Image4D=InputImageType::New();

  m_DicomReaderCommand = itk::SimpleMemberCommand<DeformableRegistration3DTimeSeries>::New();
  m_DicomReaderCommand->SetCallbackFunction(this, &DeformableRegistration3DTimeSeries::ProcessDicomReaderInteraction);
  m_DicomVolumeReader.AddObserver( m_DicomReaderCommand );


}


DeformableRegistration3DTimeSeries
::~DeformableRegistration3DTimeSeries()
{
  m_AdaptorFilter->GetImporter()->Delete();
  m_SegmentedAdaptorFilter->GetImporter()->Delete();
  m_ConnectorFilter->GetImporter()->Delete();
}


void DeformableRegistration3DTimeSeries
::ViewVolume()
{
  if(m_segmented_loaded)
  {
    m_SegmentedAdaptorFilter->GetImporter()->Update();
    fltkRenderWindowInteractor2->Initialize();
    this->Select3DView(m_SegmentedAdaptorFilter->GetImporter()->GetOutput());
    MotionPathView->show();
    fltkRenderWindowInteractor->show();
    Fl::check();
  }
  
  if(m_image_loaded&&!m_segmented_loaded)
  {
    m_AdaptorFilter->GetImporter()->Update(); 
    //This is the only way to get proprely the extent
    fltkRenderWindowInteractor2->Initialize();
    this->Select3DView(m_AdaptorFilter->GetImporter()->GetOutput());
    MotionPathView->show();
    fltkRenderWindowInteractor->show();
    Fl::check();
  }
}


void DeformableRegistration3DTimeSeries
::SelectTime(int timeserie)
{
  index[0] =0 ;
  index[1] =0 ;
  index[2] =0 ;
  index[3] =timeserie;
  m_timeserie=timeserie;
  desiredRegion.SetIndex( index );
  if(m_image_loaded&&!m_deformable_loaded)
  {
    size[0] =m_Image4D->GetLargestPossibleRegion().GetSize()[0];
    size[1] =m_Image4D->GetLargestPossibleRegion().GetSize()[1];
    size[2] =m_Image4D->GetLargestPossibleRegion().GetSize()[2];
    size[3] =0;
    desiredRegion.SetSize(  size  );
    m_extractfilter->SetInput(m_Image4D); 
    m_extractfilter->SetExtractionRegion(desiredRegion );
    m_extractfilter->UpdateLargestPossibleRegion();
    m_AdaptorFilter->SetInput(m_extractfilter->GetOutput());
    m_AdaptorFilter->GetImporter()->Update(); 
    //This is the only way to get proprely the extent
    this->SetImageVtkExtent(m_AdaptorFilter->GetImporter()->GetOutput()); 
    this->SetImageVtk(m_AdaptorFilter->GetImporter()->GetOutput());  
  }
  if(!m_image_loaded&&m_deformable_loaded)
  {
    size[0] =m_VectorImage4D->GetLargestPossibleRegion().GetSize()[0];
    size[1] =m_VectorImage4D->GetLargestPossibleRegion().GetSize()[1];
    size[2] =m_VectorImage4D->GetLargestPossibleRegion().GetSize()[2];
    size[3] =0;
    desiredRegion.SetSize(  size  );
    m_extractdeformablefilter->SetInput(m_VectorImage4D); 
    m_extractdeformablefilter->SetExtractionRegion(desiredRegion );
    m_ConnectorFilter->SetInput(m_extractdeformablefilter->GetOutput());
    m_ConnectorFilter->GetImporter()->Update();
    this->SetImageVtkExtent(m_ConnectorFilter->GetImporter()->GetOutput()); 
    this->SetDeformableFieldImageVtk(m_ConnectorFilter->GetOutput()); 
  }
  if(m_image_loaded&&m_deformable_loaded)
  {
    size[0] =m_Image4D->GetLargestPossibleRegion().GetSize()[0];
    size[1] =m_Image4D->GetLargestPossibleRegion().GetSize()[1];
    size[2] =m_Image4D->GetLargestPossibleRegion().GetSize()[2];
    size[3] =0;
    desiredRegion.SetSize(  size  );
    m_extractfilter->SetExtractionRegion(desiredRegion );
  //  m_extractfilter->SetInput(m_file1_reader->GetOutput()); 
     m_extractfilter->SetInput(m_Image4D); 
    m_extractfilter->UpdateLargestPossibleRegion();
    m_AdaptorFilter->GetExporter()->SetInput(m_extractfilter->GetOutput());
    m_AdaptorFilter->GetImporter()->Update(); 
    this->SetImageVtkExtent(m_AdaptorFilter->GetImporter()->GetOutput()); 
    this->SetImageVtk(m_AdaptorFilter->GetImporter()->GetOutput()); 
    if(timeserie>0)//m_VectorImage4D->GetLargestPossibleRegion().GetSize()[3])
    {
      if(m_deformable_visible)this->ShowDeformableFieldData();
      index[0] =0 ;
      index[3] =timeserie-1;
      desiredRegion.SetIndex( index );
      m_extractdeformablefilter->SetInput(m_VectorImage4D); 
      m_extractdeformablefilter->SetExtractionRegion(desiredRegion );
      m_ConnectorFilter->SetInput(m_extractdeformablefilter->GetOutput());
      m_ConnectorFilter->GetImporter()->Update();
      this->SetDeformableFieldImageVtk(m_ConnectorFilter->GetImporter()->GetOutput());
    }
    else 
    {
      if(m_deformable_visible)this->HideDeformableFieldData();
    }
  }
  if(m_segmented_loaded&&m_deformable_loaded)
  {
    if(timeserie==0)
    {
      typedef itk::CastImageFilter< 
                            InputSegmentedImageType,
                            OutputImageType > CastFilterType;
     
      CastFilterType::Pointer  caster =  CastFilterType::New();
      caster->SetInput( m_file3_reader->GetOutput() );
      caster->Update();
      m_SegmentedAdaptorFilter->GetExporter()->SetInput(caster->GetOutput());
      m_SegmentedAdaptorFilter->GetImporter()->Update();
      this->SetImageVtkExtent(m_SegmentedAdaptorFilter->GetImporter()->GetOutput()); 
      this->SetSegmentedImageVtk(m_SegmentedAdaptorFilter->GetImporter()->GetOutput()); 
    }
    else
    {
      m_WarperField->SetInterpolator( m_interpolator );
      m_WarperField->SetInput( m_file3_reader->GetOutput());
      m_WarperField->SetDeformationField(m_extractdeformablefilter->GetOutput());
      m_WarperField->SetOutputSpacing(m_file3_reader->GetOutput()->GetSpacing() );
      m_WarperField->SetOutputOrigin(m_file3_reader->GetOutput()->GetOrigin() );
      InputSegmentedImageType::PixelType padValue = 2;
      m_WarperField->SetEdgePaddingValue( padValue );
      
      /*itk::ImageFileWriter<InputSegmentedImageType>::Pointer file1_writer
        = itk::ImageFileWriter<InputSegmentedImageType>::New();
      file1_writer->SetFileName("rez.mhd");
      file1_writer->SetInput(m_WarperField->GetOutput());
      file1_writer->Write();*/
      typedef itk::CastImageFilter< 
                          InputSegmentedImageType,
                          OutputImageType > CastFilterType;
   
      CastFilterType::Pointer  caster =  CastFilterType::New();
      caster->SetInput( m_WarperField->GetOutput() );
      caster->Update();

      m_SegmentedAdaptorFilter->GetExporter()->SetInput(caster->GetOutput());
    } 
    m_SegmentedAdaptorFilter->GetImporter()->Update();
    this->SetSegmentedImageVtk(m_SegmentedAdaptorFilter->GetImporter()->GetOutput()); 
  }
}


void DeformableRegistration3DTimeSeries
::LoadRawData()
{
  const char * filename = fl_file_chooser("4D image filename","*.*","");
  if( !filename || strlen(filename) == 0 )
  {
   // std::cout << "Filename read error" << std::endl;
    return;
  }
  m_file1_reader->SetFileName(filename);
  try {
    m_file1_reader->UpdateLargestPossibleRegion();
  } 
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  } 
  m_image_loaded=1;
  m_Image4D=m_file1_reader->GetOutput();
 
  this->SelectTime(0);
  this->slidertime->maximum(m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[3]-1);
  this->slider->maximum(this->GetMaximum());
  this->slider->activate();
  this->slidertime->activate();
  this->Axial->activate();
  this->Sagittal->activate();
  this->Coronar->activate();
  this->start->activate();
  this->stop->activate();
 }
void DeformableRegistration3DTimeSeries
::LoadDeformableFieldData()
{
  const char * filename = fl_file_chooser("DeformationField filename","*.*","");
  if( !filename  || strlen(filename) == 0 )
  {
    return;
  }
  m_file2_reader->SetFileName( filename);
  try
  {
    m_file2_reader->Update();
  } 
   catch( itk::ExceptionObject & excep )
  {
     std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  }
  m_deformable_loaded=1;
  m_VectorImage4D=m_file2_reader->GetOutput();
  this->SelectTime(0);
  if(!m_image_loaded)
  {
    this->slidertime->maximum(m_file2_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[3]-1);
    this->slider->maximum(this->GetMaximum());
    this->slider->activate();
    this->slidertime->activate();
    this->Axial->activate();
    this->Sagittal->activate();
    this->Coronar->activate();
    this->start->activate();
    this->stop->activate();
  }
}


void DeformableRegistration3DTimeSeries
::LoadDicomSeries()
{
     
  const char * directoryname = fl_dir_chooser("DICOM Volume directory","");
  
  if( !directoryname  || strlen(directoryname) == 0 )
    {
    return;
    }
  m_DicomVolumeReader.SetDirectory( directoryname );
  
  // Attempt to read
 
  m_DicomVolumeReader.CollectSeriesAndSelect();
}


void DeformableRegistration3DTimeSeries
::ProcessDicomReaderInteraction( void )
{
  std::cout << "Processing Dicom Reader Interaction " << std::endl; 
  bool volumeIsLoaded = m_DicomVolumeReader.IsVolumeLoaded();
   m_image_loaded=1;
  if( volumeIsLoaded )
  {
    m_Image4D=m_DicomVolumeReader.GetOutput();
    this->SelectTime(0);  
    this->slidertime->maximum(m_Image4D->GetLargestPossibleRegion().GetSize()[3]-1);
    this->slider->maximum(this->GetMaximum());
    this->slider->activate();
    this->slidertime->activate();
    this->Axial->activate();
    this->Sagittal->activate();
    this->Coronar->activate();
    this->start->activate();
    this->stop->activate();
  }
}



void DeformableRegistration3DTimeSeries
::LoadSegmentedData()
{
  const char * filename = fl_file_chooser("DeformationField filename","*.*","");
  if( !filename  || strlen(filename) == 0 )
  {
    return;
  }
  m_file3_reader->SetFileName(filename);
  try
  {
    m_file3_reader->Update();
  } 
  catch( itk::ExceptionObject & excep )
  {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
  } 
  m_segmented_loaded=1;
  this->SelectTime(0);
}



void DeformableRegistration3DTimeSeries
::SetImage(OutputImageType * img)
{
}



void 
DeformableRegistration3DTimeSeries
::Show(void)
{
  this->SetInteractor(fltkRenderWindowInteractor);
  fltkRenderWindowInteractor->Initialize();
  DeformableRegistration3DTimeSeriesGUI::Show(); 
}



void 
DeformableRegistration3DTimeSeries
::Hide(void)
{
  DeformableRegistration3DTimeSeriesGUI::Hide();
}



class CommandIterationUpdate : public itk::Command 
  {
  public:
    typedef  CommandIterationUpdate   Self;
    typedef  itk::Command             Superclass;
    typedef  itk::SmartPointer<CommandIterationUpdate>  Pointer;
    itkNewMacro( CommandIterationUpdate );
  protected:
    CommandIterationUpdate() {};

    typedef itk::Image< float, 3 > InternalImageType;
    typedef itk::Vector< float, 3 >    VectorPixelType;
    typedef itk::Image<  VectorPixelType, 3 > DeformationFieldType;

    typedef itk::DemonsRegistrationFilter<
                                InternalImageType,
                                InternalImageType,
                                DeformationFieldType>   RegistrationFilterType;

  public:

    void Execute(itk::Object *caller, const itk::EventObject & event)
      {
        Execute( (const itk::Object *)caller, event);
      }

    void Execute(const itk::Object * object, const itk::EventObject & event)
      {
         const RegistrationFilterType * filter = 
          dynamic_cast< const RegistrationFilterType * >( object );
        if( typeid( event ) != typeid( itk::IterationEvent ) )
          {
          return;
          }
        std::cout << "Progress: " << filter->GetProgress() << "  ";
        std::cout << "Iter: " << filter->GetElapsedIterations() << "  ";
        std::cout << "RMSChange: " << filter->GetRMSChange() << "  ";
        std::cout << "Metric: "   << filter->GetMetric() << std::endl;
      }
};



void 
DeformableRegistration3DTimeSeries
::StartDemonsDeformation()
{
  itk::TimeProbe  myprobe;
  typedef float InternalPixelType;
  typedef itk::Image< InternalPixelType, 3 > InternalImageType;
  typedef itk::CastImageFilter< OutputImageType, 
                                InternalImageType > FixedImageCasterType;
  typedef itk::CastImageFilter< OutputImageType, 
                                InternalImageType > MovingImageCasterType;
  typedef itk::ExtractImageFilter< InputImageType, 
                                       OutputImageType> ExtractFilterType;
  typedef itk::HistogramMatchingImageFilter<
                                    InternalImageType,
                                    InternalImageType >   MatchingFilterType;
  m_DeformableFilter = RegistrationFilterType::New();
  FixedImageCasterType::Pointer fixedImageCaster   = FixedImageCasterType::New();
  MovingImageCasterType::Pointer movingImageCaster = MovingImageCasterType::New();

  MatchingFilterType::Pointer matcher = MatchingFilterType::New();
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();

  ExtractFilterType::Pointer          m_extractfilter1= ExtractFilterType::New();
  ExtractFilterType::Pointer          m_extractfilter2= ExtractFilterType::New();

  typedef itk::ImageRegionConstIterator< OutputDeformableImageType >  Iterator3D;
  typedef itk::ImageRegionIterator< InputDeformableImageType >  Iterator4D;
  //if(m_VectorImage4D==NULL)
  //{
    InputDeformableImageType::SizeType size4D;
    InputDeformableImageType::IndexType start4D;
    start4D[0] =0 ;
    start4D[1] =0 ;
    start4D[2] =0 ;
    start4D[3] =0;
    size4D[ 0 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
    size4D[ 1 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
    size4D[ 2 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];
    size4D[ 3 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[3]-1;
    InputDeformableImageType::RegionType region;
    region.SetSize( size4D );
    region.SetIndex( start4D );  
    m_VectorImage4D->SetRegions( region );
    m_VectorImage4D->Allocate();
  //}
  Iterator4D it4( m_VectorImage4D, m_VectorImage4D->GetBufferedRegion() );
  it4.GoToBegin();
  
  OutputDeformableImageType::Pointer VectorImage3D = OutputDeformableImageType::New();
  OutputDeformableImageType::RegionType  region3D;
  OutputDeformableImageType::IndexType   index3D;
  OutputDeformableImageType::SizeType    size3D; 
  size3D[ 0 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
  size3D[ 1 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
  size3D[ 2 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];
  index3D.Fill(0);
  region3D.SetSize( size3D );
  region3D.SetIndex( index3D );
  VectorImage3D->SetLargestPossibleRegion( region3D );
  VectorImage3D->SetBufferedRegion( region3D );
  VectorImage3D->Allocate();

   // fill initial deformation with zero vectors
  PixelType zeroVec;
  zeroVec.Fill( 0.0 );
  VectorImage3D->FillBuffer( zeroVec );
  
  myprobe.Start();
  m_DeformableFilter->AddObserver( itk::IterationEvent(), observer );
  for(int i=1;i<m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[3];i++)
  {
  std::cout<<"i:"<<i<<i+1<<std::endl;
  InputImageType::IndexType start;
  InputImageType::SizeType size;
  start[0] =0 ;
  start[1] =0 ;
  start[2] =0 ;
  start[3] =i;
  size[0] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
  size[1] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
  size[2] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];
  size[3] =0; 

  InputImageType::RegionType desiredRegion;
  desiredRegion.SetSize(  size  );
  desiredRegion.SetIndex( start );
  m_extractfilter1->SetExtractionRegion(desiredRegion );
  m_extractfilter1->SetInput(m_file1_reader->GetOutput()); 
  fixedImageCaster->SetInput(m_extractfilter1->GetOutput() );


  start[0] =0 ;
  start[1] =0 ;
  start[2] =0 ;
  start[3] =0;//i
  size[0] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
  size[1] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
  size[2] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];
  size[3] =0;
  desiredRegion.SetSize(  size  );
  desiredRegion.SetIndex( start );
  m_extractfilter2->SetExtractionRegion(desiredRegion );
  m_extractfilter2->SetInput(m_file1_reader->GetOutput()); 
  movingImageCaster->SetInput( m_extractfilter2->GetOutput()  ); 
  
  matcher->SetInput( movingImageCaster->GetOutput() );
  matcher->SetReferenceImage( fixedImageCaster->GetOutput() );
  matcher->SetNumberOfHistogramLevels( 1024 );
  matcher->SetNumberOfMatchPoints( 7 );
  matcher->ThresholdAtMeanIntensityOn();

 
  m_DeformableFilter->SetFixedImage( fixedImageCaster->GetOutput() );
  m_DeformableFilter->SetMovingImage( matcher->GetOutput() );
  m_DeformableFilter->SetNumberOfIterations( 150 );
  m_DeformableFilter->SetStandardDeviations( 4.0 );
  m_DeformableFilter->SetMaximumError( 0.08 );
  m_DeformableFilter->SetMaximumKernelWidth( 150 );
  m_DeformableFilter->SetIntensityDifferenceThreshold( 0.001 );
  m_DeformableFilter->SetInitialDeformationField(VectorImage3D);

  //this->progressBar->value( 1/m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[3] );

  this->progressBar->Observe(m_DeformableFilter);
  try {
      m_DeformableFilter->UpdateLargestPossibleRegion();
     }
  catch (itk::AbortEvent & exc) {
      m_DeformableFilter->ResetPipeline();
      std::cerr << "ExceptionObject caught !" << std::endl; 
      std::cerr << exc << std::endl; 
      return;
      }
  VectorImage3D = m_DeformableFilter->GetOutput();
  region3D = VectorImage3D->GetLargestPossibleRegion();
  index3D  = region3D.GetIndex();
  size3D   = region3D.GetSize(); 
  Iterator3D it3( VectorImage3D, region3D );
  it3.GoToBegin();
  while( !it3.IsAtEnd() ){
     it4.Set( it3.Get() );
     ++it3;
     ++it4;
   }
   //m_VectorImage4D->DisconnectPipeline();
  }
  myprobe.Stop();
  this->progressBar->value(0);
  m_VectorImage4D->SetSpacing(m_file1_reader->GetOutput()->GetSpacing());
  this->TotalTime->value(myprobe.GetMeanTime());
  m_deformable_loaded=1;
  this->SelectTime(0);
}


void 
DeformableRegistration3DTimeSeries
::StartBSplineDeformation()
{

  metric        = MetricType::New();
  optimizer     = OptimizerType::New();
  BSplineInterpolator  = BSInterpolatorType::New();
  registration  = RegistrationType::New();
  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  BSplineInterpolator  );
  TransformType::Pointer  transformLow = TransformType::New();
  registration->SetTransform( transformLow );

  ExtractFilterType::Pointer          m_extractfilter1= ExtractFilterType::New();
  ExtractFilterType::Pointer          m_extractfilter2= ExtractFilterType::New();

  InputDeformableImageType::SizeType size4D;
  InputDeformableImageType::IndexType start4D;
  start4D[0] =0 ;
  start4D[1] =0 ;
  start4D[2] =0 ;
  start4D[3] =0;
  size4D[ 0 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
  size4D[ 1 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
  size4D[ 2 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];
  size4D[ 3 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[3]-1;
  InputDeformableImageType::RegionType region;
  region.SetSize( size4D );
  region.SetIndex( start4D );  
  m_VectorImage4D->SetRegions( region );
  m_VectorImage4D->Allocate();

  Iterator4D it4( m_VectorImage4D, m_VectorImage4D->GetBufferedRegion() );
  it4.GoToBegin();
  
//  myprobe.Start();
  
  for(int i=1;i<m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[3];i++)
  {
  std::cout<<"i:"<<i<<i+1<<std::endl;
  InputImageType::IndexType start;
  InputImageType::SizeType size;
  start[0] =0 ;
  start[1] =0 ;
  start[2] =0 ;
  start[3] =i;
  size[0] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
  size[1] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
  size[2] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];
  size[3] =0; 

  InputImageType::RegionType desiredRegion;
  desiredRegion.SetSize(  size  );
  desiredRegion.SetIndex( start );
  m_extractfilter1->SetExtractionRegion(desiredRegion );
  m_extractfilter1->SetInput(m_file1_reader->GetOutput()); 
  
  start[0] =0 ;
  start[1] =0 ;
  start[2] =0 ;
  start[3] =0;//i
  size[0] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
  size[1] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
  size[2] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];
  size[3] =0;
  desiredRegion.SetSize(  size  );
  desiredRegion.SetIndex( start );
  m_extractfilter2->SetExtractionRegion(desiredRegion );
  m_extractfilter2->SetInput(m_file1_reader->GetOutput()); 
  m_extractfilter1->Update();
  m_extractfilter2->Update();
  registration->SetFixedImage(m_extractfilter1->GetOutput());
  registration->SetMovingImage(m_extractfilter2->GetOutput());

  OutputImageType::RegionType fixedRegion = m_extractfilter1->GetOutput()->GetBufferedRegion();
  registration->SetFixedImageRegion( fixedRegion );
  typedef TransformType::RegionType RegionType;
  RegionType bsplineRegionLow;
  RegionType::SizeType   gridBorderSize;
  RegionType::SizeType   totalGridSize;
  gridBorderSize.Fill( 3 );    // Border for spline order = 3 ( 1 lower, 2 upper )
  RegionType::SizeType   gridLowSizeOnImage;
  gridLowSizeOnImage.Fill( 5 );
  totalGridSize = gridLowSizeOnImage + gridBorderSize;
  RegionType bsplineRegion;
  bsplineRegion.SetSize( totalGridSize );
  typedef TransformType::SpacingType SpacingType;
  SpacingType spacingLow = m_extractfilter1->GetOutput()->GetSpacing();
  typedef TransformType::OriginType OriginType;
  OriginType originLow = m_extractfilter1->GetOutput()->GetOrigin();;
  OutputImageType::SizeType fixedImageSize = fixedRegion.GetSize();
  for(unsigned int r=0; r<3; r++)
    {
    spacingLow[r] *= floor( static_cast<double>(fixedImageSize[r] - 1)  / 
                            static_cast<double>(gridLowSizeOnImage[r] - 1) );
    originLow[r]  -=  spacingLow[r]; 
    }
  transformLow->SetGridSpacing( spacingLow );
  transformLow->SetGridOrigin( originLow );
  transformLow->SetGridRegion( bsplineRegion );
  typedef TransformType::ParametersType     ParametersType;
  const unsigned int numberOfParameters =
               transformLow->GetNumberOfParameters();
  ParametersType parametersLow( numberOfParameters );
  parametersLow.Fill( 0.0 );
  transformLow->SetParameters( parametersLow );
  registration->SetInitialTransformParameters( transformLow->GetParameters() );
  

  OptimizerType::BoundSelectionType boundSelect( transformLow->GetNumberOfParameters() );
  OptimizerType::BoundValueType upperBound( transformLow->GetNumberOfParameters() );
  OptimizerType::BoundValueType lowerBound(transformLow->GetNumberOfParameters() );

  boundSelect.Fill( 0 );
  upperBound.Fill( 0.0 );
  lowerBound.Fill( 0.0 );

  optimizer->SetBoundSelection( boundSelect );
  optimizer->SetUpperBound( upperBound );
  optimizer->SetLowerBound( lowerBound );

  optimizer->SetCostFunctionConvergenceFactor( 1e+12 );
  optimizer->SetProjectedGradientTolerance( 1.0 );
  optimizer->SetMaximumNumberOfIterations( 500 );
  optimizer->SetMaximumNumberOfEvaluations( 2000 );
  optimizer->SetMaximumNumberOfCorrections( 5 );


  
  std::cout << "Starting Registration with low resolution transform" << std::endl;
  this->progressBar->Observe(registration);
  try 
  { 
    registration->StartRegistration(); 
  } 
  catch( itk::ExceptionObject & err ) 
  { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
    //    return -1;
  } 
  //  Once the registration has finished with the low resolution grid, we
  //  proceed to instantiate a higher resolution
  TransformType::Pointer  transformHigh = TransformType::New();
  RegionType::SizeType   gridHighSizeOnImage;
  gridHighSizeOnImage.Fill( 10 );
  totalGridSize = gridHighSizeOnImage + gridBorderSize;
  bsplineRegion.SetSize( totalGridSize );
  SpacingType spacingHigh = m_extractfilter1->GetOutput()->GetSpacing();
  OriginType  originHigh  = m_extractfilter1->GetOutput()->GetOrigin();;

  for(unsigned int rh=0; rh<3; rh++)
  {
    spacingHigh[rh] *= floor( static_cast<double>(fixedImageSize[rh] - 1)  / 
                            static_cast<double>(gridHighSizeOnImage[rh] - 1) );
    originHigh[rh]  -=  spacingHigh[rh]; 
  }
  transformHigh->SetGridSpacing( spacingHigh );
  transformHigh->SetGridOrigin( originHigh );
  transformHigh->SetGridRegion( bsplineRegion );
  ParametersType parametersHigh( transformHigh->GetNumberOfParameters() );
  parametersHigh.Fill( 0.0 );
  //  Now we need to initialize the BSpline coefficients of the higher resolution
  //  transform. This is done by first computing the actual deformation field 
  //  at the higher resolution from the lower resolution BSpline coefficients. 
  //  Then a BSpline decomposition is done to obtain the BSpline coefficient of 
  //  the higher resolution transform.
  unsigned int counter = 0;
  for ( unsigned int k = 0; k < 3; k++ )
  {
    typedef TransformType::ImageType ParametersImageType;
    typedef itk::ResampleImageFilter<ParametersImageType,ParametersImageType> ResamplerType;
    ResamplerType::Pointer upsampler = ResamplerType::New();
    typedef itk::BSplineResampleImageFunction<ParametersImageType,double> FunctionType;
    FunctionType::Pointer function = FunctionType::New();
    typedef itk::IdentityTransform<double,3> IdentityTransformType;
    IdentityTransformType::Pointer identity = IdentityTransformType::New();
    upsampler->SetInput( transformLow->GetCoefficientImage()[k] );
    upsampler->SetInterpolator( function );
    upsampler->SetTransform( identity );
    upsampler->SetSize( transformHigh->GetGridRegion().GetSize() );
    upsampler->SetOutputSpacing( transformHigh->GetGridSpacing() );
    upsampler->SetOutputOrigin( transformHigh->GetGridOrigin() );
    typedef itk::BSplineDecompositionImageFilter<ParametersImageType,ParametersImageType>
      DecompositionType;
    DecompositionType::Pointer decomposition = DecompositionType::New();
    decomposition->SetSplineOrder( 3 );
    decomposition->SetInput( upsampler->GetOutput() );
    decomposition->Update();
    ParametersImageType::Pointer newCoefficients = decomposition->GetOutput();
    // copy the coefficients into the parameter array
    typedef itk::ImageRegionIterator<ParametersImageType> Iterator;
    Iterator it( newCoefficients, transformHigh->GetGridRegion() );
    while ( !it.IsAtEnd() )
    {
      parametersHigh[ counter++ ] = it.Get();
      ++it;
    }
  }
  transformHigh->SetParameters( parametersHigh );
  //  We now pass the parameters of the high resolution transform as the initial
  //  parameters to be used in a second stage of the registration process.
  std::cout << "Starting Registration with high resolution transform" << std::endl;
  registration->SetInitialTransformParameters( transformHigh->GetParameters() );
  registration->SetTransform( transformHigh );

  OptimizerType::BoundSelectionType boundSelectHigh( transformHigh->GetNumberOfParameters() );
  OptimizerType::BoundValueType upperBoundHigh( transformHigh->GetNumberOfParameters() );
  OptimizerType::BoundValueType lowerBoundHigh( transformHigh->GetNumberOfParameters() );

  boundSelectHigh.Fill( 0 );
  upperBoundHigh.Fill( 0.0 );
  lowerBoundHigh.Fill( 0.0 );

  optimizer->SetBoundSelection( boundSelectHigh );
  optimizer->SetUpperBound( upperBoundHigh );
  optimizer->SetLowerBound( lowerBoundHigh );

  optimizer->SetCostFunctionConvergenceFactor( 1e+12 );
  optimizer->SetProjectedGradientTolerance( 1.0 );
  optimizer->SetMaximumNumberOfIterations( 500 );
  optimizer->SetMaximumNumberOfEvaluations( 2000 );
  optimizer->SetMaximumNumberOfCorrections( 5 );
  try 
  { 
    registration->StartRegistration(); 
  } 
  catch( itk::ExceptionObject & err ) 
  { 
    std::cerr << "ExceptionObject caught !" << std::endl; 
    std::cerr << err << std::endl; 
  } 
   // Generate the explicit deformation field resulting from 
  // the registration.
  OutputDeformableImageType::Pointer field = OutputDeformableImageType::New();

  field->SetRegions(fixedRegion );
  field->SetOrigin(m_extractfilter1->GetOutput()->GetOrigin() );
  field->SetSpacing(m_extractfilter1->GetOutput()->GetSpacing() );
  field->Allocate();

  typedef itk::ImageRegionIterator< OutputDeformableImageType > FieldIterator;
  FieldIterator fi( field, fixedRegion );
  fi.GoToBegin();
  TransformType::InputPointType  fixedPoint;
  TransformType::OutputPointType movingPoint;
  OutputDeformableImageType::IndexType index;
  PixelType displacement;
  while( ! fi.IsAtEnd() )
  {
    index = fi.GetIndex();
    field->TransformIndexToPhysicalPoint( index, fixedPoint );
    movingPoint = transformHigh->TransformPoint( fixedPoint );
    displacement[0] = movingPoint[0] - fixedPoint[0];
    displacement[1] = movingPoint[1] - fixedPoint[1];
    displacement[2] = movingPoint[2] - fixedPoint[2];
    fi.Set( displacement );
    ++fi;
  }
  OutputDeformableImageType::RegionType  region3D = field->GetBufferedRegion();
  OutputDeformableImageType::IndexType   index3D  = region3D.GetIndex();
  OutputDeformableImageType::SizeType    size3D   = region3D.GetSize(); 
  
  OutputDeformableImageType::RegionType region = field->GetLargestPossibleRegion();
  Iterator3D it3( field, region );
  it3.GoToBegin();
  while( !it3.IsAtEnd() )
  {
    it4.Set( it3.Get() );
    ++it3;
    ++it4;
  }
  }
  //myprobe.Stop();
  m_VectorImage4D->SetSpacing(m_file1_reader->GetOutput()->GetSpacing());
  this->progressBar->value(0);
  //this->TotalTime->value(myprobe.GetMeanTime());
  m_deformable_loaded=1;
  this->SelectTime(0);
}
void 
DeformableRegistration3DTimeSeries
::StartFEMDeformation()
{
  
  cout<<"Start FEM Registration";
  itk::TimeProbe  myprobe;
  typedef float InternalPixelType;
  typedef itk::Image< InternalPixelType, 3 > InternalImageType;
  typedef itk::CastImageFilter< OutputImageType, 
                                InternalImageType > FixedImageCasterType;
  typedef itk::CastImageFilter< OutputImageType, 
                                InternalImageType > MovingImageCasterType;
  
  typedef itk::HistogramMatchingImageFilter<
                                    InternalImageType,
                                    InternalImageType >   MatchingFilterType;
  FixedImageCasterType::Pointer fixedImageCaster   = FixedImageCasterType::New();
  MovingImageCasterType::Pointer movingImageCaster = MovingImageCasterType::New();

  MatchingFilterType::Pointer matcher = MatchingFilterType::New();
  //CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();

  ExtractFilterType::Pointer          m_extractfilter1= ExtractFilterType::New();
  ExtractFilterType::Pointer          m_extractfilter2= ExtractFilterType::New();

  typedef itk::ImageRegionConstIterator< OutputDeformableImageType >  Iterator3D;
  typedef itk::ImageRegionIterator< InputDeformableImageType >  Iterator4D;

  // Register the correct load implementation with the element-typed visitor dispatcher. 
  {
  Element3DType::LoadImplementationFunctionPointer fp = 
    &itk::fem::ImageMetricLoadImplementation<ImageLoadType>::ImplementImageMetricLoad;
  DispatcherType::RegisterVisitor((ImageLoadType*)0,fp);
  }

  {
  Element3DType2::LoadImplementationFunctionPointer fp =
    &itk::fem::ImageMetricLoadImplementation<ImageLoadType>::ImplementImageMetricLoad;
  DispatcherType2::RegisterVisitor((ImageLoadType*)0,fp);
  }

    FEMRegistrationType::Pointer registrationFilter = FEMRegistrationType::New(); 

  // Attempt to read the parameter file, and exit if an error occurs
  registrationFilter->SetConfigFileName("FEMRParameters1.txt");
  registrationFilter->ReadConfigFile(registrationFilter->GetConfigFileName().c_str()); 

  InputDeformableImageType::SizeType size4D;
  InputDeformableImageType::IndexType start4D;
  start4D[0] =0 ;
  start4D[1] =0 ;
  start4D[2] =0 ;
  start4D[3] =0;
  size4D[ 0 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
  size4D[ 1 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
  size4D[ 2 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];
  size4D[ 3 ] = m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[3]-1;
  InputDeformableImageType::RegionType region;
  region.SetSize( size4D );
  region.SetIndex( start4D );  
  m_VectorImage4D->SetRegions( region );
  m_VectorImage4D->Allocate();
  Iterator4D it4( m_VectorImage4D, m_VectorImage4D->GetBufferedRegion() );
  it4.GoToBegin();
  
  myprobe.Start();
  
  for(int i=1;i<m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[3];i++)
  {
    std::cout<<"i:"<<i<<i+1<<std::endl;
    InputImageType::IndexType start;
    InputImageType::SizeType size;
    start[0] =0 ;
    start[1] =0 ;
    start[2] =0 ;
    start[3] =i;
    size[0] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
    size[1] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
    size[2] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];
    size[3] =0; 

    InputImageType::RegionType desiredRegion;
    desiredRegion.SetSize(  size  );
    desiredRegion.SetIndex( start );
    m_extractfilter1->SetExtractionRegion(desiredRegion );
    m_extractfilter1->SetInput(m_file1_reader->GetOutput()); 
    fixedImageCaster->SetInput(m_extractfilter1->GetOutput() );
    start[0] =0 ;
    start[1] =0 ;
    start[2] =0 ;
    start[3] =0;//i
    size[0] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
    size[1] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
    size[2] =m_file1_reader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];
    size[3] =0;
    desiredRegion.SetSize(  size  );
    desiredRegion.SetIndex( start );
    m_extractfilter2->SetExtractionRegion(desiredRegion );
    m_extractfilter2->SetInput(m_file1_reader->GetOutput()); 
    movingImageCaster->SetInput( m_extractfilter2->GetOutput()  ); 
    
   
    // Rescale the image intensities so that they fall between 0 and 255
    typedef itk::RescaleIntensityImageFilter<InternalImageType,InternalImageType> FilterType;
    FilterType::Pointer movingrescalefilter = FilterType::New();
    FilterType::Pointer fixedrescalefilter = FilterType::New();

    movingrescalefilter->SetInput(movingImageCaster->GetOutput());
    fixedrescalefilter->SetInput(fixedImageCaster->GetOutput());
    const double desiredMinimum =  0.0;
    const double desiredMaximum =  255.0;
    movingrescalefilter->SetOutputMinimum( desiredMinimum );
    movingrescalefilter->SetOutputMaximum( desiredMaximum );
    movingrescalefilter->UpdateLargestPossibleRegion();
    fixedrescalefilter->SetOutputMinimum( desiredMinimum );
    fixedrescalefilter->SetOutputMaximum( desiredMaximum );
    fixedrescalefilter->UpdateLargestPossibleRegion();

    matcher->SetInput( movingrescalefilter->GetOutput() );
    matcher->SetReferenceImage(fixedrescalefilter->GetOutput() );
    matcher->SetNumberOfHistogramLevels( 100 );
    matcher->SetNumberOfMatchPoints( 15 );
    matcher->ThresholdAtMeanIntensityOn();
    
    
    registrationFilter->SetFixedImage(fixedImageCaster->GetOutput());
    registrationFilter->SetMovingImage(matcher->GetOutput());
   
    // Create the material properties
    
    itk::fem::MaterialLinearElasticity::Pointer m;
    m = itk::fem::MaterialLinearElasticity::New();
    m->GN = 0;                  // Global number of the material
    m->E = registrationFilter->GetElasticity();  // Young's modulus -- used in the membrane
    m->A = 1.0;                 // Cross-sectional area
    m->h = 1.0;                 // Thickness
    m->I = 1.0;                 // Moment of inertia
    m->nu = 0.0;                 // Poisson's ratio -- DONT CHOOSE 1.0!!
    m->RhoC = 1.0;              // Density
    
    // Create the element type 
    Element3DType::Pointer e1=Element3DType::New();
    e1->m_mat=dynamic_cast<itk::fem::MaterialLinearElasticity*>( m );
    registrationFilter->SetElement(e1);
    registrationFilter->SetMaterial(m);
    this->progressBar->Observe(registrationFilter);

    //BIG problem is not working with itk 2.0
    
    //registrationFilter->RunRegistration();
    registrationFilter->Update();
    /*
    if (registrationFilter->GetWriteDisplacements()) 
      {
      registrationFilter->WriteDisplacementField(0);
      registrationFilter->WriteDisplacementField(1);
      registrationFilter->WriteDisplacementField(2);

      // We can also write it as a multicomponent vector field
      registrationFilter->WriteDisplacementFieldMultiComponent();
      }
      */

    OutputDeformableImageType::ConstPointer VectorImage3D =( OutputDeformableImageType*)( registrationFilter->GetDeformationField() );
    OutputDeformableImageType::RegionType  region3D = VectorImage3D->GetBufferedRegion();
    OutputDeformableImageType::IndexType   index3D  = region3D.GetIndex();
    OutputDeformableImageType::SizeType    size3D   = region3D.GetSize(); 
    
    OutputDeformableImageType::RegionType region = VectorImage3D->GetLargestPossibleRegion();
    Iterator3D it3( VectorImage3D, region );
    it3.GoToBegin();
    while( !it3.IsAtEnd() )
    {
      it4.Set( it3.Get() );
      ++it3;
      ++it4;
    }
  }
  myprobe.Stop();
  m_VectorImage4D->SetSpacing(m_file1_reader->GetOutput()->GetSpacing());
  this->progressBar->value(0);
  this->TotalTime->value(myprobe.GetMeanTime());
  m_deformable_loaded=1;
  //this->SelectTime(0);   
  
}

void 
DeformableRegistration3DTimeSeries
::AbortDeformation()
{
  //m_DeformableFilter->AbortGenerateDataOn(); 
  m_DeformableFilter->SetAbortGenerateData(true);
  m_DeformableFilter->ResetPipeline();
}
void 
DeformableRegistration3DTimeSeries
::Save4DDeformationImage()
{
  typedef itk::ImageFileWriter<InputDeformableImageType > WriterType;
  WriterType::Pointer writer4D = WriterType::New();
  writer4D->SetFileName("field.mhd");
  writer4D->SetInput( m_VectorImage4D );
  try
    {
    writer4D->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    }
}
void 
DeformableRegistration3DTimeSeries
::Save4DSegmentedImage()
{
  typedef itk::ImageFileWriter<InputDeformableImageType > WriterType;
  WriterType::Pointer writer4D = WriterType::New();
  writer4D->SetFileName("segm.mhd");
  writer4D->SetInput( m_VectorImage4D );
  try
    {
    writer4D->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << excp << std::endl;
    }
}
void 
DeformableRegistration3DTimeSeries
::ComputePathSelectedVolume()
{
  m_extractdeformablefilter2 =ExtractDeformableFilterType::New();
   m_WarperField2 = WarperType::New();
  std::cout<<"Compute path selected volume"<<std::endl;
  InputSegmentedImageType::SpacingType spacing =m_file3_reader->GetOutput()->GetSpacing();
  m_point.reserve(m_VectorImage4D->GetLargestPossibleRegion().GetSize()[3]);
  m_file3_reader->Update();
  calculator->SetImage(m_file3_reader->GetOutput());
  calculator->Compute();
  //double area = calculator->GetTotalMass();
  ImageMomentsType::VectorType cfg=calculator->GetCenterOfGravity();
  m_point[0][0]=cfg[0];m_point[0][1]=cfg[1];m_point[0][2]=cfg[2];
  //std::cout<<m_point[0][0]*spacing[0]<<"mm "<<m_point[0][1]*spacing[1]<<"mm "<<m_point[0][2]*spacing[2]<<"mm"<<std::endl;
  for(int i=0;i<m_VectorImage4D->GetLargestPossibleRegion().GetSize()[3];++i)
    {
    index[0] =0 ;
    index[1] =0 ;
    index[2] =0 ;
    index[3] =i;
    desiredRegion.SetIndex( index );
    size[0] =m_VectorImage4D->GetLargestPossibleRegion().GetSize()[0];
    size[1] =m_VectorImage4D->GetLargestPossibleRegion().GetSize()[1];
    size[2] =m_VectorImage4D->GetLargestPossibleRegion().GetSize()[2];
    size[3] =0;
    desiredRegion.SetSize(  size  );
    m_extractdeformablefilter2->SetInput(m_VectorImage4D); 
    m_extractdeformablefilter2->SetExtractionRegion(desiredRegion );
    m_WarperField2->SetInterpolator( m_interpolator );
    m_WarperField2->SetInput( m_file3_reader->GetOutput());
    m_WarperField2->SetDeformationField(m_extractdeformablefilter2->GetOutput());
    m_WarperField2->SetOutputSpacing(m_file3_reader->GetOutput()->GetSpacing() );
    m_WarperField2->SetOutputOrigin(m_file3_reader->GetOutput()->GetOrigin() );
    InputSegmentedImageType::PixelType padValue = 2;
    m_WarperField2->SetEdgePaddingValue( padValue );
    m_WarperField2->Update();
    calculator->SetImage(m_WarperField2->GetOutput());
    calculator->Compute();
    ImageMomentsType::VectorType cfg=calculator->GetCenterOfGravity();
    m_point[i+1][0]=cfg[0];m_point[i+1][1]=cfg[1];m_point[i+1][2]=cfg[2];
    std::cout<<"Dispacement["<<i<<"]"<<std::endl;
    std::cout<<"A-P:"<<(m_point[i+1][0]-m_point[i][0])*spacing[0]<<"mm "<<std::endl;
    std::cout<<"S-I:"<<(m_point[i+1][1]-m_point[i][1])*spacing[1]<<"mm "<<std::endl;
    std::cout<<"L-R:"<<(m_point[i+1][2]-m_point[i][2])*spacing[2]<<"mm"<<std::endl;
    }  
  fltkRenderWindowInteractor2->Initialize();
  this->ShowMotionPath();
}
void
DeformableRegistration3DTimeSeries
::ComputePathSelectedPoint()
{ 
  m_extractdeformablefiltermotion =ExtractDeformableFilterType::New();
  //if(m_VectorImage4D==NULL)return;
  m_point.reserve(m_VectorImage4D->GetLargestPossibleRegion().GetSize()[3]);
  m_point[0]=m_SelectedPoint;
 
  for(int i=0;i<m_VectorImage4D->GetLargestPossibleRegion().GetSize()[3];i++)
  {
    
  InputDeformableImageType::IndexType start;
  InputDeformableImageType::SizeType size;
  start[0] =0 ;
  start[1] =0 ;
  start[2] =0 ;
  start[3] =i;
  size[0] =m_VectorImage4D->GetLargestPossibleRegion().GetSize()[0];
  size[1] =m_VectorImage4D->GetLargestPossibleRegion().GetSize()[1];
  size[2] =m_VectorImage4D->GetLargestPossibleRegion().GetSize()[2];
  size[3] =0; 

  InputDeformableImageType::RegionType desiredRegion;
  desiredRegion.SetSize(  size  );
  desiredRegion.SetIndex( start );

  m_extractdeformablefiltermotion->SetExtractionRegion(desiredRegion );
  m_extractdeformablefiltermotion->SetInput(m_VectorImage4D); 
  OutputDeformableImageType::ConstPointer VectorImage3D =m_extractdeformablefiltermotion->GetOutput();
  m_extractdeformablefiltermotion->Update();
  //std::cout<<"i:"<<i<<i+1<<std::endl;
    
  PixelType displacement;
  interpolator = VectorInterpolatorType::New();
  interpolator->SetInputImage(VectorImage3D);
    if(interpolator->IsInsideBuffer( m_SelectedPoint))
    {
      OutputType interpolatedValue = interpolator->Evaluate(  m_SelectedPoint);
      for( unsigned int k = 0; k < 3; k++ )
      {
        displacement[k] = static_cast<ValueType>( interpolatedValue[k] );
      }
      for(unsigned int j = 0; j < 3; j++ )
      {
        if(i>0)m_point[i+1][j] = m_SelectedPoint[j] +m_point[i][j] +displacement[j];
        else m_point[i+1][j] =m_SelectedPoint[j]+displacement[j];
      }
      m_SelectedPoint=m_point[i+1];
      //std::cout<<"Point selected"<<m_SelectedPoint[0]<<" "<<m_SelectedPoint[1]<<" "<<m_SelectedPoint[2]<<std::endl;
    }
  }
  fltkRenderWindowInteractor2->Initialize();
  this->ShowMotionPath();
  point_selected=false;
}
} // end namespace itk
