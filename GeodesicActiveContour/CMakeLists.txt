PROJECT(GeodesicActiveContour)

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
  ${ITKApps_SOURCE_DIR}/GeodesicActiveContour
  ${ITKApps_BINARY_DIR}/GeodesicActiveContour
)

FLTK_WRAP_UI(GeodesicActiveContour GeodesicActiveContourGUI.fl)
ITK_DISABLE_FLTK_GENERATED_WARNINGS("${GeodesicActiveContour_FLTK_UI_SRCS};GeodesicActiveContour.cxx")

SET(GeodesicActiveContour_SRCS
  GeodesicActiveContourBase.cxx
  GeodesicActiveContour.cxx
  ${GeodesicActiveContour_FLTK_UI_SRCS}
  )

ADD_GUI_EXECUTABLE(GeodesicActiveContour "${GeodesicActiveContour_SRCS}")

TARGET_LINK_LIBRARIES(GeodesicActiveContour   ${ITK_LIBRARIES}
                      vtkHybrid vtkIO ITKVtkFltk ITKFltkImageViewer)
