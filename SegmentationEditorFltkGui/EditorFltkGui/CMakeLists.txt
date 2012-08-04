PROJECT( EditorFltkGui )

INCLUDE_DIRECTORIES(
${ITKApps_SOURCE_DIR}/Auxiliary/FltkImageViewer
${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
${ITKApps_SOURCE_DIR}/Auxiliary/VtkFltk
${ITKApps_BINARY_DIR}/Auxiliary/VtkFltk
${ITKApps_SOURCE_DIR}/Auxiliary/vtk
${EditorFltkGui_SOURCE_DIR}
${FLTK_INCLUDE_PATH}
${OPENGL_INCLUDE_PATH}
)

LINK_DIRECTORIES(
${ITKApps_BINARY_DIR}/Auxiliary/FltkImageViewer
${ITKApps_BINARY_DIR}/Auxiliary/VtkFltk
${ITKApps_BINARY_DIR}/Auxiliary/vtk
${EditorFltkGui_BINARY_DIR}
)

SET(EditorFltkGui_SRCS
EditorConsole.cxx
EditorConsoleBase.cxx
Editor.cxx
vtkPatchedImageReader.cxx
vtkWSLookupTableManager.cxx
vtkWSBoundingBoxManager.cxx
vtkLookupTableEquivalencyHash.cxx
vtkBoundingBox.cxx
vtkBinaryVolume.cxx
vtkBinaryVolumeLogic.cxx
vtkFlImageViewer.cxx
SegmentedViewer.cxx
SourceViewer.cxx
BinaryViewer.cxx
)

SET(EditorFltkGui_GUI_SRCS
EditorConsoleGUI.fl
)

FLTK_WRAP_UI(EditorFltkGui   ${EditorFltkGui_GUI_SRCS})
ITK_DISABLE_FLTK_GENERATED_WARNINGS("EditorConsole.cxx;${EditorFltkGui_GUI_SRCS}")

ADD_EXECUTABLE(EditorFltkGui ${EditorFltkGui_SRCS} ${EditorFltkGui_FLTK_UI_SRCS})
INSTALL_TARGETS(/bin EditorFltkGui)

TARGET_LINK_LIBRARIES (EditorFltkGui
  ${ITK_LIBRARIES}
  vtkCommon
  vtkFiltering
  vtkImaging
  vtkIO
  vtkRendering
  ITKVtkFltk
  ITKFltkImageViewer
)


