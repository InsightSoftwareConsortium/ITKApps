PROJECT(Example_ITKFilterLib)

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


LINK_LIBRARIES (
Example_ITKFilterLib
)

LINK_LIBRARIES (
ITKNumerics
ITKCommon
ITKIO
ITKBasicFilters
)


ADD_LIBRARY(Example_ITKFilterLib ITKFilterLib )
INSTALL_TARGETS(/lib Example_ITKFilterLib)
TARGET_LINK_LIBRARIES(Example_ITKFilterLib ITKCommon ITKNumerics ITKIO ITKBasicFilters)
ADD_EXECUTABLE(Example_ITKFilterLibTest ITKFilterLibTest.cxx )
INSTALL_TARGETS(/bin Example_ITKFilterLibTest)
TARGET_LINK_LIBRARIES(Example_ITKFilterLibTest Example_ITKFilterLib)



