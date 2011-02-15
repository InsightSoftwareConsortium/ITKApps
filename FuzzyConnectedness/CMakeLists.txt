PROJECT(FuzzyConnectedness)


IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build ITKApps without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )



INCLUDE_DIRECTORIES(${ITKApps_SOURCE_DIR}/FuzzyConnectedness
                    ${ITKApps_SOURCE_DIR}/IBSRValidation/Common)

ADD_EXECUTABLE(FuzzyConnectedness FuzzyConnectApp.cxx FuzzyConnectedness.cxx)
INSTALL_TARGETS(/bin FuzzyConnectedness)

TARGET_LINK_LIBRARIES(FuzzyConnectedness ${ITK_LIBRARIES})
