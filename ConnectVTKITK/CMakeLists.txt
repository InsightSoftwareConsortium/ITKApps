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

IF(ITK_CSWIG_TCL AND VTK_WRAP_TCL)
  SET_SOURCE_FILES_PROPERTIES(ConnectVTKITK_wrapTcl.cxx GENERATED)
  ADD_LIBRARY(ConnectVTKITKTcl  SHARED ConnectVTKITK_wrapTcl.cxx)
  INSTALL_TARGETS(/lib ConnectVTKITKTcl)
  TARGET_LINK_LIBRARIES(ConnectVTKITKTcl ITKBasicFilters
  vtkCommonTCL vtkImaging SwigRuntimeTcl ${TCL_LIBRARY})
  ADD_CUSTOM_COMMAND(
    COMMENT "ConnectVTKITK_wrapTcl.cxx from ConnectVTKITK.i"
    SOURCE ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i
    COMMAND ${CableSwig_cswig_EXE}
    ARGS -I${ConnectVTKITK_SOURCE_DIR}
         -nocable -noruntime ${IGNORE_WARNINGS}
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
ENDIF(ITK_CSWIG_TCL AND VTK_WRAP_TCL)

IF(ITK_CSWIG_PYTHON AND VTK_WRAP_PYTHON)
  SET_SOURCE_FILES_PROPERTIES(ConnectVTKITK_wrapPython.cxx GENERATED)
  ADD_LIBRARY( _ConnectVTKITKPython MODULE ConnectVTKITK_wrapPython.cxx)
  INSTALL_TARGETS(/lib _ConnectVTKITKPython)
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
         -nocable -noruntime ${IGNORE_WARNINGS}
         -o ConnectVTKITK_wrapPython.cxx
#         -bindir "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}"
         -python -c++ ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i
    TARGET _ConnectVTKITKPython
    OUTPUTS ${ConnectVTKITK_BINARY_DIR}/ConnectVTKITK_wrapPython.cxx
    DEPENDS ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i)

  FIND_PROGRAM(PYTHON_EXECUTABLE
    NAMES python2.3 python2.2 python2.1 python2.0 python1.6 python1.5 python
    PATHS
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.3\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.2\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.1\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\2.0\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\1.6\\InstallPath]
    [HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\1.5\\InstallPath]
    )
  MARK_AS_ADVANCED(PYTHON_EXECUTABLE)
  IF(PYTHON_EXECUTABLE)
    # Add a target to copy the ConnectVTKITK.py module to the build tree.
    ADD_CUSTOM_TARGET(connectvtkitk_pyc ALL echo "...")

    # Now create a list of Python files.
    SET(ITK_PYTHON_FILES ConnectVTKITK)

    # Now copy these files if necessary.
    SET(ITK_PYTHON_SOURCE_FILES)
    SET(ITK_PYTHON_OUTPUT_FILES)
    IF(CMAKE_CONFIGURATION_TYPES)
      FOREACH(file ${ITK_PYTHON_FILES})
        SET(src "${ConnectVTKITK_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${file}.py")
        SET(ITK_PYTHON_SOURCE_FILES ${ITK_PYTHON_SOURCE_FILES} ${src})
      ENDFOREACH(file)
    ELSE(CMAKE_CONFIGURATION_TYPES)
      FOREACH(file ${ITK_PYTHON_FILES})
        SET(src "${ConnectVTKITK_BINARY_DIR}/${file}.py")
        SET(ITK_PYTHON_SOURCE_FILES ${ITK_PYTHON_SOURCE_FILES} ${src})
      ENDFOREACH(file)
    ENDIF(CMAKE_CONFIGURATION_TYPES)
    IF ("${ConnectVTKITK_BINARY_DIR}" MATCHES "^${ConnectVTKITK_SOURCE_DIR}$")
      #MESSAGE("In source build -- no need to copy Python files.")
    ELSE ("${ConnectVTKITK_BINARY_DIR}" MATCHES "^${ConnectVTKITK_SOURCE_DIR}$")
      IF(CMAKE_CONFIGURATION_TYPES)
        FOREACH(file ${ITK_PYTHON_FILES})
          SET(src "${ConnectVTKITK_SOURCE_DIR}/${file}.py")
          SET(tgt "${ConnectVTKITK_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${file}.py")
          ADD_CUSTOM_COMMAND(SOURCE ${src}
                             COMMAND ${CMAKE_COMMAND}
                             ARGS -E copy ${src} ${tgt}
                             OUTPUTS ${tgt}
                             TARGET connectvtkitk_pyc
                             COMMENT "source copy")
        ENDFOREACH(file)
      ELSE(CMAKE_CONFIGURATION_TYPES)
        FOREACH(file ${ITK_PYTHON_FILES})
          SET(src "${ConnectVTKITK_SOURCE_DIR}/${file}.py")
          SET(tgt "${ConnectVTKITK_BINARY_DIR}/${file}.py")
          ADD_CUSTOM_COMMAND(SOURCE ${src}
                            COMMAND ${CMAKE_COMMAND}
                            ARGS -E copy ${src} ${tgt}
                            OUTPUTS ${tgt}
                            TARGET connectvtkitk_pyc
                            COMMENT "source copy")
        ENDFOREACH(file)
      ENDIF(CMAKE_CONFIGURATION_TYPES)
    ENDIF ("${ConnectVTKITK_BINARY_DIR}" MATCHES "^${ConnectVTKITK_SOURCE_DIR}$")

    # Byte compile the Python files.
    WRITE_FILE(${ConnectVTKITK_BINARY_DIR}/compile_all_connectvtkitk
      "import compileall\n"
      "compileall.compile_dir('${ConnectVTKITK_BINARY_DIR}')\n"
      "file = open('${ConnectVTKITK_BINARY_DIR}/connectvtkitk_compile_complete', 'w')\n"
      "file.write('Done')\n")

    ADD_CUSTOM_COMMAND(
      SOURCE ${ConnectVTKITK_BINARY_DIR}/compile_all_connectvtkitk
      COMMAND ${PYTHON_EXECUTABLE}
      ARGS ${ConnectVTKITK_BINARY_DIR}/compile_all_connectvtkitk
      DEPENDS ${ITK_PYTHON_SOURCE_FILES}
      OUTPUTS "${ConnectVTKITK_BINARY_DIR}/connectvtkitk_compile_complete"
      TARGET connectvtkitk_pyc
      )

    # Add a driver target.
    ADD_CUSTOM_COMMAND(
      SOURCE connectvtkitk_pyc
      DEPENDS "${ConnectVTKITK_BINARY_DIR}/connectvtkitk_compile_complete"
      TARGET connectvtkitk_pyc
      )
  ENDIF(PYTHON_EXECUTABLE)
ENDIF(ITK_CSWIG_PYTHON AND VTK_WRAP_PYTHON)
