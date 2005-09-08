PROJECT( ThresholdSegmentationLevelSetFltkGui )


IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

  FIND_PACKAGE(FLTK)
  IF(FLTK_FOUND)
    INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
  ENDIF(FLTK_FOUND)

  FIND_PACKAGE(VTK)
  IF (VTK_FOUND)
      INCLUDE (${VTK_USE_FILE})
  ENDIF (VTK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

INCLUDE_DIRECTORIES(
${ITK_SOURCE_DIR}/Code/Common
${ITK_SOURCE_DIR}/Code/BasicFilters
${ITK_SOURCE_DIR}/Code/Algorithms
${ITK_SOURCE_DIR}/Code/IO
${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
${ThresholdSegmentationLevelSetFltkGui_SOURCE_DIR}
${ThresholdSegmentationLevelSetFltkGui_BINARY_DIR}
${FLTK_INCLUDE_PATH}
${OPENGL_INCLUDE_PATH}
)

LINK_DIRECTORIES(
${ITK_BINARY_DIR}/Code/Common
${ITK_BINARY_DIR}/Code/BasicFilters
${ITK_BINARY_DIR}/Code/Algorithms
${ITK_BINARY_DIR}/Code/IO
${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
${ThresholdSegmentationLevelSetFltkGui_BINARY_DIR}
)

SET(ThresholdSegmentationLevelSetFltkGui_GUI_SRCS
SegmenterConsoleGUI.fl
ImageViewerGUI.fl
)

FLTK_WRAP_UI(ThresholdSegmentationLevelSetFltkGui  ${ThresholdSegmentationLevelSetFltkGui_GUI_SRCS})
ITK_DISABLE_FLTK_GENERATED_WARNINGS("${ThresholdSegmentationLevelSetFltkGui_GUI_SRCS}")

LINK_LIBRARIES (
ITKBasicFilters 
ITKIO
ITKNumerics
ITKFltkImageViewer
)

SET(ThresholdSegmentationLevelSetFltkGui_SRCS
SegmenterConsole.cxx
SegmenterConsoleBase.cxx
Segmenter.cxx
ImageViewer.txx
${ThresholdSegmentationLevelSetFltkGui_FLTK_UI_SRCS}
)

ADD_EXECUTABLE(ThresholdSegmentationLevelSetFltkGui ${ThresholdSegmentationLevelSetFltkGui_SRCS})

