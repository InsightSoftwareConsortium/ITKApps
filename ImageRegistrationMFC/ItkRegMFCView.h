/*=========================================================================
  ItkRegMFCView.h : interface of the CItkRegMFCView class

  Include the essential header files for ITK image registration.
  Core functionality for image registration comes from ImageRegistration1 example.
  Please refer to InsightToolkit-3.0.0\Examples\Registration\ImageRegistration1.cxx
  
  Author  Yong Su
  E-mail  jean.timex@gmail.com
  Date    January 29, 2007
=========================================================================*/
#pragma once

// Header files for ITK image registration
#include "itkImageRegistrationMethod.h"
#include "itkTranslationTransform.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkRegularStepGradientDescentOptimizer.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSubtractImageFilter.h"

#include "itkImage.h"


class CItkRegMFCView : public CView
{
protected: // create from serialization only
  CItkRegMFCView();
  DECLARE_DYNCREATE(CItkRegMFCView)

// Attributes
public:
  CItkRegMFCDoc* GetDocument() const;

  // Please refer to InsightToolkit-3.0.0\Examples\Registration\ImageRegistration1.cxx
  // Software Guide : BeginCodeSnippet
  typedef itk::RegularStepGradientDescentOptimizer     OptimizerType;
  typedef const OptimizerType                         *OptimizerPointer;

  // The types of each one of the components in the registration methods should
  // be instantiated first. With that purpose, we start by selecting the image
  // dimension and the type used for representing image pixels.
  const static unsigned int    Dimension = 2;
  typedef  float           PixelType;

  //  The types of the input images are instantiated by the following lines.
  typedef itk::Image< PixelType, Dimension >  FixedImageType;
  typedef itk::Image< PixelType, Dimension >  MovingImageType;

  //  The transform that will map the fixed image space into the moving image
  //  space is defined below.
  typedef itk::TranslationTransform< double, Dimension > TransformType;

  //  An optimizer is required to explore the parameter space of the transform
  //  in search of optimal values of the metric.
  typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;

  //  The metric will compare how well the two images match each other. Metric
  //  types are usually parameterized by the image types as it can be seen in
  //  the following type declaration.
  typedef itk::MeanSquaresImageToImageMetric< 
                                    FixedImageType, 
                                    MovingImageType >    MetricType;

  //  Finally, the type of the interpolator is declared. The interpolator will
  //  evaluate the intensities of the moving image at non-grid positions.
  typedef itk:: LinearInterpolateImageFunction< 
                                    MovingImageType,
                                    double          >    InterpolatorType;

  //  The registration method type is instantiated using the types of the
  //  fixed and moving images. This class is responsible for interconnecting
  //  all the components that we have described so far.
  typedef itk::ImageRegistrationMethod< 
                                    FixedImageType, 
                                    MovingImageType >    RegistrationType;
  // Software Guide : EndCodeSnippet

  CImage imgBase;      // an instance of CImage for fixed image
  CImage imgInput;    // an instance of CImage for moving image
  CImage imgAlign;    // an instance of CImage for output image

  CString name_imgBase;  // file name of fixed image
  CString name_imgInput;  // file name of moving image
  CString name_imgAlign;  // file name of output image

  CString result_info;  // the last result information

  int margin;        // distance from the left edge to the box
  int margin_d;      // distance from the bottom to the box
  int interval;      // the distance between two boxes

  bool freeViewMode;    // show the images as their actual size
  bool dragMode;      // allow images to be dragged by the mouse

  int m_nFilterLoad;    // dlg.m_ofn.nFilterIndex
  int showMode;      // 0: Base & Input; 1: Base & Align; 2: Align & Input

  int showBaseWidth;    // actual width of fixed image
  int showBaseHeight;    // actual height of fixed image
  int showInputWidth;    // actual width of moving image
  int showInputHeight;  // actual height of moving image
  int showAlignWidth;    // actual width of output image
  int showAlignHeight;  // actual height of output image

  int offset_Base_x;    // when dragging the images, the x offset from the edge of the box
  int offset_Base_y;    // the y offset from the edge of the box to fixed image
  int offset_Input_x;    // the x offset from the edge of the box to moving image
  int offset_Input_y;    // the y offset from the edge of the box to moving image 
  int offset_Align_x;    // the x offset from the edge of the box to output image
  int offset_Align_y;    // the y offset from the edge of the box to output image

