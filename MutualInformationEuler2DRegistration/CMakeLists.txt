PROJECT( MutualInformationEuler2DRegistration )


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



INCLUDE_REGULAR_EXPRESSION(".*")

INCLUDE_DIRECTORIES(
${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
${MutualInformationEuler2DRegistration_SOURCE_DIR}
${MutualInformationEuler2DRegistration_BINARY_DIR}
)

FLTK_WRAP_UI(MutualInformationEuler2DRegistration registerGUI.fl)
ITK_DISABLE_FLTK_GENERATED_WARNINGS(registerGUI.fl)

SET(MutualInformationEuler2DRegistration_SRCS
  RegisterApplication.cxx
  RegisterWindow.cxx
  register.cxx
  ${MutualInformationEuler2DRegistration_FLTK_UI_SRCS}
)

ADD_GUI_EXECUTABLE(MutualInformationEuler2DRegistration
  "${MutualInformationEuler2DRegistration_SRCS}")

TARGET_LINK_LIBRARIES(MutualInformationEuler2DRegistration
                      ITKNumerics ITKBasicFilters ITKIO ITKFltkImageViewer)


