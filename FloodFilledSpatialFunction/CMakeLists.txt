PROJECT(FloodFilledSpatialFunction)

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build ITKApps without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


INCLUDE_DIRECTORIES(
${FloodFilledSpatialFunction_SOURCE_DIR}
)


ADD_EXECUTABLE(itkFloodFilledSpatialFunctionExample itkFloodFilledSpatialFunctionExample.cxx)
INSTALL_TARGETS(/bin itkFloodFilledSpatialFunctionExample)
TARGET_LINK_LIBRARIES(itkFloodFilledSpatialFunctionExample ${ITK_LIBRARIES})
