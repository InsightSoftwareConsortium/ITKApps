PROJECT(GeodesicActiveContour)


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

  FIND_PACKAGE(VTK)
  IF (VTK_FOUND)
      INCLUDE (${VTK_USE_FILE})
  ENDIF (VTK_FOUND)


ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


INCLUDE_DIRECTORIES(
  ${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_SOURCE_DIR}/Auxiliary/VtkFltk
  ${InsightApplications_SOURCE_DIR}/Auxiliary/vtk
  ${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_BINARY_DIR}/Auxiliary/VtkFltk
  ${InsightApplications_SOURCE_DIR}/GeodesicActiveContour
  ${InsightApplications_BINARY_DIR}/GeodesicActiveContour
)

SET(GeodesicActiveContour_SRCS
  GeodesicActiveContourBase.cxx
  GeodesicActiveContour.cxx
  )

ADD_GUI_EXECUTABLE(GeodesicActiveContour "${GeodesicActiveContour_SRCS}")

FLTK_WRAP_UI(GeodesicActiveContour GeodesicActiveContourGUI.fl)
ITK_DISABLE_FLTK_GENERATED_WARNINGS(GeodesicActiveContourGUI.fl)
  
TARGET_LINK_LIBRARIES(GeodesicActiveContour ITKNumerics ITKBasicFilters
                      ITKIO vtkHybrid vtkIO ITKVtkFltk ITKFltkImageViewer)
