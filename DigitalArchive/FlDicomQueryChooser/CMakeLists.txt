PROJECT(FlDicomQueryChooser)

FIND_PATH(DCMTK_DIR configure)

IF(DCMTK_DIR)
# Use the version of FindDCMTK in this directory
  INCLUDE(${FlDicomQueryChooser_SOURCE_DIR}/FindDCMTK.cmake)
FIND_PACKAGE(ZLIB)

INCLUDE(${FlDicomQueryChooser_SOURCE_DIR}/FindITKApps.cmake)

#Find ITK.
FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE( FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)


# Look for OpenGL.
FIND_PACKAGE(OpenGL)

IF(OPENGL_INCLUDE_PATH)
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_PATH})
ENDIF(OPENGL_INCLUDE_PATH)


# Make sure the compiler can find include files from our library.
INCLUDE_DIRECTORIES(
 ${FlDicomQueryChooser_SOURCE_DIR}
 ${FlDicomQueryChooser_BINARY_DIR}
 ${DCMTK_DIR}/imagectn/include
 ${DCMTK_ofstd_INCLUDE_DIR}
 ${DCMTK_config_INCLUDE_DIR}
 ${DCMTK_dcmdata_INCLUDE_DIR}
 ${DCMTK_DIR}/dcmnet/include
 ${DCMTK_dcmingle_INCLUDE_DIR}
 ${DCMTK_dcmnet_INCLUDE_DIR}
 ${DCMTK_imagedb_INCLUDE_DIR}
)

FIND_LIBRARY(DCMTK_imagedb_LIBRARY libimagedb ${DCMTK_DIR}/imagectn/libsrc)
FIND_LIBRARY(DCMTK_dcmnet_LIBRARY dcmnet ${DCMTK_DIR}/dcmnet/libsrc)

# set _REENTRANT flag
ADD_DEFINITIONS(-D_REENTRANT)
IF(NOT WIN32)
  ADD_DEFINITIONS(-DHAVE_CONFIG_H)
ENDIF(NOT WIN32)

INCLUDE(${CMAKE_ROOT}/Modules/FindFLTK.cmake)

IF(HAS_FLTK)
  INCLUDE_DIRECTORIES(
    ${FLTK_INCLUDE_DIR}
    )
  LINK_DIRECTORIES(
    ${FLTK_DIR}/bin
    ${FLTK_DIR}/lib
    )
  LINK_LIBRARIES(
   ${FLTK_LIBRARIES}
    )
ENDIF(HAS_FLTK)


INCLUDE_DIRECTORIES(
  ${DCMTKTEST_SOURCE_DIR}
  ${DCMTKTEST_BINARY_DIR}
  ${ITKApps_SOURCE_DIR}/Auxiliary/FltkImageViewer
  ${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
)


IF(CYGWIN)
  LINK_LIBRARIES(ole32 uuid comctl32 wsock32 supc++ -lm -lgdi32)
ENDIF(CYGWIN)


SET(FlDicomQueryChooser_GUI_SRCS
  fldqcPreferencesGUI.fl
  FlDicomQueryChooserGUI.fl
  ImageViewerGUI.fl
)


FLTK_WRAP_UI(FlDicomQueryChooser ${FlDicomQueryChooser_GUI_SRCS})


SET(FlDicomQueryChooser_SRCS
  ${FlDicomQueryChooser_FLTK_UI_SRCS}
  fldqcDatatypes.h
  fldqcNet.cxx
  fldqcNet.h
  fldqcLocalDirectoryQuery.h
  fldqcLocalDirectoryQuery.cxx
  fldqcRemoteNetworkQuery.cxx
  fldqcRemoteNetworkQuery.h
  fldqcPreferences.cxx
  fldqcPreferences.h
  FlDicomQueryChooser.h
  FlDicomQueryChooser.cxx
  main.cxx
)


ADD_EXECUTABLE(FlDicomQueryChooser ${FlDicomQueryChooser_SRCS})
INSTALL_TARGETS(/bin FlDicomQueryChooser)


IF(WIN32)
 TARGET_LINK_LIBRARIES(
  FlDicomQueryChooser
  ${DCMTK_LIBRARIES}
  ${OPENGL_LIBRARY}
  ITKCommon
  ITKIO
  ITKBasicFilters
  ITKFltkImageViewer
  )
ELSE(WIN32)
 TARGET_LINK_LIBRARIES(
  FlDicomQueryChooser
  ${DCMTK_LIBRARIES}
  ${OPENGL_LIBRARY}
  ITKCommon
  ITKIO
  ITKBasicFilters
  ITKFltkImageViewer
  wrap
)

IF(ZLIB_FOUND)
 LINK_LIBRARIES(${ZLIB_LIBRARIES})
ENDIF(ZLIB_FOUND)

ENDIF(WIN32)
ENDIF(DCMTK_DIR)

