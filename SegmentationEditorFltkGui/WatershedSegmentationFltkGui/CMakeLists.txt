PROJECT( WatershedSegmentationFltkGui )

INCLUDE_DIRECTORIES(
${ITK_SOURCE_DIR}/Code/Common
${ITK_SOURCE_DIR}/Code/BasicFilters
${ITK_SOURCE_DIR}/Code/Algorithms
${ITK_SOURCE_DIR}/Code/IO
${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
${WatershedSegmentationFltkGui_SOURCE_DIR}
${WatershedSegmentationFltkGui_BINRY_DIR}
${FLTK_INCLUDE_PATH}
${OPENGL_INCLUDE_PATH}
)

LINK_DIRECTORIES(
${ITK_BINARY_DIR}/Code/Common
${ITK_BINARY_DIR}/Code/BasicFilters
${ITK_BINARY_DIR}/Code/Algorithms
${ITK_BINARY_DIR}/Code/IO
${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
${WatershedSegmentationFltkGui_BINARY_DIR}
${FLTK_LIBRARY}
${OPENGL_LIBRARY}
${OPENGL_glu_LIBRARY}
)

SET(WatershedSegmentationFltkGui3D_SRCS
SegmenterConsole3D.cxx
SegmenterConsole3DBase.cxx
Segmenter3D.cxx
itkWatershedFilterAndWriter.txx
itkWatershedSegmentTreeWriter.txx
fltkRGBImageViewer.txx	
)

SET(WatershedSegmentationFltkGui3D_GUI_SRCS
SegmenterConsole3DGUI.fl
fltkRGBImageViewerGUI.fl	
)

LINK_LIBRARIES (
ITKBasicFilters 
ITKIO
ITKNumerics
ITKFltkImageViewer
${FTLK_LIBRARY}
${OPENGL_LIBRARY}
${OPENGL_glu_LIBRARY}
)

IF(WIN32)
  LINK_LIBRARIES(
  wsock32
  )
ENDIF(WIN32)

ADD_EXECUTABLE(WatershedSegmentationFltkGui3D ${WatershedSegmentationFltkGui3D_SRCS})

FLTK_WRAP_UI(WatershedSegmentationFltkGui3D   ${WatershedSegmentationFltkGui3D_GUI_SRCS})
