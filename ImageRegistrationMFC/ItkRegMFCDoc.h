// ItkRegMFCDoc.h : interface of the CItkRegMFCDoc class
//


#pragma once

class CItkRegMFCDoc : public CDocument
{
protected: // create from serialization only
  CItkRegMFCDoc();
  DECLARE_DYNCREATE(CItkRegMFCDoc)

// Attributes
public:

// Operations
public:

// Overrides
  public:
  virtual BOOL OnNewDocument();
  virtual void Serialize(CArchive& ar);

// Implementation
public:
  virtual ~CItkRegMFCDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
  DECLARE_MESSAGE_MAP()
};


