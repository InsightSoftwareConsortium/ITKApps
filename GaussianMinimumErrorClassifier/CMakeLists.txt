PROJECT(GaussianMinimumErrorClassifier)

INCLUDE_DIRECTORIES(
${GaussianMinimumErrorClassifier_SOURCE_DIR}
${GaussianMinimumErrorClassifier_SOURCE_DIR}/../MRIBiasCorrection
)

LINK_LIBRARIES (
ITKCommon
ITKBasicFilters
ITKIO
)

SET (GaussianMinimumErrorClassifier_SRCS
${GaussianMinimumErrorClassifier_SOURCE_DIR}/../MRIBiasCorrection/OptionList.cxx
GaussianMinimumErrorClassifier.cxx
)

ADD_EXECUTABLE(GaussianMinimumErrorClassifier ${GaussianMinimumErrorClassifier_SRCS})
