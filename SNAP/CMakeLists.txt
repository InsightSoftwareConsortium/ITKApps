#############################################
# PROJECT: SNAP                             #
#############################################
PROJECT(SNAP)

#############################################
# SOURCE FILE SPECIFICATION                 #
#############################################

# The part of the source code devoted to the SNAP application logic 
# is organized into a separate library
SET(LOGIC_CXX
  Common/CommandLineArgumentParser.cxx
  Common/IRISException.cxx
  Common/Registry.cxx
  Common/SystemInterface.cxx
  Logic/Common/ImageCoordinateGeometry.cxx
  Logic/Common/ImageCoordinateTransform.cxx
  Logic/Common/SNAPRegistryIO.cxx
  Logic/Framework/GlobalState.cxx
  Logic/Framework/IRISApplication.cxx
  Logic/Framework/IRISImageData.cxx
  Logic/Framework/SNAPImageData.cxx
  Logic/ImageWrapper/GreyImageWrapper.cxx
  Logic/ImageWrapper/LabelImageWrapper.cxx
  Logic/ImageWrapper/LevelSetImageWrapper.cxx
  Logic/ImageWrapper/SpeedImageWrapper.cxx
  Logic/LevelSet/SnakeParameters.cxx
  Logic/LevelSet/SNAPLevelSetDriver.cxx
  Logic/Mesh/IRISMeshPipeline.cxx
  Logic/Mesh/LevelSetMeshPipeline.cxx
  Logic/Mesh/MeshObject.cxx
  Logic/Mesh/MeshOptions.cxx
  Logic/Mesh/VTKMeshPipeline.cxx
  Logic/Preprocessing/EdgePreprocessingSettings.cxx
  Logic/Preprocessing/ThresholdSettings.cxx
  Logic/Slicing/IntensityCurveVTK.cxx
)

# The headers for the Logic code
SET(LOGIC_HEADERS
  Common/CommandLineArgumentParser.h
  Common/Credits.h
  Common/IRISException.h
  Common/IRISVectorTypes.h
  Common/IRISVectorTypesToITKConversion.h
  Common/IRISVectorTypes.txx
  Common/Registry.h
  Common/SNAPCommon.h
  Common/SystemInterface.h
  Logic/Common/ColorLabel.h
  Logic/Common/ImageCoordinateGeometry.h
  Logic/Common/ImageCoordinateTransform.h
  Logic/Common/ImageRayIntersectionFinder.h
  Logic/Common/ImageRayIntersectionFinder.txx
  Logic/Common/SNAPRegistryIO.h
  Logic/Common/SNAPSegmentationROISettings.h
  Logic/Framework/GlobalState.h
  Logic/Framework/IRISApplication.h
  Logic/Framework/IRISImageData.h
  Logic/Framework/SNAPImageData.h
  Logic/ImageWrapper/GreyImageWrapper.h
  Logic/ImageWrapper/ImageIORoutines.h
  Logic/ImageWrapper/ImageWrapper.h
  Logic/ImageWrapper/ImageWrapper.txx
  Logic/ImageWrapper/LabelImageWrapper.h
  Logic/ImageWrapper/LevelSetImageWrapper.h
  Logic/ImageWrapper/SpeedImageWrapper.h
  Logic/LevelSet/LevelSetExtensionFilter.h
  Logic/LevelSet/SnakeParameters.h
  Logic/LevelSet/SignedDistanceFilter.h
  Logic/LevelSet/SignedDistanceFilter.txx
  Logic/LevelSet/SNAPAdvectionFieldImageFilter.h
  Logic/LevelSet/SNAPAdvectionFieldImageFilter.txx
  Logic/LevelSet/SNAPLevelSetDriver.h
  Logic/LevelSet/SNAPLevelSetFunction.h
  Logic/LevelSet/SNAPLevelSetFunction.txx
  Logic/LevelSet/SNAPLevelSetStopAndGoFilter.h
  Logic/LevelSet/SNAPLevelSetStopAndGoFilter.txx
  Logic/Mesh/IRISMeshPipeline.h
  Logic/Mesh/LevelSetMeshPipeline.h
  Logic/Mesh/MeshObject.h
  Logic/Mesh/MeshOptions.h
  Logic/Mesh/VTKMeshPipeline.h
  Logic/Preprocessing/EdgePreprocessingImageFilter.h
  Logic/Preprocessing/EdgePreprocessingImageFilter.txx
  Logic/Preprocessing/EdgePreprocessingSettings.h
  Logic/Preprocessing/SmoothBinaryThresholdImageFilter.h
  Logic/Preprocessing/SmoothBinaryThresholdImageFilter.txx
  Logic/Preprocessing/ThresholdSettings.h
  Logic/Slicing/IntensityCurveInterface.h
  Logic/Slicing/IntensityCurveVTK.h
  Logic/Slicing/IRISSlicer.h
  Logic/Slicing/IRISSlicer.txx
  Logic/Slicing/UnaryFunctorCache.h
  Logic/Slicing/UnaryFunctorCache.txx
)

