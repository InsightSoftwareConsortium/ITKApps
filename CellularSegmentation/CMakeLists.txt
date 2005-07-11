PROJECT( CellularSegmentation )

#
#  Define the list of source files
#
INCLUDE_DIRECTORIES(
${CellularSegmentationSimplexMesh_SOURCE_DIR}
${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
${InsightApplications_SOURCE_DIR}/Auxiliary/VtkFltk
${InsightApplications_SOURCE_DIR}/Auxiliary/vtk
${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
${InsightApplications_BINARY_DIR}/Auxiliary/VtkFltk
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
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
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



ADD_EXECUTABLE( CellularSegmentation ${APPLICATION_SOURCES})

FLTK_WRAP_UI(CellularSegmentation 
  CellularSegmentationApplicationGUI.fl 
  )


INCLUDE_DIRECTORIES(
  ${CellularSegmentation_SOURCE_DIR}
  ${CellularSegmentation_BINARY_DIR}
  )


TARGET_LINK_LIBRARIES( CellularSegmentation 
  ITKAlgorithms ITKBasicFilters ITKCommon ITKIO ITKNumerics ITKIO 
  ITKFltkImageViewer ITKVtkFltk
  vtkCommon
  vtkFiltering
  vtkGraphics
  vtkImaging
  vtkIO
  vtkHybrid 
  vtkRendering
  ${FLTK_LIBRARIES}
  )


