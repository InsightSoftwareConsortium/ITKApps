PROJECT( DeformableModelSimplexMesh )

#
#  Define the list of source files
#
INCLUDE_DIRECTORIES(
${DeformableModelSimplexMesh_SOURCE_DIR}
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
  vtkPolyDataToitkMesh.cxx
  itkMeshTovtkPolyData.cxx
  ImageSurfaceViewer.cxx
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

FLTK_WRAP_UI(DeformableModelSimplexMesh
  DeformableModelApplicationGUI.fl
  )

ADD_EXECUTABLE(DeformableModelSimplexMesh ${APPLICATION_SOURCES} ${DeformableModelSimplexMesh_FLTK_UI_SRCS})
INSTALL_TARGETS(/bin DeformableModelSimplexMesh)

INCLUDE_DIRECTORIES(
  ${DeformableModels_SOURCE_DIR}
  ${DeformableModels_BINARY_DIR}
  )


TARGET_LINK_LIBRARIES( DeformableModelSimplexMesh
  ITKAlgorithms ITKBasicFilters ITKCommon ITKIO ITKNumerics ITKIO
  ITKFltkImageViewer ITKVtkFltk
  vtkCommon
  vtkFiltering
  vtkGraphics
  vtkImaging
  vtkIO
  vtkRendering
  ${FLTK_LIBRARIES}
  )

SET(VTK_VERSION_DECIMAL "${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}")

IF(VTK_VERSION_DECIMAL LESS 4.5)
TARGET_LINK_LIBRARIES(DeformableModelSimplexMesh vtkPatented)
ENDIF(VTK_VERSION_DECIMAL LESS 4.5)

  # Stuff was moved from vtkHybrid to vtkWidgets
  # Link against vtkWidgets if it is found, otherwise link against the older
  # library, vtkHybrid .
  IF(VTK_KITS MATCHES WIDGETS)
    TARGET_LINK_LIBRARIES(DeformableModelSimplexMesh vtkWidgets)
  ELSE(VTK_KITS MATCHES WIDGETS)
    TARGET_LINK_LIBRARIES(DeformableModelSimplexMesh vtkHybrid)
  ENDIF(VTK_KITS MATCHES WIDGETS)

