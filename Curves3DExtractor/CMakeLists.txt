PROJECT(Curves3DExtractor)

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)


  FIND_PACKAGE(FLTK)
  IF(FLTK_FOUND)
    INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
  ENDIF(FLTK_FOUND)


  # Macro for adding resources to GUI application on the Mac
  #
  IF(APPLE)
    FIND_PROGRAM(ITK_APPLE_RESOURCE Rez /Developer/Tools)
    FIND_FILE(ITK_FLTK_RESOURCE mac.r /usr/local/include/FL)
    IF(NOT ITK_FLTK_RESOURCE)
      MESSAGE("Fltk resources not found, GUI application will not respond to mouse events")
    ENDIF(NOT ITK_FLTK_RESOURCE)

    MACRO(ADD_GUI_EXECUTABLE name sources)
      ADD_EXECUTABLE(${name} ${sources})
      SET(EXEC_PATH ${EXECUTABLE_OUTPUT_PATH}) 
      IF(NOT EXEC_PATH)
        SET(EXEC_PATH ${CMAKE_CURRENT_BINARY_DIR})
      ENDIF(NOT EXEC_PATH)
        IF(ITK_APPLE_RESOURCE)
          ADD_CUSTOM_COMMAND(SOURCE ${name}
                             COMMAND ${ITK_APPLE_RESOURCE}
                             ARGS -t APPL ${ITK_FLTK_RESOURCE} -o 
                             ${EXEC_PATH}/${name}
                             TARGET ${name})
        ENDIF(ITK_APPLE_RESOURCE)
    ENDMACRO(ADD_GUI_EXECUTABLE)
  ELSE(APPLE)
    MACRO(ADD_GUI_EXECUTABLE name sources)
      ADD_EXECUTABLE(${name} ${sources})
    ENDMACRO(ADD_GUI_EXECUTABLE)
  ENDIF(APPLE)

  INCLUDE_DIRECTORIES(
    ${Curves3DExtractor_SOURCE_DIR}
    ${Curves3DExtractor_BINARY_DIR}
    ${Curves3DExtractor_SOURCE_DIR}/../FltkImageViewer
    ${Curves3DExtractor_BINARY_DIR}/../FltkImageViewer
  )

  ADD_LIBRARY(Curves3DExtractorLib ceExtractorConsole.cxx
              ceExtractorConsoleBase.cxx)

  # Take the list of GUI files, generate .cxx and .h from them and add
  # them to the build process of the target
  FLTK_WRAP_UI(Curves3DExtractorLib ceExtractorConsoleGUI.fl)

  TARGET_LINK_LIBRARIES(
      Curves3DExtractorLib
      ITKBasicFilters ITKIO ITKFltkImageViewer
      ${FLTK_LIBRARIES} ${OPENGL_glu_LIBRARY} ${OPENGL_LIBRARIES} )

  ADD_GUI_EXECUTABLE(Curves3DExtractor main.cxx)
  TARGET_LINK_LIBRARIES(Curves3DExtractor Curves3DExtractorLib)

ELSE( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  INCLUDE_DIRECTORIES(
    ${InsightApplications_SOURCE_DIR}/Curves3DExtractor
    ${InsightApplications_BINARY_DIR}/Curves3DExtractor
    ${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
    ${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
  )

  ADD_LIBRARY(Curves3DExtractorLib ceExtractorConsole.cxx
              ceExtractorConsoleBase.cxx)

  # Take the list of GUI files, generate .cxx and .h from them and add
  # them to the build process of the target
  FLTK_WRAP_UI(Curves3DExtractorLib ceExtractorConsoleGUI.fl)
  ITK_DISABLE_FLTK_GENERATED_WARNINGS(ceExtractorConsoleGUI.fl)

  TARGET_LINK_LIBRARIES( Curves3DExtractorLib
                         ITKBasicFilters ITKIO ITKFltkImageViewer )

  ADD_GUI_EXECUTABLE(Curves3DExtractor main.cxx)
  TARGET_LINK_LIBRARIES(Curves3DExtractor Curves3DExtractorLib)



ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


