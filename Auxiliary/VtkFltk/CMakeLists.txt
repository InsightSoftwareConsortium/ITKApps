# This directory only builds if FLTK and VTK can be found
INCLUDE_DIRECTORIES(
  ${ITKApps_SOURCE_DIR}/Auxiliary/VtkFltk
  ${ITKApps_SOURCE_DIR}/Auxiliary/vtk
)

FLTK_WRAP_UI(ITKVtkFltk fltkVTKImageViewerGUI.fl)

ITK_DISABLE_FLTK_GENERATED_WARNINGS(
  "vtkFlRenderWindowInteractor.cxx;${ITKVtkFltk_FLTK_UI_SRCS}"
)


ADD_LIBRARY(ITKVtkFltk
  vtkFlRenderWindowInteractor.cxx
  fltkVTKImageViewerBase.cxx
  ${ITKVtkFltk_FLTK_UI_SRCS}
)
INSTALL_TARGETS(/lib ITKVtkFltk)

TARGET_LINK_LIBRARIES(ITKVtkFltk vtkRendering ${FLTK_LIBRARIES} ${OPENGL_glu_LIBRARY} ${OPENGL_gl_LIBRARY})

ADD_EXECUTABLE( vtkFltkExample vtkFlRenderWindowInteractorExample.cxx)
TARGET_LINK_LIBRARIES( vtkFltkExample ITKVtkFltk )

IF(VTK_USE_VOLUMERENDERING)
  TARGET_LINK_LIBRARIES(ITKVtkFltk vtkVolumeRendering)
ENDIF(VTK_USE_VOLUMERENDERING)
