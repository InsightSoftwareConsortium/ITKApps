PROJECT(LevelSetSegmentation)


IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build ITKApps without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


ADD_EXECUTABLE(itk2DThresholdSegmentationLevelSetImageFilter itk2DThresholdSegmentationLevelSetImageFilter.cxx)
INSTALL_TARGETS(/bin itk2DThresholdSegmentationLevelSetImageFilter)
TARGET_LINK_LIBRARIES (itk2DThresholdSegmentationLevelSetImageFilter
ITKAlgorithms
ITKBasicFilters
ITKCommon
ITKIO
)


ADD_EXECUTABLE(itk2DCannySegmentationLevelSetImageFilter itk2DCannySegmentationLevelSetImageFilter.cxx)
INSTALL_TARGETS(/bin itk2DCannySegmentationLevelSetImageFilter)
TARGET_LINK_LIBRARIES (itk2DCannySegmentationLevelSetImageFilter
ITKAlgorithms
ITKBasicFilters
ITKCommon
ITKIO
)




