// ItkMFCDoc.cpp : implementation of the CItkMFCDoc class
//

#include "stdafx.h"
#include "ItkMFC.h"

#include "ItkMFCDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItkMFCDoc

IMPLEMENT_DYNCREATE(CItkMFCDoc, CDocument)

BEGIN_MESSAGE_MAP(CItkMFCDoc, CDocument)
  //{{AFX_MSG_MAP(CItkMFCDoc)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
  //}}AFX_MSG_MAP
  ON_COMMAND(ID_FILE_OPEN, CItkMFCDoc::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItkMFCDoc construction/destruction

CItkMFCDoc::CItkMFCDoc()
{
  // TODO: add one-time construction code here
}

CItkMFCDoc::~CItkMFCDoc()
{
}

BOOL CItkMFCDoc::OnNewDocument()
{
  if (!CDocument::OnNewDocument())
    return FALSE;

  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)
  return TRUE;
}


void CItkMFCDoc::OnFileOpen()
{
  CString strFilter;

  strFilter = "MetaImage|*.mhd;*.mha|Analyze|*.hdr|VTK Image|*.vtk|JPEG|*.jpeg|TIFF Image|*.tiff|PNG Image|*.png|GIPL Image|*.gipl||";

  CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, strFilter);

  HRESULT hResult;
  hResult = (int)dlg.DoModal();
  if( hResult != IDOK )
    {
    return;
    }

  m_ItkPipeline.SetInputFileName( dlg.GetFileName() );

  try
    {
    m_ItkPipeline.LoadInputFile();
    }
  catch( itk::ExceptionObject & excp )
    {
    CString fmt;
    fmt.Format("Load image failed:\n%s",excp.GetDescription() );
    ::AfxMessageBox(fmt);
    }

}


/////////////////////////////////////////////////////////////////////////////
// CItkMFCDoc serialization

void CItkMFCDoc::Serialize(CArchive& ar)
{
  if (ar.IsStoring())
  {
    // TODO: add storing code here
    m_ItkPipeline.SetOutputFileName("myFile.mhd");
  }
  else
  {
    // TODO: add loading code here
    m_ItkPipeline.SetInputFileName("myFile.mhd");
  }
}

/////////////////////////////////////////////////////////////////////////////
// CItkMFCDoc diagnostics

#ifdef _DEBUG
void CItkMFCDoc::AssertValid() const
{
  CDocument::AssertValid();
}

void CItkMFCDoc::Dump(CDumpContext& dc) const
{
  CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CItkMFCDoc commands
