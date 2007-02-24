/*=========================================================================
  ItkRegMFCView.cpp : implementation of the CItkRegMFCView class
  
  Note:
     OnPaint() is used for updating images showing. Do not call this function
     or invoke this function by calling Invalidate() from CommandIterationUpdate.
   Because registration runs fast while refreshing the images runs slow.
   UpdateView() is created for you to handle iteration information.

  Author  Yong Su
  E-mail  jean.timex@gmail.com
  Date    January 29, 2007
=========================================================================*/
#include "stdafx.h"
#include "ItkRegMFC.h"

#include "ItkRegMFCDoc.h"
#include "ItkRegMFCView.h"
#include ".\itkregmfcview.h"

#include "time.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_RESULT  WM_USER+1

// CItkRegMFCView

IMPLEMENT_DYNCREATE(CItkRegMFCView, CView)

BEGIN_MESSAGE_MAP(CItkRegMFCView, CView)
  // Standard printing commands
  ON_WM_PAINT()

  ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

  ON_COMMAND(ID_REGISTRATION_START, OnRegistrationStart)
  
  ON_COMMAND(ID_FILE_OPENBASE, OnFileOpenbase)
  ON_COMMAND(ID_FILE_OPENINPUTIMAGE, OnFileOpeninputimage)
  
  ON_COMMAND(ID_COMPAREMODE_BASEINPUT, OnComparemodeBaseinput)
  ON_COMMAND(ID_COMPAREMODE_BASEALIGN, OnComparemodeBasealign)
  ON_COMMAND(ID_COMPAREMODE_ALIGNINPUT, OnComparemodeAligninput)

  ON_UPDATE_COMMAND_UI(ID_COMPAREMODE_BASEINPUT, OnUpdateComparemodeBaseinput)
  ON_UPDATE_COMMAND_UI(ID_COMPAREMODE_BASEALIGN, OnUpdateComparemodeBasealign)
  ON_UPDATE_COMMAND_UI(ID_COMPAREMODE_ALIGNINPUT, OnUpdateComparemodeAligninput)

  ON_COMMAND(ID_VIEW_FREEVIEW, OnViewFreeview)
  ON_UPDATE_COMMAND_UI(ID_VIEW_FREEVIEW, OnUpdateViewFreeview)

  ON_COMMAND(ID_VIEW_DRAG, OnViewDrag)
  ON_UPDATE_COMMAND_UI(ID_VIEW_DRAG, OnUpdateViewDrag)

  ON_COMMAND(ID_FILE_CLEARBASE, OnFileClearbase)
  ON_COMMAND(ID_FILE_CLEARINPUTIMAGE, OnFileClearinputimage)
  ON_COMMAND(ID_FILE_CLEARALIGEDIMAGE, OnFileClearaligedimage)

  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEWHEEL()

  ON_MESSAGE(WM_RESULT, NotifyResult)
END_MESSAGE_MAP()

// CItkRegMFCView construction/destruction
// Initialize the properties
CItkRegMFCView::CItkRegMFCView()
:margin(10),interval(5),margin_d(60)
,freeViewMode(false),dragMode(true),showMode(0)
,onPress_Align(false), onPress_Base(false),onPress_Input(false)
,m_multiple_Align(1), m_multiple_Base(1), m_multiple_Input(1)
,m_bRunning(false)
{
  name_imgAlign = "output.png";

  rectSrc_Base.left = 0;
  rectSrc_Base.right = 0;
  rectSrc_Base.top = 0;
  rectSrc_Base.bottom = 0;

  rectSrc_Input.left = 0;
  rectSrc_Input.right = 0;
  rectSrc_Input.top = 0;
  rectSrc_Input.bottom = 0;

  rectSrc_Align.left = 0;
  rectSrc_Align.right = 0;
  rectSrc_Align.top = 0;
  rectSrc_Align.bottom = 0;

  offset_Base_x = 0;
  offset_Base_y = 0;
  offset_Input_x = 0;
  offset_Input_y = 0;
  offset_Align_x = 0;
  offset_Align_y = 0;

  mouse.x = 0;
  mouse.y = 0;
}

CItkRegMFCView::~CItkRegMFCView()
{
}

BOOL CItkRegMFCView::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs

  return CView::PreCreateWindow(cs);
}

// CItkRegMFCView drawing

void CItkRegMFCView::OnDraw(CDC* /*pDC*/)
{
  CItkRegMFCDoc* pDoc = GetDocument();
  ASSERT_VALID(pDoc);
  if (!pDoc)
    return;

  // TODO: add draw code for native data here
}


// CItkRegMFCView printing

BOOL CItkRegMFCView::OnPreparePrinting(CPrintInfo* pInfo)
{
  // default preparation
  return DoPreparePrinting(pInfo);
}

void CItkRegMFCView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add extra initialization before printing
}

void CItkRegMFCView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add cleanup after printing
}


// CItkRegMFCView diagnostics

#ifdef _DEBUG
void CItkRegMFCView::AssertValid() const
{
  CView::AssertValid();
}

void CItkRegMFCView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}

CItkRegMFCDoc* CItkRegMFCView::GetDocument() const // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CItkRegMFCDoc)));
  return (CItkRegMFCDoc*)m_pDocument;
}
#endif //_DEBUG

