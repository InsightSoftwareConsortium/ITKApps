#############################################
# PROJECT: SNAP                             #
#############################################
PROJECT(SNAP)

#############################################
# SOURCE FILE SPECIFICATION                 #
#############################################

# The source files that form the application
SET(COMMON_CXX
  Common/CommandLineArgumentParser.cxx
  Common/IRISException.cxx
  Logic/Common/ImageCoordinateTransform.cxx
  Logic/Framework/GlobalState.cxx
  Logic/Framework/IRISApplication.cxx
  Logic/Framework/IRISImageData.cxx
  Logic/Framework/SNAPImageData.cxx
  Logic/ImageWrapper/GreyImageWrapperImplementation.cxx
  Logic/ImageWrapper/LabelImageWrapperImplementation.cxx
  Logic/ImageWrapper/LevelSetImageWrapperImplementation.cxx
  Logic/ImageWrapper/SpeedImageWrapperImplementation.cxx
  Logic/LevelSet/SnakeParameters.cxx
  Logic/LevelSet/SNAPLevelSetDriver.cxx
  Logic/Mesh/IRISMeshPipeline.cxx
  Logic/Mesh/LevelSetMeshPipeline.cxx
  Logic/Mesh/MeshObject.cxx
  Logic/Mesh/MeshOptions.cxx
  Logic/Preprocessing/EdgePreprocessingSettings.cxx
  Logic/Preprocessing/ThresholdSettings.cxx
  Logic/Slicing/IntensityCurveVTK.cxx
  UserInterface/BasicComponents/FLTKCanvas.cxx
  UserInterface/BasicComponents/FunctionPlot2DBox.cxx
  UserInterface/BasicComponents/FunctionPlot2D.cxx
  UserInterface/MainComponents/IntensityCurveBox.cxx
  UserInterface/MainComponents/IntensityCurveUILogic.cxx
  UserInterface/MainComponents/PreprocessingUILogic.cxx
  UserInterface/MainComponents/UserInterfaceLogic.cxx
  UserInterface/SliceWindow/BubblesInteractionMode.cxx
  UserInterface/SliceWindow/CrosshairsInteractionMode.cxx
  UserInterface/SliceWindow/GenericSliceWindow.cxx
  UserInterface/SliceWindow/IRISSliceWindow.cxx
  UserInterface/SliceWindow/PolygonDrawing.cxx
  UserInterface/SliceWindow/PolygonInteractionMode.cxx
  UserInterface/SliceWindow/RegionInteractionMode.cxx
  UserInterface/SliceWindow/SNAPSliceWindow.cxx
  UserInterface/SliceWindow/ZoomPanInteractionMode.cxx
  UserInterface/Window3D/Trackball.cxx
  UserInterface/Window3D/Window3D.cxx
)

# The source files for the SNAP application
SET(APPLICATION_CXX
  UserInterface/SNAPMain.cxx
)

# The source files for testing
SET(TESTING_CXX
  Testing/Compatibility/SnakeWrapper.cxx
  Testing/GUITestPadLogic.cxx
  Testing/ImageWrapperTest.cxx
  Testing/SnakeTest.cxx
  Testing/TestMain.cxx
)

