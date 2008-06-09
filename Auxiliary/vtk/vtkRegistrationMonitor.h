/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    vtkRegistrationMonitor.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __RegistrationMonitor_h
#define __RegistrationMonitor_h

#include "itkMatrixOffsetTransformBase.h"
#include "itkCommand.h"
#include "itkOptimizer.h"

class vtkPolyData;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkMatrix4x4;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;


/** \class vtkRegistrationMonitor 
 *  This class provides a VTK visualization pipeline configured for monitoring
 *  the progress of a registration process. 
 */
class vtkRegistrationMonitor
{
public:
  
  typedef vtkRegistrationMonitor  Self;

  typedef itk::Optimizer       OptimizerType;

  typedef itk::MatrixOffsetTransformBase< double, 3, 3 > TransformType;

  vtkRegistrationMonitor();
  ~vtkRegistrationMonitor();

  void SetFixedSurface( vtkPolyData* surface );
  void SetMovingSurface( vtkPolyData* surface );

  void SetNumberOfIterationPerUpdate( unsigned int number );
    
  void Observe( OptimizerType * optimizer, TransformType * transform );


private:
  
  vtkMatrix4x4*                   Matrix;

  vtkPolyData*                    FixedSurface;
  vtkActor*                       FixedActor;
  vtkProperty*                    FixedProperty;
  vtkPolyDataMapper*              FixedMapper;

  vtkPolyData*                    MovingSurface;
  vtkActor*                       MovingActor;
  vtkProperty*                    MovingProperty;
  vtkPolyDataMapper*              MovingMapper;

  // Visualization pipeline
  vtkRenderer*                    Renderer;
  vtkRenderWindow*                RenderWindow;
  vtkRenderWindowInteractor*      RenderWindowInteractor;

  typedef itk::SimpleMemberCommand< Self >  ObserverType;

  ObserverType::Pointer           IterationObserver;
  ObserverType::Pointer           StartObserver;

  OptimizerType::Pointer          ObservedOptimizer;

  TransformType::Pointer          ObservedTransform;

  unsigned int                    CurrentIterationNumber;
  unsigned int                    NumberOfIterationPerUpdate;

  // These methods will only be called by the Observer
  void Update();
  void StartVisualization();

};

#endif
