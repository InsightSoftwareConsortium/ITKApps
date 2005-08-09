#
# try to find DCMTK libraries
#

# DCMTK_INCLUDE_DIR   - Directories to include to use DCMTK
# DCMTK_LIBRARIES     - Files to link against to use DCMTK
# DCMTK_FOUND         - If false, don't try to use DCMTK
# DCMTK_DIR           - (optional) Source directory for DCMTK
#
# DCMTK_DIR can be used to make it simpler to find the various include
# directories and compiled libraries if you've just compiled it in the
# source tree. Just set it to the root of the tree where you extracted
# the source.
#
# Written for VXL by Amitha Perera.
#

IF(DCMTK_DIR)
  IF(EXISTS ${DCMTK_DIR}/CMakeCache.txt)
    LOAD_CACHE(${DCMTK_DIR} READ_WITH_PREFIX _ dcmtk_SOURCE_DIR)
  ENDIF(EXISTS ${DCMTK_DIR}/CMakeCache.txt)
ENDIF(DCMTK_DIR)

SET(dcmtk_SOURCE_DIR ${_dcmtk_SOURCE_DIR})

FIND_PATH( DCMTK_config_INCLUDE_DIR osconfig.h
  ${DCMTK_DIR}/config/include 
  ${dcmtk_SOURCE_DIR}/config/include 
  ${DCMTK_DIR}/include
)

FIND_PATH( DCMTK_ofstd_INCLUDE_DIR ofstdinc.h
  ${DCMTK_DIR}/ofstd/include 
  ${dcmtk_SOURCE_DIR}/ofstd/include 
  ${DCMTK_DIR}/include/ofstd
)

FIND_LIBRARY( DCMTK_ofstd_LIBRARY ofstd
  ${DCMTK_DIR}/ofstd/libsrc
  ${DCMTK_DIR}/ofstd/libsrc/Release
  ${DCMTK_DIR}/ofstd/libsrc/RelWithDebInfo
  ${DCMTK_DIR}/ofstd/libsrc/Debug
  ${DCMTK_DIR}/ofstd/Release
  ${DCMTK_DIR}/ofstd/Debug
  ${DCMTK_DIR}/lib
)


FIND_PATH( DCMTK_dcmdata_INCLUDE_DIR dctypes.h
  ${DCMTK_DIR}/dcmdata/include
  ${DCMTK_DIR}/include/dcmdata
  ${dcmtk_SOURCE_DIR}/dcmdata/include
)

FIND_LIBRARY( DCMTK_dcmdata_LIBRARY dcmdata
  ${DCMTK_DIR}/dcmdata/libsrc
  ${DCMTK_DIR}/dcmdata/libsrc/Release
  ${DCMTK_DIR}/dcmdata/libsrc/RelWithDebInfo
  ${DCMTK_DIR}/dcmdata/libsrc/Debug
  ${DCMTK_DIR}/dcmdata/Release
  ${DCMTK_DIR}/dcmdata/Debug
  ${DCMTK_DIR}/lib
)


FIND_PATH( DCMTK_dcmimgle_INCLUDE_DIR dcmimage.h
  ${DCMTK_DIR}/dcmimgle/include
  ${dcmtk_SOURCE_DIR}/dcmimgle/include
  ${DCMTK_DIR}/include/dcmimgle
)

FIND_LIBRARY( DCMTK_dcmimgle_LIBRARY dcmimgle
  ${DCMTK_DIR}/dcmimgle/libsrc
  ${DCMTK_DIR}/dcmimgle/libsrc/Release
  ${DCMTK_DIR}/dcmimgle/libsrc/RelWithDebInfo
  ${DCMTK_DIR}/dcmimgle/libsrc/Debug
  ${DCMTK_DIR}/dcmimgle/Release
  ${DCMTK_DIR}/dcmimgle/Debug
  ${DCMTK_DIR}/lib
)

