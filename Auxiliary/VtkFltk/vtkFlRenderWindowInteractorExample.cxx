/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    vtkFlRenderWindowInteractorExample.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkInteractorStyleTrackballCamera.h"

#include "vtkFlRenderWindowInteractor.h"

int main()
{

  Fl_Window * form = new Fl_Window(301,301,"View Test");
    
  vtkFlRenderWindowInteractor * renderWindowInteractor = 
    new vtkFlRenderWindowInteractor( 10,10,280,280,"VTK Window");

  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renderWindowInteractor->SetRenderWindow( renWin );

  vtkRenderer *ren1= vtkRenderer::New();
  renWin->AddRenderer( ren1 );

  form->end();

  renderWindowInteractor->Initialize();

  form->show();

  vtkInteractorStyleTrackballCamera *style = 
    vtkInteractorStyleTrackballCamera::New();

  renderWindowInteractor->SetInteractorStyle( style );

  vtkConeSource *cone = vtkConeSource::New();
  cone->SetHeight( 3.0 );
  cone->SetRadius( 1.0 );
  cone->SetResolution( 10 );
  
  vtkPolyDataMapper *coneMapper = vtkPolyDataMapper::New();
  coneMapper->SetInputConnection( cone->GetOutputPort() );

  vtkActor *coneActor = vtkActor::New();
  coneActor->SetMapper( coneMapper );

  ren1->AddActor( coneActor );
  ren1->SetBackground( 0.1, 0.2, 0.4 );

  unsigned int i=0;

  for( i=0; i<50; i++ )
    {
    Fl::wait(0.01);
    Fl::check();
    }

  form->hide();

  for( i=0; i<50; i++ )
    {
    Fl::wait(0.01);
    Fl::check();
    }

  form->show();

  for( i=0; i<50; i++ )
    {
    Fl::wait(0.01);
    Fl::check();
    }

  form->hide();

  style->Delete();
  cone->Delete();
  coneMapper->Delete();
  coneActor->Delete();
  ren1->Delete();
  renWin->Delete();

  renderWindowInteractor->Delete();
  delete form;

  return 0;
}


