PROJECT( SymmetricEllipsoidInteriorExteriorSpatialFunctionExample )

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build ITKApps without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


ADD_EXECUTABLE(SymmetricEllipsoidInteriorExteriorSpatialFunctionExample 
  itkSymmetricEllipsoidInteriorExteriorSpatialFunctionExample.cxx)

TARGET_LINK_LIBRARIES(SymmetricEllipsoidInteriorExteriorSpatialFunctionExample
  ${ITK_LIBRARIES})

INSTALL_TARGETS(/bin SymmetricEllipsoidInteriorExteriorSpatialFunctionExample)

