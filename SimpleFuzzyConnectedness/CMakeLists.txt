PROJECT (SimpleFuzzyConnectedness)


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
${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
${SimpleFuzzyConnectedness_SOURCE_DIR}
)


FLTK_WRAP_UI(SimpleFuzzyConnectedness FuzzySegGrayUI.fl)

SET(Project_SRCS
FuzzySegMain.cpp
FuzzySegGrayRun.cxx
${SimpleFuzzyConnectedness_FLTK_UI_SRCS}
)

ITK_DISABLE_FLTK_GENERATED_WARNINGS("${Project_SRCS}")


ADD_GUI_EXECUTABLE(SimpleFuzzyConnectedness   "${Project_SRCS}")
TARGET_LINK_LIBRARIES (SimpleFuzzyConnectedness
  ${ITK_LIBRARIES}
  ITKFltkImageViewer
)

