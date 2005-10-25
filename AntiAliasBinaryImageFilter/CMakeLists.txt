PROJECT(AntiAliasBinaryImageFilter)


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
${AntiAliasBinaryImageFilter_SOURCE_DIR}
${AntiAliasBinaryImageFilter_SOURCE_DIR}/../StreamedWatershedSegmentation/ParameterFileParser
)

LINK_LIBRARIES (
  param
  ITKAlgorithms
  ITKBasicFilters
  ITKCommon
  ITKIO
  )

ADD_EXECUTABLE(itkAntiAliasBinaryImageFilterExample itkAntiAliasBinaryImageFilterExample.cxx)
INSTALL_TARGETS(/bin itkAntiAliasBinaryImageFilterExample)
ADD_EXECUTABLE(itkSampleDataGenerator itkSampleDataGenerator.cxx)
INSTALL_TARGETS(/bin itkSampleDataGenerator)

