PROJECT(FEM)

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


LINK_LIBRARIES (
  ITKFEM
  ITKCommon
  ITKNumerics
  ITKBasicFilters
  ITKStatistics
  ITKIO
)

IF(FEM_VISUALIZATION)
# Define the macro that includes all the required functions and include MFC support
ADD_DEFINITIONS( -DFEM_BUILD_VISUALIZATION /D "_AFXDLL" )
ENDIF(FEM_VISUALIZATION)

SET(FEMImageRegLMEx_SRCS
  FEMImageRegLMEx.cxx
)

# Header and template source files for FEM project
# we can only include them if compiling on windows
IF(NOT UNIX)
IF(WIN32)
SET(FEMImageRegLMEx_SRCS ${FEMImageRegLMEx_SRCS}
  FEMImageRegLMEx.h
)
ENDIF(WIN32)
ENDIF(NOT UNIX)




# Create project files for all examples
ADD_EXECUTABLE(FEMRead FEMRead.cxx)
INSTALL_TARGETS(/bin FEMRead)
ADD_EXECUTABLE(FEMTruss FEMTruss.cxx)
INSTALL_TARGETS(/bin FEMTruss)
IF(NOT BORLAND)
  ADD_EXECUTABLE(FEMImageRegLMEx ${FEMImageRegLMEx_SRCS})
  INSTALL_TARGETS(/bin FEMImageRegLMEx)
ENDIF(NOT BORLAND)
ADD_EXECUTABLE(FEMSolverHyperbolicExample FEMSolverHyperbolicExample.cxx)
INSTALL_TARGETS(/bin FEMSolverHyperbolicExample)

