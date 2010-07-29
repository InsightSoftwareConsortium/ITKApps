PROJECT(ImageColorViewer)


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

INCLUDE_DIRECTORIES (
  ${ITKApps_SOURCE_DIR}/Auxiliary/FltkImageViewer
)

SET(ImageColorViewer_GUI_SRCS
  ImageColorViewerGUI.fl
  )

FLTK_WRAP_UI( ImageColorViewer ${ImageColorViewer_GUI_SRCS} )
ITK_DISABLE_FLTK_GENERATED_WARNINGS("${ImageColorViewer_GUI_SRCS}")

SET ( ImageColorViewer_SRCS
  ImageColorViewer.cxx
  ${ImageColorViewer_FLTK_UI_SRCS}
  )

ADD_GUI_EXECUTABLE( ImageColorViewer "${ImageColorViewer_SRCS}" )

TARGET_LINK_LIBRARIES (ImageColorViewer
  ITKBasicFilters
  ITKFltkImageViewer
  ITKIO
)
