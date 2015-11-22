PROJECT( CellularSegmentation )

#
#  Define the list of source files
#
INCLUDE_DIRECTORIES(
${CellularSegmentationSimplexMesh_SOURCE_DIR}
${ITKApps_SOURCE_DIR}/Auxiliary/FltkImageViewer
${ITKApps_SOURCE_DIR}/Auxiliary/VtkFltk
${ITKApps_SOURCE_DIR}/Auxiliary/vtk
${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
${ITKApps_BINARY_DIR}/Auxiliary/VtkFltk
)
#
#  Define the list of source files
#
SET(APPLICATION_SOURCES
  CellularSegmentationApplicationBase.cxx
  CellularSegmentationApplication.cxx
  CellularAggregateViewer.cxx
  ImageSliceViewer.cxx
  InteractorObserver.cxx
  main.cxx
  )



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

FLTK_WRAP_UI(CellularSegmentation
  CellularSegmentationApplicationGUI.fl
  )

ITK_DISABLE_FLTK_GENERATED_WARNINGS("${APPLICATION_SOURCES} ${CellularSegmentation_FLTK_UI_SRCS}")

ADD_EXECUTABLE(CellularSegmentation
    ${APPLICATION_SOURCES} ${CellularSegmentation_FLTK_UI_SRCS})
INSTALL_TARGETS(/bin CellularSegmentation)


INCLUDE_DIRECTORIES(
  ${CellularSegmentation_SOURCE_DIR}
  ${CellularSegmentation_BINARY_DIR}
  )


TARGET_LINK_LIBRARIES( CellularSegmentation
  ${ITK_LIBRARIES}
  ITKFltkImageViewer ITKVtkFltk
  ${VTK_LIBRARIES}
  ${FLTK_LIBRARIES}
  )


