PROJECT( StreamedWatershedSegmentation )


IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build ITKApps without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )



SUBDIRS(
ParameterFileParser
VolumeChunker
)

INCLUDE_DIRECTORIES(
${StreamedWatershedSegmentation_SOURCE_DIR}
${StreamedWatershedSegmentation_SOURCE_DIR}/ParameterFileParser
${StreamedWatershedSegmentation_SOURCE_DIR}/VolumeChunker
)


ADD_EXECUTABLE(itkTreeGeneratorApp itkTreeGeneratorApp.cxx)
INSTALL_TARGETS(/bin itkTreeGeneratorApp)
ADD_EXECUTABLE(itkSegmenterApp itkSegmenterApp.cxx)
INSTALL_TARGETS(/bin itkSegmenterApp)
ADD_EXECUTABLE(itkResolverApp itkResolverApp.cxx)
INSTALL_TARGETS(/bin itkResolverApp)
ADD_EXECUTABLE(itkEquivalenceRelabelerApp itkEquivalenceRelabelerApp.cxx)
INSTALL_TARGETS(/bin itkEquivalenceRelabelerApp)
ADD_EXECUTABLE(itkRelabelerApp itkRelabelerApp.cxx)
INSTALL_TARGETS(/bin itkRelabelerApp)


TARGET_LINK_LIBRARIES (itkTreeGeneratorApp ${ITK_LIBRARIES}
 param chunks)

TARGET_LINK_LIBRARIES (itkSegmenterApp ${ITK_LIBRARIES}
 param chunks)

TARGET_LINK_LIBRARIES (itkResolverApp ${ITK_LIBRARIES}
 param chunks)

TARGET_LINK_LIBRARIES (itkEquivalenceRelabelerApp ${ITK_LIBRARIES}
 param chunks)

TARGET_LINK_LIBRARIES (itkRelabelerApp ${ITK_LIBRARIES}
 param chunks)