// CItkRegMFCView message handlers
// Excute the registration procedure
// Please refer to InsightToolkit-3.0.0\Examples\Registration\ImageRegistration1.cxx
// I keep most of the codes all the same.
void CItkRegMFCView::OnRegistrationStart()
{
  if(!m_bRunning){
    m_bRunning = TRUE;
    pThread = AfxBeginThread(Run,this);
  }
}
//////////////////////////////////////////////////////////////
//
// Handler for WM_PAINT event, refresh the window (UI)
// invoked by Invalidate() & UpdateWindow()
//
//////////////////////////////////////////////////////////////
void CItkRegMFCView::OnPaint()
{
  CPaintDC dc(this); // device context for painting
  // Do not call CView::OnPaint() for painting messages

  // draw the 2 boxes
  CPen *pOldPen;
  CPen *pNewPen = new CPen();
  CRect rect;
  pNewPen->CreatePen(PS_SOLID,1,RGB(125,125,125));  //Create new Pen
  pOldPen = dc.SelectObject(pNewPen);
  GetClientRect(&rect);
  float areaWidth  = (float)rect.CenterPoint().x-margin*3/2-interval*2;
  float areaHeight = (float)rect.bottom-margin-margin_d-interval*2;
  // Base Image Showing Area
  dc.MoveTo(margin,margin);
  dc.LineTo(margin,rect.bottom-margin_d);
  dc.LineTo(rect.CenterPoint().x-margin/2,rect.bottom-margin_d);
  dc.LineTo(rect.CenterPoint().x-margin/2,margin);
  dc.LineTo(margin,margin);
  // Input Image Showing Area
  dc.MoveTo(rect.CenterPoint().x+margin/2,margin);
  dc.LineTo(rect.CenterPoint().x+margin/2,rect.bottom-margin_d);
  dc.LineTo(rect.right-margin,rect.bottom-margin_d);
  dc.LineTo(rect.right-margin,margin);
  dc.LineTo(rect.CenterPoint().x+margin/2,margin);

  dc.SelectObject(pOldPen);
  pNewPen->DeleteObject();
  delete pNewPen;
  
  if(!freeViewMode)
  {
    int D1 = margin+interval;
    int D2 = rect.CenterPoint().x+margin/2+interval;
    // show aligned image
    if (!imgAlign.IsNull() && (1==showMode || 2==showMode))
    {
      dc.SetStretchBltMode(HALFTONE);
      if(1==showMode)  //show aligned image in the right box
      {
        if(imgAlign.GetWidth()<=areaWidth && imgAlign.GetHeight()<=areaHeight)
        {
          imgAlign.StretchBlt(dc,rect.CenterPoint().x+margin/2+interval,margin+interval,imgAlign.GetWidth(),imgAlign.GetHeight(),SRCCOPY);
          showAlignWidth = imgAlign.GetWidth();
          showAlignHeight = imgAlign.GetHeight();
        }
        else if((imgAlign.GetWidth()/areaWidth)>(imgAlign.GetHeight()/areaHeight))
        {
          // adjust showing ratio
          imgAlign.StretchBlt(dc,rect.CenterPoint().x+margin/2+interval,margin+interval,(int)areaWidth,(int)(imgAlign.GetHeight()*areaWidth/imgAlign.GetWidth()),SRCCOPY);
          showAlignWidth = (int)areaWidth;
          showAlignHeight = (int)(imgAlign.GetHeight()*areaWidth/imgAlign.GetWidth());
        }
        else
        {
          imgAlign.StretchBlt(dc,rect.CenterPoint().x+margin/2+interval,margin+interval,(int)(imgAlign.GetWidth()*areaHeight/imgAlign.GetHeight()),(int)areaHeight,SRCCOPY);
          showAlignWidth = (int)(imgAlign.GetWidth()*areaHeight/imgAlign.GetHeight());
          showAlignHeight = (int)areaHeight;
        }
      }
      else //showMode == 2, show aligned image in the left box
      {
        if(imgAlign.GetWidth()<=areaWidth && imgAlign.GetHeight()<=areaHeight)
        {
          imgAlign.StretchBlt(dc,margin+interval,margin+interval,imgAlign.GetWidth(),imgAlign.GetHeight(),SRCCOPY);
          showAlignWidth = imgAlign.GetWidth();
          showAlignHeight = imgAlign.GetHeight();
        }
        else if((imgAlign.GetWidth()/areaWidth)>(imgAlign.GetHeight()/areaHeight))
        {
          imgAlign.StretchBlt(dc,margin+interval,margin+interval,(int)areaWidth,(int)(imgAlign.GetHeight()*areaWidth/imgAlign.GetWidth()),SRCCOPY);
          showAlignWidth = (int)areaWidth;
          showAlignHeight = (int)(imgAlign.GetHeight()*areaWidth/imgAlign.GetWidth());
        }
        else
        {
          imgAlign.StretchBlt(dc,margin+interval,margin+interval,(int)(imgAlign.GetWidth()*areaHeight/imgAlign.GetHeight()),(int)areaHeight,SRCCOPY);
          showAlignWidth = (int)(imgAlign.GetWidth()*areaHeight/imgAlign.GetHeight());
          showAlignHeight = (int)areaHeight;
        }
      }
      
    }
    if (!imgBase.IsNull() && (0==showMode || 1==showMode)) 
    {    
      dc.SetStretchBltMode(HALFTONE);
      if(imgBase.GetWidth()<=areaWidth && imgBase.GetHeight()<=areaHeight)
      {
        imgBase.StretchBlt(dc,margin+interval,margin+interval,imgBase.GetWidth(),imgBase.GetHeight(),SRCCOPY);
        showBaseWidth = imgBase.GetWidth();
        showBaseHeight = imgBase.GetHeight();
      }
      else if((imgBase.GetWidth()/areaWidth)>(imgBase.GetHeight()/areaHeight))
      {
        imgBase.StretchBlt(dc,margin+interval,margin+interval,(int)areaWidth,(int)(imgBase.GetHeight()*areaWidth/imgBase.GetWidth()),SRCCOPY);
        showBaseWidth = (int)areaWidth;
        showBaseHeight = (int)(imgBase.GetHeight()*areaWidth/imgBase.GetWidth());
      }
      else
      {
        imgBase.StretchBlt(dc,margin+interval,margin+interval,(int)(imgBase.GetWidth()*areaHeight/imgBase.GetHeight()),(int)areaHeight,SRCCOPY);
        showBaseWidth = (int)(imgBase.GetWidth()*areaHeight/imgBase.GetHeight());
        showBaseHeight = (int)areaHeight;
      }
    }
    if (!imgInput.IsNull() && (0==showMode || 2==showMode)) 
    {    
      dc.SetStretchBltMode(HALFTONE);
      if(imgInput.GetWidth()<=areaWidth && imgInput.GetHeight()<=areaHeight)
      {
        imgInput.StretchBlt(dc,rect.CenterPoint().x+margin/2+interval,margin+interval,imgInput.GetWidth(),imgInput.GetHeight(),SRCCOPY);
        showInputWidth = imgInput.GetWidth();
        showInputHeight = imgInput.GetHeight();
      }
      else if((imgInput.GetWidth()/areaWidth)>(imgInput.GetHeight()/areaHeight))
      {
        imgInput.StretchBlt(dc,rect.CenterPoint().x+margin/2+interval,margin+interval,(int)areaWidth,(int)(imgInput.GetHeight()*areaWidth/imgInput.GetWidth()),SRCCOPY);
        showInputWidth = (int)areaWidth;
        showInputHeight = (int)(imgInput.GetHeight()*areaWidth/imgInput.GetWidth());
      }
      else
      {
        imgInput.StretchBlt(dc,rect.CenterPoint().x+margin/2+interval,margin+interval,(int)(imgInput.GetWidth()*areaHeight/imgInput.GetHeight()),(int)areaHeight,SRCCOPY);
        showInputWidth = (int)(imgInput.GetWidth()*areaHeight/imgInput.GetHeight());
        showInputHeight = (int)areaHeight;
      }
    }
  }
  else  //freeViewMode == true
  {
    RECT rectDest_Base,rectDest_Input;
    rectDest_Base.left = margin+interval;
    rectDest_Base.right = margin+interval+(int)areaWidth;
    rectDest_Base.top = margin+interval;
    rectDest_Base.bottom = margin+interval+(int)areaHeight;

    rectDest_Input.left = rect.CenterPoint().x+margin/2+interval;
    rectDest_Input.right = rect.CenterPoint().x+margin/2+interval+(int)areaWidth;
    rectDest_Input.top = margin+interval;
    rectDest_Input.bottom = margin+interval+(int)areaHeight;

    int D1 = margin + interval;
    int D2_x = offset_Base_x/m_multiple_Base;
    int D2_y = offset_Base_y/m_multiple_Base;
    int D3 = rect.CenterPoint().x+margin/2+interval;
    int D4_x = offset_Input_x/m_multiple_Input;
    int D4_y = offset_Input_y/m_multiple_Input;

    int D8_x = offset_Align_x/m_multiple_Align;
    int D8_y = offset_Align_y/m_multiple_Align;

    if (!imgAlign.IsNull() && (1==showMode || 2==showMode))
    {
      dc.SetStretchBltMode(HALFTONE);
      if(1==showMode)
      {
        rectSrc_Align.right = ((int)areaWidth+offset_Align_x)/m_multiple_Align;
        rectSrc_Align.bottom = ((int)areaHeight+offset_Align_y)/m_multiple_Align;
        rectSrc_Align.left = offset_Align_x/m_multiple_Align;
        rectSrc_Align.top = offset_Align_y/m_multiple_Align;
        imgAlign.StretchBlt(dc,(const RECT &)rectDest_Input,(const RECT &)rectSrc_Align,SRCCOPY);
      }
      else
      {
        rectSrc_Align.right = ((int)areaWidth+offset_Align_x)/m_multiple_Align;
        rectSrc_Align.bottom = ((int)areaHeight+offset_Align_y)/m_multiple_Align;
        rectSrc_Align.left = offset_Align_x/m_multiple_Align;
        rectSrc_Align.top = offset_Align_y/m_multiple_Align;
        imgAlign.StretchBlt(dc,(const RECT &)rectDest_Base,(const RECT &)rectSrc_Align,SRCCOPY);
      }
    }
    if (!imgBase.IsNull() && (0==showMode || 1==showMode)) 
    {    
      dc.SetStretchBltMode(HALFTONE);
      rectSrc_Base.right = ((int)areaWidth+offset_Base_x)/m_multiple_Base;
      rectSrc_Base.bottom = ((int)areaHeight+offset_Base_y)/m_multiple_Base;
      rectSrc_Base.left = offset_Base_x/m_multiple_Base;
      rectSrc_Base.top = offset_Base_y/m_multiple_Base;
      imgBase.StretchBlt(dc,(const RECT &)rectDest_Base,(const RECT &)rectSrc_Base,SRCCOPY);
    }
    if (!imgInput.IsNull() && (0==showMode || 2==showMode)) 
    {    
      dc.SetStretchBltMode(HALFTONE);
      rectSrc_Input.right = ((int)areaWidth+offset_Input_x)/m_multiple_Input;
      rectSrc_Input.bottom = ((int)areaHeight+offset_Input_y)/m_multiple_Input;
      rectSrc_Input.left = offset_Input_x/m_multiple_Input;
      rectSrc_Input.top = offset_Input_y/m_multiple_Input;
      imgInput.StretchBlt(dc,(const RECT &)rectDest_Input,(const RECT &)rectSrc_Input,SRCCOPY);
    }
  }
}

