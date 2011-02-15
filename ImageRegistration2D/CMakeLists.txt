PROJECT(ImageRegistration2D)


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
  ${ITKApps_SOURCE_DIR}/Auxiliary/vtk
  ${ITKApps_BINARY_DIR}/Auxiliary/vtk
  ${ITKApps_SOURCE_DIR}/ImageRegistration2D
  ${ITKApps_BINARY_DIR}/ImageRegistration2D
)

FLTK_WRAP_UI(ImageRegistration2D ImageRegistrationConsoleGUI.fl)
ITK_DISABLE_FLTK_GENERATED_WARNINGS(ImageRegistrationConsoleGUI.fl)

SET(ImageRegistration2D_SRCS
  ImageRegistrationConsole.cxx
  ImageRegistrationConsoleBase.cxx
  ImageRegistration2D.cxx
  ${ImageRegistration2D_FLTK_UI_SRCS}
  )

ADD_GUI_EXECUTABLE(ImageRegistration2D "${ImageRegistration2D_SRCS}")

TARGET_LINK_LIBRARIES(ImageRegistration2D ITKFltkImageViewer
                      ${ITK_LIBRARIES}
                      vtkRendering vtkGraphics vtkHybrid
                      vtkImaging vtkIO vtkFiltering vtkCommon
)