FIND_PATH( DCMTK_imagedb_INCLUDE_DIR imagedb.h
  ${DCMTK_DIR}/imagedb/include
  ${dcmtk_SOURCE_DIR}/imagedb/include
  ${dcmtk_SOURCE_DIR}/imagectn/include
  ${DCMTK_DIR}/imagectn/include
  ${DCMTK_DIR}/imagedb/dcmnet
)

FIND_LIBRARY(DCMTK_imagedb_LIBRARY imagedb 
${DCMTK_DIR}/imagectn/libsrc/Release
${DCMTK_DIR}/imagectn/libsrc/RelWithDebInfo
${DCMTK_DIR}/imagectn/libsrc/
${DCMTK_DIR}/imagectn/libsrc/Debug
)

FIND_PATH( DCMTK_dcmnet_INCLUDE_DIR dicom.h
  ${DCMTK_DIR}/dcmnet/include
  ${dcmtk_SOURCE_DIR}/dcmnet/include
  ${DCMTK_DIR}/include/dcmnet
)


FIND_LIBRARY(DCMTK_dcmnet_LIBRARY dcmnet 
${DCMTK_DIR}/dcmnet/libsrc/Release
${DCMTK_DIR}/dcmnet/libsrc/RelWithDebInfo
${DCMTK_DIR}/dcmnet/libsrc/Debug
${DCMTK_DIR}/dcmnet/libsrc/
)


IF( DCMTK_config_INCLUDE_DIR )
IF( DCMTK_ofstd_INCLUDE_DIR )
IF( DCMTK_ofstd_LIBRARY )
IF( DCMTK_dcmdata_INCLUDE_DIR )
IF( DCMTK_dcmdata_LIBRARY )
IF( DCMTK_dcmimgle_INCLUDE_DIR )
IF( DCMTK_dcmimgle_LIBRARY )

  SET( DCMTK_FOUND "YES" )
  SET( DCMTK_INCLUDE_DIR
    ${DCMTK_config_INCLUDE_DIR}
    ${DCMTK_ofstd_INCLUDE_DIR}
    ${DCMTK_dcmdata_INCLUDE_DIR}
    ${DCMTK_dcmimgle_INCLUDE_DIR}
    ${DCMTK_dcmnet_INCLUDE_DIR}
  )

  SET( DCMTK_LIBRARIES
    ${DCMTK_dcmimgle_LIBRARY}
    ${DCMTK_dcmdata_LIBRARY}
    ${DCMTK_ofstd_LIBRARY}
    ${DCMTK_config_LIBRARY}
  )

  IF(DCMTK_imagedb_LIBRARY)
   SET( DCMTK_LIBRARIES
   ${DCMTK_LIBRARIES}
   ${DCMTK_imagedb_LIBRARY}
   )
  ENDIF(DCMTK_imagedb_LIBRARY)

  IF(DCMTK_dcmnet_LIBRARY)
   SET( DCMTK_LIBRARIES
   ${DCMTK_LIBRARIES}
   ${DCMTK_dcmnet_LIBRARY}
   )
  ENDIF(DCMTK_dcmnet_LIBRARY)

  IF( WIN32 )
    SET( DCMTK_LIBRARIES ${DCMTK_LIBRARIES} netapi32 )
  ENDIF( WIN32 )

ENDIF( DCMTK_dcmimgle_LIBRARY )
ENDIF( DCMTK_dcmimgle_INCLUDE_DIR )
ENDIF( DCMTK_dcmdata_LIBRARY )
ENDIF( DCMTK_dcmdata_INCLUDE_DIR )
ENDIF( DCMTK_ofstd_LIBRARY )
ENDIF( DCMTK_ofstd_INCLUDE_DIR )
ENDIF( DCMTK_config_INCLUDE_DIR )

IF( NOT DCMTK_FOUND )
  SET( DCMTK_DIR "" CACHE PATH "Root of DCMTK source tree (optional)." )
  MARK_AS_ADVANCED( DCMTK_DIR )
ENDIF( NOT DCMTK_FOUND )
