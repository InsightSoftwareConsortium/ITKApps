PROJECT( LiverTumorSegmentation )

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

#
#  Define the list of source files
#
INCLUDE_DIRECTORIES(
${ITKApps_SOURCE_DIR}/Auxiliary/FltkImageViewer
${ITKApps_SOURCE_DIR}/Auxiliary/VtkFltk
${ITKApps_SOURCE_DIR}/Auxiliary/vtk
${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
${ITKApps_BINARY_DIR}/Auxiliary/VtkFltk
${LiverTumorSegmentation_SOURCE_DIR}
${LiverTumorSegmentation_BINARY_DIR}
)

SET(LIVER_TUMOR_SEGMENTATION_LIBRARY_SRCS
  ConfidenceConnectedModule.cxx
  ConnectedThresholdModule.cxx
  ThresholdLevelSetSegmentationModule.cxx
)

SET(LIVER_TUMOR_SEGMENTATION_LIBRARY_HEADS
  macros.h
  ConfidenceConnectedModule.h
  ConnectedThresholdModule.h
  ThresholdLevelSetSegmentationModule.h
)


SET(LIVER_TUMOR_SEGMENTATION_EXAMPLE_SRCS
  DicomImageReaderBase.cxx
  ImageSliceViewer.cxx
  InteractorObserver.cxx
  LiverTumorSegmentation.cxx
  LiverTumorSegmentationBase.cxx
  )


SET(LIVER_TUMOR_SEGMENTATION_EXAMPLE_HEADS
  ClickedPointEvent.h
  DicomImageReader.h
  DicomImageReaderBase.h
  ImageSliceViewer.h
  InteractorObserver.h
  LiverTumorSegmentation.h
  LiverTumorSegmentationBase.h
  )

ADD_LIBRARY(LiverTumorSegmentationLibrary
    ${LIVER_TUMOR_SEGMENTATION_LIBRARY_HEADS}
    ${LIVER_TUMOR_SEGMENTATION_LIBRARY_SRCS} )
INSTALL_TARGETS(/lib LiverTumorSegmentationLibrary)


FLTK_WRAP_UI(LiverTumorSegmentationExample
  DicomImageReaderGUI.fl
  LiverTumorSegmentationGUI.fl
  )

ITK_DISABLE_FLTK_GENERATED_WARNINGS(
  "main.cxx;InteractorObserver;LiverTumorSegmentation.cxx;${LiverTumorSegmentationExample_FLTK_UI_SRCS}"
)

ADD_EXECUTABLE(LiverTumorSegmentationExample
    main.cxx
    ${LIVER_TUMOR_SEGMENTATION_EXAMPLE_HEADS}
    ${LIVER_TUMOR_SEGMENTATION_EXAMPLE_SRCS}
    ${LiverTumorSegmentationExample_FLTK_UI_SRCS}
    )
INSTALL_TARGETS(/bin LiverTumorSegmentationExample)

TARGET_LINK_LIBRARIES( LiverTumorSegmentationExample
  LiverTumorSegmentationLibrary
  ITKAlgorithms ITKBasicFilters ITKCommon ITKIO ITKNumerics ITKIO
  ITKFltkImageViewer ITKVtkFltk
  vtkRendering vtkGraphics vtkHybrid vtkImaging vtkIO vtkFiltering vtkCommon
  ${FLTK_LIBRARIES})

ADD_EXECUTABLE(ResampleVolumes ResampleVolumes.cxx )
INSTALL_TARGETS(/bin ResampleVolumes)

TARGET_LINK_LIBRARIES(ResampleVolumes ITKIO ITKCommon ITKNumerics)

