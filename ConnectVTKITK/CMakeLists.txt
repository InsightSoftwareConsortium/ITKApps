PROJECT(ConnectVTKITK)

# Hack for CMake 1.6 compatability.
IF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 1.7)
  SET(CABLESWIG_DIR ${ITK_CableSwig_DIR})
ENDIF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 1.7)

# Load the CableSwig settings used by ITK.
SET(CableSwig_DIR ${ITK_CableSwig_DIR})
FIND_PACKAGE(CableSwig)

INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS} ${ITK_INCLUDE_DIRS}
                    ${ConnectVTKITK_SOURCE_DIR})
LINK_DIRECTORIES(${VTK_LIBRARY_DIRS} ${ITK_LIBRARY_DIRS})

IF(ITK_CSWIG_TCL)
  SET_SOURCE_FILES_PROPERTIES(ConnectVTKITK_wrapTcl.cxx GENERATED)
  ADD_LIBRARY(ConnectVTKITKTcl SHARED ConnectVTKITK_wrapTcl.cxx)
  TARGET_LINK_LIBRARIES(ConnectVTKITKTcl ITKBasicFilters
  vtkCommonTCL vtkImaging SwigRuntimeTcl ${TCL_LIBRARY})
  ADD_CUSTOM_COMMAND(
    COMMENT "ConnectVTKITK_wrapTcl.cxx from ConnectVTKITK.i"
    SOURCE ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i
    COMMAND ${CableSwig_cswig_EXE}
    ARGS -I${ConnectVTKITK_SOURCE_DIR}
         -nocable -c ${IGNORE_WARNINGS}
         -o ConnectVTKITK_wrapTcl.cxx
         -c++ ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i
    TARGET ConnectVTKITKTcl
    OUTPUTS ${ConnectVTKITK_BINARY_DIR}/ConnectVTKITK_wrapTcl.cxx
    DEPENDS ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i)
  
  # Configure pkgIndex.tcl for the build tree.
  IF(UNIX)
    SET(ConnectVTKITK_LIBNAME_PREFIX "lib")
  ELSE(UNIX)
    SET(ConnectVTKITK_LIBNAME_PREFIX "")
  ENDIF(UNIX)
  SET(ConnectVTKITK_VERSION_VTK "${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}")
  SET(ConnectVTKITK_VERSION_ITK "${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}")
  SET(ConnectVTKITK_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES})
  IF(ConnectVTKITK_CONFIGURATION_TYPES)
    FOREACH(config ${ConnectVTKITK_CONFIGURATION_TYPES})
      CONFIGURE_FILE("${ConnectVTKITK_SOURCE_DIR}/pkgIndex.tcl.in"
        "${ConnectVTKITK_BINARY_DIR}/${config}/pkgIndex.tcl"
        @ONLY IMMEDIATE)
    ENDFOREACH(config)
  ELSE(ConnectVTKITK_CONFIGURATION_TYPES)
    CONFIGURE_FILE("${ConnectVTKITK_SOURCE_DIR}/pkgIndex.tcl.in"
      "${ConnectVTKITK_BINARY_DIR}/pkgIndex.tcl"
      @ONLY IMMEDIATE)
  ENDIF(ConnectVTKITK_CONFIGURATION_TYPES)
ENDIF(ITK_CSWIG_TCL)

IF(ITK_CSWIG_PYTHON)
  SET_SOURCE_FILES_PROPERTIES(ConnectVTKITK_wrapPython.cxx GENERATED)
  ADD_LIBRARY(_ConnectVTKITKPython MODULE ConnectVTKITK_wrapPython.cxx)
  # we don't want a "lib" prefix, thanks.
  SET_TARGET_PROPERTIES(_ConnectVTKITKPython PROPERTIES PREFIX "")
  TARGET_LINK_LIBRARIES(_ConnectVTKITKPython ITKBasicFilters
  vtkCommonPython vtkImaging SwigRuntimePython
  ${PYTHON_LIBRARY})

  ADD_CUSTOM_COMMAND(
    COMMENT "ConnectVTKITK_wrapPython.cxx from ConnectVTKITK.i"
    SOURCE ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i
    COMMAND ${CableSwig_cswig_EXE}
    ARGS -I${ConnectVTKITK_SOURCE_DIR}
         -nocable -c ${IGNORE_WARNINGS}
         -o ConnectVTKITK_wrapPython.cxx
#         -bindir "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}"
         -python -c++ ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i
    TARGET _ConnectVTKITKPython
    OUTPUTS ${ConnectVTKITK_BINARY_DIR}/ConnectVTKITK_wrapPython.cxx
    DEPENDS ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i)

ENDIF(ITK_CSWIG_PYTHON)
