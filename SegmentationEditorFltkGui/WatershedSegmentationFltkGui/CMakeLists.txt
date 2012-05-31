PROJECT( WatershedSegmentationFltkGui )

INCLUDE_DIRECTORIES(
${ITKApps_SOURCE_DIR}/Auxiliary/FltkImageViewer
${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
${WatershedSegmentationFltkGui_SOURCE_DIR}
${WatershedSegmentationFltkGui_BINARY_DIR}
${FLTK_INCLUDE_PATH}
${OPENGL_INCLUDE_PATH}
)

LINK_DIRECTORIES(
${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
${WatershedSegmentationFltkGui_BINARY_DIR}
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

FLTK_WRAP_UI(WatershedSegmentationFltkGui3D   ${WatershedSegmentationFltkGui3D_GUI_SRCS})
ITK_DISABLE_FLTK_GENERATED_WARNINGS("Segmenter3D;SegmenterConsole3D.cxx;${WatershedSegmentationFltkGui3D_FLTK_UI_SRCS}")

ADD_EXECUTABLE(WatershedSegmentationFltkGui3D ${WatershedSegmentationFltkGui3D_SRCS} ${WatershedSegmentationFltkGui3D_FLTK_UI_SRCS})
INSTALL_TARGETS(/bin WatershedSegmentationFltkGui3D)

TARGET_LINK_LIBRARIES (WatershedSegmentationFltkGui3D
  ${ITK_LIBRARIES}
  ITKFltkImageViewer
  ${FTLK_LIBRARIES}
)


