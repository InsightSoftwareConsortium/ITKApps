PROJECT( IBSRClassification )

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
${IBSRClassification_SOURCE_DIR}/../Common
${IBSRClassification_SOURCE_DIR}
)

LINK_LIBRARIES (
ITKBasicFilters
ITKIO
)

ADD_EXECUTABLE(GaussianIBSRClassificationApp 
               Code/GaussianClassifierValidationApp.cxx)
ADD_EXECUTABLE(KmeansIBSRClassificationApp 
               Code/KmeansClassifierValidationApp.cxx)
ADD_EXECUTABLE(MRFGaussianIBSRClassificationApp 
               Code/MRFGaussianClassifierValidationApp.cxx)
ADD_EXECUTABLE(MRFKmeansIBSRClassificationApp 
               Code/MRFKmeansClassifierValidationApp.cxx)