# The header and template code files
SET(COMMON_HEADERS
  Common/CommandLineArgumentParser.h
  Common/Credits.h
  Common/IRISException.h
  Common/IRISTypes.h
  Common/IRISVectorTypes.h
  Common/IRISVectorTypesToITKConversion.h
  Common/IRISVectorTypes.txx
  Logic/Common/ColorLabel.h
  Logic/Common/ImageCoordinateTransform.h
  Logic/Common/ImageRayIntersectionFinder.h
  Logic/Common/ImageRayIntersectionFinder.txx
  Logic/Framework/GlobalState.h
  Logic/Framework/IRISApplication.h
  Logic/Framework/IRISImageData.h
  Logic/Framework/SNAPImageData.h
  Logic/ImageWrapper/GreyImageWrapper.h
  Logic/ImageWrapper/GreyImageWrapperImplementation.h
  Logic/ImageWrapper/ImageWrapper.h
  Logic/ImageWrapper/ImageWrapperImplementation.h
  Logic/ImageWrapper/ImageWrapperImplementation.txx
  Logic/ImageWrapper/LabelImageWrapper.h
  Logic/ImageWrapper/LabelImageWrapperImplementation.h
  Logic/ImageWrapper/LevelSetImageWrapper.h
  Logic/ImageWrapper/LevelSetImageWrapperImplementation.h
  Logic/ImageWrapper/SpeedImageWrapper.h
  Logic/ImageWrapper/SpeedImageWrapperImplementation.h
  Logic/LevelSet/SnakeParameters.h
  Logic/LevelSet/SNAPAdvectionFieldImageFilter.h
  Logic/LevelSet/SNAPAdvectionFieldImageFilter.txx
  Logic/LevelSet/SNAPLevelSetDriver.h
  Logic/LevelSet/SNAPLevelSetFunction.h
  Logic/LevelSet/SNAPLevelSetFunction.txx
  Logic/LevelSet/SNAPLevelSetStopAndGoFilter.h
  Logic/LevelSet/SNAPLevelSetStopAndGoFilter.txx
  Logic/Mesh/IRISMeshPipeline.h
  Logic/Mesh/LevelSetMeshPipeline.h
  Logic/Mesh/MeshObject.h
  Logic/Mesh/MeshOptions.h
  Logic/Preprocessing/EdgePreprocessingImageFilter.h
  Logic/Preprocessing/EdgePreprocessingImageFilter.txx
  Logic/Preprocessing/EdgePreprocessingSettings.h
  Logic/Preprocessing/SmoothBinaryThresholdImageFilter.h
  Logic/Preprocessing/SmoothBinaryThresholdImageFilter.txx
  Logic/Preprocessing/ThresholdSettings.h
  Logic/Slicing/IntensityCurveInterface.h
  Logic/Slicing/IntensityCurveVTK.h
  Logic/Slicing/IRISSlicer.h
  Logic/Slicing/IRISSlicer.txx
  Logic/Slicing/UnaryFunctorCache.h
  Logic/Slicing/UnaryFunctorCache.txx
  UserInterface/BasicComponents/FLTKCanvas.h
  UserInterface/BasicComponents/FLTKEvent.h
  UserInterface/BasicComponents/FunctionPlot2DBox.h
  UserInterface/BasicComponents/FunctionPlot2D.h
  UserInterface/BasicComponents/InteractionMode.h
  UserInterface/MainComponents/ImageIOWizardBase.h
  UserInterface/MainComponents/ImageIOWizardLogic.h
  UserInterface/MainComponents/ImageIOWizardLogic.txx
  UserInterface/MainComponents/IntensityCurveBox.h
  UserInterface/MainComponents/IntensityCurveUIBase.h
  UserInterface/MainComponents/IntensityCurveUILogic.h
  UserInterface/MainComponents/PreprocessingUIBase.h
  UserInterface/MainComponents/PreprocessingUILogic.h
  UserInterface/MainComponents/UserInterfaceBase.h
  UserInterface/MainComponents/UserInterfaceLogic.h
  UserInterface/SliceWindow/BubblesInteractionMode.h
  UserInterface/SliceWindow/CrosshairsInteractionMode.h
  UserInterface/SliceWindow/GenericSliceWindow.h
  UserInterface/SliceWindow/IRISSliceWindow.h
  UserInterface/SliceWindow/OpenGLSliceTexture.h
  UserInterface/SliceWindow/PolygonDrawing.h
  UserInterface/SliceWindow/PolygonInteractionMode.h
  UserInterface/SliceWindow/RegionInteractionMode.h
  UserInterface/SliceWindow/SNAPSliceWindow.h
  UserInterface/SliceWindow/ZoomPanInteractionMode.h
  UserInterface/Window3D/Trackball.h
  UserInterface/Window3D/Window3D.h
)

# The headers for the testing code
SET(TESTING_HEADERS
  Testing/Compatibility/SnakeWrapper.h
  Testing/GUITestPadBase.h
  Testing/GUITestPadLogic.h
)

# The FL files for SNAP
SET(COMMON_FLUIDS
  UserInterface/MainComponents/ImageIOWizard.fl
  UserInterface/MainComponents/IntensityCurveUI.fl
  UserInterface/MainComponents/PreprocessingUI.fl
  UserInterface/MainComponents/UserInterface.fl
)

