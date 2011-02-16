PROJECT(SimpleLevelSetsExample)


IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build ITKApps without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


ADD_EXECUTABLE(itk2DSimpleLevelSetsExample itk2DSimpleLevelSetsExample.cxx)
INSTALL_TARGETS(/bin itk2DSimpleLevelSetsExample)
TARGET_LINK_LIBRARIES (itk2DSimpleLevelSetsExample ${ITK_LIBRARIES})

ADD_EXECUTABLE(itk3DSimpleLevelSetsExample itk3DSimpleLevelSetsExample.cxx)
INSTALL_TARGETS(/bin itk3DSimpleLevelSetsExample)
TARGET_LINK_LIBRARIES (itk3DSimpleLevelSetsExample ${ITK_LIBRARIES})


