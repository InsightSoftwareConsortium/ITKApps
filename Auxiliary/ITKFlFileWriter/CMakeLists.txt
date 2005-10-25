ADD_LIBRARY(ITKFlFileWriter
  Fl_File_ChooserModified.cxx
  Fl_File_ChooserModified2.cxx
  flstring.c
  vsnprintf.c
)
INSTALL_TARGETS(/lib ITKFlFileWriter)

TARGET_LINK_LIBRARIES(ITKFlFileWriter ${FLTK_LIBRARIES})


