/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ColorLabel.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __ColorLabel_h_
#define __ColorLabel_h_

#include "SNAPCommon.h"

#include <assert.h>
    

/**
 * \class ColorLabel
 * \brief Information about a label used for segmentation.
 * Color labels used to describe pixels in the segmented
 * image.  These labels correspond to the intensities in the
 * segmentation image in IRISImageData class
 */
class ColorLabel {
public:
  // Dummy constructor and destructor (to make gcc happy)
  ColorLabel() {}
  virtual ~ColorLabel() {}

  // Read the Visible attribute
  irisIsMacro(Visible);

  // Set the Visible attribute
  irisSetMacro(Visible,bool);

  // Read the Valid attribute
  irisIsMacro(Valid);

  // Set the Valid attribute
  irisSetMacro(Valid,bool);

  // Read the DoMesh attribute
  irisIsMacro(DoMesh);

  // Set the DoMesh attribute
  irisSetMacro(DoMesh,bool);

  // Read the Label attribute
  virtual const char *GetLabel() const
  {
    return m_Label.c_str();
  }

  // Set the Label attribute
  irisSetMacro(Label,const char *);

  // Read the Alpha attribute
  irisGetMacro(Alpha,unsigned char);

  // Set the Alpha attribute
  irisSetMacro(Alpha,unsigned char);

  // Check Opaqueness
  bool IsOpaque() const {
    return m_Alpha == 255;
  }

  // Read the RGB attributes
  unsigned char GetRGB(unsigned int index) const {
    assert(index < 3);
    return m_RGB[index];
  }

  // Set the RGB attributes
  void SetRGB(unsigned int index, unsigned char in_Value) {
    assert(index < 3);
    m_RGB[index] = in_Value;
  }

  // Set all three at once
  void SetRGB(unsigned char in_Red,unsigned char in_Green,unsigned char in_Blue) {
    m_RGB[0] = in_Red;
    m_RGB[1] = in_Green;
    m_RGB[2] = in_Blue;
  }

  // Copy RGB into an array
  void GetRGBVector(unsigned char array[3]) const {
    array[0] = m_RGB[0];
    array[1] = m_RGB[1];
    array[2] = m_RGB[2];
  }

  // Copy RGB into an array
  void SetRGBVector(const unsigned char array[3]) {
    m_RGB[0] = array[0];
    m_RGB[1] = array[1];
    m_RGB[2] = array[2];
  }

  // Copy RGB into an array
  void GetRGBAVector(unsigned char array[4]) const {
    array[0] = m_RGB[0];
    array[1] = m_RGB[1];
    array[2] = m_RGB[2];
    array[3] = m_Alpha;
  }

  // Copy RGB into an array
  void SetRGBAVector(const unsigned char array[4]) {
    m_RGB[0] = array[0];
    m_RGB[1] = array[1];
    m_RGB[2] = array[2];
    m_Alpha  = array[3];
  }

private:  
  bool m_Visible;
  bool m_Valid;
  bool m_DoMesh;

  std::string m_Label;

  unsigned char m_Alpha;
  unsigned char m_RGB[3];
};


#endif
