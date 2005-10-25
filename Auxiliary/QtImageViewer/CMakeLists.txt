PROJECT(QtSlicer)

INCLUDE (${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/FindGLU.cmake)


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




INCLUDE (${CMAKE_ROOT}/Modules/FindQt.cmake)

INCLUDE_DIRECTORIES(
 ${QT_INCLUDE_DIR}
 ${QtSlicer_BINARY_DIR}
 ${QtSlicer_SOURCE_DIR}
)



LINK_LIBRARIES(
 ${ITK_LIBRARIES}
 ${QT_QT_LIBRARY}
 ${QT_GL_LIBRARY}
 ${OPENGL_glu_LIBRARY}
 ${OPENGL_LIBRARY}
)

SET(QtSlicer_SRCS
  QtSlicerTest.cxx
  QtGlSliceView.cxx
  QtSlicer.cxx
)

SET(QtSlicer_GUI_SRCS
  QtSlicerGUI.ui
  QtSlicerHelpGUI.ui
)


SET(QtSlicer_MOC_SRCS
  QtGlSliceView.h
)

IF(QT_WRAP_UI)
  QT_WRAP_UI(QtSlicer QtSlicer_HDRS  QtSlicer_SRCS  QtSlicer_GUI_SRCS )
ENDIF(QT_WRAP_UI)

IF(QT_WRAP_CPP)
  QT_WRAP_CPP(QtSlicer QtSlicer_SRCS QtSlicer_MOC_SRCS)
ENDIF(QT_WRAP_CPP)

ADD_DEFINITIONS(-DQT_DLL)

ADD_DEFINITIONS(-DQT_THREAD_SUPPORT)

ADD_EXECUTABLE(QtSlicer QtSlicer_SRCS)
INSTALL_TARGETS(/bin QtSlicer)





