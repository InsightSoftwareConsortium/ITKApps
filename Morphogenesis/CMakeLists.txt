PROJECT(Morphogenesis)


IF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )

  FIND_PACKAGE(ITK)
  IF(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
  ELSE(ITK_FOUND)
    MESSAGE(FATAL_ERROR
            "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
  ENDIF(ITK_FOUND)

  FIND_PACKAGE(FLTK)
  IF(FLTK_FOUND)
    INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
  ENDIF(FLTK_FOUND)

ENDIF( BUILD_OUTSIDE_INSIGHT_APPLICATIONS )


INCLUDE_DIRECTORIES(
  ${InsightApplications_SOURCE_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_BINARY_DIR}/Auxiliary/FltkImageViewer
  ${InsightApplications_SOURCE_DIR}/Morphogenesis
  ${InsightApplications_BINARY_DIR}/Morphogenesis
)

FLTK_WRAP_UI(Morphogenesis BacterialColonyGUI.fl CellsViewerGUI.fl)

SET(Morphogenesis_SRCS
  ${Morphogenesis_FLTK_UI_SRCS}
  Bacteria.cxx
  BacterialColony.cxx
  Cell.cxx
  CellsViewerBase.cxx
  CellsViewer.cxx
  CellsViewerCommand.cxx
  CellularAggregate.cxx
  Eukariote.cxx
  GradientEatingBacteria.cxx
  BilayerOrganism.cxx
  Genome.cxx
  PressureSensitiveBacteria.cxx
  Prokariote.cxx
  main.cxx
)


ADD_GUI_EXECUTABLE(Morphogenesis "${Morphogenesis_SRCS}")

ITK_DISABLE_FLTK_GENERATED_WARNINGS("BacterialColonyGUI.fl;CellsViewerGUI.fl")

TARGET_LINK_LIBRARIES(Morphogenesis ITKFltkImageViewer ITKBasicFilters ITKIO)

