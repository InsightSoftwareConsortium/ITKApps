/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    HelpViewerBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __HelpViewerBase_h_
#define __HelpViewerBase_h_

class HelpViewerBase {
public:
  virtual void OnLinkAction() = 0;
  virtual void OnBackAction() = 0;
  virtual void OnForwardAction() = 0;
  virtual void OnCloseAction() = 0;
  virtual void OnContentsAction() = 0;
};

#endif // __HelpViewerBase_h_
