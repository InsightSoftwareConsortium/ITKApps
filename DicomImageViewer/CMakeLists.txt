PROJECT(DicomImageViewer)

INCLUDE_DIRECTORIES (
  ${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
)

LINK_LIBRARIES (
  ITKBasicFilters
  ITKIO
  ITKFltkImageViewer
)


ADD_GUI_EXECUTABLE( DicomImageViewer DicomImageViewer.cxx )


