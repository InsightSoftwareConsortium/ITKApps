# Add an option to build or not build the FltkImageViewer Library
IF(FLTK_FOUND)
  SUBDIRS(FltkImageViewer ITKFlFileWriter)
  IF(VTK_FOUND)
    SUBDIRS(VtkFltk)
  ENDIF(VTK_FOUND)
ENDIF(FLTK_FOUND)

# Add an option to build or not build the vtk interface Library
IF(NOT BORLAND)
IF(VTK_FOUND)
  SUBDIRS(vtk)
ENDIF(VTK_FOUND)
ENDIF(NOT BORLAND)
