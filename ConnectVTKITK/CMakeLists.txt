PROJECT(ConnectVTKITK)

INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS} ${ITK_INCLUDE_DIRS})
LINK_DIRECTORIES(${VTK_LIBRARY_DIRS} ${ITK_LIBRARY_DIRS})

IF(ITK_CSWIG_TCL)
  SET_SOURCE_FILES_PROPERTIES(ConnectVTKITK_wrapTcl.cxx GENERATED)
  ADD_LIBRARY(ConnectVTKITKTcl SHARED ConnectVTKITK_wrapTcl.cxx)
  TARGET_LINK_LIBRARIES(ConnectVTKITKTcl ITKBasicFilters
  vtkCommonTCL vtkImaging SwigRuntimeTcl ${TCL_LIBRARY})
  ADD_CUSTOM_COMMAND(
    COMMENT "ConnectVTKITK_wrapTcl.cxx from ConnectVTKITK.i"
    SOURCE ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i
    COMMAND ${ITK_CSWIG_PATH}
    ARGS -I${ConnectVTKITK_SOURCE_DIR}
         -nocable -c ${IGNORE_WARNINGS}
         -o ConnectVTKITK_wrapTcl.cxx
         -c++ ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i
    TARGET ConnectVTKITKTcl
    OUTPUTS ${ConnectVTKITK_BINARY_DIR}/ConnectVTKITK_wrapTcl.cxx
    DEPENDS ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i)
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
    COMMAND ${ITK_CSWIG_PATH}
    ARGS -I${ConnectVTKITK_SOURCE_DIR}
         -nocable -c ${IGNORE_WARNINGS}
         -o ConnectVTKITK_wrapPython.cxx
#         -bindir "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}"
         -python -c++ ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i
    TARGET _ConnectVTKITKPython
    OUTPUTS ${ConnectVTKITK_BINARY_DIR}/ConnectVTKITK_wrapPython.cxx
    DEPENDS ${ConnectVTKITK_SOURCE_DIR}/ConnectVTKITK.i)

ENDIF(ITK_CSWIG_PYTHON)
