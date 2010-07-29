PROJECT(EdgeDetection)


IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build ITKApps without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )



INCLUDE_DIRECTORIES(${ITKApps_SOURCE_DIR}/EdgeDetection)

#ADD_EXECUTABLE(itk2DEdgeDetectionSample itk2DEdgeDetectionSample.cxx)
ADD_EXECUTABLE(itk3DEdgeDetectionSample itk3DEdgeDetectionSample.cxx)
INSTALL_TARGETS(/bin itk3DEdgeDetectionSample)
TARGET_LINK_LIBRARIES(itk3DEdgeDetectionSample ITKBasicFilters ITKIO)
