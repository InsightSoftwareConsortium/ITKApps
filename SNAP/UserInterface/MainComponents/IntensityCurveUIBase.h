/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IntensityCurveUIBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __IntensityCurveUIBase_h_
#define __IntensityCurveUIBase_h_

/**
 * \class IntensityCurveUIBase
 * \brief Base class for intensity curve FLTK interface.
 */
class IntensityCurveUIBase {
public:
  // Callbacks made from the user interface
  virtual void OnClose() = 0;
  virtual void OnReset() = 0;
  virtual void OnWindowLevelChange() = 0;
  virtual void OnControlPointNumberChange() = 0;
        
};

#endif // __IntensityCurveUIBase_h_
