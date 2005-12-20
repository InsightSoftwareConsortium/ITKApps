CMAKE_MINIMUM_REQUIRED(VERSION 1.6)
PROJECT(InsightApplications)


INCLUDE_REGULAR_EXPRESSION("^.*$")

# Tell CMake's FIND_PACKAGE command where to look for our Find<pkg>
# modules.
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${InsightApplications_SOURCE_DIR})

IF(NOT WIN32)
  OPTION(BUILD_SHARED_LIBS "Build Insight-Applications with shared libraries." OFF)
ENDIF(NOT WIN32)

# Don't build anything unless the version of CMake is high enough.
# The matching ELSE/ENDIF should be the last lines in the file.
IF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 1.5)

FIND_PACKAGE(ITK)
IF(ITK_FOUND)
  INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

#-----------------------------------------------------------------------------
# Configure Dart testing support.
INCLUDE(${CMAKE_ROOT}/Modules/Dart.cmake)
MARK_AS_ADVANCED(TCL_TCLSH)
IF(BUILD_TESTING)
  ENABLE_TESTING()
ENDIF(BUILD_TESTING)

#-----------------------------------------------------------------------------
# ITK build patented algorithms
OPTION(ITK_USE_PATENTED "Build algorithms that are covered with Patents. It is YOUR responsibility to obbtain a license from the patent holder in order to user these methods." OFF)
MARK_AS_ADVANCED(ITK_USE_PATENTED)

# Determine what packages are available
# Add an option to use or not use VTK
OPTION(USE_VTK "Use VTK (The Visualization Toolkit) (some applications need this)" OFF)
IF(USE_VTK)
  FIND_PACKAGE(VTK)
  IF (VTK_FOUND)
    INCLUDE (${VTK_USE_FILE})

    # Test for VTK using std libs
    IF(NOT VTK_USE_ANSI_STDLIB)
      MESSAGE("Warning.  Your VTK was not built with the VTK_USE_ANSI_STDLIB "
              "option ON.  Link errors may occur.  Consider re-building VTK "
              "with this option ON.  For MSVC 6, you MUST turn on the VTK "
              "option.  ITK will not link otherwise.")
    ENDIF(NOT VTK_USE_ANSI_STDLIB)

    # Test for VTK building Hybrid
    IF(NOT VTK_USE_HYBRID)
      MESSAGE("Your VTK was not built with the VTK_USE_HYBRID option ON.  "
              "Please reconfigure and recompile VTK with this option before "
              "trying to use it with ITK.")
      SET(VTK_FOUND 0)
    ENDIF(NOT VTK_USE_HYBRID)

    # If VTK was built using Patented, add an option for using patented code
    # in InsightApplications (SNAP in particular)
    IF(VTK_USE_PATENTED)
      OPTION(USE_VTK_PATENTED "Use VTK Patented Libraries (optional)" OFF)
      MARK_AS_ADVANCED(USE_VTK_PATENTED)
      IF(USE_VTK_PATENTED)
        ADD_DEFINITIONS(-DUSE_VTK_PATENTED)
      ENDIF(USE_VTK_PATENTED)
    ENDIF(VTK_USE_PATENTED)
  ENDIF (VTK_FOUND)
ENDIF(USE_VTK)

# Look for OpenGL.
FIND_PACKAGE(OpenGL)

IF(OPENGL_INCLUDE_PATH)
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_PATH})
ENDIF(OPENGL_INCLUDE_PATH)

# Add an option to use or not use FLTK (http://www.fltk.org)
OPTION(USE_FLTK "Use FLTK (The Fast Light Toolkit) for GUI (some applications need this)" OFF)
IF(USE_FLTK)
  FIND_PACKAGE(FLTK)
  IF(FLTK_FOUND)
    INCLUDE_DIRECTORIES(${FLTK_INCLUDE_DIR})
  ENDIF(FLTK_FOUND)
ENDIF(USE_FLTK)

# The fluid-generated fltk sources have many warnings.  This macro
# will disable warnings for the generated files on some compilers.
MACRO(ITK_DISABLE_FLTK_GENERATED_WARNINGS files)
  IF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.6)
    IF(CMAKE_COMPILER_IS_GNUCXX)
      FOREACH(f ${files})
        STRING(REGEX REPLACE "\\.fl$" ".cxx" SRC "${f}")
        STRING(REGEX REPLACE ".*/([^/]*)$" "\\1" SRC "${SRC}")
        SET_SOURCE_FILES_PROPERTIES(${SRC} PROPERTIES COMPILE_FLAGS -w)
      ENDFOREACH(f)
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
  ENDIF(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 1.6)
ENDMACRO(ITK_DISABLE_FLTK_GENERATED_WARNINGS)

