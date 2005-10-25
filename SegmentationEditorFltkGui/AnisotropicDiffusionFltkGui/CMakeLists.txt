INCLUDE_DIRECTORIES(
  ${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_SOURCE_DIR}/SegmentationEditorFltkGui/AnisotropicDiffusionFltkGui
  ${InsightApplications_BINARY_DIR}/SegmentationEditorFltkGui/AnisotropicDiffusionFltkGui
)

SET(AnisotropicDiffusionFltkGui3D_SRCS
liFilterConsole3D.cxx
liFilterConsole3DBase.cxx
Filters3D.cxx
)

SET(AnisotropicDiffusionFltkGui2D_SRCS
liFilterConsole2D.cxx
liFilterConsole2DBase.cxx
Filters2D.cxx
)

SET(AnisotropicDiffusionFltkGui3D_GUI_SRCS
liFilterConsole3DGUI.fl
)

SET(AnisotropicDiffusionFltkGui2D_GUI_SRCS
liFilterConsole2DGUI.fl
)

FLTK_WRAP_UI(AnisotropicDiffusionFltkGui3D   ${AnisotropicDiffusionFltkGui3D_GUI_SRCS})
FLTK_WRAP_UI(AnisotropicDiffusionFltkGui2D   ${AnisotropicDiffusionFltkGui2D_GUI_SRCS})
ITK_DISABLE_FLTK_GENERATED_WARNINGS("${AnisotropicDiffusionFltkGui3D_GUI_SRCS}")
ITK_DISABLE_FLTK_GENERATED_WARNINGS("${AnisotropicDiffusionFltkGui2D_GUI_SRCS}")

ADD_EXECUTABLE(AnisotropicDiffusionFltkGui3D ${AnisotropicDiffusionFltkGui3D_SRCS} ${AnisotropicDiffusionFltkGui3D_FLTK_UI_SRCS})
INSTALL_TARGETS(/bin AnisotropicDiffusionFltkGui3D)
ADD_EXECUTABLE(AnisotropicDiffusionFltkGui2D ${AnisotropicDiffusionFltkGui2D_SRCS} ${AnisotropicDiffusionFltkGui2D_FLTK_UI_SRCS})
INSTALL_TARGETS(/bin AnisotropicDiffusionFltkGui2D)

TARGET_LINK_LIBRARIES(AnisotropicDiffusionFltkGui2D ITKFltkImageViewer ITKIO
                      ITKBasicFilters)
TARGET_LINK_LIBRARIES(AnisotropicDiffusionFltkGui3D ITKFltkImageViewer ITKIO
                      ITKBasicFilters)
