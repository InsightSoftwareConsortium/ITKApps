PROJECT(ItkJava)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)


# Load the CableSwig settings used by ITK.
SET(CableSwig_DIR ${ITK_CableSwig_DIR})
FIND_PACKAGE(CableSwig)

INCLUDE_DIRECTORIES(${ITK_INCLUDE_DIRS}
                    ${ItkJava_SOURCE_DIR})

LINK_DIRECTORIES(${ITK_LIBRARY_DIRS})

ADD_LIBRARY(ItkModule itkModule.cxx)
TARGET_LINK_LIBRARIES(ItkModule ITKBasicFilters)


IF(READY_FOR_PRIME_TIME)
  IF(ITK_CSWIG_JAVA)
    SET_SOURCE_FILES_PROPERTIES(itkModuleJava.cxx GENERATED)
    ADD_LIBRARY(ItkModuleJava SHARED itkModuleJava.cxx)
    TARGET_LINK_LIBRARIES(ItkModuleJava ITKBasicFilters
    SwigRuntimceJava ${JAVA_LIBRARY})
    ADD_CUSTOM_COMMAND(
      COMMENT "ItkJava_wrapTcl.cxx from ItkJava.i"
      SOURCE ${ItkJava_SOURCE_DIR}/ItkJava.i
      COMMAND ${CableSwig_cswig_EXE}
      ARGS -I${ItkJava_SOURCE_DIR}
           -nocable -noruntime ${IGNORE_WARNINGS}
           -o ItkJava_wrapTcl.cxx
           -c++ ${ItkJava_SOURCE_DIR}/ItkJava.i
      TARGET ItkModuleJava
      OUTPUTS ${ItkJava_BINARY_DIR}/ItkJava_wrapTcl.cxx
      DEPENDS ${ItkJava_SOURCE_DIR}/ItkJava.i)
  ENDIF(ITK_CSWIG_JAVA)
ENDIF(READY_FOR_PRIME_TIME)