# These files contain the user interface source code
SET(UI_CXX
  UserInterface/BasicComponents/FLTKCanvas.cxx
  UserInterface/BasicComponents/FunctionPlot2DBox.cxx
  UserInterface/BasicComponents/FunctionPlot2D.cxx
  UserInterface/BasicComponents/SnakeParametersPreviewBox.cxx
  UserInterface/BasicComponents/SnakeParametersPreviewPipeline.cxx
  UserInterface/ImageIOWizard/GreyImageIOWizardLogic.cxx
  UserInterface/ImageIOWizard/PreprocessingImageIOWizardLogic.cxx
  UserInterface/ImageIOWizard/SegmentationImageIOWizardLogic.cxx
  UserInterface/MainComponents/HelpViewerLogic.cxx
  UserInterface/MainComponents/IntensityCurveBox.cxx
  UserInterface/MainComponents/IntensityCurveUILogic.cxx
  UserInterface/MainComponents/PreprocessingUILogic.cxx
  UserInterface/MainComponents/ResizeRegionDialogLogic.cxx
  UserInterface/MainComponents/RestoreSettingsDialogLogic.cxx
  UserInterface/MainComponents/SimpleFileDialogLogic.cxx
  UserInterface/MainComponents/SnakeParametersUILogic.cxx
  UserInterface/MainComponents/UserInterfaceLogic.cxx
  UserInterface/SliceWindow/BubblesInteractionMode.cxx
  UserInterface/SliceWindow/CrosshairsInteractionMode.cxx
  UserInterface/SliceWindow/GenericSliceWindow.cxx
  UserInterface/SliceWindow/IRISSliceWindow.cxx
  UserInterface/SliceWindow/PolygonDrawing.cxx
  UserInterface/SliceWindow/PolygonInteractionMode.cxx
  UserInterface/SliceWindow/RegionInteractionMode.cxx
  UserInterface/SliceWindow/SNAPSliceWindow.cxx
  UserInterface/SliceWindow/SliceWindowCoordinator.cxx
  UserInterface/SliceWindow/ZoomPanInteractionMode.cxx
  UserInterface/Window3D/Trackball.cxx
  UserInterface/Window3D/Window3D.cxx
)

# The header files for the UI project
SET(UI_HEADERS
  UserInterface/BasicComponents/FLTKCanvas.h
  UserInterface/BasicComponents/FLTKEvent.h
  UserInterface/BasicComponents/FunctionPlot2DBox.h
  UserInterface/BasicComponents/FunctionPlot2D.h
  UserInterface/BasicComponents/InteractionMode.h
  UserInterface/BasicComponents/SnakeParametersPreviewBox.h
  UserInterface/BasicComponents/SnakeParametersPreviewPipeline.h
  UserInterface/Common/SNAPCommonUI.h
  UserInterface/ImageIOWizard/GreyImageIOWizardLogic.h
  UserInterface/ImageIOWizard/ImageIOWizardBase.h
  UserInterface/ImageIOWizard/ImageIOWizardLogic.h
  UserInterface/ImageIOWizard/ImageIOWizardLogic.txx
  UserInterface/ImageIOWizard/PreprocessingImageIOWizardLogic.h
  UserInterface/ImageIOWizard/RestrictedImageIOWizardLogic.h
  UserInterface/ImageIOWizard/RestrictedImageIOWizardLogic.txx
  UserInterface/ImageIOWizard/SegmentationImageIOWizardLogic.h
  UserInterface/MainComponents/HelpViewerBase.h
  UserInterface/MainComponents/HelpViewerLogic.h
  UserInterface/MainComponents/IntensityCurveBox.h
  UserInterface/MainComponents/IntensityCurveUIBase.h
  UserInterface/MainComponents/IntensityCurveUILogic.h
  UserInterface/MainComponents/PreprocessingUIBase.h
  UserInterface/MainComponents/PreprocessingUILogic.h
  UserInterface/MainComponents/ResizeRegionDialogBase.h
  UserInterface/MainComponents/ResizeRegionDialogLogic.h
  UserInterface/MainComponents/RestoreSettingsDialogBase.h
  UserInterface/MainComponents/RestoreSettingsDialogLogic.h
  UserInterface/MainComponents/SnakeParametersUIBase.h
  UserInterface/MainComponents/SnakeParametersUILogic.h
  UserInterface/MainComponents/SimpleFileDialogBase.h
  UserInterface/MainComponents/SimpleFileDialogLogic.h
  UserInterface/MainComponents/UserInterfaceBase.h
  UserInterface/MainComponents/UserInterfaceLogic.h
  UserInterface/SliceWindow/BubblesInteractionMode.h
  UserInterface/SliceWindow/CrosshairsInteractionMode.h
  UserInterface/SliceWindow/GenericSliceWindow.h
  UserInterface/SliceWindow/IRISSliceWindow.h
  UserInterface/SliceWindow/OpenGLSliceTexture.h
  UserInterface/SliceWindow/OpenGLSliceTexture.txx
  UserInterface/SliceWindow/PolygonDrawing.h
  UserInterface/SliceWindow/PolygonInteractionMode.h
  UserInterface/SliceWindow/RegionInteractionMode.h
  UserInterface/SliceWindow/SNAPSliceWindow.h
  UserInterface/SliceWindow/SliceWindowCoordinator.h
  UserInterface/SliceWindow/ZoomPanInteractionMode.h
  UserInterface/Window3D/Trackball.h
  UserInterface/Window3D/Window3D.h
)

