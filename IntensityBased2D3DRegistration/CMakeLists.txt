PROJECT(IntensityBased2D3DRegistration)

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

#ADD_DEFINITIONS(-Wno-non-template-friend -g)
#SET(CMAKE_VERBOSE_MAKEFILE ON)

ADD_EXECUTABLE(IntensityBased2D3DRegistration IntensityBased2D3DRegistration.cxx)
TARGET_LINK_LIBRARIES( IntensityBased2D3DRegistration ITKNumerics ITKIO )

ADD_EXECUTABLE(GenerateProjection GenerateProjection.cxx)
TARGET_LINK_LIBRARIES( GenerateProjection ITKNumerics ITKIO )