# Macro for adding resources to GUI application on the Mac
#
IF(APPLE AND NOT FLTK_USE_X)
    FIND_PROGRAM(ITK_APPLE_RESOURCE Rez /Developer/Tools)
    FIND_FILE(ITK_FLTK_RESOURCE mac.r /usr/local/include/FL)
    IF(NOT ITK_FLTK_RESOURCE)
      MESSAGE("Fltk resources not found, GUI application will not respond to mouse events")
    ENDIF(NOT ITK_FLTK_RESOURCE)

    MACRO(ADD_GUI_EXECUTABLE name sources)
      ADD_EXECUTABLE(${name} ${sources})
      INSTALL_TARGETS(/bin ${name})
      SET(EXEC_PATH ${EXECUTABLE_OUTPUT_PATH})
      IF(NOT EXEC_PATH)
        SET(EXEC_PATH ${CMAKE_CURRENT_BINARY_DIR})
      ENDIF(NOT EXEC_PATH)
        IF(ITK_APPLE_RESOURCE)
          ADD_CUSTOM_COMMAND(SOURCE ${name}
                             COMMAND ${ITK_APPLE_RESOURCE}
                             ARGS -t APPL ${ITK_FLTK_RESOURCE} -o
                             ${EXEC_PATH}/${name}
                             TARGET ${name})
        ENDIF(ITK_APPLE_RESOURCE)
    ENDMACRO(ADD_GUI_EXECUTABLE)
ELSE(APPLE AND NOT FLTK_USE_X)
  MACRO(ADD_GUI_EXECUTABLE name sources)
    ADD_EXECUTABLE(${name} ${sources})
    INSTALL_TARGETS(/bin ${name})
  ENDMACRO(ADD_GUI_EXECUTABLE)
ENDIF(APPLE AND NOT FLTK_USE_X)

#This macro allows finer grained control over what gets built by default
MACRO(OPTIONAL_APP ApplicationSourceDirectoryName)
  OPTION(USE_${ApplicationSourceDirectoryName} "Build optional package ${ApplicationSourceDirectoryName}." ON)
  IF(USE_${ApplicationSourceDirectoryName})
    SUBDIRS(${ApplicationSourceDirectoryName})
  ENDIF(USE_${ApplicationSourceDirectoryName})
ENDMACRO(OPTIONAL_APP ApplicationSourceDirectoryName)


#Build the Auxiliary directory on which many applications depend
OPTIONAL_APP(Auxiliary)

#Build the example of how to do simple scalar anisotropic diffusion
#on a PNG image
OPTIONAL_APP (AnisotropicDiffusionImageFilter)

#Build the example of various methods of segmentation using level sets.
OPTIONAL_APP (LevelSetSegmentation)

#Build the example of how to create a static library for
# select filters in itk.
OPTIONAL_APP (ITKFilterLib)

#Build the example of how to create a static library for
# the multi-resolution mutual information registration framework in itk.
OPTIONAL_APP (ITKRegistrationLib)

#Build the parameter file parser necessary for several applications that follow
OPTIONAL_APP(StreamedWatershedSegmentation/ParameterFileParser)

# Build the StreamedWatershedSegmentation example
#OPTIONAL_APP(StreamedWatershedSegmentation)

IF(NOT BORLAND)
# Test the conditions for building the MRIBiasCorrection example
OPTIONAL_APP(MRIBiasCorrection)

# Add an option to build or not build the
# GaussianMinimumErrorClassifier example
OPTIONAL_APP(GaussianMinimumErrorClassifier)

# Build the AntiAliasBinaryImageFilter example
OPTIONAL_APP(AntiAliasBinaryImageFilter)

# Build the SimpleLevelSetsExample
OPTIONAL_APP(SimpleLevelSetsExample)

# Build the IBSR Validation applications
OPTIONAL_APP(IBSRValidation)

# Build the multichannel tissue classification validation
OPTIONAL_APP(MultichannelTissueClassificationValidation)

# Build the DICOMApp example to output raw and png versions of DICOM file
OPTIONAL_APP(DICOMApp)

#Build the examples of various fourth-order level set PDEs
OPTIONAL_APP (LevelSetSurfaceProcessing)

# Shared libs are needed for plugins
# Build Volview plugins. You can't use them unless you have volview
IF(BUILD_SHARED_LIBS)
  OPTIONAL_APP ( VolviewPlugIns )
ENDIF(BUILD_SHARED_LIBS)
ENDIF(NOT BORLAND)


# Build the FloodFilledSpatialFunctionExample
OPTIONAL_APP(FloodFilledSpatialFunction)

# Build the Multiresolution Registration example
OPTIONAL_APP(MultiResMIRegistration)

# Build the MI Validation application
OPTIONAL_APP(MIValidation)

# Build the Intensity based 2D-3D Registration application
OPTIONAL_APP(IntensityBased2D3DRegistration)

