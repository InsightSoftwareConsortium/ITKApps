// ItkMFC.h : main header file for the ITKMFC application
//

#if !defined(AFX_ITKMFC_H__54F6FA1C_7022_42F3_981C_EBC0E9BBED6A__INCLUDED_)
#define AFX_ITKMFC_H__54F6FA1C_7022_42F3_981C_EBC0E9BBED6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CItkMFCApp:
// See ItkMFC.cpp for the implementation of this class
//

class CItkMFCApp : public CWinApp
{
public:
  CItkMFCApp();

// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CItkMFCApp)
  public:
  virtual BOOL InitInstance();
  //}}AFX_VIRTUAL

// Implementation
  //{{AFX_MSG(CItkMFCApp)
  afx_msg void OnAppAbout();
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITKMFC_H__54F6FA1C_7022_42F3_981C_EBC0E9BBED6A__INCLUDED_)
