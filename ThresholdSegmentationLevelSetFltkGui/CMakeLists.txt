PROJECT( ThresholdSegmentationLevelSetFltkGui )

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
${FLTK_LIBRARY}
${OPENGL_LIBRARY}
${GLU_LIBRARY}
)

SET(ThresholdSegmentationLevelSetFltkGui_SRCS
SegmenterConsole.cxx
SegmenterConsoleBase.cxx
Segmenter.cxx
ImageViewer.txx
)

SET(ThresholdSegmentationLevelSetFltkGui_GUI_SRCS
SegmenterConsoleGUI.fl
ImageViewerGUI.fl
)

LINK_LIBRARIES (
ITKBasicFilters 
ITKIO
ITKNumerics
ITKFltkImageViewer
${FLTK_LIBRARY}
${OPENGL_LIBRARY}
${GLU_LIBRARY}
)

IF(WIN32)
  LINK_LIBRARIES(
  wsock32
  )
ENDIF(WIN32)

ADD_EXECUTABLE(ThresholdSegmentationLevelSetFltkGui ${ThresholdSegmentationLevelSetFltkGui_SRCS})

FLTK_WRAP_UI(ThresholdSegmentationLevelSetFltkGui  ${ThresholdSegmentationLevelSetFltkGui_GUI_SRCS})
ITK_DISABLE_FLTK_GENERATED_WARNINGS("${ThresholdSegmentationLevelSetFltkGui_GUI_SRCS}")
