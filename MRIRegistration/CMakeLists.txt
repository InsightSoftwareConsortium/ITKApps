PROJECT(MIRegistration)



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


LINK_LIBRARIES( 
  vtkRendering
  vtkGraphics	  
  vtkImaging
  vtkIO
  vtkFiltering
  vtkCommon
)

LINK_LIBRARIES(ITKCommon ITKIO ITKNumerics)

SET(MIRegistration_SRCS
OptionList
MIRegistration
)

ADD_EXECUTABLE(MIRegistration ${MIRegistration_SRCS})