  int m_multiple_Base;  // enlarge times of fixed image
  int m_multiple_Input;  // enlarge times of moving image
  int m_multiple_Align;  // enlarge times of output image

  bool onPress_Base;    // if the fixed image is pressed, then set to true
  bool onPress_Input;    // if the moving image is pressed, then set to true
  bool onPress_Align;    // if the output image is pressed, then set to true

  RECT rectSrc_Base;    // the scope of the fixed image to be showed in the box
  RECT rectSrc_Input;    // the scope of the moving image to be showed in the box
  RECT rectSrc_Align;    // the scope of the output image to be showed in the box

  CPoint old_Base_point;  // record the last mouse position on fixed image
  CPoint old_Input_point;  // record the last mouse position on moving image
  CPoint old_Align_point;  // record the last mouse position on output image

  CPoint mouse;      // mouse coordinates

  CWinThread *pThread;  // work thread pointer
  volatile bool m_bRunning;//thread status. true:running, false:stop

// Operations
public:
  static UINT Run(LPVOID p);  // thread function
  void Run();          // all image registration work
  LRESULT NotifyResult(WPARAM wParam, LPARAM lParam);

  static void UpdateView(CString str);// update iteration information

  // Please refer to InsightToolkit-3.0.0\Examples\Registration\ImageRegistration1.cxx
  class CommandIterationUpdate : public itk::Command 
  {
  public:
    typedef  CommandIterationUpdate    Self;
    typedef  itk::Command        Superclass;
    typedef itk::SmartPointer<Self>    Pointer;
    itkNewMacro( Self );

  protected:
    CommandIterationUpdate() {};

  public:

    typedef itk::RegularStepGradientDescentOptimizer     OptimizerType;
    typedef const OptimizerType                         *OptimizerPointer;

    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

    void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
              dynamic_cast< OptimizerPointer >( object );

      if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
        return;
      }
      //process the information for each iteration
      // Please refer to InsightToolkit-3.0.0\Examples\Registration\ImageRegistration1.cxx
      //std::cout << optimizer->GetCurrentIteration() << " = ";
      //std::cout << optimizer->GetValue() << " : ";
      //std::cout << optimizer->GetCurrentPosition() << std::endl;

      unsigned int CurrentIteration = optimizer->GetCurrentIteration();
      float CurrentValue = optimizer->GetValue();
      itk::Vector<double,2> p;
      p[0]=(double)optimizer->GetCurrentPosition()[0];  // x
      p[1]=(double)optimizer->GetCurrentPosition()[1];  // y
      CString CurrentResult;
      CurrentResult.Format("%ld = %f : [ %f, %f ]", CurrentIteration, CurrentValue, p[0], p[1]);
      CItkRegMFCView::UpdateView(CurrentResult);  
    }
  };

// Overrides
  public:
  virtual void OnDraw(CDC* pDC);  // overridden to draw this view
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
  virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
  virtual ~CItkRegMFCView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
  DECLARE_MESSAGE_MAP()
public:
  // Start to execute registration
  afx_msg void OnRegistrationStart();
  // handler for WM_PAINT event
  afx_msg void OnPaint();
  // handler for opening images
  afx_msg void OnFileOpenbase();
  afx_msg void OnFileOpeninputimage();
  // handler for changing the showing mode
  afx_msg void OnComparemodeBaseinput();
  afx_msg void OnComparemodeBasealign();
  afx_msg void OnComparemodeAligninput();
  // handler for changing the button status
  afx_msg void OnUpdateComparemodeBaseinput(CCmdUI *pCmdUI);
  afx_msg void OnUpdateComparemodeBasealign(CCmdUI *pCmdUI);
  afx_msg void OnUpdateComparemodeAligninput(CCmdUI *pCmdUI);
  // handler for showing actual size of images or allowing images to be dragged
  afx_msg void OnViewFreeview();
  afx_msg void OnViewDrag();
  afx_msg void OnUpdateViewFreeview(CCmdUI *pCmdUI);
  afx_msg void OnUpdateViewDrag(CCmdUI *pCmdUI);
  // handler for clear all images
  afx_msg void OnFileClearbase();
  afx_msg void OnFileClearinputimage();
  afx_msg void OnFileClearaligedimage();
  // handler for mouse events
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#ifndef _DEBUG  // debug version in ItkRegMFCView.cpp
inline CItkRegMFCDoc* CItkRegMFCView::GetDocument() const
   { return reinterpret_cast<CItkRegMFCDoc*>(m_pDocument); }
#endif

