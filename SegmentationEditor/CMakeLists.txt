PROJECT( SegmentationEditor )

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

  FIND_PACKAGE(VTK)
  IF (VTK_FOUND)
      INCLUDE (${VTK_USE_FILE})
  ENDIF (VTK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


INCLUDE_REGULAR_EXPRESSION("^.*$")

# Link in with ITK
INCLUDE_DIRECTORIES ( ${SegmentationEditor_SOURCE_DIR}/../vtkITK/Common )
INCLUDE_DIRECTORIES ( ${SegmentationEditor_SOURCE_DIR}/Common )

LINK_DIRECTORIES ( ${SegmentationEditor_BINARY_DIR}/../vtkITK/Common )
LINK_DIRECTORIES ( ${SegmentationEditor_BINARY_DIR}/Common )


# Add the common directory
SUBDIRS ( Common )
SUBDIRS ( Wrapping )
