PROJECT(EllipsoidInteriorExteriorSpatialFunction)

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
${ITK_SOURCE_DIR}/Code/Common
${ITK_SOURCE_DIR}/Code/IO
${ITK_SOURCE_DIR}/Code/BasicFilters
${EllipsoidInteriorExteriorSpatialFunction_SOURCE_DIR}
)

SET(ELLIPSOID_SPATIAL_FUNCTION_SRCS
itkEllipsoidInteriorExteriorSpatialFunctionExample.cxx
)

LINK_LIBRARIES (
ITKCommon
ITKIO
ITKBasicFilters
)


IF(UNIX)
  LINK_LIBRARIES (
  -lm
  )
ENDIF(UNIX)

ADD_EXECUTABLE(itkEllipsoidInteriorExteriorSpatialFunctionExample ${ELLIPSOID_SPATIAL_FUNCTION_SRCS})
INSTALL_TARGETS(/bin itkEllipsoidInteriorExteriorSpatialFunctionExample)

