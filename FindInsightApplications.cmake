#
# Find the native InsightApplications includes and libraries
#
# This module defines
#
# InsightApplications_SOURCE_DIR - where is the source tree 
# InsightApplications_BINARY_DIR - where is the binary tree 
#
#
# Look for a binary tree
# 

IF(CYGWIN)
FIND_PATH(
INSIGHT_APPLICATIONS_BINARY_PATH 
Auxiliary/cmake_install.cmake

${CMAKE_CURRENT_SOURCE_DIR}/../InsightApplications-bin
${CMAKE_CURRENT_SOURCE_DIR}/../InsightApplications-Bin
${CMAKE_CURRENT_SOURCE_DIR}/../InsightApplications-Cygwin
)
ELSE(CYGWIN)

FIND_PATH(
INSIGHT_APPLICATIONS_BINARY_PATH 
Auxiliary/cmake_install.cmake

${CMAKE_CURRENT_SOURCE_DIR}/../InsightApplications-VC++
${CMAKE_CURRENT_SOURCE_DIR}/../InsightApplications-bin
${CMAKE_CURRENT_SOURCE_DIR}/../InsightApplications-Bin
${CMAKE_CURRENT_SOURCE_DIR}/../InsightApplications-Cygwin
${CMAKE_CURRENT_SOURCE_DIR}/../InsightApplications-Linux
${CMAKE_CURRENT_SOURCE_DIR}/../InsightApplicationsBorland
${CMAKE_CURRENT_SOURCE_DIR}/../InsightApplications-Darwin
${CMAKE_CURRENT_SOURCE_DIR}/../InsightApplications-SunOS


  # Read from the CMakeSetup registry entries.  It is likely that
  # ITK will have been recently built.
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
)
ENDIF(CYGWIN)

IF (INSIGHT_APPLICATIONS_BINARY_PATH)
  LOAD_CACHE(${INSIGHT_APPLICATIONS_BINARY_PATH} 
  EXCLUDE
    BUILD_SHARED_LIBS
    BUILD_TESTING
    BUILD_EXAMPLES
    EXECUTABLE_OUTPUT_PATH
    LIBRARY_OUTPUT_PATH
    MAKECOMMAND 
    SITE
    BUILDNAME
    CVS_UPDATE_OPTIONS
    DART_TESTING_TIMEOUT
    VTK_INSTALL_PATH
    USE_VTK
    USE_FLTK
    PURIFYCOMMAND
    CVSCOMMAND
    CMAKE_BACKWARDS_COMPATIBILITY
  INCLUDE_INTERNALS
    FLTK_FLUID_EXE
    FLTK_INCLUDE_PATH
    FLTK_LIBRARY
    USE_FLTK_VERSION_1.1
    USE_FLTK_VERSION_1.0.11
    ITK_BINARY_DIR
    ITK_LIBRARY_PATH
    ITK_BUILD_SHARED_LIBS
    InsightApplications_BINARY_DIR
    InsightApplications_SOURCE_DIR
    InsightApplications_LIBRARY_PATH
    VTK_BINARY_DIR
    VTK_BUILD_SHARED_LIBS
    VTK_LIBRARY_PATH
    ITK_USE_SYSTEM_STDLIB
    )
 
INCLUDE_DIRECTORIES(
  ${InsightApplications_SOURCE_DIR}/Auxiliary/vtk
  ${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_SOURCE_DIR}/Auxiliary/VtkFltk
  ${InsightApplications_BINARY_DIR}/Auxiliary/VtkFltk
)

LINK_DIRECTORIES(
 ${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
 ${InsightApplications_BINARY_DIR}/Auxiliary/VtkFltk
 ${InsightApplications_LIBRARY_PATH}
)

ENDIF(INSIGHT_APPLICATIONS_BINARY_PATH)




