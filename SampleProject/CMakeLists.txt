PROJECT(SampleProject)

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

# Add the executable with our source file.
ADD_EXECUTABLE(itkSampleProject itkSampleProject.cxx)
INSTALL_TARGETS(/bin itkSampleProject)

# We only need ITK's Common components.  Link to the common library.
TARGET_LINK_LIBRARIES(itkSampleProject ITKCommon)
