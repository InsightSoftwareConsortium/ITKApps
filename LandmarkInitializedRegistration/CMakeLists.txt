IF(UNIX)
  INCLUDE( ${CMAKE_ROOT}/Modules/FindX11.cmake )
ENDIF(UNIX)
CONFIGURE_FILE(${InsightApplications_SOURCE_DIR}/LandmarkInitializedRegistration/Help/Index.html
               ${InsightApplications_BINARY_DIR}/LandmarkInitializedRegistration/Help/Index.html COPYONLY)

INCLUDE_DIRECTORIES(
  ${ITK_SOURCE_DIR}/Code/Common
  ${ITK_SOURCE_DIR}/Code/BasicFilters
  ${ITK_SOURCE_DIR}/Code/IO
  ${ITK_SOURCE_DIR}/Code/Numerics
  ${ITK_SOURCE_DIR}/Code/Numerics/vxl
  ${ITK_SOURCE_DIR}/Code/Numerics/Statistics
  ${ITK_SOURCE_DIR}/Utilities/MetaIO
  ${ITK_SOURCE_DIR}/Utilities/MetaIO/SaptialObject
  ${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_SOURCE_DIR}/LandmarkInitializedRegistration
  ${InsightApplications_BINARY_DIR}/LandmarkInitializedRegistration
)

ADD_EXECUTABLE(LandmarkInitializedRegistration
  guiMainImplementation.cxx
  main.cxx
)

FLTK_WRAP_UI(LandmarkInitializedRegistration guiMain.fld)

TARGET_LINK_LIBRARIES (LandmarkInitializedRegistration
                       ITKIO ITKMetaIO ITKBasicFilters ITKStatistics ITKFltkImageViewer ${OPENGL_LIBRARY})