//////////////////////////////////////////////////////////////
//
// Open fixed image
//
//////////////////////////////////////////////////////////////
void CItkRegMFCView::OnFileOpenbase()
{
  CString strFilter;
  CSimpleArray<GUID> aguidFileTypes;
  HRESULT hResult;

  hResult = imgBase.GetExporterFilterString(strFilter,aguidFileTypes,_T("All Image Files"));
  if (FAILED(hResult)) {
    MessageBox(_T("GetExporterFilter Calling Failed!"),0,0);
    return;
  }

  CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, strFilter);
  dlg.m_ofn.nFilterIndex = m_nFilterLoad;
  hResult = (int)dlg.DoModal();
  if(FAILED(hResult)) {
    return;
  }

  m_nFilterLoad = dlg.m_ofn.nFilterIndex;
  name_imgBase = dlg.GetFileName();
  imgBase.Destroy();
  hResult = imgBase.Load(name_imgBase);
  if (FAILED(hResult)) {
    MessageBox(_T("Base Image Loading Failed!"),0,0);
    Invalidate();
    UpdateWindow();
    return;
  }
  // invoke onPaint()
  Invalidate();
  UpdateWindow();
}

//////////////////////////////////////////////////////////////
//
// Open moving image
//
//////////////////////////////////////////////////////////////
void CItkRegMFCView::OnFileOpeninputimage()
{
  CString strFilter;
  CSimpleArray<GUID> aguidFileTypes;
  HRESULT hResult;

  hResult = imgInput.GetExporterFilterString(strFilter,aguidFileTypes,_T("All Image Files"));
  if (FAILED(hResult)) {
    MessageBox(_T("GetExporterFilter Calling Failed!"),0,0);
    return;
  }

  CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, strFilter);
  dlg.m_ofn.nFilterIndex = m_nFilterLoad;
  hResult = (int)dlg.DoModal();
  if(FAILED(hResult)) {
    return;
  }

  m_nFilterLoad = dlg.m_ofn.nFilterIndex;
  name_imgInput = dlg.GetFileName();
  imgInput.Destroy();
  hResult = imgInput.Load(name_imgInput);
  if (FAILED(hResult)) {
    MessageBox(_T("Input Image Loading Failed!"),0,0);
    Invalidate();
    UpdateWindow();
    return;
  }

  Invalidate();
  UpdateWindow();
}
//////////////////////////////////////////////////////////////
//
// show fixed image & moving image on screen
//
//////////////////////////////////////////////////////////////
void CItkRegMFCView::OnComparemodeBaseinput()
{
  showMode = 0;
  Invalidate();
  UpdateWindow();
}

