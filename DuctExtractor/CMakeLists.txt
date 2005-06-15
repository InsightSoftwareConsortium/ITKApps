PROJECT(DuctExtractor)


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
  ${InsightApplications_SOURCE_DIR}/DuctExtractor
  ${InsightApplications_BINARY_DIR}/DuctExtractor
)

SET(DuctExtractor_SRCS 
  DuctExtractorConsole.cxx
  DuctExtractorConsoleBase.cxx 
  DuctExtractor.cxx
  )

ADD_GUI_EXECUTABLE(DuctExtractor   "${DuctExtractor_SRCS}")

FLTK_WRAP_UI(DuctExtractor DuctExtractorConsoleGUI.fl)
ITK_DISABLE_FLTK_GENERATED_WARNINGS(DuctExtractorConsoleGUI.fl)

TARGET_LINK_LIBRARIES(DuctExtractor ITKFltkImageViewer ITKIO ITKBasicFilters)


