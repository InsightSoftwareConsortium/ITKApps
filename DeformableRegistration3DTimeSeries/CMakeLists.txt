PROJECT(DeformableRegistration3DTimeSeries)


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
  ${DeformableRegistration3DTimeSeries_SOURCE_DIR}
  ${DeformableRegistration3DTimeSeries_BINARY_DIR}
  )


FLTK_WRAP_UI(DeformableRegistration3DTimeSeries
    DicomImageReaderGUI.fl
    DeformableRegistration3DTimeSeriesGUI.fl
    )

SET( DeformableRegistration3DTimeSeries_SRCS
    main.cxx
    DeformableRegistration3DTimeSeriesBase.cxx
    DeformableRegistration3DTimeSeries.cxx
    InteractorObserver.cxx
    TransferFunctionEditor.cxx
    DicomImageReaderBase.cxx
    ${DeformableRegistration3DTimeSeries_FLTK_UI_SRCS}
    )

ADD_GUI_EXECUTABLE(
    DeformableRegistration3DTimeSeries
    "${DeformableRegistration3DTimeSeries_SRCS}" )


TARGET_LINK_LIBRARIES(
  DeformableRegistration3DTimeSeries

  ITKFltkImageViewer
  ITKVtkFltk

  ${ITK_LIBRARIES}

  vtkCommon
  vtkFiltering
  vtkGraphics
  vtkImaging
  vtkIO
  vtkRendering
  vtkHybrid
  vtkVolumeRendering


  ${FLTK_LIBRARIES}
  )

ITK_DISABLE_FLTK_GENERATED_WARNINGS("${DeformableRegistration3DTimeSeries_SRCS}")