//////////////////////////////////////////////////////////////
//
// show fixed image & aligned image on screen
//
//////////////////////////////////////////////////////////////
void CItkRegMFCView::OnComparemodeBasealign()
{
  showMode = 1;
  Invalidate();
  UpdateWindow();
}
//////////////////////////////////////////////////////////////
//
// show aligned image & moving image on screen
//
//////////////////////////////////////////////////////////////
void CItkRegMFCView::OnComparemodeAligninput()
{
  showMode = 2;
  Invalidate();
  UpdateWindow();
}
//////////////////////////////////////////////////////////////
//
// change UI status
//
//////////////////////////////////////////////////////////////
void CItkRegMFCView::OnUpdateComparemodeBaseinput(CCmdUI *pCmdUI)
{
  0==showMode ? pCmdUI->SetCheck(1) : pCmdUI->SetCheck(0);
}

void CItkRegMFCView::OnUpdateComparemodeBasealign(CCmdUI *pCmdUI)
{
  1==showMode ? pCmdUI->SetCheck(1) : pCmdUI->SetCheck(0);
}

void CItkRegMFCView::OnUpdateComparemodeAligninput(CCmdUI *pCmdUI)
{
  2==showMode ? pCmdUI->SetCheck(1) : pCmdUI->SetCheck(0);
}
//////////////////////////////////////////////////////////////
//
// switch on/off free view mode.
// in free view mode, picture will always show as its actual resolution
//////////////////////////////////////////////////////////////
void CItkRegMFCView::OnViewFreeview()
{
  freeViewMode=!freeViewMode;
  Invalidate();
  UpdateWindow();
}

void CItkRegMFCView::OnUpdateViewFreeview(CCmdUI *pCmdUI)
{
  false==freeViewMode ? pCmdUI->SetCheck(0) : pCmdUI->SetCheck(1);
}

//////////////////////////////////////////////////////////////
//
// switch on/off drag mode.
// if dragMode is true, then images are draggable
//////////////////////////////////////////////////////////////
void CItkRegMFCView::OnViewDrag()
{
  dragMode=!dragMode;
  Invalidate();
  UpdateWindow();
}

void CItkRegMFCView::OnUpdateViewDrag(CCmdUI *pCmdUI)
{
  false==dragMode ? pCmdUI->SetCheck(0) : pCmdUI->SetCheck(1);
}

//////////////////////////////////////////////////////////////
//
// clear images and getback to its initial status.
// 
//////////////////////////////////////////////////////////////
void CItkRegMFCView::OnFileClearbase()
{
  if(!imgBase.IsNull())
  {
    imgBase.Destroy();
    m_multiple_Base = 1;
    offset_Base_x = 0;
    offset_Base_y = 0;
    old_Base_point.x = 0;
    old_Base_point.y = 0;
    onPress_Base = false;
    Invalidate();
    UpdateWindow();
  }
}

void CItkRegMFCView::OnFileClearinputimage()
{
  if(!imgInput.IsNull())
  {
    imgInput.Destroy();
    offset_Input_x = 0;
    offset_Input_y = 0;
    m_multiple_Input = 1;
    old_Input_point.x = 0;
    old_Input_point.y = 0;
    onPress_Input = false;
    Invalidate();
    UpdateWindow();
  }
}

void CItkRegMFCView::OnFileClearaligedimage()
{
  if(!imgAlign.IsNull())
  {
    imgAlign.Destroy();
    m_multiple_Align = 1;
    offset_Align_x = 0;
    offset_Align_y = 0;
    old_Align_point.x = 0;
    old_Align_point.y = 0;
    onPress_Align = false;
    Invalidate();
    UpdateWindow();
  }
}