# The source code for the stand-alone SNAP application
SET(APPLICATION_CXX
  UserInterface/SNAPMain.cxx
)

# The source code for SNAP testing project
SET(TESTING_CXX
  Testing/Compatibility/SnakeWrapper.cxx
  Testing/SNAPTestDriver.cxx
  Testing/TestCompareLevelSets.cxx
  Testing/TestMain.cxx
)

# The headers for the testing code
SET(TESTING_HEADERS
  Testing/SNAPTestDriver.h
  Testing/TestBase.h
  Testing/TestCompareLevelSets.h
  Testing/TestImageWrapper.h
  Testing/Compatibility/SnakeWrapper.h
)

# The FL files for SNAP
SET(APPLICATION_FLUIDS
  UserInterface/ImageIOWizard/ImageIOWizard.fl
  UserInterface/MainComponents/HelpViewer.fl
  UserInterface/MainComponents/IntensityCurveUI.fl
  UserInterface/MainComponents/PreprocessingUI.fl
  UserInterface/MainComponents/ResizeRegionDialog.fl
  UserInterface/MainComponents/RestoreSettingsDialog.fl
  UserInterface/MainComponents/SimpleFileDialog.fl
  UserInterface/MainComponents/SnakeParametersUI.fl
  UserInterface/MainComponents/UserInterface.fl
)

