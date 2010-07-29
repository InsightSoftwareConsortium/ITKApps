PROJECT(ThinPlateSplines)


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
  ${ITKApps_SOURCE_DIR}/ThinPlateSplines
  ${ITKApps_SOURCE_DIR}/ThinPlateSplines
  ${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
)

FLTK_WRAP_UI(ThinPlateSplines ThinPlateSplinesApplicationGUI.fl)
ITK_DISABLE_FLTK_GENERATED_WARNINGS(ThinPlateSplinesApplicationGUI.fl)

ADD_EXECUTABLE(ThinPlateSplines ThinPlateSplinesApplicationBase.cxx ThinPlateSplinesApplication.cxx main.cxx ${ThinPlateSplines_FLTK_UI_SRCS})
INSTALL_TARGETS(/bin ThinPlateSplines)

TARGET_LINK_LIBRARIES(ThinPlateSplines ITKBasicFilters ITKIO
                      ITKFltkImageViewer ITKVtkFltk vtkHybrid)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES (ThinPlateSplinesApplication.cxx PROPERTIES COMPILE_FLAGS -Wno-cast-qual)
  SET_SOURCE_FILES_PROPERTIES (main.cxx PROPERTIES COMPILE_FLAGS -Wno-cast-qual)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
