// ItkRegMFCDoc.cpp : implementation of the CItkRegMFCDoc class
//

#include "stdafx.h"
#include "ItkRegMFC.h"

#include "ItkRegMFCDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CItkRegMFCDoc

IMPLEMENT_DYNCREATE(CItkRegMFCDoc, CDocument)

BEGIN_MESSAGE_MAP(CItkRegMFCDoc, CDocument)
END_MESSAGE_MAP()


// CItkRegMFCDoc construction/destruction

CItkRegMFCDoc::CItkRegMFCDoc()
{
  // TODO: add one-time construction code here

}

CItkRegMFCDoc::~CItkRegMFCDoc()
{
}

BOOL CItkRegMFCDoc::OnNewDocument()
{
  if (!CDocument::OnNewDocument())
    return FALSE;

  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)

  return TRUE;
}




// CItkRegMFCDoc serialization

void CItkRegMFCDoc::Serialize(CArchive& ar)
{
  if (ar.IsStoring())
  {
    // TODO: add storing code here
  }
  else
  {
    // TODO: add loading code here
  }
}


// CItkRegMFCDoc diagnostics

#ifdef _DEBUG
void CItkRegMFCDoc::AssertValid() const
{
  CDocument::AssertValid();
}

void CItkRegMFCDoc::Dump(CDumpContext& dc) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG


// CItkRegMFCDoc commands
