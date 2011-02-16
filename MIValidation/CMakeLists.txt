PROJECT( MIValidation )

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build ITKApps without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

INCLUDE_REGULAR_EXPRESSION(".*")

INCLUDE_DIRECTORIES(
${MIValidation_SOURCE_DIR}/../MultiResMIRegistration/Common
${MIValidation_SOURCE_DIR}/../IBSRValidation/Common
)

SET( MIValidation_SRCS
Code/MIValidationApp.cxx
Code/VanderbiltModality.cxx )

ADD_EXECUTABLE(MIValidationApp   ${MIValidation_SRCS} )
INSTALL_TARGETS(/bin MIValidationApp)
TARGET_LINK_LIBRARIES(MIValidationApp ${ITK_LIBRARIES})

