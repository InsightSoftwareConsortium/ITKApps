ADD_LIBRARY(ITKFlFileWriter
  Fl_File_ChooserModified.cxx
  Fl_File_ChooserModified2.cxx
  vsnprintf.c
)
INSTALL_TARGETS(/lib ITKFlFileWriter)

TARGET_LINK_LIBRARIES(ITKFlFileWriter ${FLTK_LIBRARIES})

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES (Fl_File_ChooserModified.cxx PROPERTIES COMPILE_FLAGS "-Wno-shadow -Wno-cast-qual")
  SET_SOURCE_FILES_PROPERTIES (Fl_File_ChooserModified2.cxx PROPERTIES COMPILE_FLAGS "-Wno-shadow -Wno-cast-qual")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


