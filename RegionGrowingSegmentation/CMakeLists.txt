PROJECT( RegionGrowingSegmentation )


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
${RegionGrowingSegmentation_SOURCE_DIR}
${RegionGrowingSegmentation_BINARY_DIR}
)


FLTK_WRAP_UI( RegionGrowingSegmentation   RegionGrowingSegmentationGUI.fl)
FLTK_WRAP_UI( RegionGrowingSegmentation2D RegionGrowingSegmentation2DGUI.fl)


ITK_DISABLE_FLTK_GENERATED_WARNINGS(RegionGrowingSegmentationGUI.fl)
ITK_DISABLE_FLTK_GENERATED_WARNINGS(RegionGrowingSegmentation2DGUI.fl)


SET(RegionGrowingSegmentation2D_SRCS
RegionGrowingSegmentationBase2D.cxx
RegionGrowingSegmentation2D.cxx
${RegionGrowingSegmentation2D_FLTK_UI_SRCS}
)


SET(RegionGrowingSegmentation_SRCS
RegionGrowingSegmentationBase.cxx
RegionGrowingSegmentation.cxx
${RegionGrowingSegmentation_FLTK_UI_SRCS}
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


ADD_GUI_EXECUTABLE(RegionGrowingSegmentation2D "${RegionGrowingSegmentation2D_SRCS}")
ADD_GUI_EXECUTABLE(RegionGrowingSegmentation "${RegionGrowingSegmentation_SRCS}")

