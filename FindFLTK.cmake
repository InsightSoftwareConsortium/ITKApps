#
# Find the native FLTK includes and library
#
# The following settings are defined
# FLTK_FLUID_EXECUTABLE, where to find the Fluid tool
# FLTK_WRAP_UI, This allows the FLTK_WRAP_UI command to work.
# FLTK_INCLUDE_DIR, where to find include files
# FLTK_LIBRARIES, list of fltk libraries
# FLTK_FOUND, Don't use FLTK if false.

# The following settings should not be used in general.
# FLTK_BASE_LIBRARY    = the full path to fltk.lib
# FLTK_GL_LIBRARY      = the full path to fltk_gl.lib
# FLTK_FORMS_LIBRARY   = the full path to fltk_forms.lib
# FLTK_IMAGES_LIBRARY  = the full path to fltk_images.lib
# If FLTK_INCLUDE_DIR is already defined, then avoid checking for CMake builds of FLTK
IF(FLTK_INCLUDE_DIRS)
    SET(FLTK_BUILT_WITH_CMAKE 0)
    SET(FLTK_CHECKFOR_CMAKE 0)
ELSE(FLTK_INCLUDE_DIRS)
    SET(FLTK_CHECKFOR_CMAKE 1)
ENDIF(FLTK_INCLUDE_DIRS)

