CMAKE_MINIMUM_REQUIRED(VERSION 1.6)
PROJECT(VolviewPlugIns)

# Don't build anything unless the version of CMake is high enough.
# The matching ELSE/ENDIF should be the last lines in the file.
IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 1.5)


#
#  Find where ITK was Built or Installed.
#
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build VolviewPlugIns without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)



#
#  Find where the Plugin library should be finally copied
#
FIND_PATH( VOLVIEW_BINARY_DIR  VolView.vvf "The binary directory where VolView is built")



#
#  Define where to build the shared library
#
SET (LIBRARY_OUTPUT_PATH ${VolviewPlugIns_BINARY_DIR}/bin CACHE PATH "Single output directory for building all libraries.")



# list all plugins here
SET (PLUGINS 
  vvITKAntiAlias
  vvITKCannyEdgeDetection
  vvITKCannySegmentationLevelSetModule
  vvITKConfidenceConnected
  vvITKConnectedThreshold
  vvITKCurvatureAnisotropicDiffusion
  vvITKCurvatureFlow 
  vvITKDeformableModelModule
  vvITKFastMarching
  vvITKFastMarchingModule
  vvITKGeodesicActiveContour
  vvITKGeodesicActiveContourModule
  vvITKGradientAnisotropicDiffusion
  vvITKGradientMagnitude
  vvITKGradientMagnitudeRecursiveGaussian
  vvITKImageRegistration
  vvITKIntensityWindowing
  vvITKIsolatedConnected
  vvITKMedian
  vvITKSigmoid
  vvITKShapeDetectionModule
  vvITKVectorConfidenceConnected
  vvITKWatershedModule
  )


# copy the plugins to a plugin directory
FOREACH (PLUG ${PLUGINS})
  ADD_LIBRARY( ${PLUG} MODULE ${PLUG}.cxx )
  TARGET_LINK_LIBRARIES( ${PLUG} ITKIO ITKCommon ITKBasicFilters ITKAlgorithms )
  ADD_CUSTOM_COMMAND(
    SOURCE ${PLUG}
    COMMAND ${CMAKE_COMMAND} 
    ARGS -E copy ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_MODULE_PREFIX}${PLUG}${CMAKE_SHARED_MODULE_SUFFIX} ${VOLVIEW_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Plugins/${PLUG}${CMAKE_SHARED_MODULE_SUFFIX}
    TARGET ${PLUG}
    OUTPUTS ${VOLVIEW_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Plugins/${PLUG}${CMAKE_SHARED_MODULE_SUFFIX}
    )
ENDFOREACH (PLUG)




# If the version of CMake was too old, complain and build nothing.
# These should be the last lines in this file.
ELSE("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 1.5)
  MESSAGE(SEND_ERROR
          "This version of CMake is too old to build VolviewPlugIns.  "
          "Please upgrade to CMake 1.6.")
ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 1.5)