# All program files
SET(PROGRAM_DATA_FILES
  ProgramData/SNAPProgramDataDirectory.txt
  ProgramData/Images2D/EdgeForcesExample.png
  ProgramData/Images2D/RegionForcesExample.png
  ProgramData/Presets/SnakeParameterPreviewCurve.txt
  ProgramData/HTMLHelp/Credits.html
  ProgramData/HTMLHelp/Tutorial.html
  ProgramData/HTMLHelp/TutorialSectionIntroduction.html
  ProgramData/HTMLHelp/TutorialSectionIntroductionToAutomatic.html
  ProgramData/HTMLHelp/TutorialSectionLoadingImages.html
  ProgramData/HTMLHelp/TutorialSectionManualSegmentation.html
  ProgramData/HTMLHelp/TutorialSectionRegionSegmentation.html
  ProgramData/HTMLHelp/TutorialSectionTipsAndTricks.html
  ProgramData/HTMLHelp/TutorialSectionViewingImages.html
  ProgramData/HTMLHelp/Artwork/ttAutoIntroEdgesPicture.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroEdgesPipeline01.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroEdgesPipeline02.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroEdgesPipeline03.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroEdgesPipeline04.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroEdgesScale.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroRegionScale.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroRegionsExample.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroRegionsHisto.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroRegionsRemapFunction.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroRegionsSegment01.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroRegionsSegment02.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroRegionsSegment03.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroRegionsSegment04.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroSnakeConcept01.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroSnakeConcept02.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroSnakeConcept03.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroSnakeConcept04.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroSnakeMerge01.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroSnakeMerge02.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroSnakeMerge03.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroSnakeMerge04.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroSnakeVelocities.gif
  ProgramData/HTMLHelp/Artwork/ttAutoIntroSnakeVelocitiesMotion01.gif
  ProgramData/HTMLHelp/Artwork/ttIconAction.gif
  ProgramData/HTMLHelp/Artwork/ttIconAttention.gif
  ProgramData/HTMLHelp/Artwork/ttIconExpert.gif
  ProgramData/HTMLHelp/Artwork/ttIconTip.gif
  ProgramData/HTMLHelp/Artwork/ttIntroScreenLayout.gif
  ProgramData/HTMLHelp/Artwork/ttLoadImageBrowser.gif
  ProgramData/HTMLHelp/Artwork/ttLoadImageMenuBar.gif
  ProgramData/HTMLHelp/Artwork/ttLoadImageResult.gif
  ProgramData/HTMLHelp/Artwork/ttLoadImageWizardOrientation.gif
  ProgramData/HTMLHelp/Artwork/ttLoadImageWizardPreview.gif
  ProgramData/HTMLHelp/Artwork/ttLoadImageWizardPreviewWithFile.gif
  ProgramData/HTMLHelp/Artwork/ttLoadImageWizardSummary.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentLabelEditor.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentLabelEditorChanged.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentLabelFileDialog.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentLabelSubPanel.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentLabelSubPanel02.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentPaintClosePath.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentPaintFill.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentPaintPaste.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentPaintSelectSome.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentPaintStart.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentResult.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentRightCaudate.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentSaveWizard.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentSaveWizardFilled.gif
  ProgramData/HTMLHelp/Artwork/ttManualSegmentToolbarManual.gif
  ProgramData/HTMLHelp/Artwork/ttRegion3DImaginaryLine.gif
  ProgramData/HTMLHelp/Artwork/ttRegion3DScalpelLine.gif
  ProgramData/HTMLHelp/Artwork/ttRegion3DScalpelResult.gif
  ProgramData/HTMLHelp/Artwork/ttRegion3DTools.gif
  ProgramData/HTMLHelp/Artwork/ttRegionAddBubble01.gif
  ProgramData/HTMLHelp/Artwork/ttRegionAddBubble02.gif
  ProgramData/HTMLHelp/Artwork/ttRegionFunctionForVentricles.gif
  ProgramData/HTMLHelp/Artwork/ttRegionNewInterface.gif
  ProgramData/HTMLHelp/Artwork/ttRegionParameterWindow.gif
  ProgramData/HTMLHelp/Artwork/ttRegionPreprocessor01.gif
  ProgramData/HTMLHelp/Artwork/ttRegionPreprocessor02.gif
  ProgramData/HTMLHelp/Artwork/ttRegionResult.gif
  ProgramData/HTMLHelp/Artwork/ttRegionROISelection01.gif
  ProgramData/HTMLHelp/Artwork/ttRegionROISelection02.gif
  ProgramData/HTMLHelp/Artwork/ttRegionROISelectionFinal.gif
  ProgramData/HTMLHelp/Artwork/ttRegionScalpelLabels.gif
  ProgramData/HTMLHelp/Artwork/ttRegionToolbar.gif
  ProgramData/HTMLHelp/Artwork/ttRegionVentricleResult.gif
  ProgramData/HTMLHelp/Artwork/ttRegionWizard02.gif
  ProgramData/HTMLHelp/Artwork/ttRegionWizard03.gif
  ProgramData/HTMLHelp/Artwork/ttViewImageControlPanel.gif
  ProgramData/HTMLHelp/Artwork/ttViewImageControlPanelToolbox01.gif
  ProgramData/HTMLHelp/Artwork/ttViewImageControlPanelToolbox02.gif
  ProgramData/HTMLHelp/Artwork/ttViewImageControlPanelToolOptions01.gif
  ProgramData/HTMLHelp/Artwork/ttViewImageControlPanelToolOptions02.gif
  ProgramData/HTMLHelp/Artwork/ttViewImageDisplayOptions.gif
  ProgramData/HTMLHelp/Artwork/ttViewImageIntensityDialog.gif
  ProgramData/HTMLHelp/Artwork/ttViewImageIntensityDialogWindowing.gif
  ProgramData/HTMLHelp/Artwork/ttViewImageIntensityDialogWindowingCurve.gif
  ProgramData/HTMLHelp/Artwork/ttViewImageSliceWindowClick.gif
  ProgramData/HTMLHelp/Artwork/ttViewImageSliceWindowZoom.gif
)

