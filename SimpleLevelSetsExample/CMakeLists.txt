PROJECT(SimpleLevelSetsExample)


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
ITKBasicFilters
ITKCommon
ITKIO
)

ADD_EXECUTABLE(itk2DSimpleLevelSetsExample itk2DSimpleLevelSetsExample.cxx)
INSTALL_TARGETS(/bin itk2DSimpleLevelSetsExample)
ADD_EXECUTABLE(itk3DSimpleLevelSetsExample itk3DSimpleLevelSetsExample.cxx)
INSTALL_TARGETS(/bin itk3DSimpleLevelSetsExample)


