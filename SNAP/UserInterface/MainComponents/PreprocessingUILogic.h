/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    PreprocessingUILogic.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Copyright (c) 2003 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __PreprocessingUILogic_h_
#define __PreprocessingUILogic_h_

#include "PreprocessingUI.h"
#include "IRISTypes.h"
#include "itkSmartPointer.h"
#include "EdgePreprocessingImageFilter.h"
#include "SmoothBinaryThresholdImageFilter.h"

// Forward references to application classes
class GlobalState;
class IRISApplication;
class UserInterfaceLogic;

// ITK forward references
namespace itk {
  template<class TPixel, unsigned int VDimension> class Image;
  template<class TObject> class SimpleMemberCommand;
}

/**
 * \class PreprocessingUILogic
 * \brief Logic for the preprocessing UI.
 */
class PreprocessingUILogic : public PreprocessingUI
{
public:

  // A virtual destructor to make gcc happy
  virtual ~PreprocessingUILogic() {}
  
  // Callbacks for the Edge snake preprocessing window
  void OnEdgeSettingsChange();
  void OnEdgePreviewChange();  
  void OnEdgeOk();
  void OnEdgeClose();
  void OnEdgeApply();

  // Callbacks for the InOut snake preprocessing window
  void OnThresholdDirectionChange();
  void OnThresholdLowerChange();
  void OnThresholdUpperChange();
  void OnThresholdSettingsChange();
  void OnThresholdOk();
  void OnThresholdClose();
  void OnThresholdApply();
  void OnThresholdPreviewChange();
  void OnThresholdOverlayChange();
  
  // Register this class with the parent user interface
  void Register(UserInterfaceLogic *parent);

  // Display the Edge preprocessing window
  void DisplayEdgeWindow();

  // Display the In/Out preprocessing window
  void DisplayInOutWindow();

  // Programmatically hide the preprocessing windows
  void HidePreprocessingWindows();

private:
  GlobalState *m_GlobalState;
  IRISApplication *m_Driver;
  UserInterfaceLogic *m_ParentUI;

  /** The image types used for preprocessing */
  typedef itk::Image<GreyType,3> GreyImageType;
  typedef itk::Image<float,3> SpeedImageType;
  
  /** The filter type for in/out processing */
  typedef SmoothBinaryThresholdImageFilter<
    GreyImageType,SpeedImageType> InOutFilterType;
  typedef itk::SmartPointer<InOutFilterType> InOutFilterPointer;

  /** The filter type for edge preprocessing */
  typedef EdgePreprocessingImageFilter<
    GreyImageType,SpeedImageType> EdgeFilterType;
  typedef itk::SmartPointer<EdgeFilterType> EdgeFilterPointer;

  /** A command type for progress reporting */
  typedef itk::SimpleMemberCommand<PreprocessingUILogic> CommandType;
  typedef itk::SmartPointer<CommandType> CommandPointer;

  /** The filter used for in-out thresholding */
  InOutFilterPointer m_InOutPreviewFilter[3];
  InOutFilterPointer m_InOutFilterWhole;

  /** The filters used for edge preprocessing */
  EdgeFilterPointer m_EdgePreviewFilter[3];
  EdgeFilterPointer m_EdgeFilterWhole;

  /** Update the plot shown in the edge plot box */
  void UpdateEdgePlot();

  /** Update the plot shown in the in-out plot box */
  void UpdateThresholdPlot();

  /** Progress callback for edge preprocessing */
  void OnEdgeProgress();

  /** Progress callback for thresholding preprocessing */
  void OnThresholdProgress();

  /** Common closing code for both preprocessors */
  void OnCloseCommon();

};

#endif // __PreprocessingUILogic_h_
