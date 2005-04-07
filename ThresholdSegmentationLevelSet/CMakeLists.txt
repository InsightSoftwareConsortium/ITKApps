PROJECT( ThresholdSegmentationLevelSet )


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


INCLUDE_DIRECTORIES(
${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
${InsightApplications_SOURCE_DIR}/Auxiliary/VtkFltk
${InsightApplications_SOURCE_DIR}/Auxiliary/vtk
${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
${InsightApplications_BINARY_DIR}/Auxiliary/VtkFltk
${ThresholdSegmentationLevelSet_SOURCE_DIR}
${ThresholdSegmentationLevelSet_BINARY_DIR}
)


SET(ThresholdSegmentationLevelSet_SRCS
ThresholdSegmentationLevelSetBase.cxx
ThresholdSegmentationLevelSet.cxx
)

SET(ThresholdSegmentationLevelSet_GUI_SRCS
ThresholdSegmentationLevelSetGUI.fl
)

LINK_LIBRARIES (
ITKCommon
ITKNumerics
ITKBasicFilters 
ITKIO
ITKFltkImageViewer
ITKVtkFltk
vtkRendering
vtkGraphics  
vtkHybrid  
vtkImaging
vtkIO
vtkFiltering
vtkCommon
)

IF(VTK_USE_VOLUMERENDERING)
 LINK_LIBRARIES(
 vtkVolumeRendering
)
ENDIF(VTK_USE_VOLUMERENDERING)


ADD_GUI_EXECUTABLE(ThresholdSegmentationLevelSet "${ThresholdSegmentationLevelSet_SRCS}")
FLTK_WRAP_UI( ThresholdSegmentationLevelSet ${ThresholdSegmentationLevelSet_GUI_SRCS} )
ITK_DISABLE_FLTK_GENERATED_WARNINGS("${ThresholdSegmentationLevelSet_SRCS}")
