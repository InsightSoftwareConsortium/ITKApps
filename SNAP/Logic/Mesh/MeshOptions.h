/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    MeshOptions.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __MeshOptions_h_
#define __MeshOptions_h_

#include "IRISTypes.h"

/**
 * \class MeshOptions
 * \brief A set of options for mesh display in IRIS.
 */
class MeshOptions  
{
public:
  MeshOptions();
  ~MeshOptions();

  irisGetMacro(UseGaussianSmoothing,bool);
  irisSetMacro(UseGaussianSmoothing,bool);
    
  irisGetMacro(UseDecimation,bool);
  irisSetMacro(UseDecimation,bool);

  irisGetMacro(UseMeshSmoothing,bool);
  irisSetMacro(UseMeshSmoothing,bool);

  irisGetMacro(GaussianStandardDeviation,Vector3f);
  irisSetMacro(GaussianStandardDeviation,Vector3f);

  irisGetMacro(GaussianError,float);
  irisSetMacro(GaussianError,float);

  irisGetMacro(DecimateTargetReduction,float);
  irisSetMacro(DecimateTargetReduction,float);

  irisGetMacro(DecimateInitialError,float);
  irisSetMacro(DecimateInitialError,float);

  irisGetMacro(DecimateAspectRatio,float);
  irisSetMacro(DecimateAspectRatio,float);

  irisGetMacro(DecimateFeatureAngle,float);
  irisSetMacro(DecimateFeatureAngle,float);

  irisGetMacro(DecimateErrorIncrement,float);
  irisSetMacro(DecimateErrorIncrement,float);

  irisGetMacro(DecimateMaximumIterations,unsigned int);
  irisSetMacro(DecimateMaximumIterations,unsigned int);

  irisGetMacro(DecimatePreserveTopology,bool);
  irisSetMacro(DecimatePreserveTopology,bool);

  irisGetMacro(MeshSmoothingRelaxationFactor,float);
  irisSetMacro(MeshSmoothingRelaxationFactor,float);

  irisGetMacro(MeshSmoothingIterations,unsigned int);
  irisSetMacro(MeshSmoothingIterations,unsigned int);

  irisGetMacro(MeshSmoothingConvergence,float);
  irisSetMacro(MeshSmoothingConvergence,float);

  irisGetMacro(MeshSmoothingFeatureAngle,float);
  irisSetMacro(MeshSmoothingFeatureAngle,float);

  irisGetMacro(MeshSmoothingFeatureEdgeSmoothing,bool);
  irisSetMacro(MeshSmoothingFeatureEdgeSmoothing,bool);

  irisGetMacro(MeshSmoothingBoundarySmoothing,bool);
  irisSetMacro(MeshSmoothingBoundarySmoothing,bool);


private:
  // Begin render switches
  bool m_UseGaussianSmoothing;
  bool m_UseDecimation;
  bool m_UseMeshSmoothing;
  
  // Begin gsmooth params
  Vector3f m_GaussianStandardDeviation;
  float m_GaussianError;
  
  // Begin decimate parameters
  float m_DecimateTargetReduction;
  float m_DecimateInitialError;
  float m_DecimateAspectRatio;
  float m_DecimateFeatureAngle;
  float m_DecimateErrorIncrement;
  unsigned int m_DecimateMaximumIterations;
  bool m_DecimatePreserveTopology;
  
  // Begin msmooth params
  float m_MeshSmoothingRelaxationFactor;
  unsigned int m_MeshSmoothingIterations;
  float m_MeshSmoothingConvergence;
  float m_MeshSmoothingFeatureAngle;
  bool m_MeshSmoothingFeatureEdgeSmoothing;
  bool m_MeshSmoothingBoundarySmoothing;
};

#endif // __MeshOptions_h_
/*Log: MeshOptions.h
/*Revision 1.5  2003/07/12 01:34:18  pauly
/*More final changes before ITK checkin
/*
/*Revision 1.4  2003/07/11 23:29:17  pauly
/**** empty log message ***
/*
/*Revision 1.3  2003/07/11 21:25:12  pauly
/*Code cleanup for ITK checkin
/*
/*Revision 1.2  2003/06/08 16:11:42  pauly
/*User interface changes
/*Automatic mesh updating in SNAP mode
/*
/*Revision 1.1  2003/03/07 19:29:47  pauly
/*Initial checkin
/*
/*Revision 1.1.1.1  2002/12/10 01:35:36  pauly
/*Started the project repository
/*
/*
/*Revision 1.2  2002/03/08 14:06:30  moon
/*Added Header and Log tags to all files
/**/
