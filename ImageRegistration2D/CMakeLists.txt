PROJECT(ImageRegistration2D)


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
  ${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_SOURCE_DIR}/ImageRegistration2D
  ${InsightApplications_BINARY_DIR}/ImageRegistration2D
)

SET(ImageRegistration2D_SRCS 
  ImageRegistrationConsole.cxx
  ImageRegistrationConsoleBase.cxx 
  ImageRegistration2D.cxx
  )

ADD_GUI_EXECUTABLE(ImageRegistration2D "${ImageRegistration2D_SRCS}")

FLTK_WRAP_UI(ImageRegistration2D ImageRegistrationConsoleGUI.fl)
ITK_DISABLE_FLTK_GENERATED_WARNINGS(ImageRegistrationConsoleGUI.fl)
TARGET_LINK_LIBRARIES(ImageRegistration2D ITKFltkImageViewer ITKIO
                      ITKNumerics ITKBasicFilters)
