// ItkMFCView.cpp : implementation of the CItkMFCView class
//

#include "stdafx.h"
#include "ItkMFC.h"

#include "ItkMFCDoc.h"
#include "ItkMFCView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItkMFCView

IMPLEMENT_DYNCREATE(CItkMFCView, CView)

BEGIN_MESSAGE_MAP(CItkMFCView, CView)
  //{{AFX_MSG_MAP(CItkMFCView)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
  //}}AFX_MSG_MAP
  // Standard printing commands
  ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItkMFCView construction/destruction

CItkMFCView::CItkMFCView()
{
  // TODO: add construction code here

}

CItkMFCView::~CItkMFCView()
{
}

BOOL CItkMFCView::PreCreateWindow(CREATESTRUCT& cs)
{
  // TODO: Modify the Window class or styles here by modifying
  //  the CREATESTRUCT cs

  return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CItkMFCView drawing

void CItkMFCView::OnDraw(CDC* pDC)
{
  CItkMFCDoc* pDoc = GetDocument();
  ASSERT_VALID(pDoc);
  // TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CItkMFCView printing

BOOL CItkMFCView::OnPreparePrinting(CPrintInfo* pInfo)
{
  // default preparation
  return DoPreparePrinting(pInfo);
}

void CItkMFCView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add extra initialization before printing
}

void CItkMFCView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
  // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CItkMFCView diagnostics

#ifdef _DEBUG
void CItkMFCView::AssertValid() const
{
  CView::AssertValid();
}

void CItkMFCView::Dump(CDumpContext& dc) const
{
  CView::Dump(dc);
}

CItkMFCDoc* CItkMFCView::GetDocument() // non-debug version is inline
{
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CItkMFCDoc)));
  return (CItkMFCDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CItkMFCView message handlers
