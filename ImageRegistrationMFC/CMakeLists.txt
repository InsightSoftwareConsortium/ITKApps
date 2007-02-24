
PROJECT( ItkRegMFC )

#ADD_DEFINITIONS(-D_AFXDLL)

SET(CMAKE_MFC_FLAG 2)


SET(ItkRegMFC_SRCS
   stdAfx.cpp
   stdAfx.h
   ItkRegMFC.cpp
   ItkRegMFC.h
   ItkRegMFC.rc
   ItkRegMFCDoc.cpp
   ItkRegMFCDoc.h
   MainFrm.cpp
   MainFrm.h
   ItkRegMFCView.cpp
   ItkRegMFCView.h
   )

IF(WIN32)
  LINK_LIBRARIES(
    wsock32
  )
ENDIF(WIN32)


FIND_PACKAGE(ITK)
IF(ITK_FOUND)
   INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
   MESSAGE(FATAL_ERROR
           "Cannot build without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)


ADD_EXECUTABLE(ItkRegMFC WIN32 ${ItkRegMFC_SRCS})
INSTALL_TARGETS(/bin ItkRegMFC)
TARGET_LINK_LIBRARIES(ItkRegMFC ${ITK_LIBRARIES} )