IF(FLTK_CHECKFOR_CMAKE)
# Search only if the location is not already known.
  # Get the system search path as a list.
  IF(UNIX)
    STRING(REGEX MATCHALL "[^:]+" FLTK_DIR_SEARCH1 "$ENV{PATH}")
  ELSE(UNIX)
    STRING(REGEX REPLACE "\\\\" "/" FLTK_DIR_SEARCH1 "$ENV{PATH}")
  ENDIF(UNIX)
  STRING(REGEX REPLACE "/;" ";" FLTK_DIR_SEARCH2 ${FLTK_DIR_SEARCH1})

  # Construct a set of paths relative to the system search path.
  SET(FLTK_DIR_SEARCH "")
  FOREACH(dir ${FLTK_DIR_SEARCH2})
    SET(FLTK_DIR_SEARCH ${FLTK_DIR_SEARCH} "${dir}/../lib/fltk")
  ENDFOREACH(dir)

  #
  # Look for an installation or build tree.
  #
  FIND_PATH(FLTK_DIR FLTKConfig.cmake
    # Look for an environment variable FLTK_DIR.
    $ENV{FLTK_DIR}

    # Look in places relative to the system executable search path.
    ${FLTK_DIR_SEARCH}

    # Look in standard UNIX install locations.
    /usr/local/lib/fltk
    /usr/lib/fltk
    /usr/local/include
    /usr/include
    /usr/local/fltk
    /usr/X11R6/include

    # Read from the CMakeSetup registry entries.  It is likely that
    # FLTK will have been recently built.
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild1]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild2]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild3]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild4]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild5]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild6]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild7]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild8]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild9]
    [HKEY_CURRENT_USER\\Software\\Kitware\\CMakeSetup\\Settings\\StartPath;WhereBuild10]

    # Help the user find it if we cannot.
    DOC "The directory containing FLTKConfig.cmake.  This is either the root of the build tree, or PREFIX/lib/fltk for an installation."
    )
    # If FLTK has been built using CMake we try to find everything directly
  # Check if FLTK was built using CMake
  IF(EXISTS ${FLTK_DIR}/FLTKConfig.cmake)
    SET(FLTK_BUILT_WITH_CMAKE 1)
    SET(FLTK_FOUND 1)
    INCLUDE(${FLTK_DIR}/FLTKConfig.cmake)
    SET(USE_FLTK_FILE ${FLTK_DIR}/FLTKConfig.cmake)

    # Fluid
    IF(FLUID_COMMAND)
      SET(FLTK_FLUID_EXECUTABLE ${FLUID_COMMAND} CACHE FILEPATH "Fluid executable")
    ELSE(FLUID_COMMAND)
      FIND_PROGRAM(FLTK_FLUID_EXECUTABLE fluid PATHS
        ${FLTK_EXECUTABLE_DIRS}
        ${FLTK_EXECUTABLE_DIRS}/RelWithDebInfo
        ${FLTK_EXECUTABLE_DIRS}/Debug
        ${FLTK_EXECUTABLE_DIRS}/Release
        NO_SYSTEM_PATH)
    ENDIF(FLUID_COMMAND)
    # MARK_AS_ADVANCED(FLTK_FLUID_EXECUTABLE)

    #Create alias between FLTK_INCLUDE_DIRS and FLTK_INCLUDE_DIR for backward compatibiltiy
    IF( FLTK_INCLUDE_DIRS AND NOT FLTK_INCLUDE_DIR )
      SET(FLTK_INCLUDE_DIR ${FLTK_INCLUDE_DIRS})
    ELSE(FLTK_INCLUDE_DIRS AND NOT FLTK_INCLUDE_DIR )
      # Use location of fluid to help find everything else.
      SET(FLTK_INCLUDE_SEARCH_PATH "")
      IF(FLTK_FLUID_EXECUTABLE)
        GET_FILENAME_COMPONENT(FLTK_BIN_DIR "${FLTK_FLUID_EXECUTABLE}" PATH)
        SET(FLTK_INCLUDE_SEARCH_PATH ${FLTK_INCLUDE_SEARCH_PATH}
          ${FLTK_BIN_DIR}/../include ${FLTK_BIN_DIR}/..
          ${FLTK_BIN_DIR}/../../include ${FLTK_BIN_DIR}/../../ )
      ENDIF(FLTK_FLUID_EXECUTABLE)

      FIND_PATH(FLTK_INCLUDE_DIR FL/Fl.H ${FLTK_INCLUDE_SEARCH_PATH})
    ENDIF(FLTK_INCLUDE_DIRS AND NOT FLTK_INCLUDE_DIR )
    IF( FLTK_LIBRARY_DIRS)
      LINK_DIRECTORIES(${FLTK_LIBRARY_DIRS})
      SET(FLTK_LIBRARY_SEARCH_PATH ${FLTK_LIBRARY_DIRS})
    ENDIF( FLTK_LIBRARY_DIRS)

    ### Versions of fltk built with cmake less than 1.1.6 require FLTK_DIR to point to the
    ### FLTK_BINRARY_DIR and not the CMAKE_PREFIX installed location.
    ### The FLTK_BINARY_DIR contains a Cache file that has extra variables needed to link
    ### fltk packages to.
    ### Versions of FLTK greather than or equal 1.1.7 have all the necessary information properly defined in FLTKConfig.cmake (including dependancies).
    IF(NOT FLTK_VERSION_PATCH GREATER 6) ##ONLY NEEDED FOR FLTK <= 1.1.6

      
    SET(FLTK_BASE_LIBRARY "fltk" CACHE INTERNAL "FLTK_BASE_LIBRARY var")
    SET(FLTK_GL_LIBRARY "fltk_gl" CACHE INTERNAL "FLTK_GL_LIBRARY var")
    SET(FLTK_FORMS_LIBRARY "fltk_forms" CACHE INTERNAL "FLTK_FORMS_LIBRARY var")
    SET(FLTK_IMAGES_LIBRARY "fltk_images" CACHE INTERNAL "FLTK_IMAGES_LIBRARY var")

       # Add the extra libraries
       LOAD_CACHE(${FLTK_DIR}
        READ_WITH_PREFIX
        FL FLTK_USE_SYSTEM_JPEG
        FL FLTK_USE_SYSTEM_PNG
        FL FLTK_USE_SYSTEM_ZLIB
      )

      SET(FLTK_IMAGES_LIBS "")
      IF(NOT FLFLTK_USE_SYSTEM_JPEG)
        SET(FLTK_IMAGES_LIBS ${FLTK_IMAGES_LIBS} fltk_jpeg)
      ENDIF(NOT FLFLTK_USE_SYSTEM_JPEG)
      IF(NOT FLFLTK_USE_SYSTEM_PNG)
        SET(FLTK_IMAGES_LIBS ${FLTK_IMAGES_LIBS} fltk_png)
      ENDIF(NOT FLFLTK_USE_SYSTEM_PNG)
      IF(NOT FLFLTK_USE_SYSTEM_ZLIB)
        SET(FLTK_IMAGES_LIBS ${FLTK_IMAGES_LIBS} fltk_zlib)
      ENDIF(NOT FLFLTK_USE_SYSTEM_ZLIB)
      SET(FLTK_IMAGES_LIBS "${FLTK_IMAGES_LIBS}" CACHE INTERNAL "Extra libraries for fltk_images library.")
    ENDIF(NOT FLTK_VERSION_PATCH GREATER 6) ##ONLY NEEDED FOR FLTK <= 1.1.6

  ENDIF(EXISTS ${FLTK_DIR}/FLTKConfig.cmake)

