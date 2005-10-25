FIND_PACKAGE(ITK)

IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
 ENDIF(ITK_FOUND)

ADD_EXECUTABLE(ITKThumbnailGenerator GenerateThumbnail.cxx)
INSTALL_TARGETS(/bin ITKThumbnailGenerator)
TARGET_LINK_LIBRARIES(ITKThumbnailGenerator ITKCommon ITKBasicFilters ITKIO)

ADD_EXECUTABLE(DicomThumbnailGenerator DicomGenerateThumbnail.cxx)
INSTALL_TARGETS(/bin DicomThumbnailGenerator)
TARGET_LINK_LIBRARIES(DicomThumbnailGenerator ITKCommon ITKBasicFilters ITKIO)

