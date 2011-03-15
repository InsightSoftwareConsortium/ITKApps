PROJECT(MultichannelTissueClassificationValidation)


IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build ITKApps without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )



INCLUDE_DIRECTORIES(
  ${ITKApps_SOURCE_DIR}/MultichannelTissueClassificationValidation/Common
  ${ITKApps_SOURCE_DIR}/MultichannelTissueClassificationValidation/Code
)

ADD_EXECUTABLE(GaussianMSClassificationApp Code/GaussianMSClassifierValidationApp.cxx)
INSTALL_TARGETS(/bin GaussianMSClassificationApp)
ADD_EXECUTABLE(KmeansMSClassificationApp Code/KmeansMSClassifierValidationApp.cxx)
INSTALL_TARGETS(/bin KmeansMSClassificationApp)
#ADD_EXECUTABLE(MRFGaussianIBSRClassificationApp
# Code/MRFGaussianClassifierValidationApp.cxx)
#ADD_EXECUTABLE(MRFKmeansIBSRClassificationApp
# Code/MRFKmeansClassifierValidationApp.cxx)

TARGET_LINK_LIBRARIES(GaussianMSClassificationApp ${ITK_LIBRARIES})
TARGET_LINK_LIBRARIES(KmeansMSClassificationApp ${ITK_LIBRARIES})
