// ItkRegMFC.h : main header file for the ItkRegMFC application
//
#pragma once

#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

// CItkRegMFCApp:
// See ItkRegMFC.cpp for the implementation of this class
//

class CItkRegMFCApp : public CWinApp
{
public:
  CItkRegMFCApp();


// Overrides
public:
  virtual BOOL InitInstance();

// Implementation
  afx_msg void OnAppAbout();
  DECLARE_MESSAGE_MAP()
};

extern CItkRegMFCApp theApp;
