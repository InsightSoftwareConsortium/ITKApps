IF(UNIX)
  INCLUDE( ${CMAKE_ROOT}/Modules/FindX11.cmake )
ENDIF(UNIX)
CONFIGURE_FILE(${InsightApplications_SOURCE_DIR}/LandmarkInitializedMutualInformationRegistration/Help/Index.html
               ${InsightApplications_BINARY_DIR}/LandmarkInitializedMutualInformationRegistration/Help/Index.html COPYONLY)

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
  ${InsightApplications_SOURCE_DIR}/LandmarkInitializedMutualInformationRegistration
  ${InsightApplications_BINARY_DIR}/LandmarkInitializedMutualInformationRegistration
)

ADD_EXECUTABLE(LandmarkInitializedMutualInformationRegistration
  guiMainImplementation.cxx
  LandmarkRegistrator.cxx
  main.cxx
)

FLTK_WRAP_UI(LandmarkInitializedMutualInformationRegistration guiMain.fl)
ITK_DISABLE_FLTK_GENERATED_WARNINGS(guiMain.fl)

TARGET_LINK_LIBRARIES (LandmarkInitializedMutualInformationRegistration
                       ITKIO ITKMetaIO ITKBasicFilters ITKStatistics
ITKFltkImageViewer ${OPENGL_LIBRARY})