ENDIF(FLTK_CHECKFOR_CMAKE)



##
##
##  If a CMake version of FLTK was not found, then use a fall back strategy
##
##
# If CMake build of FLTK was found, load the configuration file to get the rest of the
# settings.
IF(NOT FLTK_BUILT_WITH_CMAKE)
  # if FLTK was not built using CMake
  # Find fluid executable.
  FIND_PROGRAM(FLTK_FLUID_EXECUTABLE fluid ${FLTK_INCLUDE_DIR}/fluid ${FLTK_INCLUDE_DIR}
      /usr/local/bin
      /usr/bin
      /usr/local/fltk/bin
      /usr/X11R6/bin
      )

  # Use location of fluid to help find everything else.
  SET(FLTK_INCLUDE_SEARCH_PATH "")
  SET(FLTK_LIBRARY_SEARCH_PATH "")
  IF(FLTK_FLUID_EXECUTABLE)
    GET_FILENAME_COMPONENT(FLTK_BIN_DIR "${FLTK_FLUID_EXECUTABLE}" PATH)
    SET(FLTK_INCLUDE_SEARCH_PATH ${FLTK_INCLUDE_SEARCH_PATH}
      ${FLTK_BIN_DIR}/../include ${FLTK_BIN_DIR}/..)
    SET(FLTK_LIBRARY_SEARCH_PATH ${FLTK_LIBRARY_SEARCH_PATH}
      ${FLTK_BIN_DIR}/../lib)
    SET(FLTK_WRAP_UI 1)
  ENDIF(FLTK_FLUID_EXECUTABLE)

  SET(FLTK_INCLUDE_SEARCH_PATH ${FLTK_INCLUDE_SEARCH_PATH}
    /usr/local/include
    /usr/include
    /usr/local/fltk
    /usr/X11R6/include
    )

  FIND_PATH(FLTK_INCLUDE_DIR FL/Fl.H ${FLTK_INCLUDE_SEARCH_PATH})

  SET(FLTK_LIBRARY_SEARCH_PATH ${FLTK_LIBRARY_SEARCH_PATH}
    /usr/lib
    /usr/local/lib
    /usr/local/fltk/lib
    /usr/X11R6/lib
    ${FLTK_INCLUDE_DIR}/lib
    )

  FIND_LIBRARY(FLTK_BASE_LIBRARY NAMES fltk fltkd
    PATHS ${FLTK_LIBRARY_SEARCH_PATH})
  FIND_LIBRARY(FLTK_GL_LIBRARY NAMES fltkgl fltkgld fltk_gl
    PATHS ${FLTK_LIBRARY_SEARCH_PATH})
  FIND_LIBRARY(FLTK_FORMS_LIBRARY NAMES fltkforms fltkformsd fltk_forms
    PATHS ${FLTK_LIBRARY_SEARCH_PATH})
  FIND_LIBRARY(FLTK_IMAGES_LIBRARY NAMES fltkimages fltkimagesd fltk_images
    PATHS ${FLTK_LIBRARY_SEARCH_PATH})

  # Find the extra libraries needed for the fltk_images library.
  IF(UNIX)
    FIND_PROGRAM(FLTK_CONFIG_SCRIPT fltk-config PATHS ${FLTK_BIN_DIR})
    IF(FLTK_CONFIG_SCRIPT)
      EXEC_PROGRAM(${FLTK_CONFIG_SCRIPT} ARGS --use-images --ldflags
        OUTPUT_VARIABLE FLTK_IMAGES_LDFLAGS)
      SET(FLTK_LIBS_EXTRACT_REGEX ".*-lfltk_images (.*) -lfltk.*")
      IF("${FLTK_IMAGES_LDFLAGS}" MATCHES "${FLTK_LIBS_EXTRACT_REGEX}")
        STRING(REGEX REPLACE "${FLTK_LIBS_EXTRACT_REGEX}" "\\1"
          FLTK_IMAGES_LIBS "${FLTK_IMAGES_LDFLAGS}")
        STRING(REGEX REPLACE " +" ";" FLTK_IMAGES_LIBS "${FLTK_IMAGES_LIBS}")
        # The EXEC_PROGRAM will not be inherited into subdirectories from

        # the file that originally included this module.  Save the answer.
        SET(FLTK_IMAGES_LIBS "${FLTK_IMAGES_LIBS}" CACHE INTERNAL
          "Extra libraries for fltk_images library.")
      ENDIF("${FLTK_IMAGES_LDFLAGS}" MATCHES "${FLTK_LIBS_EXTRACT_REGEX}")
    ENDIF(FLTK_CONFIG_SCRIPT)
  ENDIF(UNIX)



  SET(FLTK_FOUND 1)
  FOREACH(var FLTK_FLUID_EXECUTABLE FLTK_INCLUDE_DIR
      FLTK_BASE_LIBRARY FLTK_GL_LIBRARY
      FLTK_FORMS_LIBRARY FLTK_IMAGES_LIBRARY)
    IF(NOT ${var})
      SET(FLTK_FOUND 0)
      MESSAGE(FATAL "FLTK Missing required component ${var}")
    ENDIF(NOT ${var})
  ENDFOREACH(var)
