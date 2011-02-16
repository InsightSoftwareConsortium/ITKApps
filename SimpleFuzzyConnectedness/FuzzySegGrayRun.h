#include <FL/Fl_File_Chooser.H>

#include "GLSliceView.h"
#include "itkSimpleFuzzyConnectednessScalarImageFilter.h"

class FuzzySegGrayRun
{

public:
  typedef itk::Image<float,3> FloatImage;
  typedef FloatImage::IndexType IndexType;
  typedef itk::Image<unsigned char ,3> OutputImage;
  typedef itk::Image<unsigned short,3> FuzzySceneImage;
  typedef GLSliceView<float,bool>   ViewerType;
  typedef itk::SimpleFuzzyConnectednessScalarImageFilter<FloatImage,OutputImage> FuzzyFilter;

  FuzzySegGrayRun();
  virtual ~FuzzySegGrayRun();

protected:
  virtual bool LoadImage (void);
  virtual int Load (const char * inputFileName);
  void ReadImage(const char * inputFileName);  
  void ClearClicked(void); 
  void ComputeStats(void); 
  void PickSeed(){};
  void GenerateData();
  void ShowResult();
  void Save(void);
  void Save(const char *outputFileName);


  int m_Cols;
  int m_Rows;
  int m_Pages;
  float m_Mean;
  float m_Variance;
  unsigned int m_Seed[3];
  float m_Threshold;

  char m_Datatype;
  unsigned int m_Offset;

  bool m_AskScene;
  unsigned long m_ImageSize;

  FloatImage::Pointer m_ViewResultImage;
  FloatImage::Pointer m_DataImage;
  OutputImage::Pointer m_OutputObject;


  ViewerType *m_Viewer;
  ViewerType *m_ViewerResult;
  unsigned int m_DisplaySlice;

  unsigned int m_SampleRegion;
  FuzzyFilter::Pointer m_Segmenter;

  FloatImage::RegionType m_ImageRegion;
};