//////////////////////////////////////////////////////////////
//
// mouse moving event handler
//
//////////////////////////////////////////////////////////////
void CItkRegMFCView::OnMouseMove(UINT nFlags, CPoint point)
{
  CRect rect;
  GetClientRect(&rect);
  float areaWidth  = (float)rect.CenterPoint().x-margin*3/2-interval*2;
  float areaHeight = (float)rect.bottom-2*margin-interval*2;
  if(!imgAlign.IsNull() && (showMode == 1 || showMode == 2))
  {
    if(freeViewMode && dragMode && onPress_Align)
    {
      if(2==showMode && point.x>=margin+interval && point.x<=margin+interval+areaWidth && point.y>=margin+interval && point.y<=margin+interval+areaHeight)
      {
        offset_Align_x -= point.x - old_Align_point.x;
        offset_Align_y -= point.y - old_Align_point.y;
        //record the positon of mouse
        old_Align_point.x = point.x;  
        old_Align_point.y = point.y;
      }
      if(showMode == 1 && point.x>=rect.CenterPoint().x+margin/2+interval && point.x<=rect.CenterPoint().x+margin/2+interval+areaWidth && point.y>=margin+interval && point.y<=margin+interval+areaHeight)
      {
        offset_Align_x -= point.x - old_Align_point.x;
        offset_Align_y -= point.y - old_Align_point.y;
        old_Align_point.x = point.x;
        old_Align_point.y = point.y;
      }
    }
    mouse.x = point.x;  
    mouse.y = point.y;
  }
  if(!imgBase.IsNull() && (0==showMode || 1==showMode))
  {
    if(freeViewMode && dragMode && onPress_Base)
    {
      if(point.x>=margin+interval && point.x<=margin+interval+areaWidth && point.y>=margin+interval && point.y<=margin+interval+areaHeight)
      {
        offset_Base_x -= point.x - old_Base_point.x;
        offset_Base_y -= point.y - old_Base_point.y;
        //record the positon of mouse
        old_Base_point.x = point.x;  
        old_Base_point.y = point.y;
      }
    }
    mouse.x = point.x;  
    mouse.y = point.y;
  }
  if(!imgInput.IsNull() && (0==showMode || 2==showMode))
  {
    if(freeViewMode && dragMode && onPress_Input)
    {
      if(point.x>=rect.CenterPoint().x+margin/2+interval && point.x<=rect.CenterPoint().x+margin/2+interval+areaWidth && point.y>=margin+interval && point.y<=margin+interval+areaHeight)
      {
        offset_Input_x -= point.x - old_Input_point.x;
        offset_Input_y -= point.y - old_Input_point.y;
        old_Input_point.x = point.x;
        old_Input_point.y = point.y;
      }
    }
    mouse.x = point.x;  
    mouse.y = point.y;
  }
  Invalidate(0);
  UpdateWindow();
  CView::OnMouseMove(nFlags, point);
}

//////////////////////////////////////////////////////////////
//
// mouse left button pressed event handler
//
//////////////////////////////////////////////////////////////
void CItkRegMFCView::OnLButtonDown(UINT nFlags, CPoint point)
{
  CRect rect;
  GetClientRect(&rect);
  float areaWidth  = (float)rect.CenterPoint().x-margin*3/2-interval*2;
  float areaHeight = (float)rect.bottom-2*margin-interval*2;
  if(!imgAlign.IsNull() && (1==showMode || 2==showMode))
  {
    if(freeViewMode && dragMode)
    {
      if(2==showMode && point.x>=margin+interval && point.x<=margin+interval+areaWidth && point.y>=margin+interval && point.y<=margin+interval+areaHeight)
      {
        old_Align_point.x = point.x;
        old_Align_point.y = point.y;
        onPress_Align = true;
        onPress_Input = false;
      }
      if(1==showMode && point.x>=rect.CenterPoint().x+margin/2+interval && point.x<=rect.CenterPoint().x+margin/2+interval+areaWidth && point.y>=margin+interval && point.y<=margin+interval+areaHeight)
      {
        old_Align_point.x = point.x;
        old_Align_point.y = point.y;
        onPress_Align = true;
        onPress_Base = false;
      }
    }
  }
  if(!imgBase.IsNull() && (0==showMode || 1==showMode))
  {
    if(freeViewMode && dragMode)
    {
      if(point.x>=margin+interval && point.x<=margin+interval+areaWidth && point.y>=margin+interval && point.y<=margin+interval+areaHeight)
      {
        old_Base_point.x = point.x;
        old_Base_point.y = point.y;
        onPress_Base = true;
        onPress_Input = false;
      }
    }
  }
  if(!imgInput.IsNull() && (0==showMode || 2==showMode))
  {
    if(freeViewMode && dragMode)
    {
      if(point.x>=rect.CenterPoint().x+margin/2+interval && point.x<=rect.CenterPoint().x+margin/2+interval+areaWidth && point.y>=margin+interval && point.y<=margin+interval+areaHeight)
      {
        old_Input_point.x = point.x;
        old_Input_point.y = point.y;
        onPress_Input = true;
      }
    }
  }
  CView::OnLButtonDown(nFlags, point);
}

//////////////////////////////////////////////////////////////
//
// mouse left button up event handler
//
//////////////////////////////////////////////////////////////
void CItkRegMFCView::OnLButtonUp(UINT nFlags, CPoint point)
{
  onPress_Align = false;
  onPress_Base = false;
  onPress_Input = false;
  CView::OnLButtonUp(nFlags, point);
}

