
PROJECT( ItkMFC )

ADD_DEFINITIONS(-D_AFXDLL) 

SET(CMAKE_MFC_FLAG 6)


SET(ItkMFC_SRCS 
   StdAfx.cpp
   ItkMFC.cpp     
   ItkMFC.rc     
   ItkMFCDoc.cpp
   MainFrm.cpp
   ItkMFCView.cpp
   ItkPipeline.cxx
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


ADD_EXECUTABLE (ItkMFC WIN32 ${ItkMFC_SRCS})
TARGET_LINK_LIBRARIES(ItkMFC ${ITK_LIBRARIES} )


