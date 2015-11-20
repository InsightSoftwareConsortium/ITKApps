PROJECT( ShapeDetectionLevelSet )

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

  FIND_PACKAGE(VTK)
  IF (VTK_FOUND)
      INCLUDE (${VTK_USE_FILE})
  ENDIF (VTK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


INCLUDE_DIRECTORIES(
${ITKApps_SOURCE_DIR}/Auxiliary/FltkImageViewer
${ITKApps_SOURCE_DIR}/Auxiliary/VtkFltk
${ITKApps_SOURCE_DIR}/Auxiliary/vtk
${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
${ITKApps_BINARY_DIR}/Auxiliary/VtkFltk
${ShapeDetectionLevelSet_SOURCE_DIR}
${ShapeDetectionLevelSet_BINARY_DIR}
)

SET(ShapeDetectionLevelSet_GUI_SRCS
ShapeDetectionLevelSetGUI.fl
)

FLTK_WRAP_UI( ShapeDetectionLevelSet ${ShapeDetectionLevelSet_GUI_SRCS} )

SET(ShapeDetectionLevelSet_SRCS
ShapeDetectionLevelSetBase.cxx
ShapeDetectionLevelSet.cxx
${ShapeDetectionLevelSet_FLTK_UI_SRCS}
)

ITK_DISABLE_FLTK_GENERATED_WARNINGS("${ShapeDetectionLevelSet_SRCS}")

ADD_GUI_EXECUTABLE(ShapeDetectionLevelSet "${ShapeDetectionLevelSet_SRCS}")

TARGET_LINK_LIBRARIES (ShapeDetectionLevelSet
  ${ITK_LIBRARIES}
  ${VTK_LIBRARIES}
  ITKVtkFltk
  ITKFltkImageViewer
)


