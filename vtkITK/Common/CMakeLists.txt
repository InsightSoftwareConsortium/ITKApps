SET( VTKITKCommon_SRCS
vtkITKAntiAliasBinaryImageFilter.h
vtkITKBilateralImageFilter.h
vtkITKConfidenceConnectedImageFilter.h
vtkITKConnectedThresholdImageFilter.h
vtkITKCurvatureAnisotropicDiffusionImageFilter.h
vtkITKCurvatureAnisotropicDiffusionImageFilterSS.h
vtkITKDanielssonDistanceMapImageFilter.h
vtkITKDiscreteGaussianImageFilter.h
vtkITKGradientAnisotropicDiffusionImageFilter.h
vtkITKGradientMagnitudeImageFilter.h
vtkITKGrayscaleFillholeImageFilter.h
vtkITKImageToImageFilter.h
vtkITKImageToImageFilter2DFF.h
vtkITKImageToImageFilterFF.h
vtkITKImageToImageFilterFUL.h
vtkITKImageToImageFilterULUL.h
vtkITKImageToImageFilterSF.h
vtkITKImageToImageFilterSUL.h
vtkITKImageToImageFilterUSUL.h
vtkITKImageToImageFilterUSUS.h
vtkITKImageToImageFilterSS.h
vtkITKIsolatedConnectedImageFilter.h
vtkITKLaplacianSegmentationLevelSetImageFilter.h
vtkITKMutualInformationTransform.cxx
vtkITKNeighborhoodConnectedImageFilter.h
vtkITKNormalizeImageFilterSF.h
vtkITKNormalizeImageFilter.h
vtkITKPDEDeformableRegistrationFilter.h
vtkITKSmoothingRecursiveGaussianImageFilterSS.h
vtkITKGradientMagnitudeRecursiveGaussianImageFilterSS.h
vtkITKThresholdSegmentationLevelSetImageFilter.h
vtkITKTobogganImageFilter.h
vtkITKWatershedImageFilter.h
vtkITKWatershedImageFilterSUL.h
vtkITKRelabelComponentImageFilter.h
)

SET_SOURCE_FILES_PROPERTIES ( 
vtkITKImageToImageFilter.h
vtkITKImageToImageFilter2DFF.h
vtkITKImageToImageFilterFF.h
vtkITKImageToImageFilterF2F.h
vtkITKImageToImageFilterF2F2.h
vtkITKImageToImageFilterF3F3.h
vtkITKImageToImageFilterFUL.h
vtkITKImageToImageFilterULUL.h
vtkITKImageToImageFilterSF.h
vtkITKImageToImageFilterSUL.h
vtkITKImageToImageFilterUSUL.h
vtkITKImageToImageFilterUSUS.h
vtkITKImageToImageFilterSS.h
ABSTRACT
)

ADD_LIBRARY ( vtkVTKITKCommon ${VTKITKCommon_SRCS} )

# Configure Tcl wrappers.
VTK_WRAP_TCL2(vtkVTKITKCommonTCL VTKITKCommonTCL_SRCS ${VTKITKCommon_SRCS})
ADD_LIBRARY ( vtkVTKITKCommonTCL ${VTKITKCommonTCL_SRCS} )
TARGET_LINK_LIBRARIES(vtkVTKITKCommonTCL vtkFilteringTCL vtkCommonTCL)

# Configure Python wrappers.
IF (VTK_WRAP_PYTHON)
  VTK_WRAP_PYTHON2(vtkVTKITKCommonPython VTKITKCommonPython_SRCS
                   ${VTKITKCommon_SRCS})
  ADD_LIBRARY(vtkVTKITKCommonPython SHARED ${VTKITKCommonPython_SRCS})
  TARGET_LINK_LIBRARIES (vtkVTKITKCommonPython 
                         vtkVTKITKCommon
                         vtkFilteringPython 
                         vtkCommonPython)
ENDIF (VTK_WRAP_PYTHON)

IF(BORLAND)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w-8004")
ENDIF(BORLAND)

TARGET_LINK_LIBRARIES(vtkVTKITKCommon vtkImaging
                      ITKIO ITKNumerics ITKBasicFilters)

