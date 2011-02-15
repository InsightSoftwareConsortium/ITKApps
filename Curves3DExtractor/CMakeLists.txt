PROJECT(Curves3DExtractor)

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build ITKApps without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)


  FIND_PACKAGE(FLTK)
  IF(FLTK_FOUND)
    INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
  ENDIF(FLTK_FOUND)


  # Macro for adding resources to GUI application on the Mac
  #
  IF(APPLE AND NOT FLTK_USE_X)
    FIND_PROGRAM(ITK_APPLE_RESOURCE Rez /Developer/Tools)
    FIND_FILE(ITK_FLTK_RESOURCE mac.r /usr/local/include/FL)
    IF(NOT ITK_FLTK_RESOURCE)
      MESSAGE("Fltk resources not found, GUI application will not respond to mouse events")
    ENDIF(NOT ITK_FLTK_RESOURCE)

    MACRO(ADD_GUI_EXECUTABLE name sources)
      ADD_EXECUTABLE(${name} ${sources})
      INSTALL_TARGETS(/bin ${name})
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
  ELSE(APPLE AND NOT FLTK_USE_X)
    MACRO(ADD_GUI_EXECUTABLE name sources)
      ADD_EXECUTABLE(${name} ${sources})
      INSTALL_TARGETS(/bin ${name})
    ENDMACRO(ADD_GUI_EXECUTABLE)
  ENDIF(APPLE AND NOT FLTK_USE_X)

  INCLUDE_DIRECTORIES(
    ${Curves3DExtractor_SOURCE_DIR}
    ${Curves3DExtractor_BINARY_DIR}
    ${Curves3DExtractor_SOURCE_DIR}/../FltkImageViewer
    ${Curves3DExtractor_BINARY_DIR}/../FltkImageViewer
  )

  # Take the list of GUI files, generate .cxx and .h from them and add
  # them to the build process of the target
  FLTK_WRAP_UI(Curves3DExtractorLib ceExtractorConsoleGUI.fl)
  ADD_LIBRARY(Curves3DExtractorLib  ceExtractorConsole.cxx
              ceExtractorConsoleBase.cxx ${Curves3DExtractorLib_FLTK_UI_SRCS})
  INSTALL_TARGETS(/lib Curves3DExtractorLib)

  TARGET_LINK_LIBRARIES(
      Curves3DExtractorLib
      ${ITK_LIBRARIES} ITKFltkImageViewer
      ${FLTK_LIBRARIES} ${OPENGL_glu_LIBRARY} ${OPENGL_LIBRARIES} )


  SET(Curves3DExtractor_SRCS
  main.cxx
  ${Curves3DExtractor_FLTK_UI_SRCS}
  )

  ADD_GUI_EXECUTABLE(Curves3DExtractor ${Curves3DExtractor_SRCS})
  TARGET_LINK_LIBRARIES(Curves3DExtractor Curves3DExtractorLib)

ELSE( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  INCLUDE_DIRECTORIES(
    ${ITKApps_SOURCE_DIR}/Curves3DExtractor
    ${ITKApps_BINARY_DIR}/Curves3DExtractor
    ${ITKApps_SOURCE_DIR}/Auxiliary/FltkImageViewer
    ${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
  )

  # Take the list of GUI files, generate .cxx and .h from them and add
  # them to the build process of the target
  FLTK_WRAP_UI(Curves3DExtractorLib ceExtractorConsoleGUI.fl)
  ITK_DISABLE_FLTK_GENERATED_WARNINGS(ceExtractorConsoleGUI.fl)

  ADD_LIBRARY(Curves3DExtractorLib  ceExtractorConsole.cxx
              ceExtractorConsoleBase.cxx ${Curves3DExtractorLib_FLTK_UI_SRCS})
  INSTALL_TARGETS(/lib Curves3DExtractorLib)

  TARGET_LINK_LIBRARIES( Curves3DExtractorLib
                         ${ITK_LIBRARIES} ITKFltkImageViewer )
  ADD_DEPENDENCIES(Curves3DExtractorLib ITKFltkImageViewer)

  SET(Curves3DExtractor_SRCS
  main.cxx
  ${Curves3DExtractor_FLTK_UI_SRCS}
  )

  ADD_GUI_EXECUTABLE(Curves3DExtractor ${Curves3DExtractor_SRCS})
  TARGET_LINK_LIBRARIES(Curves3DExtractor Curves3DExtractorLib)



ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


