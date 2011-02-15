PROJECT(FastMarchingLevelSet)

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build ITKApps without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)


  FIND_PACKAGE(FLTK)
  IF(FLTK_FOUND)
    INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
  ENDIF(FLTK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


INCLUDE_DIRECTORIES(
  ${ITKApps_SOURCE_DIR}/Auxiliary/FltkImageViewer
  ${ITKApps_SOURCE_DIR}/Auxiliary/VtkFltk
  ${ITKApps_SOURCE_DIR}/Auxiliary/vtk
  ${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
  ${ITKApps_BINARY_DIR}/Auxiliary/VtkFltk
  ${ITKApps_SOURCE_DIR}/FastMarchingLevelSet
  ${ITKApps_BINARY_DIR}/FastMarchingLevelSet
)

SET(FastMarchingLevelSet_GUI_SRCS
FastMarchingLevelSetGUI.fl
)

FLTK_WRAP_UI(FastMarchingLevelSet FastMarchingLevelSetGUI.fl)

SET(FastMarchingLevelSet_SRCS
  FastMarchingLevelSetBase.cxx
  FastMarchingLevelSet.cxx
  ${FastMarchingLevelSet_FLTK_UI_SRCS}
  )

ITK_DISABLE_FLTK_GENERATED_WARNINGS("${FastMarchingLevelSet_SRCS}")

ADD_GUI_EXECUTABLE(FastMarchingLevelSet "${FastMarchingLevelSet_SRCS}")

TARGET_LINK_LIBRARIES(FastMarchingLevelSet   ${ITK_LIBRARIES} ITKVtkFltk
                      ITKFltkImageViewer)
