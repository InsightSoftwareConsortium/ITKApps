PROJECT(FastMarchingLevelSet)

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
  ${InsightApplications_SOURCE_DIR}/Auxiliary/VtkFltk
  ${InsightApplications_SOURCE_DIR}/Auxiliary/vtk
  ${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_BINARY_DIR}/Auxiliary/VtkFltk
  ${InsightApplications_SOURCE_DIR}/FastMarchingLevelSet
  ${InsightApplications_BINARY_DIR}/FastMarchingLevelSet
)

SET(FastMarchingLevelSet_SRCS 
  FastMarchingLevelSetBase.cxx
  FastMarchingLevelSet.cxx
  )

ADD_GUI_EXECUTABLE(FastMarchingLevelSet "${FastMarchingLevelSet_SRCS}")

FLTK_WRAP_UI(FastMarchingLevelSet FastMarchingLevelSetGUI.fl)

TARGET_LINK_LIBRARIES(FastMarchingLevelSet ITKIO ITKVtkFltk
                      ITKBasicFilters ITKNumerics ITKFltkImageViewer)
