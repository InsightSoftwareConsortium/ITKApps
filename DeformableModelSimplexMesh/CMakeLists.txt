PROJECT( DeformableModelSimplexMesh )

#
#  Define the list of source files
#
INCLUDE_DIRECTORIES(
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
  DeformableModelApplicationBase.cxx
  DeformableModelApplication.cxx
  ImageSliceViewer.cxx
  ImageSurfaceViewer.cxx
  InteractorObserver.cxx
  main.cxx
  )



IF( BUILD_OUTSIDE_INSIGHT_APPLICATION )

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

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATION )



ADD_EXECUTABLE( DeformableModelSimplexMesh ${APPLICATION_SOURCES})

FLTK_WRAP_UI(DeformableModelSimplexMesh 
  DeformableModelApplicationGUI.fl 
  )


INCLUDE_DIRECTORIES(
  ${DeformableModels_SOURCE_DIR}
  ${DeformableModels_BINARY_DIR}
  )


TARGET_LINK_LIBRARIES( DeformableModelSimplexMesh 
	ITKAlgorithms ITKBasicFilters ITKCommon ITKIO ITKNumerics ITKIO 
	ITKFltkImageViewer ITKVtkFltk
	vtkRendering vtkGraphics vtkHybrid vtkImaging vtkIO vtkFiltering vtkCommon vtkPatented 
	${FLTK_LIBRARIES}
  )