//////////////////////////////////////////////////////////////
//
// mouse wheel event handler
//
//////////////////////////////////////////////////////////////
BOOL CItkRegMFCView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
  CRect rect;
  GetClientRect(&rect);
  float areaWidth  = (float)rect.CenterPoint().x-margin*3/2-interval*2;
  float areaHeight = (float)rect.bottom-2*margin-interval*2;
  if(!imgAlign.IsNull() && (showMode == 1 || showMode == 2))
  {
    if(freeViewMode)
    {
      if(2==showMode && point.x>=margin+interval && point.x<=margin+interval+areaWidth && point.y>=margin+interval && point.y<=margin+interval+areaHeight)
      {
        if(zDelta>0)    //zoom in
        {
          if(m_multiple_Align<100)
          {
            //m_Wheel = true;
            m_multiple_Align++;
            Invalidate(0);
            UpdateWindow();
          }
        }
        else if(zDelta<0)  //zoom out
        {
          if(m_multiple_Align>1)
          {
            //m_Wheel = true;
            m_multiple_Align--;
            Invalidate(0);
            UpdateWindow();
          }
        }
        else{}
      }
      if(1==showMode && point.x>=rect.CenterPoint().x+margin/2+interval && point.x<=rect.CenterPoint().x+margin/2+interval+areaWidth && point.y>=margin+interval && point.y<=margin+interval+areaHeight)
      {
        if(zDelta>0)    //zoom in
        {
          if(m_multiple_Align<100)
          {
            m_multiple_Align++;
            Invalidate(0);
            UpdateWindow();
          }
        }
        else if(zDelta<0)  //zoom out
        {
          if(m_multiple_Align>1)
          {
            m_multiple_Align--;
            Invalidate(0);
            UpdateWindow();
          }
        }
        else{}
      }
    }
  }
  if(!imgBase.IsNull() && (0==showMode || 1==showMode))
  {
    if(freeViewMode)
    {
      if(point.x>=margin+interval && point.x<=margin+interval+areaWidth && point.y>=margin+interval && point.y<=margin+interval+areaHeight)
      {
        if(zDelta>0)    //zoom in
        {
          if(m_multiple_Base<100)
          {
            //m_Wheel = true;
            m_multiple_Base++;
            Invalidate(0);
            UpdateWindow();
          }
        }
        else if(zDelta<0)  //zoom out
        {
          if(m_multiple_Base>1)
          {
            //m_Wheel = true;
            m_multiple_Base--;
            Invalidate(0);
            UpdateWindow();
          }
        }
        else{}
      }
    }
  }
  if(!imgInput.IsNull() && (0==showMode || 2==showMode))
  {
    if(freeViewMode)
    {
      if(point.x>=rect.CenterPoint().x+margin/2+interval && point.x<=rect.CenterPoint().x+margin/2+interval+areaWidth && point.y>=margin+interval && point.y<=margin+interval+areaHeight)
      {
        if(zDelta>0)    //zoom in
        {
          if(m_multiple_Input<50)
          {
            m_multiple_Input++;
            Invalidate(0);
            UpdateWindow();
          }
        }
        else if(zDelta<0)  //zoom out
        {
          if(m_multiple_Input>1)
          {
            m_multiple_Input--;
            Invalidate(0);
            UpdateWindow();
          }
        }
        else{}
      }
    }
  }
  return CView::OnMouseWheel(nFlags, zDelta, point);
}
//////////////////////////////////////////////////////////////
//
// thread function. a worker thread
// 
//////////////////////////////////////////////////////////////
UINT CItkRegMFCView::Run(LPVOID p)
{
  CItkRegMFCView *pView = (CItkRegMFCView *)p;
  // recorde the time
  clock_t start=clock();
  pView->Run();
  clock_t finish=clock();
  clock_t duration = finish-start;
  pView->PostMessage(WM_RESULT, NULL, duration);
  return 0;
}

