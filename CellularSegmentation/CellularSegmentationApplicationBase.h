#ifndef __CellularSegmentationApplicationBase__h__
#define __CellularSegmentationApplicationBase__h__

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4503 )
#endif

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCommand.h"


class CellularSegmentationApplicationBase 
{

public:
  
  typedef float          PixelType;

  typedef unsigned char  VisualizationPixelType;
  
  typedef itk::Image< PixelType, 2 >                 ImageType;
  
  typedef itk::Image< VisualizationPixelType, 2 >     VisualizationImageType;
  
  typedef itk::ImageFileReader< ImageType >          ImageReaderType;

  typedef itk::RescaleIntensityImageFilter< 
                          ImageType,
                          VisualizationImageType >   RescaleIntensityFilterType;

  typedef itk::ImageToVTKImageFilter< 
                          VisualizationImageType >   ITK2VTKAdaptorFilterType;


  typedef itk::SimpleMemberCommand< CellularSegmentationApplicationBase >  IterationObserverType;

public:
  
  CellularSegmentationApplicationBase();
  
  virtual ~CellularSegmentationApplicationBase();
  
  virtual void SetSeedPoint( double x, double y, double z );
  
  virtual void  GetSeedPoint(double data[3]); 
  
  virtual void  IterationCallback(); 
  
protected:
  
  double                                 m_SeedPoint[3];
  
  ImageReaderType::Pointer               m_ImageReader;

  RescaleIntensityFilterType::Pointer    m_RescaleIntensity;

  ITK2VTKAdaptorFilterType::Pointer      m_ITK2VTKAdaptor;
  
  IterationObserverType::Pointer         m_IterationObserver;

};


#endif
