
#ifndef __CellularSegmentationApplication__h__
#define __CellularSegmentationApplication__h__


#include "CellularSegmentationApplicationGUI.h"
#include "ImageSliceViewer.h"
#include "itkBioCellularAggregate.h"
#include "CellularAggregateViewer.h"
#include <time.h>



class CellularSegmentationApplication : public CellularSegmentationApplicationGUI
{
 public:
  
  CellularSegmentationApplication();
  virtual ~CellularSegmentationApplication();
  
  virtual void Show();
  virtual void Hide();
  virtual void Quit();
  virtual void Load();
  virtual void Load(const char * filename);
  virtual void CreateEgg();
  virtual void Run();
  virtual void Stop();
  virtual void Clear(); // destroy current cellular aggregate
  virtual void SetDisplayOption();

 private:

  virtual void IterationCallback();
  virtual void ProcessImageViewInteraction();
   
  ImageSliceViewer  m_ImageViewer;

  itk::bio::CellularAggregate::Pointer  m_CellularAggregate;

  itk::bio::CellularAggregateViewer::Pointer  m_CellularAggregateViewer;

  itk::SimpleMemberCommand<CellularSegmentationApplication>::Pointer      m_ImageViewerCommand;
  
  bool     m_Stop;
  clock_t  m_StartTime;

};


#endif
