PROJECT(MRIBiasCorrection)


IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


INCLUDE_DIRECTORIES(MRIBiasCorrection_SOURCE_DIR)

FOREACH(exe BiasFieldEstimator BiasImageGenerator IntensityHistogram3D
            InterSliceIntensityCorrector SlabIdentifier EnergyTablePlotter
            ImageCorrector BiasCorrector)
  ADD_EXECUTABLE(${exe} ${exe}.cxx OptionList.cxx)
  INSTALL_TARGETS(/bin ${exe})
  INSTALL_TARGETS(/bin ${exe})
  TARGET_LINK_LIBRARIES(${exe} ITKIO ITKBasicFilters ITKStatistics ITKNumerics)
ENDFOREACH(exe)
