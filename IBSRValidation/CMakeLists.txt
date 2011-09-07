
# Build application for altas-based segmentation of brain volume
SUBDIRS( AtlasSegmentation )

if (ITK_VERSION_MAJOR STRLESS 4)
# Build FEM application for altas-based segmentation of brain volume
SUBDIRS( FEMAtlasSegmentation )
endif()

# Build application for gray, white matter classification of brain volume
SUBDIRS( IBSRClassification )
