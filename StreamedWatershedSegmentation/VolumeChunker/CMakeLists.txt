# Sources of non-templated classes.
SOURCE_FILES(chunk_SRCS
chunks.cxx
)

ADD_LIBRARY(chunks  chunk_SRCS)
INSTALL_TARGETS(/lib chunks)

INCLUDE_DIRECTORIES(
${StreamedWatershedSegmentation_SOURCE_DIR}/ParameterFileParser
${StreamedWatershedSegmentation_SOURCE_DIR}/VolumeChunker
)

LINK_DIRECTORIES(
${StreamedWatershedSegmentation_SOURCE_DIR}/ParameterFileParser
${StreamedWatershedSegmentation_SOURCE_DIR}/VolumeChunker
)

LINK_LIBRARIES (
chunks
param
)

ADD_EXECUTABLE(catchunkfile catchunkfile)
INSTALL_TARGETS(/bin catchunkfile)
ADD_EXECUTABLE(chunker chunker)
INSTALL_TARGETS(/bin chunker)
ADD_EXECUTABLE(vhchunker vhchunker)
INSTALL_TARGETS(/bin vhchunker)
ADD_EXECUTABLE(vhunchunker vhunchunker)
INSTALL_TARGETS(/bin vhunchunker)