ENDIF(NOT FLTK_BUILT_WITH_CMAKE)


IF(NOT FLTK_VERSION_PATCH GREATER 6) ##ONLY NEEDED FOR FLTK <= 1.1.6
  IF(FLTK_FOUND)
    # Platform dependent libraries required by FLTK
    IF(WIN32)
      IF(NOT CYGWIN)
        IF(BORLAND)
          SET( FLTK_PLATFORM_DEPENDENT_LIBS import32 )
        ELSE(BORLAND)
          SET( FLTK_PLATFORM_DEPENDENT_LIBS wsock32 comctl32 )
        ENDIF(BORLAND)
      ENDIF(NOT CYGWIN)
    ENDIF(WIN32)

    IF(UNIX OR FLTK_USE_X)
      INCLUDE(${CMAKE_ROOT}/Modules/FindX11.cmake)
      SET( FLTK_PLATFORM_DEPENDENT_LIBS ${X11_LIBRARIES} -lm)
    ENDIF(UNIX OR FLTK_USE_X)

    IF(APPLE)
        IF(FLTK_USE_X)
    #        SET( FLTK_PLATFORM_DEPENDENT_LIBS  "-framework ApplicationServices -lz")
        ELSE(FLTK_USE_X)
            SET( FLTK_PLATFORM_DEPENDENT_LIBS  "-framework Carbon -framework Cocoa -framework ApplicationServices -lz")
        ENDIF(FLTK_USE_X)
    ENDIF(APPLE)

    IF(CYGWIN)
      SET( FLTK_PLATFORM_DEPENDENT_LIBS ole32 uuid comctl32 wsock32 supc++ -lm -lgdi32)
    ENDIF(CYGWIN)

    SET(FLTK_LIBRARIES ${FLTK_IMAGES_LIBRARY} ${FLTK_IMAGES_LIBS} ${FLTK_BASE_LIBRARY} ${FLTK_GL_LIBRARY} ${FLTK_FORMS_LIBRARY})
    IF(APPLE) ## Apple needs platforms defined before libraries
      SET(FLTK_LIBRARIES ${FLTK_PLATFORM_DEPENDENT_LIBS} ${FLTK_LIBRARIES})
    ELSE(APPLE)
      SET(FLTK_LIBRARIES ${FLTK_LIBRARIES} ${FLTK_PLATFORM_DEPENDENT_LIBS})
    ENDIF(APPLE)
  ENDIF(FLTK_FOUND)
ENDIF(NOT FLTK_VERSION_PATCH GREATER 6) ##ONLY NEEDED FOR FLTK <= 1.1.6

IF(FLTK_FOUND)
  # The following deprecated settings are for compatibility with CMake 1.4
  SET (HAS_FLTK ${FLTK_FOUND})
  SET (FLTK_INCLUDE_PATH ${FLTK_INCLUDE_DIR})
  SET (FLTK_FLUID_EXE ${FLTK_FLUID_EXECUTABLE})
  SET (FLTK_LIBRARY ${FLTK_LIBRARIES})
ENDIF(FLTK_FOUND)
