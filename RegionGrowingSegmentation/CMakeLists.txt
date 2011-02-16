PROJECT( RegionGrowingSegmentation )


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


INCLUDE_DIRECTORIES(
${ITKApps_SOURCE_DIR}/Auxiliary/FltkImageViewer
${ITKApps_SOURCE_DIR}/Auxiliary/VtkFltk
${ITKApps_SOURCE_DIR}/Auxiliary/vtk
${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
${ITKApps_BINARY_DIR}/Auxiliary/VtkFltk
${RegionGrowingSegmentation_SOURCE_DIR}
${RegionGrowingSegmentation_BINARY_DIR}
)


FLTK_WRAP_UI( RegionGrowingSegmentation   RegionGrowingSegmentationGUI.fl)
FLTK_WRAP_UI( RegionGrowingSegmentation2D RegionGrowingSegmentation2DGUI.fl)


ITK_DISABLE_FLTK_GENERATED_WARNINGS(RegionGrowingSegmentationGUI.cxx)
ITK_DISABLE_FLTK_GENERATED_WARNINGS(RegionGrowingSegmentation2DGUI.cxx)


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


ADD_GUI_EXECUTABLE(RegionGrowingSegmentation2D "${RegionGrowingSegmentation2D_SRCS}")
TARGET_LINK_LIBRARIES (RegionGrowingSegmentation2D
  ${ITK_LIBRARIES}
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

ADD_GUI_EXECUTABLE(RegionGrowingSegmentation "${RegionGrowingSegmentation_SRCS}")
TARGET_LINK_LIBRARIES (RegionGrowingSegmentation
  ${ITK_LIBRARIES}
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

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES (RegionGrowingSegmentation.cxx PROPERTIES COMPILE_FLAGS -Wno-cast-qual)
  SET_SOURCE_FILES_PROPERTIES (RegionGrowingSegmentation2D.cxx PROPERTIES COMPILE_FLAGS -Wno-cast-qual)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)



