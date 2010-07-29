PROJECT(FEM)

IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build ITKApps without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


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
TARGET_LINK_LIBRARIES (FEMRead
  ITKFEM
  ITKCommon
  ITKNumerics
  ITKBasicFilters
  ITKStatistics
  ITKIO
)


ADD_EXECUTABLE(FEMTruss FEMTruss.cxx)
INSTALL_TARGETS(/bin FEMTruss)
TARGET_LINK_LIBRARIES (FEMTruss
  ITKFEM
  ITKCommon
  ITKNumerics
  ITKBasicFilters
  ITKStatistics
  ITKIO
)

IF(NOT BORLAND)
  ADD_EXECUTABLE(FEMImageRegLMEx ${FEMImageRegLMEx_SRCS})
  INSTALL_TARGETS(/bin FEMImageRegLMEx)
  TARGET_LINK_LIBRARIES (FEMImageRegLMEx
    ITKFEM
    ITKCommon
    ITKNumerics
    ITKBasicFilters
    ITKStatistics
    ITKIO
)
ENDIF(NOT BORLAND)

ADD_EXECUTABLE(FEMSolverHyperbolicExample FEMSolverHyperbolicExample.cxx)
INSTALL_TARGETS(/bin FEMSolverHyperbolicExample)
TARGET_LINK_LIBRARIES (FEMSolverHyperbolicExample
  ITKFEM
  ITKCommon
  ITKNumerics
  ITKBasicFilters
  ITKStatistics
  ITKIO
)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES (FEMSolverHyperbolicExample.cxx PROPERTIES COMPILE_FLAGS -Wno-unused)
  SET_SOURCE_FILES_PROPERTIES (FEMTruss.cxx PROPERTIES COMPILE_FLAGS -Wno-unused)
  SET_SOURCE_FILES_PROPERTIES (FEMRead.cxx PROPERTIES COMPILE_FLAGS -Wno-unused)
  SET_SOURCE_FILES_PROPERTIES (FEMImageRegLMEx.cxx PROPERTIES COMPILE_FLAGS -Wno-unused)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
