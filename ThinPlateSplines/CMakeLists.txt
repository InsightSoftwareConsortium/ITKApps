PROJECT(ThinPlateSplines)


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
  ${InsightApplications_SOURCE_DIR}/ThinPlateSplines
  ${InsightApplications_SOURCE_DIR}/ThinPlateSplines
  ${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
)

ADD_EXECUTABLE(ThinPlateSplines ThinPlateSplinesApplicationBase.cxx
               ThinPlateSplinesApplication.cxx main.cxx)
FLTK_WRAP_UI(ThinPlateSplines ThinPlateSplinesApplicationGUI.fl)
ITK_DISABLE_FLTK_GENERATED_WARNINGS(ThinPlateSplinesApplicationGUI.fl)
TARGET_LINK_LIBRARIES(ThinPlateSplines ITKBasicFilters ITKIO
                      ITKFltkImageViewer ITKVtkFltk vtkHybrid)
