PROJECT( CannySegmentationLevelSet )

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
${ITKApps_SOURCE_DIR}/Auxiliary/vtk
${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
${CannySegmentationLevelSet_SOURCE_DIR}
${CannySegmentationLevelSet_BINARY_DIR}
)

SET(CannySegmentationLevelSet_GUI_SRCS
CannySegmentationLevelSetGUI.fl
)

FLTK_WRAP_UI( CannySegmentationLevelSet ${CannySegmentationLevelSet_GUI_SRCS} )

SET(CannySegmentationLevelSet_SRCS
CannySegmentationLevelSetBase.cxx
CannySegmentationLevelSet.cxx
${CannySegmentationLevelSet_FLTK_UI_SRCS}
)

ITK_DISABLE_FLTK_GENERATED_WARNINGS("${CannySegmentationLevelSet_SRCS}")

ADD_GUI_EXECUTABLE(CannySegmentationLevelSet "${CannySegmentationLevelSet_SRCS}")
TARGET_LINK_LIBRARIES (
  CannySegmentationLevelSet
  ${ITK_LIBRARIES}
  vtkRendering
  vtkGraphics
  vtkHybrid
  vtkImaging
  vtkIO
  vtkFiltering
  vtkCommon
  ITKFltkImageViewer
)



