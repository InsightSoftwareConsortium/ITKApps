PROJECT(AnisotropicDiffusionImageFilter)

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


ADD_EXECUTABLE(itk2DAnisotropicDiffusionImageFilter itk2DAnisotropicDiffusionImageFilter.cxx)
INSTALL_TARGETS(/bin itk2DAnisotropicDiffusionImageFilter)

TARGET_LINK_LIBRARIES (itk2DAnisotropicDiffusionImageFilter
  ITKAlgorithms
  ITKBasicFilters
  ITKCommon
  ITKIO
  ITKNumerics
  )




