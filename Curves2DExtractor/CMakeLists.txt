PROJECT(Curves2DExtractor)

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
  ${InsightApplications_SOURCE_DIR}/Curves2DExtractor
  ${InsightApplications_BINARY_DIR}/Curves2DExtractor
  ${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
)

ADD_LIBRARY(Curves2DExtractorLib ceExtractorConsole.cxx
            ceExtractorConsoleBase.cxx)

# Take the list of GUI files, generate .cxx and .h from them and add
# them to the build process of the target
FLTK_WRAP_UI(Curves2DExtractorLib ceExtractorConsoleGUI.fl)
ITK_DISABLE_FLTK_GENERATED_WARNINGS(ceExtractorConsoleGUI.fl)

TARGET_LINK_LIBRARIES(Curves2DExtractorLib
                      ITKBasicFilters ITKIO ITKFltkImageViewer)

ADD_GUI_EXECUTABLE(Curves2DExtractor main.cxx ${Curves2DExtractor_FLTK_UI_SRCS})
TARGET_LINK_LIBRARIES(Curves2DExtractor Curves2DExtractorLib)

