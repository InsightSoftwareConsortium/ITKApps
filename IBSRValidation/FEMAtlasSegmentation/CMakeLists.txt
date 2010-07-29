PROJECT( FEMAtlasSegmentation )

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build ITKApps without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


SET(FEMBrainStripApp_SRCS
  Code/FEMBrainStripValidationApp.cxx
#  Code/
)

# Header and template source files for FEM project
# we can only include them if compiling on windows
IF(NOT UNIX)
IF(WIN32)
SET(FEMBrainStripApp_SRCS ${FEMBrainStripApp_SRCS}
  Code/FEMValidationInputParser.h
  Code/FEMValidationInputParser.txx
  Code/FEMPreprocessor.h
  Code/FEMPreprocessor.txx
  Code/FEMRegistrator.h
  Code/FEMRegistrator.txx
  Code/FEMAtlasLabeler.h
  Code/FEMAtlasLabeler.txx
  Code/FEMBrainStripValidationApp.h
  Code/FEMBrainStripValidationApp.txx
)
ENDIF(WIN32)
ENDIF(NOT UNIX)

INCLUDE_DIRECTORIES(
${FEMAtlasSegmentation_SOURCE_DIR}/../Common
${FEMAtlasSegmentation_SOURCE_DIR}/../FEMAtlasSegmentation
)

SOURCE_GROUP("Source Files Template" REGULAR_EXPRESSION ./*.txx)


IF(FEM_VISUALIZATION)
# Define the macro that includes all the required functions and include MFC support
ADD_DEFINITIONS( -DFEM_BUILD_VISUALIZATION /D "_AFXDLL" )
ENDIF(FEM_VISUALIZATION)

ADD_EXECUTABLE(FEMBrainStripValidationApp ${FEMBrainStripApp_SRCS} )
INSTALL_TARGETS(/bin FEMBrainStripValidationApp)
TARGET_LINK_LIBRARIES(FEMBrainStripValidationApp ITKBasicFilters ITKNumerics
                                                 ITKFEM ITKIO ITKStatistics)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES (Code/FEMBrainStripValidationApp.cxx PROPERTIES COMPILE_FLAGS -w)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
