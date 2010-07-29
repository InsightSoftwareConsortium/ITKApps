PROJECT(DistanceMapFilter)

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
${DistanceMapFilter_SOURCE_DIR}
${DistanceMapFilter_BINARY_DIR}
)

SET(DistanceMapFilter_SRCS
DistanceMapFilterBase.cxx
DistanceMapFilter.cxx
main.cxx
)

SET(DistanceMapFilter_GUI_SRCS
DistanceMapFilterGUI.fl
)

FLTK_WRAP_UI(DistanceMapFilter  ${DistanceMapFilter_GUI_SRCS})

SET(DistanceMapFilter_SRCS
${DistanceMapFilter_SRCS}
${DistanceMapFilter_FLTK_UI_SRCS}
)

ADD_GUI_EXECUTABLE(DistanceMapFilter "${DistanceMapFilter_SRCS}")

ITK_DISABLE_FLTK_GENERATED_WARNINGS("main.cxx;DistanceMapFilter.cxx;DistanceMapFilterBase.cxx;${DistanceMapFilter_FLTK_UI_SRCS}")

TARGET_LINK_LIBRARIES (DistanceMapFilter
ITKBasicFilters
ITKIO
ITKFltkImageViewer
)