# Applications requiring MetaImages and Fltk GUI
IF(FLTK_FOUND)
  IF(WIN32)
    ADD_DEFINITIONS(-DWIN32)
  ENDIF(WIN32)
  OPTIONAL_APP(ConvertBetweenFileFormats)
  OPTIONAL_APP(DigitalArchive)
  OPTIONAL_APP(DistanceMapFilter)
  OPTIONAL_APP(DicomImageViewer)
  OPTIONAL_APP(GaussianFilter)
  OPTIONAL_APP(Curves2DExtractor)
  OPTIONAL_APP(Curves3DExtractor)
  OPTIONAL_APP(DuctExtractor)
  OPTIONAL_APP(Morphogenesis)
  OPTIONAL_APP(MutualInformationEuler2DRegistration)
  OPTIONAL_APP(ImageRegistration)
  OPTIONAL_APP(LandmarkInitializedMutualInformationRegistration)
  OPTIONAL_APP(RawImageReaderViewer)
  OPTIONAL_APP(VoronoiSegmentation)
  IF(ITK_USE_PATENTED)
    OPTIONAL_APP(SimpleFuzzyConnectedness)
    OPTIONAL_APP(RegionGrowingSegmentation)
  ENDIF(ITK_USE_PATENTED)
  OPTIONAL_APP(ThresholdSegmentationLevelSetFltkGui)
  IF(VTK_FOUND)
    OPTIONAL_APP(ThresholdSegmentationLevelSet)
    OPTIONAL_APP(FastMarchingLevelSet)
    OPTIONAL_APP(ShapeDetectionLevelSet)
    OPTIONAL_APP(GeodesicActiveContour)
    OPTIONAL_APP(SegmentationEditorFltkGui)
    OPTIONAL_APP(SNAP)
    OPTIONAL_APP(LiverTumorSegmentation)
    IF(NOT ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION} LESS 4.5)
      OPTIONAL_APP(DeformableModelSimplexMesh)
      OPTIONAL_APP(DeformableRegistration3DTimeSeries)
    ENDIF(NOT ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION} LESS 4.5)
    OPTIONAL_APP(CellularSegmentation)
    OPTIONAL_APP(ImageRegistration2D)
  ENDIF(VTK_FOUND)
  OPTIONAL_APP(ImageViewer)
  OPTIONAL_APP(ImageColorViewer)
ENDIF(FLTK_FOUND)

# MetaImage Utilities
OPTIONAL_APP(MetaImageImporter)
OPTIONAL_APP(MetaImageReadWrite)

# Add an option to build FEM applications
OPTIONAL_APP(FEM)

# Add an option to build or not build the FuzzyConnectedness example
IF(ITK_USE_PATENTED)
  OPTIONAL_APP(FuzzyConnectedness)
ENDIF(ITK_USE_PATENTED)


# Add an option to build or not build the EllipsoidInteriorExteriorSpatialFunction example
OPTIONAL_APP(EllipsoidInteriorExteriorSpatialFunction)

# Add an option to build or not build the SymmetricEllipsoidInteriorExteriorSpatialFunction example
OPTIONAL_APP(SymmetricEllipsoidInteriorExteriorSpatialFunction)

# Test the conditions for building the ThinPlateSplines example
IF(VTK_FOUND)
  IF(FLTK_FOUND)
    OPTIONAL_APP(ThinPlateSplines)
  ENDIF(FLTK_FOUND)

  IF(VTK_WRAP_TCL)
    OPTIONAL_APP(MRIRegistration)
    IF(NOT BORLAND)
      OPTIONAL_APP(SegmentationEditor)
      OPTIONAL_APP(vtkITK)
    ENDIF(NOT BORLAND)
  ENDIF(VTK_WRAP_TCL)

  SET (VTK_AND_ITK_WRAPPED 0)
  IF(VTK_WRAP_PYTHON AND ITK_CSWIG_PYTHON)
    SET(VTK_AND_ITK_WRAPPED 1)
  ENDIF(VTK_WRAP_PYTHON AND ITK_CSWIG_PYTHON)
  IF(VTK_WRAP_TCL AND ITK_CSWIG_TCL)
    SET(VTK_AND_ITK_WRAPPED 1)
  ENDIF(VTK_WRAP_TCL AND ITK_CSWIG_TCL)

  IF(VTK_AND_ITK_WRAPPED)
    OPTIONAL_APP(ConnectVTKITK)
  ENDIF(VTK_AND_ITK_WRAPPED)

ENDIF(VTK_FOUND)

LINK_LIBRARIES (
ITKAlgorithms
ITKCommon
ITKBasicFilters
)

# If the version of CMake was too old, complain and build nothing.
# These should be the last lines in this file.
ELSE("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 1.5)
  MESSAGE(SEND_ERROR
          "This version of CMake is too old to build InsightApplications.  "
          "Please upgrade to CMake 1.6.")
ENDIF("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 1.5)
