CMAKE_MINIMUM_REQUIRED(VERSION 1.6)
PROJECT(ValidationPlugins)

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
#FIND_PATH( VOLVIEW_BINARY_DIR  VolView.vvf "The binary directory where VolView is built")



#
#  Define where to build the shared library
#
#SET (LIBRARY_OUTPUT_PATH ${VolviewPlugIns_BINARY_DIR}/bin CACHE PATH "Single output directory for building #all libraries.")


# list all plugins here
SET (PLUGINS
  vvITKVolume
  vvITKAntiAliasingVoxelCount
  vvITKHausdorffDistance
  vvITKContourMeanDistance
  vvITKVolumeOverlap
  )


IF( WIN32 )
  SET(LIBRARY_BUILD_DIR ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR})
#  SET(LIBRARY_DESTINATION_DIR ${VOLVIEW_BINARY_DIR}/${CMAKE_CFG_INTDIR}/Plugins)
ENDIF( WIN32 )

IF( UNIX )
  SET(LIBRARY_BUILD_DIR ${LIBRARY_OUTPUT_PATH})
#  SET(LIBRARY_DESTINATION_DIR ${VOLVIEW_BINARY_DIR}/Plugins)
ENDIF( UNIX )


# copy the plugins to a plugin directory
FOREACH (PLUG ${PLUGINS})
  ADD_LIBRARY(${PLUG}  MODULE ${PLUG}.cxx       )
  INSTALL_TARGETS(/lib ${PLUG})
  TARGET_LINK_LIBRARIES( ${PLUG} ${ITK_LIBRARIES} )
#  ADD_CUSTOM_COMMAND(
#    SOURCE ${PLUG}
#    COMMAND ${CMAKE_COMMAND}
#    ARGS -E copy ${LIBRARY_BUILD_DIR}/${CMAKE_SHARED_MODULE_PREFIX}${PLUG}${CMAKE_SHARED_MODULE_SUFFIX} #${LIBRARY_DESTINATION_DIR}/${PLUG}${CMAKE_SHARED_MODULE_SUFFIX}
#    TARGET ${PLUG}
#    OUTPUTS ${LIBRARY_DESTINATION_DIR}/${PLUG}${CMAKE_SHARED_MODULE_SUFFIX}
#  )
ENDFOREACH (PLUG)




# If the version of CMake was too old, complain and build nothing.
# These should be the last lines in this file.
ELSE("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 1.5)
  MESSAGE(SEND_ERROR
          "This version of CMake is too old to build VolviewPlugIns.  "
          "Please upgrade to CMake 1.6.")
ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 1.5)

