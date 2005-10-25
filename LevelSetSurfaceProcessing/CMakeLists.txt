PROJECT(LevelSetSurfaceProcessing)


IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )



ADD_EXECUTABLE(itk3DIsotropicFourthOrderLevelSetImageFilter itk3DIsotropicFourthOrderLevelSetImageFilter.cxx)
INSTALL_TARGETS(/bin itk3DIsotropicFourthOrderLevelSetImageFilter)
ADD_EXECUTABLE(itk3DAnisotropicFourthOrderLevelSetImageFilter itk3DAnisotropicFourthOrderLevelSetImageFilter.cxx)
INSTALL_TARGETS(/bin itk3DAnisotropicFourthOrderLevelSetImageFilter)
ADD_EXECUTABLE(itk3DUnsharpMaskLevelSetImageFilter itk3DUnsharpMaskLevelSetImageFilter.cxx)
INSTALL_TARGETS(/bin itk3DUnsharpMaskLevelSetImageFilter)

TARGET_LINK_LIBRARIES (itk3DIsotropicFourthOrderLevelSetImageFilter ITKAlgorithms ITKBasicFilters ITKCommon ITKNumerics ITKIO )
TARGET_LINK_LIBRARIES (itk3DAnisotropicFourthOrderLevelSetImageFilter ITKAlgorithms ITKBasicFilters ITKCommon ITKNumerics ITKIO )
TARGET_LINK_LIBRARIES (itk3DUnsharpMaskLevelSetImageFilter ITKAlgorithms ITKBasicFilters ITKCommon ITKIO ITKNumerics ITKIO)
