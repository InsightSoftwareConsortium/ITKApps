#ifndef __ISIS_LiverTumorSegmentationBase__h__
#define __ISIS_LiverTumorSegmentationBase__h__

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkThresholdImageFilter.h"

#include "DicomImageReader.h"
#include "ConfidenceConnectedModule.h"
#include "ConnectedThresholdModule.h"
#include "ThresholdLevelSetSegmentationModule.h"

/** 
  \class LiverTumorSegmentationBase 
  \brief This class is the base class for the Liver Tumor Segmentation. 

  This class can be used for command line Liver Tumor Segmentation application.
  Currently this class supports four segmentation modules: Threshold module,
  Threshold Level Set module, Confidence Connected module and Connected
  Threshold module. Threshold module is defined in this class internally using 
  a threshold filter. Other module are defined in the Liver Tumor Segmentation
  Library.
  The pixel type of the input image has to be "unsigned char". Therefore 
  any input data from CT modality needs to be preprocessed. The ResampleVolume
  code may be used for converting a CT volume data to a "unsigned char" volume
  data. The resampling code also does resampling for homogenizing the data. 
*/

const int NUMBER_OF_ALGORITHMS = 7;

static char *ModuleNames[] = { "Threshold Module", "Threshold Level Set Module", "Confidence Connected Module", 
    "Connected Threshold Module", "Isolated Connected Module", "Fast Marching Module",
    "Geodesic Active Contour Module", "Watershed Module"
    }; 

typedef enum { THRESHOLD = 1, THRESHOLD_LEVEL_SET, CONFIDENCE_CONNECTED, CONNECTED_THRESHOLD, 
  ISOLATED_CONNECTED, FAST_MARCHING, GEODESIC_ACTIVE_CONTOUR, WATERSHED} SegmentationModuleType;

class LiverTumorSegmentationBase 
{
public:
  
  typedef unsigned char   PixelType;

  typedef unsigned char  VisualizationPixelType;
  
  typedef itk::Image< PixelType, 3 >                  VolumeType;
  
  typedef itk::Image< PixelType, 2 >                  ImageType;
  
  typedef itk::ImageFileReader< VolumeType >          VolumeReaderType;
  
  typedef ISIS::DicomImageReader< VolumeType >        DicomReaderType;
  
  typedef itk::Image< VisualizationPixelType, 3 >     VisualizationVolumeType;
  
  typedef itk::RescaleIntensityImageFilter< 
                          VolumeType,
                          VisualizationVolumeType >   RescaleIntensityFilterType;

  typedef itk::ImageToVTKImageFilter< 
                          VisualizationVolumeType >   ITK2VTKAdaptorFilterType;

  typedef itk::ThresholdImageFilter< VolumeType >     ThresholdFilterType;
  
  typedef ISIS::ThresholdLevelSetSegmentationModule::InputImageType    InputImageType;

  typedef ISIS::ThresholdLevelSetSegmentationModule::OutputImageType   OutputImageType;

  /** Writer that will send the content of the segmented image to
      a file.  This is expected to be a binary mask */

  typedef  itk::ImageFileWriter<  OutputImageType  > WriterType;

public:
  
  LiverTumorSegmentationBase();
  
  virtual ~LiverTumorSegmentationBase();
  
  /** SetSeedPoint sets a seed point for the segmentation process.
  */
  virtual void SetSeedPoint( double x, double y, double z );
  
  /** GetSeedPoint gets the seed point chosen for segmentation.
  */
  virtual void GetSeedPoint(double data[3]); 
  
  /** DoSegmentation method does the segmentation of data. The input parameter 
  is the module id of the segmentation module to be used for segmentation. The
  segmentation parameters are those previously set in the module being used.
  */
  virtual bool DoSegmentation( SegmentationModuleType sType = THRESHOLD_LEVEL_SET );
  
  /** WriteSegmentedVolume method writes the segmentation result into a file.
  The pixel type of the data written is "unsigned char". The pixel values are 
  retained the same as in the input image data. The format of writing data is 
  decided by the extension of the filename provided for writing the data.
  */
  virtual void WriteSegmentedVolume( const char *fname );

  /** WriteBinarySegmentedVolume method writes the segmentation result into a file.
  The pixel type of the data written is "unsigned char". The pixel values are 
  converted into binary, i.e. all non-zero entries are replaced by value '1'. The 
  zero values remain as zeroes. The format of writing data is decided by the extension 
  of the filename provided for writing the data.
  */
  virtual void WriteBinarySegmentedVolume( const char *fname );

  /**
  */
  virtual bool Load( const char *filename );

protected:
  
  double                                   m_SeedPoint[3];

  int                                     m_SeedIndex[3];

  double                                    m_SeedValue;

  VolumeReaderType::Pointer               m_VolumeReader;
  
  RescaleIntensityFilterType::Pointer     m_RescaleIntensity;

  ITK2VTKAdaptorFilterType::Pointer       m_ITK2VTKAdaptor;
  
  RescaleIntensityFilterType::Pointer     m_SegmentedVolumeRescaleIntensity;

  ITK2VTKAdaptorFilterType::Pointer       m_SegmentedVolumeITK2VTKAdaptor;
  
  DicomReaderType                         m_DicomVolumeReader;

  VolumeType::ConstPointer                m_LoadedVolume;

  VolumeType::ConstPointer                m_SegmentedVolume;

  ThresholdFilterType::Pointer            m_ThresholdVolumeFilter;

  ISIS::ThresholdLevelSetSegmentationModule    m_ThresholdLevelSetModule;

  ISIS::ConfidenceConnectedModule          m_ConfidenceConnectedModule;

  ISIS::ConnectedThresholdModule          m_ConnectedThresholdModule;

  char                                    m_MessageString[256];

  WriterType::Pointer                      m_Writer;
};


#endif
