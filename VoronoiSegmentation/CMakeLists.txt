PROJECT (VoronoiSegmentation)

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
${VoronoiSegmentation_SOURCE_DIR}
)

LINK_LIBRARIES (
ITKCommon
ITKNumerics
ITKBasicFilters 
ITKIO
ITKFltkImageViewer

)

SET(Project_FLTK_SRCS
VDSegGrayUI.fl
)

FLTK_WRAP_UI(VoronoiSegmentation ${Project_FLTK_SRCS})
ITK_DISABLE_FLTK_GENERATED_WARNINGS("${Project_FLTK_SRCS}")

SET(Project_SRCS
VDSegMain.cpp
VDSegGrayRun.cxx
${VoronoiSegmentation_FLTK_UI_SRCS}
)

ADD_GUI_EXECUTABLE(VoronoiSegmentation   "${Project_SRCS}")

