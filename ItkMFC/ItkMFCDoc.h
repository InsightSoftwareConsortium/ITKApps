// ItkMFCDoc.h : interface of the CItkMFCDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITKMFCDOC_H__B588CC89_3B8A_462F_A7F8_9B7C1F376660__INCLUDED_)
#define AFX_ITKMFCDOC_H__B588CC89_3B8A_462F_A7F8_9B7C1F376660__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ItkPipeline.h"

class CItkMFCDoc : public CDocument
{
protected: // create from serialization only
  CItkMFCDoc();
  DECLARE_DYNCREATE(CItkMFCDoc)

// Attributes
public:

// Operations
public:

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CItkMFCDoc)
  public:
  virtual BOOL OnNewDocument();
  virtual void OnFileOpen();
  virtual void OnFileSave();
  virtual void Serialize(CArchive& ar);
  //}}AFX_VIRTUAL

// Implementation
public:
  virtual ~CItkMFCDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
  //{{AFX_MSG(CItkMFCDoc)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
private:

  ItkPipeline m_ItkPipeline;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITKMFCDOC_H__B588CC89_3B8A_462F_A7F8_9B7C1F376660__INCLUDED_)
