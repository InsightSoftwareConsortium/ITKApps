PROJECT(ITKRegistrationLib)

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


#INCLUDE_REGULAR_EXPRESSION(".*")

INCLUDE_DIRECTORIES(
${ITKRegistrationLib_SOURCE_DIR}/../MultiResMIRegistration/Common
${ITKRegistrationLib_SOURCE_DIR}/../IBSRValidation/Common
)


### HERE:  Modified this line
ADD_LIBRARY(Example_ITKRegistrationLib ITKRegistrationLib.cxx )
INSTALL_TARGETS(/lib Example_ITKRegistrationLib)
TARGET_LINK_LIBRARIES(Example_ITKRegistrationLib ITKNumerics ITKIO ITKBasicFilters ITKCommon )
ADD_EXECUTABLE(Example_ITKRegistrationLibTest ITKRegistrationLibTest.cxx)
INSTALL_TARGETS(/bin Example_ITKRegistrationLibTest)
TARGET_LINK_LIBRARIES(Example_ITKRegistrationLibTest Example_ITKRegistrationLib)
