/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    FEMMeshApplication.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/



#include "FEMMeshApplication.h"
#include <FL/fl_ask.H>

FEMMeshApplication
::FEMMeshApplication()
{
 
}




FEMMeshApplication
::~FEMMeshApplication()
{

}





void
FEMMeshApplication
::CreateFEMMesh(void)
{
 
  try
    {
    this->FEMMeshApplicationBase::CreateFEMMesh();
    }
  catch( itk::ExceptionObject & excp )
  {
    fl_alert( excp.GetDescription() );
    return;
  }
  
  m_FlRenderWindowInteractor->redraw();
  Fl::check();

}




void
FEMMeshApplication
::DisplayFEMMesh(void)
{

  try
    {
    this->FEMMeshApplicationBase::DisplayFEMMesh();
    }
  catch( itk::ExceptionObject & excp )
  {
    fl_alert( excp.what() );
    return;
  }

  m_FlRenderWindowInteractor->redraw();
  Fl::check();
}





void
FEMMeshApplication
::ComputeArea(void) const
{
 
  try
    {
    this->FEMMeshApplicationBase::ComputeArea();
    }
  catch( itk::ExceptionObject & excp )
  {
    fl_alert( excp.GetDescription() );
    return;
  }
  
  m_FlRenderWindowInteractor->redraw();
  Fl::check();

}




void
FEMMeshApplication
::ComputeHeatConduction(void) const
{
 
  try
    {
    this->FEMMeshApplicationBase::ComputeHeatConduction();
    }
  catch( itk::ExceptionObject & excp )
  {
    fl_alert( excp.GetDescription() );
    return;
  }
  
  m_FlRenderWindowInteractor->redraw();
  Fl::check();

}



