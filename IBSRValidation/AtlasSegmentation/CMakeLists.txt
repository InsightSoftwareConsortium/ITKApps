PROJECT( AtlasSegmentation )

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


INCLUDE_DIRECTORIES(
${AtlasSegmentation_SOURCE_DIR}/../Common
${AtlasSegmentation_SOURCE_DIR}
)

ADD_EXECUTABLE(BrainStripValidationApp Code/BrainStripValidationApp.cxx)
TARGET_LINK_LIBRARIES(BrainStripValidationApp ITKBasicFilters ITKIO)