//////////////////////////////////////////////////////////////
//
// Itk registration
// Please refer to InsightToolkit-3.0.0\Examples\Registration\ImageRegistration1.cxx
//////////////////////////////////////////////////////////////
void CItkRegMFCView::Run()
{
  if(m_bRunning)
  {
    if (!imgBase.IsNull() && !imgInput.IsNull()){
      //  Each one of the registration components is created using its
      //  \code{New()} method and is assigned to its respective 
      //  \doxygen{SmartPointer}.
      MetricType::Pointer         metric        = MetricType::New();
      TransformType::Pointer      transform     = TransformType::New();
      OptimizerType::Pointer      optimizer     = OptimizerType::New();
      InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
      RegistrationType::Pointer   registration  = RegistrationType::New();

      //  Each component is now connected to the instance of the registration method.
      //  \index{itk::RegistrationMethod!SetMetric()}
      //  \index{itk::RegistrationMethod!SetOptimizer()}
      //  \index{itk::RegistrationMethod!SetTransform()}
      //  \index{itk::RegistrationMethod!SetFixedImage()}
      //  \index{itk::RegistrationMethod!SetMovingImage()}
      //  \index{itk::RegistrationMethod!SetInterpolator()}

      registration->SetMetric(        metric        );
      registration->SetOptimizer(     optimizer     );
      registration->SetTransform(     transform     );
      registration->SetInterpolator(  interpolator  );

      typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
      typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;
      FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
      MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

      fixedImageReader->SetFileName(  name_imgBase );
      movingImageReader->SetFileName( name_imgInput );

      //  In this example, the fixed and moving images are read from files. This
      //  requires the \doxygen{ImageRegistrationMethod} to acquire its inputs from
      //  the output of the readers.
      registration->SetFixedImage(    fixedImageReader->GetOutput()    );
      registration->SetMovingImage(   movingImageReader->GetOutput()   );

      //  The registration can be restricted to consider only a particular region
      //  of the fixed image as input to the metric computation. This region is
      //  defined with the \code{SetFixedImageRegion()} method.  You could use this
      //  feature to reduce the computational time of the registration or to avoid
      //  unwanted objects present in the image from affecting the registration outcome.
      //  In this example we use the full available content of the image. This
      //  region is identified by the \code{BufferedRegion} of the fixed image.
      //  Note that for this region to be valid the reader must first invoke its
      //  \code{Update()} method.
      //
      //  \index{itk::ImageRegistrationMethod!SetFixedImageRegion()}
      //  \index{itk::Image!GetBufferedRegion()}
      fixedImageReader->Update();
      registration->SetFixedImageRegion( 
              fixedImageReader->GetOutput()->GetBufferedRegion() );

      //  The parameters of the transform are initialized by passing them in an
      //  array. This can be used to setup an initial known correction of the
      //  misalignment. In this particular case, a translation transform is
      //  being used for the registration. The array of parameters for this
      //  transform is simply composed of the translation values along each
      //  dimension. Setting the values of the parameters to zero 
      //  initializes the transform to an \emph{Identity} transform. Note that the
      //  array constructor requires the number of elements to be passed as an
      //  argument.
      //
      //  \index{itk::TranslationTransform!GetNumberOfParameters()}
      //  \index{itk::RegistrationMethod!SetInitialTransformParameters()}
      typedef RegistrationType::ParametersType ParametersType;
      ParametersType initialParameters( transform->GetNumberOfParameters() );

      initialParameters[0] = 0.0;  // Initial offset in mm along X
      initialParameters[1] = 0.0;  // Initial offset in mm along Y
      
      registration->SetInitialTransformParameters( initialParameters );
      
      //  At this point the registration method is ready for execution. The
      //  optimizer is the component that drives the execution of the
      //  registration.  However, the ImageRegistrationMethod class
      //  orchestrates the ensemble to make sure that everything is in place
      //  before control is passed to the optimizer.
      //
      //  It is usually desirable to fine tune the parameters of the optimizer.
      //  Each optimizer has particular parameters that must be interpreted in the
      //  context of the optimization strategy it implements. The optimizer used in
      //  this example is a variant of gradient descent that attempts to prevent it
      //  from taking steps that are too large.  At each iteration, this optimizer
      //  will take a step along the direction of the \doxygen{ImageToImageMetric}
      //  derivative. The initial length of the step is defined by the user. Each
      //  time the direction of the derivative abruptly changes, the optimizer
      //  assumes that a local extrema has been passed and reacts by reducing the
      //  step length by a half. After several reductions of the step length, the
      //  optimizer may be moving in a very restricted area of the transform
      //  parameter space. The user can define how small the step length should be
      //  to consider convergence to have been reached. This is equivalent to defining
      //  the precision with which the final transform should be known.
      //
      //  The initial step length is defined with the method
      //  \code{SetMaximumStepLength()}, while the tolerance for convergence is
      //  defined with the method \code{SetMinimumStepLength()}.
      //
      //  \index{itk::Regular\-Setp\-Gradient\-Descent\-Optimizer!SetMaximumStepLength()}
      //  \index{itk::Regular\-Step\-Gradient\-Descent\-Optimizer!SetMinimumStepLength()}
      optimizer->SetMaximumStepLength( 4.00 );  
      optimizer->SetMinimumStepLength( 0.01 );

      //  In case the optimizer never succeeds reaching the desired
      //  precision tolerance, it is prudent to establish a limit on the number of
      //  iterations to be performed. This maximum number is defined with the
      //  method \code{SetNumberOfIterations()}.
      //
      //  \index{itk::Regular\-Setp\-Gradient\-Descent\-Optimizer!SetNumberOfIterations()}
      optimizer->SetNumberOfIterations( 200 );

      // Connect an observer
      CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
      optimizer->AddObserver( itk::IterationEvent(), observer );
      //  The registration process is triggered by an invocation to the
      //  \code{Update()} method. If something goes wrong during the
      //  initialization or execution of the registration an exception will be
      //  thrown. We should therefore place the \code{Update()} method
      //  inside a \code{try/catch} block as illustrated in the following lines.
      try 
      { 
        registration->Update(); 
      } 
      catch( itk::ExceptionObject) 
      { 
        MessageBox("error");
        exit(0);
      }

      //  The result of the registration process is an array of parameters that
      //  defines the spatial transformation in an unique way. This final result is
      //  obtained using the \code{GetLastTransformParameters()} method.
      //
      //  \index{itk::RegistrationMethod!GetLastTransformParameters()}
      ParametersType finalParameters = registration->GetLastTransformParameters();

      //  In the case of the \doxygen{TranslationTransform}, there is a
      //  straightforward interpretation of the parameters.  Each element of the
      //  array corresponds to a translation along one spatial dimension.
      const double TranslationAlongX = finalParameters[0];
      const double TranslationAlongY = finalParameters[1];

      //  The optimizer can be queried for the actual number of iterations
      //  performed to reach convergence.  The \code{GetCurrentIteration()}
      //  method returns this value. A large number of iterations may be an
      //  indication that the maximum step length has been set too small, which
      //  is undesirable since it results in long computational times.
      //
      //  \index{itk::Regular\-Setp\-Gradient\-Descent\-Optimizer!GetCurrentIteration()}
      const unsigned int numberOfIterations = optimizer->GetCurrentIteration();

      //  The value of the image metric corresponding to the last set of parameters
      //  can be obtained with the \code{GetValue()} method of the optimizer.
      const double bestValue = optimizer->GetValue();

      // Print out results
      // std::cout << "Result = " << std::endl;
      // std::cout << " Translation X = " << TranslationAlongX  << std::endl;
      // std::cout << " Translation Y = " << TranslationAlongY  << std::endl;
      // std::cout << " Iterations    = " << numberOfIterations << std::endl;
      // std::cout << " Metric value  = " << bestValue          << std::endl;

      result_info.Format("Result = \nTranslation X= %f\nTranslation Y = %f\nIterations = %ld\nMetric value = %f",
                TranslationAlongX,TranslationAlongY,numberOfIterations,bestValue);

      //  Let's execute this example over two of the images provided in
      //  \code{Examples/Data}:
      //  
      //  \begin{itemize}
      //  \item \code{BrainProtonDensitySliceBorder20.png} 
      //  \item \code{BrainProtonDensitySliceShifted13x17y.png}
      //  \end{itemize}
      //
      //  The second image is the result of intentionally translating the first
      //  image by $(13,17)$ millimeters. Both images have unit-spacing and
      //  are shown in Figure \ref{fig:FixedMovingImageRegistration1}. The
      //  registration takes 18 iterations and the resulting transform parameters are:
      //
      //  \begin{verbatim}
      //  Translation X = 12.9959
      //  Translation Y = 17.0001
      //  \end{verbatim}
      // 
      //  As expected, these values match quite well the misalignment that we
      //  intentionally introduced in the moving image.
      //
      // \begin{figure}
      // \center
      // \includegraphics[width=0.44\textwidth]{BrainProtonDensitySliceBorder20.eps}
      // \includegraphics[width=0.44\textwidth]{BrainProtonDensitySliceShifted13x17y.eps}
      // \itkcaption[Fixed and Moving images in registration framework]{Fixed and
      // Moving image provided as input to the registration method.}
      // \label{fig:FixedMovingImageRegistration1}
      // \end{figure}

      //  It is common, as the last step of a registration task, to use the
      //  resulting transform to map the moving image into the fixed image space.
      //  This is easily done with the \doxygen{ResampleImageFilter}. Please
      //  refer to Section~\ref{sec:ResampleImageFilter} for details on the use
      //  of this filter.  First, a ResampleImageFilter type is instantiated
      //  using the image types. It is convenient to use the fixed image type as
      //  the output type since it is likely that the transformed moving image
      //  will be compared with the fixed image.
      typedef itk::ResampleImageFilter< 
                  MovingImageType, 
                  FixedImageType >    ResampleFilterType;
      
      //  A resampling filter is created and the moving image is connected as
      //  its input.
      ResampleFilterType::Pointer resampler = ResampleFilterType::New();
      resampler->SetInput( movingImageReader->GetOutput() );

      //  The Transform that is produced as output of the Registration method is
      //  also passed as input to the resampling filter. Note the use of the
      //  methods \code{GetOutput()} and \code{Get()}. This combination is needed
      //  here because the registration method acts as a filter whose output is a
      //  transform decorated in the form of a \doxygen{DataObject}. For details in
      //  this construction you may want to read the documentation of the
      //  \doxygen{DataObjectDecorator}.
      //
      //  \index{itk::ImageRegistrationMethod!Resampling image}
      //  \index{itk::ImageRegistrationMethod!Pipeline}
      //  \index{itk::ImageRegistrationMethod!DataObjectDecorator}
      //  \index{itk::ImageRegistrationMethod!GetOutput()}
      //  \index{itk::DataObjectDecorator!Use in Registration}
      //  \index{itk::DataObjectDecorator!Get()}
      resampler->SetTransform( registration->GetOutput()->Get() );

      //  As described in Section \ref{sec:ResampleImageFilter}, the
      //  ResampleImageFilter requires additional parameters to be specified, in
      //  particular, the spacing, origin and size of the output image. The default
      //  pixel value is also set to a distinct gray level in order to highlight
      //  the regions that are mapped outside of the moving image.
      FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();
      resampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
      resampler->SetOutputOrigin(  fixedImage->GetOrigin() );
      resampler->SetOutputSpacing( fixedImage->GetSpacing() );
      resampler->SetDefaultPixelValue( 100 );

      // \begin{figure}
      // \center
      // \includegraphics[width=0.32\textwidth]{ImageRegistration1Output.eps}
      // \includegraphics[width=0.32\textwidth]{ImageRegistration1DifferenceBefore.eps}
      // \includegraphics[width=0.32\textwidth]{ImageRegistration1DifferenceAfter.eps}
      // \itkcaption[HelloWorld registration output images]{Mapped moving image and its
      // difference with the fixed image before and after registration}
      // \label{fig:ImageRegistration1Output}
      // \end{figure}

      //  The output of the filter is passed to a writer that will store the
      //  image in a file. An \doxygen{CastImageFilter} is used to convert the
      //  pixel type of the resampled image to the final type used by the
      //  writer. The cast and writer filters are instantiated below.
      typedef unsigned char OutputPixelType;
      typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
      typedef itk::CastImageFilter< 
                FixedImageType,
                OutputImageType > CastFilterType;
      typedef itk::ImageFileWriter< OutputImageType >  WriterType;

      //  The filters are created by invoking their \code{New()}
      //  method.
      WriterType::Pointer      writer =  WriterType::New();
      CastFilterType::Pointer  caster =  CastFilterType::New();

      writer->SetFileName( name_imgAlign );

      //  The filters are connected together and the \code{Update()} method of the
      //  writer is invoked in order to trigger the execution of the pipeline.
      caster->SetInput( resampler->GetOutput() );
      writer->SetInput( caster->GetOutput()   );
      writer->Update();

      if(imgAlign != NULL)
      {
        imgAlign.Destroy();
      }
      HRESULT hResult;
      hResult = imgAlign.Load(name_imgAlign);
      if (FAILED(hResult)) {
        MessageBox(_T("Aligned Image Loading Failed!"),0,0);
        Invalidate();
        UpdateWindow();
        return;
      }

      showMode = 1;
      Invalidate();
      UpdateWindow();
    }else{
      MessageBox("fixed/moving image can't be null");
    }
  }
  m_bRunning = FALSE;
}

//////////////////////////////////////////////////////////////
//
// handle registration results
// 
//////////////////////////////////////////////////////////////
LRESULT CItkRegMFCView::NotifyResult(WPARAM wParam, LPARAM lParam)
{
  CString str;
  double time=(double)lParam;
  str.Format("Done!\nTotal elapsed time : %.0f ms",time);
  str = str+"\n"+result_info;
  MessageBox(str);
  return 0;
}

//////////////////////////////////////////////////////////////
//
// handle Itk registration iteration information
// 
//////////////////////////////////////////////////////////////
void CItkRegMFCView::UpdateView(CString str)
{
  CMainFrame *pMainFrame = (CMainFrame *)AfxGetApp()->m_pMainWnd;
  CEdit *pEdit = (CEdit*)pMainFrame->m_wndDialogBar.GetDlgItem(IDC_INFO);
  pEdit->SetWindowText(str);
}
//end
