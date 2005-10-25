PROJECT( EditorFltkGui )

INCLUDE_DIRECTORIES(
${ITK_SOURCE_DIR}/Code/Common
${ITK_SOURCE_DIR}/Code/BasicFilters
${ITK_SOURCE_DIR}/Code/Algorithms
${ITK_SOURCE_DIR}/Code/IO
${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
${InsightApplications_SOURCE_DIR}/Auxiliary/VtkFltk
${InsightApplications_BINARY_DIR}/Auxiliary/VtkFltk
${InsightApplications_SOURCE_DIR}/Auxiliary/vtk
${VTK_SOURCE_DIR}/Common
${VTK_SOURCE_DIR}/Filtering
${VTK_SOURCE_DIR}/Imaging
${VTK_SOURCE_DIR}/Rendering
${VTK_SOURCE_DIR}/IO
${VTK_SOURCE_DIR}/Patented
${EditorFltkGui_SOURCE_DIR}
${FLTK_INCLUDE_PATH}
${OPENGL_INCLUDE_PATH}
)

LINK_DIRECTORIES(
${ITK_BINARY_DIR}/Code/Common
${ITK_BINARY_DIR}/Code/BasicFilters
${ITK_BINARY_DIR}/Code/Algorithms
${ITK_BINARY_DIR}/Code/IO
${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
${InsightApplications_BINARY_DIR}/Auxiliary/VtkFltk
${InsightApplications_BINARY_DIR}/Auxiliary/vtk
${VTK_BINARY_DIR}/Common
${VTK_BINARY_DIR}/Filtering
${VTK_BINARY_DIR}/Imaging
${VTK_BINARY_DIR}/Rendering
${VTK_BINARY_DIR}/IO
${VTK_BINARY_DIR}/Patented
${EditorFltkGui_BINARY_DIR}
${FLTK_LIBRARY}
${OPENGL_LIBRARY}
${OPENGL_glu_LIBRARY}
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

LINK_LIBRARIES (
ITKBasicFilters
ITKIO
ITKNumerics
vtkCommon
vtkFiltering
vtkImaging
vtkIO
vtkRendering
ITKVtkFltk
ITKFltkImageViewer
)

FLTK_WRAP_UI(EditorFltkGui   ${EditorFltkGui_GUI_SRCS})
ITK_DISABLE_FLTK_GENERATED_WARNINGS("${EditorFltkGui_GUI_SRCS}")

ADD_EXECUTABLE(EditorFltkGui ${EditorFltkGui_SRCS} ${EditorFltkGui_FLTK_UI_SRCS})
INSTALL_TARGETS(/bin EditorFltkGui)

