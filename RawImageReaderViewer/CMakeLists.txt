PROJECT( RawImageReaderViewer )

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

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

INCLUDE_DIRECTORIES(
${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
${RawImageReaderViewer_SOURCE_DIR}
)

SET(RawImageReaderViewer_GUI_SRCS
itkRawImageReaderViewerGUI.fl
)

FLTK_WRAP_UI(RawImageReaderViewer   ${RawImageReaderViewer_GUI_SRCS})

SET(RawImageReaderViewer_SRCS
itkRawImageReaderViewer.cxx
RawImageReaderViewer.cxx
${RawImageReaderViewer_FLTK_UI_SRCS}
)

ITK_DISABLE_FLTK_GENERATED_WARNINGS("${RawImageReaderViewer_SRCS}")
ADD_GUI_EXECUTABLE(RawImageReaderViewer   "${RawImageReaderViewer_SRCS}")

TARGET_LINK_LIBRARIES (RawImageReaderViewer
ITKBasicFilters
ITKIO
ITKFltkImageViewer
)


