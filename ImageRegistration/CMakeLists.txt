PROJECT( ImageRegistration )

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
${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
${ImageRegistration_SOURCE_DIR}
${ImageRegistration_BINARY_DIR}
)

SET(ImageRegistration_GUI_SRCS
liImageRegistrationConsoleGUI.fl
)

FLTK_WRAP_UI( ImageRegistration ${ImageRegistration_GUI_SRCS} )

SET(ImageRegistration_SRCS
liImageRegistrationConsole.cxx
liImageRegistrationConsoleBase.cxx
ImageRegistration.cxx
${ImageRegistration_FLTK_UI_SRCS}
)
ITK_DISABLE_FLTK_GENERATED_WARNINGS("${ImageRegistration_SRCS}")

ADD_GUI_EXECUTABLE(ImageRegistration "${ImageRegistration_SRCS}")
TARGET_LINK_LIBRARIES (ImageRegistration
  ${ITK_LIBRARIES}
  ITKFltkImageViewer
)
