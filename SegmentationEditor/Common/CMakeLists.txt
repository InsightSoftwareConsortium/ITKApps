SET(WSTCommon_SRCS
  vtkBinaryVolume.cxx
  vtkBinaryVolumeLogic.cxx
  vtkWSBoundingBox.cxx
  vtkITKWatershedFilterAndWriter.h
  vtkLookupTableEquivalencyHash.cxx
  vtkWSBoundingBoxManager.cxx
  vtkWSLookupTableManager.cxx
)

SET_SOURCE_FILES_PROPERTIES(vtkLookupTableEquivalencyHash.cxx vtkITKWatershedFilterAndWriter.h WRAP_EXCLUDE)

ADD_LIBRARY(vtkWSTCommon  ${WSTCommon_SRCS})
INSTALL_TARGETS(/lib vtkWSTCommon)
TARGET_LINK_LIBRARIES(vtkWSTCommon vtkVTKITKCommon)

# Configure Tcl wrappers.
VTK_WRAP_TCL2(vtkWSTCommonTCL WSTCommonTCL_SRCS ${WSTCommon_SRCS})
ADD_LIBRARY(vtkWSTCommonTCL  ${WSTCommonTCL_SRCS})
INSTALL_TARGETS(/lib vtkWSTCommonTCL)

TARGET_LINK_LIBRARIES(vtkWSTCommonTCL vtkWSTCommon vtkFilteringTCL
                      vtkCommonTCL)

IF(BORLAND)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w-8004")
ENDIF(BORLAND)

