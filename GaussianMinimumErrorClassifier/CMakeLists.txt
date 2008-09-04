PROJECT(GaussianMinimumErrorClassifier)

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
${GaussianMinimumErrorClassifier_SOURCE_DIR}
${GaussianMinimumErrorClassifier_SOURCE_DIR}/../MRIBiasCorrection
)

SET (GaussianMinimumErrorClassifier_SRCS
${GaussianMinimumErrorClassifier_SOURCE_DIR}/../MRIBiasCorrection/OptionList.cxx
GaussianMinimumErrorClassifier.cxx
)

ADD_EXECUTABLE(GaussianMinimumErrorClassifier ${GaussianMinimumErrorClassifier_SRCS})
INSTALL_TARGETS(/bin GaussianMinimumErrorClassifier)
TARGET_LINK_LIBRARIES (GaussianMinimumErrorClassifier
ITKCommon
ITKBasicFilters
ITKIO
)
