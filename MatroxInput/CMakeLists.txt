PROJECT(MatroxInput)


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


INCLUDE_DIRECTORIES (
  ${MatroxInput_SOURCE_DIR}
  ${ITK_SOURCE_DIR}
  ${ITKApps_SOURCE_DIR}/Auxiliary/FltkImageViewer
  ${ITKApps_SOURCE_DIR}/Auxiliary/VtkFltk
  ${FLTK_INCLUDE_DIR}
  ${VTK_INCLUDE_DIR}
)

LINK_DIRECTORIES(
  ${ITK_LIBRARY_PATH}
  ${VTK_LIBRARY_PATH}
)


LINK_LIBRARIES (
  ITKFltkImageViewer
  ITKVtkFltk
  vtkRendering
  vtkGraphics
  vtkHybrid
  vtkImaging
  vtkIO
  vtkFiltering
  vtkCommon
  ITKCommon
)

SET(MatroxInput_SRCS
  MatroxViewerBase.cxx
  MatroxViewer.cxx
  main.cxx
)

SET(MatroxInput_GUI_SRCS
  MatroxViewerGUI.fl
)


ADD_EXECUTABLE(MatroxInput ${MatroxInput_SRCS})
INSTALL_TARGETS(/bin MatroxInput)


FLTK_WRAP_UI(MatroxInput ${MatroxInput_GUI_SRCS})
ITK_DISABLE_FLTK_GENERATED_WARNINGS("${MatroxInput_GUI_SRCS}")
