PROJECT(IntensityBased2D3DRegistration)

# Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

ADD_DEFINITIONS(-Wno-non-template-friend -g)

SET(CMAKE_VERBOSE_MAKEFILE ON)

ADD_EXECUTABLE(IntensityBased2D3DRegistration IntensityBased2D3DRegistration.cxx)
TARGET_LINK_LIBRARIES( IntensityBased2D3DRegistration ITKNumerics ITKIO )