# The sources needed to build the SNAP logic library
SET(LOGIC_SRCS
  ${LOGIC_CXX}
  ${LOGIC_HEADERS}
)

# The sources needed to build the SNAP application
SET(APPLICATION_SRCS
  ${LOGIC_SRCS}
  ${UI_CXX}  
  ${UI_HEADERS}
  ${APPLICATION_CXX}
)

# Combined sources for the testing application
SET(TESTING_SRCS
  ${LOGIC_SRCS}
  ${TESTING_CXX}
  ${TESTING_HEADERS}
)

# The Logic library
# ADD_LIBRARY(SNAPLogic ${LOGIC_SRCS})

# The SNAP executable
ADD_GUI_EXECUTABLE(SNAP "${APPLICATION_SRCS}")

# The testing executable
ADD_GUI_EXECUTABLE(SNAPTests "${TESTING_SRCS}")

# FLTK Wrapping
FLTK_WRAP_UI(SNAP ${APPLICATION_FLUIDS})

# Disable FLTK warnings 
ITK_DISABLE_FLTK_GENERATED_WARNINGS("${APPLICATION_FLUIDS}")

#############################################
# LIBRARIES AND EXTERNAL CODE               #
#############################################

# Shorthand to source and binary directories
SET(MY_SOURCE_DIR ${InsightApplications_SOURCE_DIR}/SNAP)
SET(MY_BINARY_DIR ${InsightApplications_BINARY_DIR}/SNAP)

# Include directories
INCLUDE_DIRECTORIES(
  ${MY_SOURCE_DIR}/Common
  ${MY_SOURCE_DIR}/Logic
  ${MY_SOURCE_DIR}/Logic/Common
  ${MY_SOURCE_DIR}/Logic/Framework
  ${MY_SOURCE_DIR}/Logic/ImageWrapper
  ${MY_SOURCE_DIR}/Logic/LevelSet
  ${MY_SOURCE_DIR}/Logic/Mesh
  ${MY_SOURCE_DIR}/Logic/Preprocessing
  ${MY_SOURCE_DIR}/Logic/Slicing
  ${MY_SOURCE_DIR}/Testing
  ${MY_SOURCE_DIR}/Testing/Compatibility
  ${MY_SOURCE_DIR}/Testing/Compatibility/CVS
  ${MY_SOURCE_DIR}/UserInterface
  ${MY_SOURCE_DIR}/UserInterface/BasicComponents
  ${MY_SOURCE_DIR}/UserInterface/Common
  ${MY_SOURCE_DIR}/UserInterface/ImageIOWizard
  ${MY_SOURCE_DIR}/UserInterface/MainComponents
  ${MY_SOURCE_DIR}/UserInterface/MainComponents/Artwork
  ${MY_SOURCE_DIR}/UserInterface/SliceWindow
  ${MY_SOURCE_DIR}/UserInterface/Window3D
  ${MY_BINARY_DIR}/UserInterface/ImageIOWizard
  ${MY_BINARY_DIR}/UserInterface/MainComponents
  ${FLTK_INCLUDE_PATH}
  ${OPENGL_INCLUDE_PATH}
)

# Link libraries
SET(MY_GRAPHICS_LIBS
  vtkCommon
  vtkFiltering
  vtkGraphics
 # vtkHybrid
  vtkImaging
  vtkRendering
  ${FLTK_LIBRARIES} 
  ${OPENGL_glu_LIBRARY}
  ${OPENGL_LIBRARIES}
)

SET(MY_ITK_LIBS
  ITKIO
)

TARGET_LINK_LIBRARIES(SNAP ${MY_GRAPHICS_LIBS} ${MY_ITK_LIBS})
TARGET_LINK_LIBRARIES(SNAPTests ${MY_GRAPHICS_LIBS} ${MY_ITK_LIBS})

# Copy documentation from the source tree to the build tree
FOREACH(DATAFILE ${PROGRAM_DATA_FILES})
  CONFIGURE_FILE(
    ${MY_SOURCE_DIR}/${DATAFILE}
    ${MY_BINARY_DIR}/${DATAFILE}
    COPYONLY)
ENDFOREACH(DATAFILE)

# Copy the InnoSetup installer configuration script
CONFIGURE_FILE(
  ${MY_SOURCE_DIR}/Utilities/InnoSetup/InstallSNAP.iss.in
  ${MY_BINARY_DIR}/Utilities/InnoSetup/InstallSNAP.iss)