# The FL files for testing
SET(TESTING_FLUIDS
  Testing/GUITestPad.fl
)

# Documentation HTML files
SET(DOCUMENTATION_HTML
  UserInterface/HTMLHelp/Tutorial.html
)

# Combined sources for the SNAP application
SET(APPLICATION_SRCS
  ${COMMON_CXX} 
  ${APPLICATION_CXX}
  ${COMMON_HEADERS}
)

# Combined sources for the testing application
SET(TESTING_SRCS
  ${COMMON_CXX}
  ${TESTING_CXX}
  ${COMMON_HEADERS}
  ${TESTING_HEADERS}
)

# The SNAP executable
ADD_GUI_EXECUTABLE(SNAP "${APPLICATION_SRCS}")

# The testing executable
ADD_GUI_EXECUTABLE(SNAPTests "${TESTING_SRCS}")

# FLTK Wrapping
FLTK_WRAP_UI(SNAP ${COMMON_FLUIDS})
FLTK_WRAP_UI(SNAPTests ${COMMON_FLUIDS} ${TESTING_FLUIDS})

SET(ALL_FLUIDS ${COMMON_FLUIDS} ${TESTING_FLUIDS})
ITK_DISABLE_FLTK_GENERATED_WARNINGS("${ALL_FLUIDS}")

#############################################
# LIBRARIES AND EXTERNAL CODE               #
#############################################

# Shorthand to source and binary directories
SET(MY_SOURCE_DIR ${InsightApplications_SOURCE_DIR}/SNAP)
SET(MY_BINARY_DIR ${InsightApplications_BINARY_DIR}/SNAP)

# Include directories
INCLUDE_DIRECTORIES(
  ${MY_SOURCE_DIR}/Common
  ${MY_SOURCE_DIR}/Logic
  ${MY_SOURCE_DIR}/Logic/Common
  ${MY_SOURCE_DIR}/Logic/Framework
  ${MY_SOURCE_DIR}/Logic/ImageWrapper
  ${MY_SOURCE_DIR}/Logic/LevelSet
  ${MY_SOURCE_DIR}/Logic/Mesh
  ${MY_SOURCE_DIR}/Logic/Preprocessing
  ${MY_SOURCE_DIR}/Logic/Slicing
  ${MY_SOURCE_DIR}/Testing
  ${MY_SOURCE_DIR}/Testing/Compatibility
  ${MY_SOURCE_DIR}/Testing/Compatibility/CVS
  ${MY_SOURCE_DIR}/UserInterface
  ${MY_SOURCE_DIR}/UserInterface/BasicComponents
  ${MY_SOURCE_DIR}/UserInterface/HTMLHelp
  ${MY_SOURCE_DIR}/UserInterface/MainComponents
  ${MY_SOURCE_DIR}/UserInterface/MainComponents/Artwork
  ${MY_SOURCE_DIR}/UserInterface/SliceWindow
  ${MY_SOURCE_DIR}/UserInterface/Window3D
  ${MY_BINARY_DIR}/Testing
  ${MY_BINARY_DIR}/UserInterface/MainComponents
  ${FLTK_INCLUDE_PATH}
  ${OPENGL_INCLUDE_PATH}
)

# Link libraries
SET(MY_GRAPHICS_LIBS
  vtkCommon
  vtkFiltering
  vtkGraphics
  vtkHybrid
  vtkImaging
  vtkPatented
  vtkRendering
  ${FLTK_LIBRARIES} 
  ${OPENGL_glu_LIBRARY}
  ${OPENGL_LIBRARIES}
)

SET(MY_ITK_LIBS
  ITKIO
)

TARGET_LINK_LIBRARIES(SNAP ${MY_GRAPHICS_LIBS} ${MY_ITK_LIBS})
TARGET_LINK_LIBRARIES(SNAPTests ${MY_GRAPHICS_LIBS} ${MY_ITK_LIBS})

# Copy documentation from the source tree to the build tree
FOREACH(DOCFILE ${DOCUMENTATION_HTML})
  CONFIGURE_FILE(
    ${MY_SOURCE_DIR}/${DOCFILE}
    ${MY_BINARY_DIR}/${DOCFILE}
    COPYONLY)
ENDFOREACH(DOCFILE)
